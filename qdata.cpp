
/*
    IL : Intlog Language
    Object Oriented Prolog Project
    Copyright (C) 1992-2016 - Ing. Capelli Carlo

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


///////////////////////
// query data access
//	solution displayer
///////////////////////

#include "stdafx.h"
#include "qdata.h"
#include "defsys.h"
#include "parsemsg.h"
#include "eng.h"
#include <cstdarg>
#include <stdexcept>

//////////////////////////////////////////
// keep parser active, using parsed terms
//
void IntlogExec::use_file()
{
    ProofStatus stglobal(this), stq(this);

    // catch input file Data
    IntlogIStream *is = ips();
    Clause *c = 0;

    while (is->ateof() == 0) {

        user_prompt();

        switch (is->read()) {

        case IntlogIStream::NewClause:

            if (c) {
                delete c;
                stq.restore();
            }

            if ((c = build_clause(is)) == 0) {	// semantic error
                stq.save();
                break;
            }

            if (c->is_query()) {
                stq.save();
                if (query(c))
                    show_solution(c, 0);
                else {
                    no_solution();
                    delete c;
                    c = 0;
                    stq.restore();
                }
            } else if (c->is_command()) {
                stq.save();
                query(c);
            } else {
                db->Add(c);
                c = 0;
            }
            break;

        case IntlogIStream::RedoCommand:
            if (c) {
                if (query())
                    show_solution(c, 0);
                else {
                    no_solution();
                    delete c;
                    c = 0;
                    stq.restore();
                }
            } else
                no_solution();
            break;

        case IntlogIStream::EndFile:
        case IntlogIStream::Error:
            break;
        }
    }

    if (c)
        delete c;
    stglobal.restore();
}

//////////////////////////////////////////
// enable overriding clause construction
//
Clause *IntlogExec::build_clause(IntlogIStream *is)
{
    Clause *c = new Clause(is->t_data, &is->t_vars, get_db(), is->name());

    if (!c->is_ok())
    {
        delete c;
        return 0;
    }
    return c;
}

/////////////////////////////////
// open a file and run interface
//
int IntlogExec::use_file(kstring fileid)
{
    if (see(fileid)) {
        use_file();
        seen();
        return 1;
    }

    GetEngines()->ErrMsg(FILE_NOT_FOUND, CCP(fileid));
    return 0;
}

////////////////////////////////////////////////
// variable data access
//	make a copy of term	to be freed after usage
//
Term IntlogExec::value(Var rv, stkpos posenv) const
{
    if (rv == ANONYM_IX)
        return f_NOTERM;

    // get top node
    Env *e = ps->get(posenv);
    Term v = vs->getvar(rv + e->vspos, &posenv, &posenv);

    // make copy
    if (!v.type(f_NOTERM))
        v = copy_term(v, posenv);

    return v;
}

////////////////////////////////////////
// copy term built into variables stack
//
Term IntlogExec::copy_term(Term t, stkpos e) const
{
    Term t1;

#ifndef NDEBUG
    t1.NoTerm();
#endif

    switch (t.type()) {

    case f_INT:
    case f_ATOM:
        t1 = t;
        break;

    case f_DOUBLE:
        t1 = Term(Double(t));
        break;

    case f_VAR: {
        Var v = Var(t);
        if (v != ANONYM_IX && e != STKNULL) {
            stkpos e1;
            Term tv = vs->getvar(v + e, &e1, &e1);
            if (!tv.type(f_NOTERM))
                t1 = copy_term(tv, e1);
            else
                t1 = Term(Var(e1));
        } else
            t1 = Term(v);
    }
    break;

    case f_STRUCT: {
        int a = t.get_arity();
        t1 = Term(t.get_funct(), a);
        for (int i = 0; i < a; i++)
            t1.setarg(i, copy_term(t.getarg(i), e));
    }
    break;

    case f_LIST:
        if (!t.LNULL()) {	// not empty list
            const List& l = t;
            t1 = Term(copy_term(l.head(), e), copy_term(l.tail(), e));
        } else
            t1 = t;
        break;

    case f_SYSDATA: {
        SysData *sd = t;
        if (sd->copy_able())
            t1 = sd->copy();
        else
            t1 = t;		// WARNING: shared data
    }
    break;

    default:
        ASSERT(0);
    }

    ASSERT(!t1.type(f_NOTERM));
    return t1;
}

/////////////////////////////////////
// display all vars of current query
//
int IntlogExec::show_solution(const Clause *q, stkpos base) const
{
    ostream &s = out();
    Var currvar = 0;

    TermVisit tv(q->get_image());
    Term t;

    while (!(t = tv.next()).type(f_NOTERM))
        if (t.type(f_VAR) && Var(t) == currvar)
        {
            s << CCP(q->get_varid(currvar)) << '=';

            // get base node
            stkpos pres;
            Term tv = vs->getvar(currvar + ps->get(base)->vspos, &pres, &pres);
            if (!tv.type(f_NOTERM))
                write(tv, s, pres) << endl;
            else
                s << '_' << pres << endl;

            currvar++;
        }

    if (currvar == 0)
        yes_solution();

    return currvar;
}

///////////////////////////////////////
// release bindings of environment <e>
//
void IntlogExec::unbind(stkpos atpos)
{
    stkpos cpos = ts->curr_dim();
    while (cpos-- > atpos)
    {
        vs->clearcell(ts->get(cpos));
        ts->pop();
    }
}

///////////////////////////////
// insert temporary store list
//
Term IntlogExec::save(Term t)
{
    tmpt.insert(new ElemTmp(t, ps->curr_dim() - 1), 0);
    return t;
}
Clause* IntlogExec::save(Clause *c)
{
    tmpt.insert(new ElemTmp(c, ps->curr_dim() - 1), 0);
    return c;
}
BltinData* IntlogExec::save(BltinData *d)
{
    tmpt.insert(new ElemTmp(d, ps->curr_dim() - 1), 0);
    return d;
}

/////////////////////////////////////
// build a clause (to be added to DB)
//
Clause *IntlogExec::make_clause(Term t)
{
    // adjust variables
    OffVars rix(t);
    Env *etop = ps->topget();
    const Clause *c = etop->call->get_clause();
    kstr_list vnames;
    for (unsigned v = 0; v < rix.nvars(); v++)
        vnames.addnc(c->get_varid(rix.new2old(v) - etop->vspos));

    // memory instance
    Clause *nc = new Clause(t, &vnames, db, c->get_source());
    if (nc->get_image().type(f_NOTERM)) {
        delete nc;
        return 0;
    }

    return nc;
}

//////////////////////////////////
// read from current input stream
//
static void absVars(Term t, stkpos p)
{
    stack<Term> st;
    st.push(t);

    for ( ; st.size() > 0; )
    {
        t = st.get();
        st.pop();

        if (t.type(f_STRUCT|f_LIST))
            for (int ixson = 0, arity = t.get_arity(); ixson < arity; ixson++)
            {
                Term son = t.getarg(ixson);
                if (son.type(f_VAR))
                {
                    Var v = son;
                    if (v != ANONYM_IX)
                        t.setarg(ixson, Term(Var(v + p)));
                }
                else
                    st.push(son);
            }
    }
}
Term IntlogExec::readTerm()
{
    IntlogIStream *i = ips();
    if (!i)
        see(user);
    i = ips();

    if (!i->ateof())
    {
        i->t_data.NoTerm();
        if (i->read() == IntlogIStream::NewClause)
        {
            // make space to vars, if any
            unsigned nv = i->t_vars.numel();
            if (nv > 0)
            {
                absVars(i->t_data, vs->curr_dim());
                vs->reserve(nv);
            }

            return i->t_data;
        }
    }

    return f_NOTERM;
}

/////////////////////////////////
// display builtin error message
//
int IntlogExec::BtErr(int nCodeMsg, ...)
{
    va_list argptr;
    MsgTable::MsgLink *l = GetEngines()->get_msgtbl()->search(nCodeMsg);

    if (!l)
        out() << "message not found: " << nCodeMsg << endl;
    else
    {
        va_start(argptr, nCodeMsg);

        char buf[512];
        vsnprintf(buf, sizeof buf, l->string, argptr);

        if (TermData(cc->val()) != f_NOTERM)
        {
            teWrite w(cc->val(), this);
            out() << "builtin error:" << w << ':' << buf << endl;
        }
        else
            out() << "builtin error:" << buf << endl;
    }

    //throw std::runtime_error("exception");
    return 0;
}

///////////////////////
// save current status
//
void ProofStatus::save()
{
    psdim = p->ps->curr_dim();
    vsdim = p->vs->curr_dim();
    tsdim = p->ts->curr_dim();
}

///////////////////////////
// restore previous status
//
void ProofStatus::restore()
{
    ASSERT(ok());

    p->ps->pop(p->ps->curr_dim() - psdim);
    p->vs->pop(p->vs->curr_dim() - vsdim);
    p->ts->pop(p->ts->curr_dim() - tsdim);

    if (p->ps->curr_dim() == 0) {
        p->tmpt.clear();
        p->vs->pop(p->vs->curr_dim());
        p->ts->pop(p->ts->curr_dim());
        p->db->ClearStatus();
    }

    init();
}

////////////////////////////////////////////////////////
// store all non anonymous variables, reindexing from 0
//	save previous indexes, to be queried later
//
void OffVars::SetTerm(Term& tin, stkpos offbase)
{
    count = 0;

    if (tin.type(f_VAR) && Var(tin) != ANONYM_IX) {
        tin = Term(lookup(Var(tin)) + offbase);
        return;
    }

    stack<Term> st;
    st.push(tin);

    for ( ; st.size() > 0; ) {
        Term t = st.get();
        st.pop();

        if (t.type(f_STRUCT|f_LIST)) {
            int arity = t.get_arity();
            for (int ixson = 0; ixson < arity; ixson++) {
                Term son = t.getarg(ixson);
                if (son.type(f_VAR) && Var(son) != ANONYM_IX)
                    t.setarg(ixson, Term(lookup(Var(son)) + offbase));
                else
                    st.push(son);
            }
        }
    }
}

////////////////////////////////////////////
// search for variable, adding if not found
//	return new index from 0
//
Var OffVars::lookup(Var v)
{
    unsigned rv = search(v);
    if (rv == unsigned(-1)) {
        // add variable, avoiding overflow
        if ((rv = count) == dim())
            grow(16);
        setat(count++, v);
    }
    return Var(rv);
}

///////////////////////////////////////////
// display command prompter if appropriate
//
void IntlogExec::user_prompt() const
{
    if (ips()->name() == name() && ops()->name() == name())
        (out() << "> ").flush();
}

///////////////////////////
// display failure message
//
void IntlogExec::no_solution() const
{
    out() << "no" << endl;
}

///////////////////////////
// display success message
//
void IntlogExec::yes_solution() const
{
    out() << "yes" << endl;
}

/////////////////////////////////////////////////
// change all clauses contained in required file
//
int IntlogExec::reuse_file(kstring fileId)
{
    slistvptr updated;

    DbIntlog *r = get_db()->RemoveFileClauses(fileId, updated);
    if (r)
    {
        r->ClearStatus();

        see(fileId);
        IntlogIStream *is = ips();

        while (is->read() != IntlogIStream::EndFile)
        {
            Term d = is->t_data;
            if (!d.is_query() && !d.is_command())
            {
                Clause *c = new Clause(d, &is->t_vars, r, fileId);
                r->RestoreClause(c);
            }
            else
                d.Destroy();
        }

        seen();
    }
    return 1;	// dont' fail anyway
}

BltinData* IntlogExec::get_btdata() const
{
    Env *e = ps->topget();
    ASSERT(e->call->get_type() == CallData::BUILTIN);
    return e->ptr;
}
void IntlogExec::set_btdata(BltinData* ptr)
{
    Env *e = ps->topget();
    ASSERT(e->call->get_type() == CallData::BUILTIN);
    e->ptr = ptr;
}

// return which CallData is currently executed
const CallData *IntlogExec::on_call() const
{
    return ps->topget()->call;
}

// build a copy of term using variable access
Term IntlogExec::copy(Term t) const
{
    Env *e = ps->topget();
    ASSERT(!t.type(f_NOTERM) && e);
    return copy_term(t, e->vspos);
}

// variable data access: get top node
Term IntlogExec::tval(Var rv, stkpos posenv) const
{
    return rv == ANONYM_IX? Term(f_NOTERM) : vs->getvar(rv + ps->topget()->vspos, &posenv, &posenv);
}
