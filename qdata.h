
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


#ifndef _QDATA_H_
#define _QDATA_H_

////////////////////////////////////////////
// internal query evaluator data structures
//	stack: Proof Tree, Vars binding, Trail
//	other data for variables and status
////////////////////////////////////////////

#include "iafx.h"
#include "query.h"
#include "dbintlog.h"

//////////////////////////////////
// stack base behaviour
//	a vector of allocation blocks
//
template<class T>
class IAFX_API BaseStack : protected vect<T>
{
public:

    // allocate top elements
    void push(unsigned n = 1) {
        for (unsigned c = 0; c < n; ++c) {
            this->push_back(T());
            ++free;
        }
    }

    // release top element(s)
    stkpos pop(unsigned n = 1) {
        ASSERT(free >= n);
        free -= n;
        return free;
    }

    // return current dimension
    stkpos curr_dim() const { return free; }

    // ensure empty on create
    void init() {
    }

protected:
    unsigned free = 0;
};

// stack vector entry
struct elbind {

    // binder Env reference
    stkpos envp;

    // shared or ground: check envp
    union {
        stkpos		share;
        TermData	value;
    };
};

/////////////////////////////
// keep unification bindings
//
class IAFX_API BindStack : public BaseStack<elbind>
{
public:
    BindStack() {
        init();
    }

    // alloc and return base of reserved cells
    stkpos reserve(unsigned);

    // return valued var from <off> to base
    Term getvar(stkpos, stkpos *, stkpos *) const;

    // change offset var from base
    void setvar(stkpos off, Term t, stkpos envp) {
        ASSERT(envp != STKNULL);
        elbind *vval = getptr(off);
        vval->value = t;
        vval->envp = envp;
    }

    // make a shared variable cell
    void setshare(stkpos cell, stkpos rpos) {
        ASSERT(rpos != STKNULL);
        ASSERT(cell != rpos);
        elbind *e = getptr(cell);
        e->share = rpos;
        e->envp = STKNULL;
    }

    // clear binding of requested cell
    void clearcell(stkpos cell)  {
        elbind *e = getptr(cell);
        e->value = f_NOTERM;
        e->envp = 0;
    }

    // display current proof segment
    void show(ostream &) const;

};

/////////////////////////////////////////////
// register bindings to undo on backtracking
//
class IAFX_API TrailStack : public BaseStack<stkpos>
{
public:

    TrailStack() {
        init();
    }

    // register a bind
    void bind(unsigned v) {
        push();
        set(free - 1, v);
    }

    // access n-th element
    stkpos get(stkpos n) const {
        return getat(n);
    }

    void set(stkpos n, stkpos v) {
        setat(n, v);
    }

    // display current bindings segment
    void show(std::ostream &) const;
};

/////////////////////////////
// Proof main data structure
//	keep track of a try
//
struct Env
{
    // current RHB procedure
    CallData* call;

    // location in DB (if DBPRED) or other infos
    union {
        const e_DbList*	dbpos;
        BltinData*		ptr;
    };

    // father of this (-1 if none)
    stkpos father;

    // query variables start
    stkpos vspos;

    // binded trail start index
    stkpos trail;
};

//////////////////////////////////////////////////////
// encapsulate proof tree navigation
//	not a full stack:
//	access needed to elements with incomplete searchs
//
class IAFX_API ProofStack : public BaseStack<Env>
{
public:
    ProofStack() {
        init();
    }

    // get element at pos from base
    Env* get(stkpos pos) const {
        return getptr(pos);
    }

    // return <nth> from top
    Env* topget() const {
        return get(free - 1);
    }

    // insert a new element
    stkpos push(stkpos from) {
        BaseStack::push();
        stkpos ret = free - 1;
        Env *e = get(ret);
        e->father = from;
        return ret;
    }

    // find base of required, following fathers' chain
    stkpos baseof(stkpos = STKNULL) const;

    // true if <a> is ancestor of <n>
    int isancestor(stkpos a, stkpos n) const;

    // display current proof segment
    void show(ostream &) const;

    // attempt to recover variables env
    const Env *findVarEnv(stkpos) const;
};

/////////////////////////////////
// keep current execution status
//
class IAFX_API ProofStatus
{
public:
    ProofStatus(IntlogExec *pe) {
        p = pe;
        save();
    }

    void save();
    void restore();

    int	ok() const {
        return psdim != STKNULL;
    }

    int empty() const {
        return psdim == 0;
    }

    stkpos pdim() const {
        return psdim;
    }

    stkpos vdim() const {
        return vsdim;
    }

    stkpos tdim() const {
        return tsdim;
    }

    IntlogExec *eng() const {
        return p;
    }

private:
    void init() {
        psdim = STKNULL;
    }

    IntlogExec *p;
    stkpos psdim, vsdim, tsdim;
};

/////////////////////////////////////////////////////////////
// enable storage of builtins context sensitive data
//	data will be deleted when tried bltin become unreachable
//
class BltinData
{
public:
    virtual ~BltinData() {}
};

/////////////////////////////////////////////////
// temporary terms/clauses/builtins data storage
//
class ElemTmp : public e_slist
{
    friend class IntlogExec;

    union {
        TermData t;
        Clause* c;
        BltinData* d;
    };
    enum {
        vT, vC, vD
    } type;
    stkpos spos;

    ElemTmp(Term ts, stkpos atp) {
        t = TermData(ts);
        spos = atp;
        type = vT;
    }
    ElemTmp(Clause *cs, stkpos atp) {
        c = cs;
        spos = atp;
        type = vC;
    }
    ElemTmp(BltinData *ds, stkpos atp) {
        d = ds;
        spos = atp;
        type = vD;
    }

    ~ElemTmp() {
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
};

//////////////////////////////////////////////
// visit reindexing on linear search position
//
class OffVars : vect<Var>
{
public:
    OffVars(Term& t, unsigned off = 0) {
        SetTerm(t, off);
    }

    void SetTerm(Term&, unsigned);

    Var old2new(Var oldv) const {
        return Var(search(oldv));
    }

    Var new2old(Var newv) const {
        return getat(unsigned(newv));
    }

    unsigned nvars() const {
        return count;
    }

private:
    unsigned search(Var v) const {
        for (unsigned i = 0; i < count; i++)
            if (getat(i) == v)
                return i;
        return unsigned(-1);
    }

    Var lookup(Var);
    unsigned count;
};

#endif
