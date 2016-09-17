/*
    IL : Prolog interpreter
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

#include "iafx.h"
#include "term.h"
#include "fastree.h"

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

    // init state and check semantic correctness
    Clause(Term tData, kstr_list *vars, DbIntlog *dbOwn, kstring fileId = MSR_NULL, SrcPosTree * = 0);

    // release allocated memory and term image
    ~Clause();

    // verify construction process
    int is_ok() const {
        return !get_image().type(f_NOTERM);
    }

    // qualifiers
    int	is_rule() const {
        return image.is_rule();
    }

    int is_fact() const {
        return !image.is_rule() && !image.is_query() && !image.is_command();
    }

    int	is_query() const {
        return image.is_query();
    }

    int	is_command() const {
        return image.is_command();
    }


    // accessors
    Term get_image() const {
        return image;
    }

    Term get_head() const {
        ASSERT(!is_query());
        return image.is_rule()? image.getarg(0) : image;
    }

    kstring get_source() const {
        return src;
    }

    void set_source(kstring newsrc) {
        src = newsrc;
    }

    TermArgs h_args() const {
        ASSERT(!is_query());
        return argp;
    }

    int h_arity() const {
        ASSERT(!is_query());
        return arity;
    }


    CallData* get_body() const {
        return body;
    }

    unsigned get_nvars() const {
        return nvars;
    }

    kstring get_varid(Var) const;

    // DB owner (where stored) control
    DbIntlog* get_db() const {
        return db;
    }

    void set_db(DbIntlog *owner) {
        db = owner;
    }

    // release local pointer to term
    void no_image() {
        image = Term(f_NOTERM);
    }

    // if no variables names, set number
    void set_nvars(unsigned nv) {
        ASSERT(varids == 0);
        nvars = nv;
    }

    // find term image from calldata pointer
    Term cd_image(const CallData*) const;

    // file source position storage
    SrcPosTree* GetSrcPosTree() const {
        return spt;
    }


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
    Term val() const {
        return owner->cd_image(this);
    }

    TermArgs args() const {
        return argp;
    }


    // source position
    NodeIndex srcpos() const {
        return spos;
    }

    unsigned row() const;
    unsigned col() const;
    unsigned len() const;

    const Clause* get_clause() const {
        return owner;
    }

    enum cdtype get_type() const {
        return type;
    }

    CallData* get_orelse() const {
        ASSERT(type == DISJUNCT);
        return orelse;
    }

    BuiltIn* get_builtin() const {
        ASSERT(type == BUILTIN);
        return bltin;
    }

    DbEntry* get_dbe() const {
        ASSERT(type == DBPRED);
        return entry;
    }


    // return direct right hand brother
    CallData* next() const {
        return link;
    }

    // true if last right hand brother
    int is_last() const {
        return link? 0 : 1;
    }

private:

    CallData(Clause *c) {
        owner = c;
        link = 0;
        spos = INVALID_NODE;
    }

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

#endif
