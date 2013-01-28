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

#ifndef _CLAUSE_H_
#define _CLAUSE_H_

//----------------------------------------------------------------
// executable terms, ready for query
//	resolve at compile time references to builtins and DB entries
//	see DBINTLOG for details
//----------------------------------------------------------------

#ifndef _IAFX_H_
#include "iafx.h"
#endif

#ifndef _TERM_H_
#include "term.h"
#endif

#ifndef _FASTREE_H_
#include "fastree.h"
#endif

// local classes
class Clause;
class CallData;

// external definitions
class IntlogExec;
class DbIntlog;
class DbEntry;
class SrcPosTree;
struct BuiltIn;

class IAFX_API Clause
{
public:

    // init state and  check semantic correctness
    Clause(Term tData, kstr_list *vars, DbIntlog *dbOwn, kstring fileId = MSR_NULL, SrcPosTree * = 0);

    // release allocated memory and term image
    ~Clause();

    // verify construction process
    int is_ok() const;

    // qualifiers
    int	is_rule() const;
    int is_fact() const;
    int	is_query() const;
    int	is_command() const;

    // accessors
    Term get_image() const;
    Term get_head() const;

    kstring get_source() const;
    void set_source(kstring newsrc);

    TermArgs h_args() const;
    int h_arity() const;

    CallData* get_body() const;
    unsigned get_nvars() const;
    kstring get_varid(Var) const;

    // DB owner (where stored) control
    DbIntlog* get_db() const;
    void set_db(DbIntlog *dbOwn);

    // release local pointer to term
    void no_image();

    // if no variables names, set number
    void set_nvars(unsigned);

    // find term image from calldata pointer
    Term cd_image(const CallData*) const;

    // file source position storage
    SrcPosTree* GetSrcPosTree() const;

private:
    Term		image;	// parsed term (semantically correct!)

    unsigned	nvars;	// number of logical variables
    kstring*	varids;	// keep variables names

    TermArgs	argp;	// head arguments
    int			arity;	// head arity

    CallData*	body;	// clause body (0 if fact)

    DbIntlog*	db;		// 'owner' DB

    kstring		src;	// source file (null if asserted)
    SrcPosTree*	spt;	// with optional tree position

    // body elements constructor
    CallData* makebody(Term *tData, DbIntlog* pDb, SrcPosTree *pSrc, NodeIndex nIndex);

    // check for builtin in table
    BuiltIn* builtin(Term) const;

    // display error
    int err_msg(int, const char* = 0) const;

    // check for ok functor (identifiable)
    int check_funct(Term) const;
};

//-----------------------
// ready to use for call
//
class IAFX_API CallData
{
public:
    enum cdtype {
        DBPRED,			// DB stored procedure entry
        BUILTIN,		// system defined procedure
        CUT,			// cut (not implemented ancestor cutting)
        DISJUNCT,		// disjunction
        TAILREC			// tail recursion optimization
    };

    // generic accessors
    Term val() const;
    TermArgs args() const;

    // source position
    NodeIndex srcpos() const;
    unsigned row() const;
    unsigned col() const;
    unsigned len() const;

    const Clause* get_clause() const;
    enum cdtype get_type() const;
    CallData* get_orelse() const;
    BuiltIn* get_builtin() const;
    DbEntry* get_dbe() const;

    // return direct right hand brother
    CallData* next() const;

    // true if last right hand brother
    int is_last() const;

private:
    CallData(Clause *c);
    ~CallData();

    TermArgs	argp;	// fast access to arguments

    CallData*	link;	// right hand brother in clause body
    Clause*		owner;	// back link to fast access body constructor

    cdtype		type;	// union type selector
    union {
        DbEntry*	entry;	// if DBPRED
        BuiltIn*	bltin;	// if BUILTIN
        CallData*	orelse;	// if DISJUNCT
    };

    NodeIndex	spos;	// keep source position index (default to none)

    friend class Clause;
};

#ifndef _DEBUG
#include "clause.hpp"
#endif

#endif
