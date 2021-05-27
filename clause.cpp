
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


#include "query.h"
#include "dbintlog.h"
#include "builtin.h"
#include "eng.h"
#include "parse.h"
#include "parsemsg.h"
#include "defsys.h"
#include "reduce.h"

#include <map>
#include <QDebug>

//////////////////////////////////////////////
// make a  clause: check for semantic
//  variable names are behaviour sectors here
//
Clause::Clause(Term t, kstr_list *vars, DbIntlog *pdb, kstring srcid, SrcPosTree *sp) {
//qDebug() << t.show().c_str();

    memset(this, 0, sizeof(Clause));

    image = Term(f_NOTERM);
    db = pdb;

    // track down calldata construction indexes
    NodeIndex nIndex = (spt = sp)? sp->Root() : INVALID_NODE;

    n_vars = n_anon = 0;

    // save variables names
    if (vars && (n_vars = vars->numel()) > 0) {
        varids = new kstring[n_vars];
        for (unsigned ixv = 0; ixv < n_vars; ixv++)
            varids[ixv] = vars->get(ixv);
    }

    if (srcid == kstring(MSR_NULL)) {
        if ((body = makebody(&t, sp, nIndex)) == nullptr)
            return;
    }
    else if (t.is_query() || t.is_command()) {
        Term tr = t.getarg(0);
        if (sp)
            nIndex = sp->FirstSon(nIndex);
        if ((body = makebody(&tr, sp, nIndex)) == nullptr)
            return;
    }
    else if (t.is_rule()) {
        if (check_funct(t.getarg(0)))
            return;

        Term tr = t.getarg(1);
        if (sp)
            nIndex = sp->NextSon(sp->FirstSon(nIndex));
        if ((body = makebody(&tr, sp, nIndex)) == nullptr)
            return;
    }
    else {   // a fact (no body)
        if (check_funct(t))
            return;
    }

    image = t;
    src = srcid;

    if (srcid != kstring(MSR_NULL) && !t.is_query()) {
        Term h = t.is_rule()? t.getarg(0) : t;
        argp = h.getargs();
        arity = h.get_arity();
    }
    map<unsigned, unsigned> cntref;

//qDebug() << "before" << t.show().c_str();
    TermData tdata = t;
    TermDataVisit tdv(&tdata);
    while (auto p = tdv.next()) {
        Term P = *p;
        if (P.type(f_VAR)) {
            Var v = P;
            if (v == ANONYM_IX)
                *p = Term(Var(n_vars + n_anon++));
            else
                cntref[v]++;
        }
//qDebug() << 'P' << P.show().c_str();
    }
//qDebug() << "after" << t.show().c_str();

    assert(!vars || n_vars == unsigned(cntref.size()));
    if (!vars)
        n_vars = unsigned(cntref.size());

    if (!t.is_query() && varids)
        for (auto p: cntref)
            if (p.second == 1) {
                CCP idv = CCP(varids[p.first]);
                if (idv[0] != '_')
                    err_msg(UNUSED_VAR, idv);
            }
}

////////////////////////////
// release allocated memory
//
Clause::~Clause() {
    image.Destroy();
    delete body;
    delete varids;
    delete spt;
}

///////////////////////////////////
// recursive building of body part
//
CallData *Clause::makebody(Term *pt, SrcPosTree *sp, NodeIndex nIndex) {
    Term t = *pt;

//teWrite::showDebug(__FUNCTION__, t);

    if (t.is_and()) {
        // conjunctions: get left and right
        CallData *cl, *cr, *l;
        Term tl = t.getarg(0), tr = t.getarg(1);

        NodeIndex ixl = INVALID_NODE, ixr = INVALID_NODE;
        if (sp)
            ixr = sp->NextSon(ixl = sp->FirstSon(nIndex));

        if ((cl = makebody(&tl, sp, ixl)) == nullptr)
            return nullptr;

        if ((cr = makebody(&tr, sp, ixr)) == nullptr) {
            delete cl;
            return nullptr;
        }

        if (cl->type == CallData::DISJUNCT) {
            // link all terminals paths to right part
            stack<void*> s;
            s.push(cl->link);
            s.push(cl->orelse);
            while (s.size()) {
                l = reinterpret_cast<CallData *>(s.get());
                s.pop();

                // find last
                while (l->link) {
                    if (l->type == CallData::DISJUNCT)
                        s.push(l->orelse);
                    l = l->link;
                }

                // connect to right
                l->link = cr;
            }
        }
        else
            cl->link = cr;

        return cl;
    }

    if (t.is_or()) {
        // disjunction: make else path node
        CallData *cl, *cr, *cd;
        Term tl = t.getarg(0), tr = t.getarg(1);

        NodeIndex ixl = INVALID_NODE, ixr = INVALID_NODE;
        if (sp)
            ixr = sp->NextSon(ixl = sp->FirstSon(nIndex));

        if ((cl = makebody(&tl, sp, ixl)) == nullptr)
            return nullptr;
        if ((cr = makebody(&tr, sp, ixr)) == nullptr) {
            delete cl;
            return nullptr;
        }

        cd = new CallData(this);
        cd->type = CallData::DISJUNCT;

        cd->link = cl;      // link to first path
        cd->orelse = cr;    // and make alternate path

        cd->spos = nIndex;

        return cd;
    }

    CallData *cd = new CallData(this);
    cd->spos = nIndex;

    if (t.is_cut())
        cd->type = CallData::CUT;
    else {
        if (t.type(f_VAR)) {  // transform X to call(X)
            if (Var(t) != ANONYM_IX) {
                CCP vid = get_varid(Var(t));
                if (vid)
                    err_msg(METAVAR_CALL, vid);

                Term tc = *pt = Term(kstring("call"), 1);
                tc.setarg(0, t);
                t = tc;
            }
            else {
                delete cd;
                err_msg(ANONYMVAR_CALL);
                return nullptr;
            }
        }

        if (t.type(f_LIST|f_INT|f_DOUBLE)) {
            err_msg(UNCALL_AS_FUNCTOR);
            return nullptr;
        }

        BuiltIn *bt = builtin(t);
        if (bt) {
            cd->bltin = bt;
            cd->type = CallData::BUILTIN;
        }
        else {
            assert(db != nullptr);
            if ((db = db->FixPathName(&t)) == nullptr) {
                delete cd;
                err_msg(INVALID_DB_PATH);
                return nullptr;
            }
            cd->entry = db->GetEntryRef(t);
            cd->type = CallData::DBPRED;
        }
    }

    cd->argp = t.getargs();

    return cd;
}

/////////////////////////////////////////////
// verify that functor can be used to search
//
int Clause::check_funct(Term h) const {
    if (h.is_or())
        return err_msg(OR_AS_FUNCTOR);
    if (h.is_and())
        return err_msg(AND_AS_FUNCTOR);
    if (h.type(f_LIST))
        return err_msg(LIST_AS_FUNCTOR);
    if (h.type(f_INT))
        return err_msg(INT_AS_FUNCTOR);
    if (h.type(f_DOUBLE))
        return err_msg(REAL_AS_FUNCTOR);
    if (builtin(h))
        return err_msg(BUILTIN_REDEFINITION, CCP(h.get_funct()));

    return 0;
}

/////////////////////////////////////////////////////
// display semantic error (reply on parser messager)
//
int Clause::err_msg(int code, const char *s) const {
    IntlogParser *p = GetEngines()->get_parser();
    if (p)
        p->err_msg(code, s);
    return -1;
}

///////////
// utility
//
BuiltIn* Clause::builtin(Term t) const {
    return db->is_builtin(t);
}

////////////////////////////////////
// get a variable identifier string
//
kstring Clause::get_varid(Var v) const {
    if (varids && v < n_vars)
        return varids[v];
    return kstring(MSR_NULL);
}

/////////////////////////////////////////////
// attempt to find image of CallData pointer
//
Term Clause::cd_image(const CallData *pData) const {
    stack<void*> cs;    // CallData stack
    stack<Term> ts; // image Term stack

    // prepare stacks
    cs.push(body);

    if (image.is_rule())
        ts.push(image.getarg(1));
    else if (image.is_query())
        ts.push(image.getarg(0));
    else
        ts.push(image);

    for ( ; ; ) {
        // substitution step
        if (cs.size() == 0)
            return f_NOTERM;

        CallData *g = reinterpret_cast<CallData*>(cs.get());
        cs.pop();

        Term t = ts.get();
        ts.pop();

        // found?
        if (g == pData)
            return t.is_and()? t.getarg(0) : t;

        if (t.is_and()) {
            CallData *gd = g->link;

            if (g->type == CallData::DISJUNCT) {
                // seek reconjunction point
                slistvptr slp;
                while (gd) {
                    slp.insert(gd, 0);
                    gd = gd->link;
                }

                // follow rightmost path
                gd = g->orelse;
                while (gd) {
                    slistvptr_iter i(slp);
                    void *p;
                    while ((p = i.next()) != nullptr)
                        if (p == gd)
                            goto found;
                    gd = gd->type == CallData::DISJUNCT?
                                gd->orelse : gd->link;
                }
found:
                assert(gd);
            }

            ts.push(t.getarg(1));
            cs.push(gd);

            ts.push(t.getarg(0));
            cs.push(g);

        }
        else if (t.is_or()) {
            assert(g->type == CallData::DISJUNCT);

            ts.push(t.getarg(1));
            cs.push(g->orelse);

            ts.push(t.getarg(0));
            cs.push(g->link);
        }
    }
}

void Clause::show() const {
    using namespace std;
    cout << CCP(src) << image.show() << endl;
    for (size_t x = 0; x < n_vars; ++x)
        cout << x << ' ' << CCP(varids[x]) << endl;
}

////////////////////////////////////////////////////////
// recursive destructor
//  on multiple paths avoid multiple recursive deletion
//
CallData::~CallData() {
    if (type == DISJUNCT) {   // find path reconjunction point(s)
        slistvptr s;
        s.append(orelse);

        CallData *or_, *l, *r;
        while ((or_ = reinterpret_cast<CallData *>(s.get(0))) != nullptr) {
            s.remove(0);

            for (l = link; l; l = l->link) {
                for (r = or_; r; r = r->link) {
                    if (r->type == DISJUNCT &&
                        s.seek(r->orelse) == SLIST_INVPOS)
                        s.insert(r->orelse, 0);

                    if (r->link == l) { // disconnect
                        r->link = nullptr;
                        break;
                    }
                }

                if (r)
                    break;
            }
        }
        delete orelse;
    }
    delete link;
}
