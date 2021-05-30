
/*
    IL : Intlog Language
    Object Oriented Prolog Project
    Copyright (C) 1992-2021 - Ing. Capelli Carlo

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
*/


//-------------------------
// IntlogExec query engine
//-------------------------

#include <iostream>
#include <sstream>
using namespace std;

#include "eng.h"
#include "qdata.h"
#include "builtin.h"
#include "defsys.h"
#include "tracer.h"
#include "unify.h"
#include "parse.h"

#include <QDebug>

//----------------------
// initialize/terminate
//
IntlogExec::IntlogExec(DbIntlog *dbp) {
    db = dbp;
    ps = new ProofStack;
    vs = new BindStack;
    ts = new TrailStack;
    tr = nullptr;

    ncycle = 100;
}
IntlogExec::~IntlogExec() {
    delete ps;
    delete vs;
    delete ts;
    delete tr;
}

//---------------------------------------------
// from main.cpp, factored commodity interface
//
int IntlogExec::query(Term tquery, kstr_list *var_ids, Term *v, int nv) {
    for (int j = 0; j < nv; j++)
        v[j] = Term(f_NOTERM);

    // save current status
    ProofStatus ps(this);
    stkpos cvbase = ps.pdim();

    Clause q(tquery, var_ids, get_db());
    int rc = query(&q);

    if (rc) // query succed: copy variables to caller
        for (int i = 0; i < nv; i++)
            v[i] = value(Var(i), cvbase);

    q.no_image();
    ps.restore();

    return rc;
}
int IntlogExec::runquery(const char *src) {
    int rc = -1;

    Term *var_val = nullptr;
    kstr_list var_ids;

    // prepare input source stream
    IntlogParser* parser = GetEngines()->get_parser();
    istringstream srcstream(src);

    // save status
    parser->SetSource(&srcstream, "-q", &var_ids);

    // submit source line: if parse ok, query
    if (parser->getinput() == IntlogParser::NewClause) {
        Term tparsed = parser->parsed;

        auto nv = var_ids.numel();
        if (nv > 0)
            var_val = new Term[nv];

        rc = query(tparsed, &var_ids, var_val, int(var_ids.numel()));

        delete [] var_val;
        tparsed.Destroy();
    }

    // some error occurred
    return rc;
}

//--------------------------
// evaluation core of query
//  use algorithm ABC
//
int IntlogExec::query(const Clause *q) {

    unsigned nc = 0;

    Env *pcn, *pfn;
    stkpos cn = STKNULL, fn;
#define PCN (pcn = ps->get(cn))
#define PFN (pfn = ps->get(fn))

    UnifyStack us(vs, ts);

    if (!q)
        goto C;

    fn = ps->push(STKNULL);
    PFN->vspos = vs->reserve(q->get_nvars());
    pfn->trail = ts->curr_dim();
    pfn->dbpos = nullptr;
    pfn->call = nullptr;

    // current query is empty?
A:  if (!q->get_body()) {

        // search untried calls
    A1: assert(cn != STKNULL);
        fn = cn;

        Env *e = ps->get(fn);
        while (e->father != STKNULL) {
            if (tr && e->call && tr->exit(fn, e->call))
                return -1;
            if (e->call && !e->call->is_last())
                break;
            fn = e->father;
            e = ps->get(fn);
        }
        if (e->father == STKNULL)
            return 1;

        // set call to next untried brother
        cn = ps->push(PFN->father);
static int cnt = 0;
++cnt;
        PCN->call = PFN->call->next();

        pcn->vspos = pfn->vspos;
        fn = pfn->father;

    } else {
        cn = ps->push(fn);
        PCN->call = q->get_body();
    }

A2: PFN;
    pcn->dbpos = nullptr;
    cc = pcn->call;

    if (nc++ == ncycle) {
        nc = 0;
        sighandler();
    }

    // trace the call
    if (tr && tr->call(cn, cc))
        return -1;

    switch (cc->get_type()) {

    case CallData::BUILTIN: {
        BuiltIn *btp = cc->get_builtin();

        pcn->trail = ts->curr_dim();
        pcn->vspos = pfn->vspos;

        // if evaluate OK
        if (btp->eval(cc->args(), this, 0)) {
            goto A1;
        }

        PCN;

        if (tr && tr->fail(cn, pcn->call))
            return -1;

        unbind(pcn->trail);
    }
        goto C1;

    case CallData::CUT: {
        stkpos gf = PFN->father;
        if (gf != STKNULL &&
                pfn->call->is_last() &&
                pfn->call == pcn->call->next()) {
            // tail recursion optimization
            Env *pgf = ps->get(gf);
            pgf->vspos = pfn->vspos;

            assert(!pcn->call->is_last());

            slist_iter s(tmpt);
            ElemTmp *t;
            while ((t = static_cast<ElemTmp*>(s.next())) != nullptr && t->spos > fn)
                t->spos = fn;

            CallData *cproc = pcn->call;
            cn = ps->pop(cn - fn) - 1;
            PCN->call = cproc->next();
            fn = pcn->father;

            goto A2;
        }

        pcn->trail = ts->curr_dim();
        pcn->vspos = pfn->vspos;
    }
    goto A1;

    case CallData::DISJUNCT:            // replace with catenated try
        pcn->vspos = pfn->vspos;
        pcn->trail = ts->curr_dim();
        cn = ps->push(fn);
        PCN->call = cc->next();         // left side
        goto A2;

    case CallData::DBPRED:

        // initialize DB search
        pcn->dbpos = db->StartProc(cc->get_dbe());

        // DB matching & unification
    B:  if (pcn->dbpos && (q = pcn->dbpos->get()) != nullptr) {

            unsigned nvars = q->get_nvars();
            pcn->vspos = vs->reserve(nvars);
            pcn->trail = ts->curr_dim();
//q->show();
            /*
            if (!unify( pfn->vspos, cc->args(),
                pcn->vspos, q->h_args(), q->h_arity()))
            */
            if (q->h_arity() > 0) {
                TermArgs
                    pa1 = cc->args(),
                    pa2 = q->h_args();

                us.clear();
                for (int i = q->h_arity() - 1; i > 0; i--) {
                    UnifyStack::termPair *tp = us.push();
                    tp->t1 = pa1.getarg(i);
                    tp->i1 = pfn->vspos;
                    tp->t2 = pa2.getarg(i);
                    tp->i2 = pcn->vspos;
                }
                us.check_overflow();

                if (!us.work(   pa1.getarg(0), pfn->vspos,
                                pa2.getarg(0), pcn->vspos)) {
                    // undo changes
                    unbind(pcn->trail);
                    vs->pop(nvars);

                    // try next match
                    pcn->dbpos = pcn->dbpos->succ(db);
                    goto B;
                }
            }

            fn = cn;
            goto A;
        }
        break;

    default:
        assert(0);
    }

    if (tr && PCN->call && tr->fail(cn, cc))
        return -1;

    // backtracking
C1: query_fail(ps->curr_dim() - cn);

    // resume top query
C:  cn = ps->curr_dim() - 1;
    unbind(PCN->trail);

C2: if ((fn = pcn->father) == STKNULL)
        return 0;

    if ((cc = pcn->call) == nullptr)
        goto C1;

    switch (cc->get_type()) {

    case CallData::CUT:  {      // change satisfaction path up to father
        stkpos fvp = PFN->vspos;
        query_fail(cn - fn + 1);
        if ((cn = ps->curr_dim() - 1) != STKNULL) {
            unbind(PCN->trail);
            vs->pop(vs->curr_dim() - fvp);
            goto C2;
        }
        return 0;
    }

    case CallData::BUILTIN: {   // check builtins retry
        BuiltIn *btp = cc->get_builtin();

        if (btp->args & BuiltIn::retry) {

            if (tr && tr->redo(cn, cc))
                return -1;

            // could be resatisfied
            pcn->trail = ts->curr_dim();
            pcn->vspos = PFN->vspos;

            // if evaluate OK
            if (btp->eval(cc->args(), this, 1))
                goto A1;
        }

        // failed
        goto C1;
    }

    case CallData::DISJUNCT:    // evaluate right side
        if (tr && tr->redo(cn, cc))
            return -1;

        pcn->call = cc->get_orelse();
        goto A2;

    case CallData::DBPRED:      // a DB query node to retry
        if (tr) {   // display REDOs (TBD)
            if (pcn->dbpos && pcn->dbpos->succ(db) && tr->redo(cn, cc))
                return -1;
        }
        vs->pop(vs->curr_dim() - pcn->vspos);
        pcn->dbpos = pcn->dbpos->succ(db);
        PFN;
        goto B;

    default:
        assert(0);
    }

    return -1;
}

//------------------------------------------------------------
// attempt a local call in engine
//  expand top node with a query() instance
//
//  on return ok adjust stack(s) contents, binding local vars
//  and saving in trail
//
int IntlogExec::call(Term tc) {
    // get top call clause (for variables...)
    Env *e = ps->topget();
    OffVars rix(tc);

    DbIntlog *dbc = e->call->get_clause()->get_db();
    if (dbc == nullptr)
        dbc = db;

    Clause *c = new Clause(tc, nullptr, dbc);
    if (c->get_image().type(f_NOTERM)) {
        delete c;
        return 0;
    }
    c->set_nvars(rix.nvars());
    save(c);

    // fix env/vars binding bases
    stkpos  psc = ps->curr_dim(),
            tsc = ts->curr_dim();

    unsigned ixv = 0;
    for ( ; ixv < rix.nvars(); ixv++)
        ts->bind(STKNULL);

    int rc = query(c);
    if (rc) {

        // set father and solved query vars
        ps->get(psc)->father = psc - 1;
        stkpos sqv = ps->get(psc)->vspos;

        // share variables from caller env to solved query env
        for (ixv = 0; ixv < rix.nvars(); ixv++, tsc++, sqv++) {
            Var vcsp = rix.new2old(Var(ixv));   // absolute pos
            ts->set(tsc, vcsp);
            vs->setshare(vcsp, sqv);            // hold instances
        }

    } else {
        vs->pop(rix.nvars());
        ts->pop(rix.nvars());
        ps->pop(1);
    }

    return rc;
}

//----------------------------------------
// external call: copy terms at interface
//
class callSave : public BltinData {
public:
    callSave(Term, IntlogExec*);
    ~callSave();

    Clause c;
    OffVars rix;
    ProofStatus stat;
    unsigned nnv, offv;
};
callSave::callSave(Term t, IntlogExec *p)
:   c(t, nullptr, p->get_db()),
    rix(t),
    stat(p) {
    c.set_nvars(rix.nvars());
    nnv = 0;
    offv = unsigned(-1);
}
callSave::~callSave() {
    stat.restore();
}

int IntlogExec::gcall(Term pcall, IntlogExec *eng) {
    // find top call clause to get variable names
    callSave *cs;
    Clause *pc = nullptr;

    if (!eng) {
        assert(pcall.type(f_NOTERM));

        // retry last
        cs = static_cast<callSave*>(get_btdata());
        eng = cs->stat.eng();

    } else {

        // first try: save status to later calls
        cs = new callSave(pcall, eng);
        if (cs->c.get_image().type(f_NOTERM)) {
            delete cs;
            return 0;
        }

        set_btdata(save(cs));
        pc = &cs->c;
    }

    Env *locenv = ps->topget();

    int rc = eng->query(pc);
    if (rc == 1)
        // instance variables to caller env
        for (unsigned ixv = 0; ixv < cs->rix.nvars(); ixv++) {
            // bind the var in caller space
            Term vsrcv = eng->value(ixv, cs->stat.pdim());
            if (!vsrcv.type(f_NOTERM)) {

                OffVars vnew(vsrcv, cs->offv + cs->nnv);
                cs->nnv += vnew.nvars();

                stkpos vdstp = cs->rix.new2old(ixv);
                ts->bind(vdstp);
                vs->setvar(vdstp, save(vsrcv), locenv->vspos);
            }
        }

    return rc;
}

//----------------------------------------
// external call: copy terms at interface
//
int IntlogExec::call(Term pcall, IntlogExec *eng) {
    // count new variables required
    unsigned nnv = 0, offv = vs->curr_dim() - ps->topget()->vspos;

    // find top call clause to get variable names
    Env *locenv = ps->topget();

    OffVars rix(pcall);

    Clause c(pcall, nullptr, eng->db);
    if (c.get_image().type(f_NOTERM))
        return 0;
    c.set_nvars(rix.nvars());

    ProofStatus stat(eng);

    int rc = eng->query(&c);
    if (rc)
        // instance variables to caller env
        for (unsigned ixv = 0; ixv < rix.nvars(); ixv++) {
            // bind the var in caller space
            Term vsrcv = eng->value(ixv, stat.pdim());
            if (!vsrcv.type(f_NOTERM)) {

                OffVars vnew(vsrcv, offv + nnv);
                nnv += vnew.nvars();

                stkpos vdstp = rix.new2old(ixv);
                ts->bind(vdstp);
                vs->setvar(vdstp, save(vsrcv), locenv->vspos);
            }
        }

    vs->reserve(nnv);

    stat.restore();
    return rc;
}

//-----------------------
// search matching terms
//
int IntlogExec::find_match(Term t, DbEntryIter &i, DbIntlog *dbs) {
    Clause *c;
    int tpos = int(ts->curr_dim()), found = 0;
    Term k2 = t.is_rule()? t.getarg(0) : t;

    if (dbs)    // first call: search
        dbs->Search(k2, i, db);

    while (!found && (c = i.next()) != nullptr) {
        Term k1 = c->get_head();
        if (c->is_rule() && t.is_rule() && unify_abs(k1, k2))
            found = 1;
        else if (!c->is_rule() && !t.is_rule() && unify_abs(k1, k2))
            found = 1;
        else
            unbind(stkpos(tpos));
    }

    return found;
}

//------------------------------
// if not already exist, create
//
ProofTracer* IntlogExec::tracer(int mk) {
    if (!tr && mk)
        tr = make_tracer();
    return tr;
}

//-------------------------------
// display current engine status
//
void IntlogExec::showstatus(ostream &s, statusmode mode) const {
    if (mode & Base) {
        s << "proof tree:"    << ps->curr_dim() << ' '
            << "variables:"     << vs->curr_dim() << ' '
            << "trail:"         << ts->curr_dim() << ' '
            << "temporary:"     << tmpt.numel() << ' '
            << endl;
    }

    if (mode & Proof)
        ps->show(s);
    if (mode & Vars)
        vs->show(s);
    if (mode & Trail)
        ts->show(s);

    if (mode & DbHash) {
    }
}

//----------------------------------------------------------------
// release current proof segment with related temporary variables
//
void IntlogExec::query_fail(stkpos n) {
    stkpos limit = ps->pop(n);

    slist_scan s(tmpt);
    ElemTmp *t;
    while ((t = static_cast<ElemTmp*>(s.next())) != nullptr && t->spos >= limit)
        s.delitem();
}

////////////////////////////
// build a tracer at request
//
ProofTracer *IntlogExec::make_tracer() {
    return new ProofTracer(this);
}

ostream& operator<<(ostream& s, teWrite& data) {
    return data.m_pExec->write(data.m_toWrite, s, data.m_stkPos);
}
ostream& operator<<(ostream& s, Term t) {
    teWrite w(t, nullptr);
    return s << w;
}
