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

#ifndef _QDATA_H_
#define _QDATA_H_

////////////////////////////////////////////
// internal query evaluator data structures
//	stack: Proof Tree, Vars binding, Trail
//	other data for variables and status
////////////////////////////////////////////

#ifndef _IAFX_H_
#include "iafx.h"
#endif

#ifndef _QUERY_H_
#include "query.h"
#endif

#ifndef _DBINTLOG_H_
#include "dbintlog.h"
#endif

//////////////////////////////////
// stack base behaviour
//	a vector of allocation blocks
//
class IAFX_API BaseStack : protected vectvptr
{
public:
    // allocate top elements
    void push(unsigned = 1);

    // release top element(s)
    stkpos pop(unsigned = 1);

    // return current dimension
    stkpos curr_dim() const;

    // ensure empty on create
    void init();

    // release memory
    ~BaseStack();

protected:

    // next free location
    stkpos free;

    // allocation control
    unsigned vtop;

    // elements allocator
    virtual void* blkalloc(unsigned) = 0;
};

/////////////////////////////
// keep unification bindings
//
class IAFX_API BindStack : public BaseStack
{
public:
    BindStack();

    // alloc and return base of reserved cells
    stkpos reserve(unsigned);

    // return valued var from <off> to base
    Term getvar(stkpos, stkpos *, stkpos *) const;

    // change offset var from base
    void setvar(stkpos, Term, stkpos);

    // make a shared variable cell
    void setshare(stkpos, stkpos);

    // clear binding of requested cell
    void clearcell(stkpos);

    // display current proof segment
    void show(ostream &) const;

private:

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

    // fetch element at position
    elbind* getptr(stkpos) const;

    // allocate entries
    void* blkalloc(unsigned);
};

/////////////////////////////////////////////
// register bindings to undo on backtracking
//
class IAFX_API TrailStack : public BaseStack
{
public:

    TrailStack();

    // register a bind
    void bind(unsigned);

    // access n-th element
    stkpos get(stkpos) const;
    void set(stkpos, stkpos);

    // display current bindings segment
    void show(ostream &) const;

private:

    // allocate entries
    void* blkalloc(unsigned);
};

//////////////////////////////////////////////////////
// encapsulate proof tree navigation
//	not a full stack:
//	access needed to elements with incomplete searchs
//
class IAFX_API ProofStack : public BaseStack
{
public:
    ProofStack();

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

    // get element at pos from base
    Env* get(stkpos) const;

    // return <nth> from top
    Env* topget() const;

    // insert a new element
    stkpos push(stkpos);

    // find base of required, following fathers' chain
    stkpos baseof(stkpos = STKNULL) const;

    // true if <a> is ancestor of <n>
    int isancestor(stkpos, stkpos) const;

    // display current proof segment
    void show(ostream &) const;

    // attempt to recover variables env
    const Env *findVarEnv(stkpos) const;

private:

    // allocate entries
    void* blkalloc(unsigned);
};

/////////////////////////////////
// keep current execution status
//
class IAFX_API ProofStatus
{
public:
    ProofStatus(IntlogExec *);

    void save();
    void restore();

    int	ok() const;
    int empty() const;
    stkpos pdim() const;
    stkpos vdim() const;
    stkpos tdim() const;
    IntlogExec *eng() const;

private:
    void init();

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

    ElemTmp(Term, stkpos);
    ElemTmp(Clause *, stkpos);
    ElemTmp(BltinData *, stkpos);
    ~ElemTmp();
};

//////////////////////////////////////////////
// visit reindexing on linear search position
//
decl_vect(Var);
class OffVars : vectVar
{
public:
    OffVars(Term&, unsigned = 0);
    void SetTerm(Term&, unsigned);

    Var old2new(Var) const;
    Var new2old(Var) const;
    unsigned nvars() const;

private:
    unsigned search(Var) const;
    Var lookup(Var);
    unsigned count;
};

#ifndef _DEBUG
#include "qdata.hpp"
#endif

#endif
