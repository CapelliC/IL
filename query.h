
/*
    IL : Intlog Language
    Object Oriented Prolog Project
    Copyright (C) 1992-2020 - Ing. Capelli Carlo

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


#ifndef QUERY_H_
#define QUERY_H_

//-----------------------------------
// query evaluation execution engine
//-----------------------------------

#include "iafx.h"
#include "proios.h"
#include "term.h"

// forward classes
class ProofStack;
class BindStack;
class TrailStack;
class ProofTracer;
class ProofStatus;
class BltinData;
class Clause;
class CallData;
class DbIntlog;
class DbEntryIter;

// internal stacks index type
typedef unsigned	stkpos;
#define STKNULL		stkpos(-1)

class IAFX_API IntlogExec : public IntlogIOStreams
{
public:
    // init object and bind to a DB
    IntlogExec(DbIntlog*);

    // release memory
    virtual ~IntlogExec();

    // from main.cpp, factored commodity interface
    int query(Term tquery, kstr_list *var_ids, Term *v, int nv);
    int runquery(const char *src);

    // start/continue a query
    virtual int query(const Clause * = 0);

    // make a call on a term
    int call(Term);

    // as before, copying at interface
    int call(Term, IntlogExec *);

    // as before, copying at interface and keeping retry info
    int gcall(Term, IntlogExec *);

    // standard findall implementation
    int findall(Term toCollect, Term toCall, Term Result);

    // like findall, but check for var usage
    int bagof(Term toCollect, Term toCall, Term Result);

    // like bagof, but collapse duplicate terms (and sort?)
    int setof(Term toCollect, Term toCall, Term Result);

    // build a clause to be added to DB
    Clause *make_clause(Term);

    // return which CallData is currently executed
    const CallData *on_call() const;

    // builtin context sensitive data
    BltinData* get_btdata() const;
    void set_btdata(BltinData* ptr);

    // return current database
    DbIntlog* get_db() const{
        return db;
    }

    // change current database
    void set_db(DbIntlog* d) {
        db = d;
    }

    // evalute current term value
    Term eval_term(Term t) const {
        return t.type(f_VAR)? tval(Var(t)) : t;
    }

    // build a copy of term dereferencing variables
    Term copy(Term t) const;

    // insert in temporary store list
    Term save(Term);
    Clause* save(Clause *);
    BltinData* save(BltinData *);

    // show term on current output stream
    ostream& write(Term, ostream &, stkpos = STKNULL) const;
    ostream& display(Term, ostream &, stkpos = STKNULL);

    // keep write mode
    enum writeModeBits {
        UseOpDecl	= 0x01,
        NameVars	= 0x01,
        NumberVars	= 0x02
    };
    static int writeMode;

    // read from current input stream
    Term readTerm();

    // variable value access
    Term value(Var, stkpos = STKNULL) const;	// copy full value
    Term tval(Var rv, stkpos posenv = STKNULL) const; // only first reference

    // attempt unification of terms t1 & t2 (top env for both term)
    int unify(Term t1, Term t2);

    // as above but use absolute var position for second term
    int unify_abs(Term relPos, Term absPos);

    // check for generated vars
    int unify_gen(Term inStack, Term withVars);

    // fetch clauses from file
    int use_file(kstring);
    int reuse_file(kstring);
    void use_file();

    // search for first/next match in DB
    int find_match(Term, DbEntryIter &, DbIntlog * = 0);

    // if not already exist & requested, create
    ProofTracer* tracer(int = 1);

    // display builtin error (could hook into debugger...)
    virtual int BtErr(int nCodeMsg, ...);

    // display status as required
    enum statusmode {
        Base	= 0x01,
        Proof	= 0x02,
        Vars	= 0x04,
        Trail	= 0x10,
        DbHash	= 0x20,
        All		= Base|Proof|Vars|Trail|DbHash
    };
    void showstatus(ostream &, statusmode = All) const;

protected:

    DbIntlog*	db;	// clauses database

    ProofStack*	ps;	// proof evaluation stack
    BindStack*	vs;	// variables binding stack
    TrailStack*	ts;	// tracking of bindings

    ProofTracer*	tr;	// debugging tracer (0 if disabled)

    CallData*	cc;	// current call

    slist		tmpt;	// hold temporary terms/Clauses

    int		unify(stkpos, TermArgs, stkpos, TermArgs, int);
    void	unbind(stkpos);
    void	query_fail(stkpos);

    Term	copy_term(Term, stkpos) const;

    int		show_solution(const Clause*, stkpos) const;

    void	chktracer();

    // build a tracer at request
    virtual ProofTracer *make_tracer();

    // build a clause (can add debug info...)
    virtual Clause *build_clause(IntlogIStream *is);

    // default prompt interface
    virtual void user_prompt() const;
    virtual void no_solution() const;
    virtual void yes_solution() const;

    // recursive display helpers
    ostream& writeVar(Var v, ostream &s, stkpos p) const;
    ostream& writeStruct(Term t, ostream &s, stkpos p) const;
    ostream& writeList(Term t, ostream &s, stkpos p) const;

    // execution break (every ncycle get called to sense external status request)
    unsigned ncycle;
    virtual void sighandler(){}

    friend class ProofTracer;
    friend class ProofStatus;
};

/////////////////////////////
// term writing without copy
//
class IAFX_API teWrite
{
public:
    teWrite(Term toWrite, IntlogExec *pExec = 0, stkpos stkPos = STKNULL) {
        m_toWrite = toWrite;
        m_pExec = pExec;
        m_stkPos = stkPos;
    }

    Term m_toWrite;
    IntlogExec* m_pExec;
    stkpos m_stkPos;
};

IAFX_API ostream& operator<<(ostream& s, teWrite& data);
IAFX_API ostream& operator<<(ostream& s, Term t);

#endif
