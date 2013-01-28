/*
    IL : Prolog interpreter
    Copyright (C) 1992-2012 - Ing. Capelli Carlo

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

/*
 Intlog Language
 Object Oriented Prolog Project
 Ing. Capelli Carlo - Brescia
*/

#ifndef _FASTACC_H_
#include "fastacc.h"
#endif

#ifdef _DEBUG
#define inline
#endif

/************ Base Stack Handler **************/

// return current dimension
inline stkpos BaseStack::curr_dim() const
{
    return free;
}

/********** Bind Stack behaviour ***********/

inline BindStack::BindStack()
{
    init();
}
inline void* BindStack::blkalloc(unsigned cnt)
{
    return new elbind[cnt];
}

// fetch element at position
inline BindStack::elbind* BindStack::getptr(stkpos pos) const
{
#if 0 //def _DEBUG
    extern void CHECKCOND(int);
    CHECKCOND(pos < free);
#endif
    ASSERT(pos < free);
    return (elbind*)getat(OPDIV(pos, BLKDIM)) + OPMOD(pos, BLKDIM);
}

// clear binding of requested cell
inline void BindStack::clearcell(stkpos cell)
{
    elbind *e = getptr(cell);
    e->value = f_NOTERM;
    e->envp = 0;
}

// make a shared variable cell
inline void BindStack::setshare(stkpos cell, stkpos rpos)
{
    ASSERT(rpos != STKNULL);
    ASSERT(cell != rpos);
    elbind *e = getptr(cell);
    e->share = rpos;
    e->envp = STKNULL;
}

// change offset var from base
inline void BindStack::setvar(stkpos off, Term t, stkpos envp)
{
    ASSERT(envp != STKNULL);
    elbind *vval = getptr(off);
    vval->value = t;
    vval->envp = envp;
}

/************** Trail Stack Control **************/

inline TrailStack::TrailStack()
{
    init();
}
inline void* TrailStack::blkalloc(unsigned cnt)
{
    return new stkpos[cnt];
}

// access n-th element
inline stkpos TrailStack::get(stkpos n) const
{
    ASSERT(n < free);
    stkpos *p = (stkpos*)getat(OPDIV(n, BLKDIM));
    return p[OPMOD(n, BLKDIM)];
}
inline void TrailStack::set(stkpos n, stkpos v)
{
    ASSERT(n < free);
    stkpos *p = (stkpos*)getat(OPDIV(n, BLKDIM));
    p[OPMOD(n, BLKDIM)] = v;
}

// register a bind
inline void TrailStack::bind(unsigned v)
{
    push();
    set(free - 1, v);
}

/*************** Proof Stack Control *************/

inline ProofStack::ProofStack()
{
    init();
}
inline void* ProofStack::blkalloc(unsigned cnt)
{
    return new Env[cnt];
}

// get element at pos from base
inline ProofStack::Env* ProofStack::get(stkpos pos) const
{
    ASSERT(pos < free);
    return (Env*)getat(OPDIV(pos, BLKDIM)) + OPMOD(pos, BLKDIM);
}

// return <nth> from top
inline ProofStack::Env* ProofStack::topget() const
{
    stkpos pos = free - 1;
    return (Env*)getat(OPDIV(pos, BLKDIM)) + OPMOD(pos, BLKDIM);
}

// insert a new element
inline stkpos ProofStack::push(stkpos from)
{
    BaseStack::push();
    stkpos ret = free - 1;
    Env *e = get(ret);
    e->father = from;
    return ret;
}

/************ Proof Status Data ***************/

inline ProofStatus::ProofStatus(IntlogExec *pe)
{
    p = pe;
    save();
}
inline int ProofStatus::ok() const
{
    return psdim != STKNULL;
}
inline void ProofStatus::init()
{
    psdim = STKNULL;
}
inline int ProofStatus::empty() const
{
    return psdim == 0;
}
inline stkpos ProofStatus::pdim() const
{
    return psdim;
}
inline stkpos ProofStatus::vdim() const
{
    return vsdim;
}
inline stkpos ProofStatus::tdim() const
{
    return tsdim;
}
inline IntlogExec *ProofStatus::eng() const
{
    return p;
}

/********* Temporary Storage Data ****************/

inline ElemTmp::ElemTmp(Term ts, stkpos atp)
{
    t = TermData(ts);
    spos = atp;
    type = vT;
}
inline ElemTmp::ElemTmp(Clause *cs, stkpos atp)
{
    c = cs;
    spos = atp;
    type = vC;
}
inline ElemTmp::ElemTmp(BltinData *ds, stkpos atp)
{
    d = ds;
    spos = atp;
    type = vD;
}

inline ElemTmp::~ElemTmp()
{
    switch (type) {
    case vT:
        Term(t).Destroy();
        break;
    case vC:
        delete c;
        break;
    case vD:
        delete d;
        break;
    }
}

/******** Offseting Vars Data ***********/

inline OffVars::OffVars(Term& t, unsigned off)
{
    SetTerm(t, off);
}
inline Var OffVars::old2new(Var oldv) const
{
    return Var(search(oldv));
}
inline Var OffVars::new2old(Var newv) const
{
    return getat(unsigned(newv));
}
inline unsigned OffVars::nvars() const
{
    return count;
}
// search position in vector
inline unsigned OffVars::search(Var v) const
{
    for (unsigned i = 0; i < count; i++)
        if (getat(i) == v)
            return i;
    return unsigned(-1);
}

/************** Engine Code ************/

// builtins context sensitive data
inline BltinData* IntlogExec::get_btdata() const
{
    ProofStack::Env *e = ps->topget();
    ASSERT(e->call->get_type() == CallData::BUILTIN);
    return e->ptr;
}
inline void IntlogExec::set_btdata(BltinData* ptr)
{
    ProofStack::Env *e = ps->topget();
    ASSERT(e->call->get_type() == CallData::BUILTIN);
    e->ptr = ptr;
}

// return which CallData is currently executed
inline const CallData *IntlogExec::on_call() const
{
    return ps->topget()->call;
}

// build a copy of term using variable access
inline Term IntlogExec::copy(Term t) const
{
    ProofStack::Env *e = ps->topget();
    ASSERT(!t.type(f_NOTERM) && e);
    return copy_term(t, e->vspos);
}

// variable data access: get top node
inline Term IntlogExec::tval(Var rv, stkpos posenv) const
{
    return rv == ANONYM_IX? Term(f_NOTERM) : vs->getvar(rv + ps->topget()->vspos, &posenv, &posenv);
}
