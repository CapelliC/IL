
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


#ifndef DBINTLOG_H_
#define DBINTLOG_H_

///////////////////////////////////////////////////////////////////
// IntLog database
//
// an object oriented approach, that implement
//  encapsulation,
//  multiple inheritance,
//  polymorphism
//
// contains
//  type names (other databases)
//      from which inherit
//  procedures (clauses with matched functor/arity)
//      that can be declared exported/imported
//
// links between procedures, used at evaluation time,
//  are resolved when clauses are added to database
//  contains Clauses, VTables or ExtRef (external references)
//
// a VTable is a set of pair DbTag/EntryPtr
//  (as DbTag use the data pointer)
//
// an exported/imported procedure add a VTable at declaration point
//  while references are added when no ambiguity arise
///////////////////////////////////////////////////////////////////

#include "iafx.h"
#include "clause.h"
#include "builtin.h"

// storage classes
class e_DbList;
class DbIntlog;
class DbEntry;
class DbList;
class DbListSeek;
class DbVTable;

// iterators for external access
class DbEntryIter;
class DbIdArityIter;
class DbInherIter;

// debugging (external declared)
class ProofTracer;
class DbDisplayer;

//////////////////////////////////////
// linked entries for procedure(s)
//  navigate virtual tables on succ()
//
class IAFX_API e_DbList : public e_slist {
public:

    // get current procedure clause
    const Clause* get() const {
        assert(type == tClause || type == tLocData || type == tShared || type == tBuiltin);
        return clause;
    }

    // fetch next element in procedure list
    const e_DbList* succ(const DbIntlog *dbs) const {
        assert(this);
        return next()->fix_clause(dbs);
    }

    ostream& Display(ostream& s, int mode) const;

private:

    enum eType {
        tClause,    // local clause definition
        tVTable,    // virtual table
        tExtRef,    // external chaining
        tLocData,   // asserted (local data)
        tShared,    // shared dynamic data
        tBuiltin    // builtin pointer
    } type;

    union {
        Clause*         clause; // instance data
        DbVTable*       table;  // virtual links
        const DbEntry*  extref; // unambiguos link
        BuiltIn*        bltin;  // system defined builtin
    };

    e_DbList(Clause *c) {
        type = tClause;
        clause = c;
    }

    e_DbList(DbVTable *t) {
        type = tVTable;
        table = t;
    }

    e_DbList(const DbEntry *e) {
        type = tExtRef;
        extref = e;
    }

    e_DbList(BuiltIn *bt) {
        type = tBuiltin;
        bltin = bt;
    }

    ~e_DbList();

    // search in VTables chainings
    const e_DbList *fix_clause(const DbIntlog *) const;
    const e_DbList* searchVTbl(const DbIntlog *) const;
    const e_DbList *next() const {
        return static_cast<const e_DbList *>(e_slist::next);
    }


    friend class DbEntry;
    friend class DbEntryIter;
    friend class DbIdArityIter;
    friend class DbList;
    friend class DbListSeek;
    friend class DbIntlog;
    friend class DbDisplayer;
};

//////////////////////////////////////
// a list of Clauses, VTables, ExtRef
//
class IAFX_API DbList : slist {
    int match(e_slist *, void *) const;

    e_DbList *seekptr(void* e) const {
        return static_cast<e_DbList *>(slist::seekptr(e));
    }

    unsigned seek(void *e) const {
        return slist::seek(e);
    }

    ostream& Display(ostream& s, int mode) const;

    friend class DbIntlog;
    friend class DbEntry;
    friend class DbDisplayer;
};

///////////////////////////
// a procedure entry point
//
class DbEntry : e_hashtable {
public:

    DbEntry(kstring f, int a) {
        funct = f;
        arity = a;
        vProp = local;
    }
    ~DbEntry() override;

    e_DbList *get_first() const {
        return static_cast<e_DbList*>(entries.get_first());
    }

    // inter DB entries scope properties
    enum scopemode {
        local       = 0x01,
        exported    = 0x02,
        import      = 0x04,
        dynamic     = 0x08
    };

    ostream& Display(ostream& s, int mode) const;

private:

    const char* getstr() const override {
        return funct;
    }

    // proc identification
    kstring funct;
    int arity;

    // actual scope tag for procedure
    scopemode vProp;

    // virtual table control (one for entry!)
    DbVTable* addVTbl(unsigned = unsigned(-1));
    void addExtRef(DbEntry*);

    // e_DbList(s) list
    DbList entries;

    friend class DbIntlog;
    friend class DbDisplayer;
};

/////////////////////
// a database object
//
class IAFX_API DbIntlog : protected hashtable, protected e_slist {
public:

    // construct/destroy a database
    DbIntlog();
    DbIntlog(DbIntlog *);
    DbIntlog(kstring, DbIntlog *);
    ~DbIntlog() override {}

    ///////// LOCAL STORE CONTROL ///////

    // create entry for clause
    virtual void Add(Clause *c, int first = 0, DbIntlog * = nullptr);

    // return matching entry point, inserting if needed
    DbEntry *GetEntryRef(Term);

    // get first clause in procedure list entry
    const e_DbList* StartProc(const DbEntry *dbe) const {
    #if 0 //def _DEBUG
        CCP tstring = dbe->funct;
    #endif

        return dbe->get_first()->fix_clause(this);
    }


    // delete matched by name/arity
    virtual int Del(DbEntryIter&);

    // search first clause matching term
    int Search(Term, DbEntryIter &, DbIntlog *);

    // release temporary invalidated clauses
    virtual void ClearStatus(DbIntlog* = nullptr);

    // remove entries indexed by file id
    DbIntlog *RemoveFileClauses(kstring fileId, slistvptr &updated);

    // and reinsert at named position
    bool RestoreClause(Clause *);

    ////////// BUILTINS CONTROL //////////

    // insert all builtins fron a table
    void addtable(BuiltIn*, int);

    // check if already in table
    BuiltIn* is_builtin(CCP, int) const;

    // check term is callable as builtin
    BuiltIn* is_builtin(Term) const;

    // keep root Builtin database
    static DbIntlog *_pGlobalBuiltins;

protected:

    // track deleted procs
    slist m_deleted;

    // override: compare kstring and arity
    int keymatch(const e_hashtable *, const e_hashtable *) const override;

    DbEntry* isin(DbEntry *e) const;
    DbEntry* isin(DbEntry &e) const;

    ///////// INTERFACE CONTROL //////////

public:

    // return DB identifier
    kstring GetId() const {
        return m_name;
    }


    // enable to change identifier
    void SetId(kstring);

    // get father
    DbIntlog *GetFather() const {
        return m_father;
    }

    // start definition of new local interface
    DbIntlog *BeginInterface(kstring);

    // terminate this interface, back to 'father'
    DbIntlog *EndInterface();

    // on qualified name, select direct DB
    DbIntlog *FixPathName(Term *) const;

    // look in local types list
    DbIntlog *IsLocalInterface(kstring) const;

    // make an entry, changing her properties as required
    int ChangeEntryProperty(kstring, int, DbEntry::scopemode);

    // open an inherited interface
    int InheritInterface(DbIntlog *);

    // check for inheritance relationship
    int IsA(const DbIntlog *) const;

    // get info about properties entries
    struct EntryWithProp : e_slist {
        kstring id;
        int arity;
    };
    void EntriesWithProperty(DbEntry::scopemode, slist &) const;

    // display with some option
    enum {
        Recurs  = 0x01,
        Header  = 0x02,
        Entries = 0x04,
        PropInt = 0x08,
        Arity   = 0x10,
        Clauses = 0x20,
        Indent  = 0x40,

        Base    = Recurs|Header|Entries|PropInt|Arity,
        All     = Base|Clauses
    };
    virtual ostream& Display(ostream& s, int mode = All) const;

protected:

    // DB identifier (local unique)
    kstring m_name;

    // locally defined types (DbIntlog)
    slist m_types;

    // in which DB definition of this occurs
    DbIntlog* m_father;

    // inherited interfaces
    slistvptr m_inherited;

    // search entries in inherited interfaces
    DbEntry* find_inherited_entry(DbEntry *, DbInherIter &) const;

    // seek entry (if !found, insert)
    DbEntry* make_entry(kstring, int);

    // check if imported/exported from some inherited
    void check_inherited_entries(DbEntry *);

    friend class DbInherIter;
    friend class DbIdArityIter;
    friend class DbDisplayer;
};

inline BuiltIn* DbIntlog::is_builtin(Term t) const {
    return is_builtin(t.get_funct(), t.get_arity());
}

//////////////////////////////
// a virtual table
//  keep DB tag and pointers
//
class DbVTable : public slist {
public:
    struct e_VTable : public e_slist {
        DbIntlog*   db;     // tag owner DB
        DbEntry*    first;  // pointer to first entry
    };
    void add(DbIntlog *, DbEntry *);
    ostream& Display(ostream& s, int mode) const;
};

/////////////////////////////////////////////////////////////////
// iterators to gain access to subsequent matching clauses in DB
//  initialize by call DbIntlog::Search()
//
class DbEntryIter {
public:
    Clause* next();
    Clause* curr() const {
        return pcp? pcp->clause : nullptr;
    }

    DbIntlog* owner() const {
        return own;
    }


private:
    e_DbList *pcc;
    e_DbList *pcp;
    DbIntlog *own;
    DbIntlog *mod;

    friend class DbIntlog;
};

class DbIdArityIter : hashtable_iter {
public:
    DbIdArityIter(DbIntlog *, kstring, int);
    Clause *next();

private:
    int arity;
    DbEntry *dbe;
    e_DbList *pcp;
    DbIntlog *own;

    friend class DbIntlog;
};

/////////////////////////////
// iterate inherited members
//
class IAFX_API DbInherIter : slistvptr_iter {
public:
    DbInherIter(const DbIntlog *db) : slistvptr_iter(db->m_inherited) {
    }

    DbIntlog* next() {
        return reinterpret_cast<DbIntlog*>(slistvptr_iter::next());
    }

private:
    friend class DbIntlog;
};

#endif
