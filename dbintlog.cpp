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

///////////////////
// IntLog database
///////////////////

#include "stdafx.h"
#include "dbintlog.h"
#include "builtin.h"
#include "eng.h"

///////////////////////////////////////
// helper class to seek data in DbList
//
class DbListSeek
{
public:
    DbListSeek(e_DbList::eType t, void *d = 0)
    {
        type = t;
        dptr = d;
    }

private:
    e_DbList::eType type;
    void *dptr;
    friend class DbList;
};

#define DIMBASE 13

DbIntlog::DbIntlog() : hashtable(DIMBASE)
{
    m_name = kstring(MSR_NULL);
    m_father = 0;
}
DbIntlog::DbIntlog(DbIntlog *father) : hashtable(DIMBASE)
{
    m_name = kstring(MSR_NULL);
    m_father = father;
}
DbIntlog::DbIntlog(kstring name, DbIntlog *father) : hashtable(DIMBASE)
{
    m_name = name;
    m_father = father;
}

///////////////////////////////////
// get a local procedure reference
//
DbEntry *DbIntlog::GetEntryRef(Term t)
{
    DbEntry *e = make_entry(t.get_funct(), t.get_arity());

#ifdef _DEBUG
    CCP tstring = t.get_funct();
#endif

    check_inherited_entries(e);
    return e;
}

///////////////////////////////////////
// on qualified name, select direct DB
//
DbIntlog *DbIntlog::FixPathName(Term *t) const
{
    DbIntlog *db = (DbIntlog*)this;
    Term r = *t;

    while (db && r.is_expr(Operator::PATHNAME) && r.getarg(0).type(f_ATOM))
    {
        kstring dbid = r.getarg(0).kstr();

        if (!strcmp(dbid, ".."))
            db = db->m_father;
        else
        {
            DbInherIter itdb(db);
            while ((db = itdb.next()) != 0)
                if (db->GetId() == dbid)
                    break;
        }

        if (db)
            r = r.getarg(1);
    }

    *t = r;
    return db;
}

///////////////////////////////////
// storage entry point to database
//
void DbIntlog::Add(Clause *c, int first, DbIntlog *owner)
{
    Term h = c->get_head();
    ASSERT(!h.type(f_NOTERM));
#ifdef _DEBUG
    CCP tstring = h.get_funct();
#endif

    e_DbList *edbl = new e_DbList(c);

    // retrieve or create the entry
    kstring funct = h.get_funct();
    int arity = h.get_arity();
    DbEntry e(funct, arity), *dbe = isin(e);

    // verify location on create
    if (dbe == 0 && owner && owner != this)
        dbe = owner->isin(e);
    if (!dbe)
    {
        dbe = new DbEntry(funct, arity);
        insert(dbe);
    }
    if (dbe->arity == -1)
        dbe->arity = arity;

    DbIntlog *dbwork = this;
    if (dbe->vProp == DbEntry::dynamic)
    {
        if (!owner)
            owner = this;
        dbe = owner->isin(dbe);
        ASSERT(dbe);
        c->set_db(dbwork = owner);
    }
    else if (owner && owner != this)
    {
        edbl->type = e_DbList::tLocData;
        c->set_db(owner);
    }

    if (first)
        dbe->entries.insert(edbl, 0);
    else
    {
        DbListSeek eref(e_DbList::tExtRef);
        DbListSeek vtbl(e_DbList::tVTable);
        unsigned iref = dbe->entries.seek(&eref);
        unsigned itbl = dbe->entries.seek(&vtbl);

        if (iref != SLIST_INVPOS)
            dbe->entries.insert(edbl, iref);
        else
            if (itbl != SLIST_INVPOS)
                dbe->entries.insert(edbl, itbl);
            else
                dbe->entries.append(edbl);
    }

    // close inheritance chainings
    dbwork->check_inherited_entries(dbe);
}

//////////////////////////////////////
// initialize matching terms iterator
//
int DbIntlog::Search(Term t, DbEntryIter &iter, DbIntlog *dbs)
{
#ifdef _DEBUG
    CCP tstring = t.get_funct();
#endif

    DbEntry e = DbEntry(t.get_funct(), t.get_arity()),
            *dbe = isin(e);

    if (!dbe)
    {
        if (dbs == 0 ||
                (dbe = dbs->isin(e)) == 0 ||
                dbe->vProp != DbEntry::dynamic)
        {
            iter.pcc = 0;
            iter.own = iter.mod = 0;
            return 0;
        }
        iter.mod =
		iter.own = dbs;
    }
    else
    {
        iter.mod = this;
        iter.own = dbs;
    }

    // initialize for later search
    iter.pcc = (e_DbList*)dbe->get_first()->fix_clause(dbs);
    iter.pcp = 0;

    return 1;
}

////////////////////////////////////////////////////
// delete matched by name/arity
//	iterator must be initialized by call to Search()
//
int	DbIntlog::Del(DbEntryIter& i)
{
    e_DbList* e = i.pcp;
    if (e)
    {
        // save current DB entry in deleted list
        e_DbList *eNew = new e_DbList(e->clause);
        eNew->type = e->type;
        i.mod->m_deleted.insert(eNew, 0);

        // disable in DB access
        e->clause = 0;
        return 1;
    }

    return 0;
}

////////////////////////////////////
// resync clauses list
//	this invalidate all DbEntryIter
//
void DbIntlog::ClearStatus(DbIntlog *owner)
{
    slist_scan ld(m_deleted);
    e_DbList *edbd;

    while ((edbd = (e_DbList*)ld.next()) != 0)
    {
        Term tk = edbd->clause->get_head();
#ifdef _DEBUG
        CCP tstring = tk.get_funct();
#endif

        DbEntry x(tk.get_funct(), tk.get_arity());
        DbEntry *dbe = isin(x);
        ASSERT(dbe);

        slist_scan tla(dbe->entries);
        e_DbList *edbl;

        while ((edbl = (e_DbList*)tla.next()) != 0)
            if (edbl->clause == 0)
            {
                tla.delitem();
                break;
            }

        if (!owner || edbd->type != e_DbList::tLocData ||
                edbd->clause->get_db() == owner)
            ld.delitem();
    }

    // clear all inherited DBs
    DbInherIter it(this);
    DbIntlog *db;
    while ((db = (DbIntlog *)it.next()) != 0)
        db->ClearStatus(this);
}


//////////////////////////////////////////////
// remove all clauses entries indexed by file
//
DbIntlog *DbIntlog::RemoveFileClauses(kstring fileId, slistvptr &updated)
{
    // if already updated, skip
    if (updated.seek(this) != SLIST_INVPOS)
        return 0;

    hashtable_iter hit(this);
    DbEntry *he;
    DbIntlog *found = 0;

    while ((he = (DbEntry *)hit.next()) != 0)
    {
        slist_iter lit(he->entries);
        e_DbList *e;
        while ((e = (e_DbList*)lit.next()) != 0)
            if (e->type == e_DbList::tClause && e->clause && e->clause->get_source() == fileId)
            {
                e_DbList *eNew = new e_DbList(e->clause);
                eNew->type = e->type;
                m_deleted.insert(eNew, 0);
                e->clause = 0;

                found = this;
            }
    }

    if (!found)
    {
        slist_iter dbit(m_types);
        DbIntlog *dbt;
        while ((dbt = (DbIntlog *)dbit.next()) != 0)
            if ((found = dbt->RemoveFileClauses(fileId, updated)) != 0)
                break;
    }
    else
        updated.append(this);

    return found;
}

//////////////////////////////////
// and reinsert at named position
//
bool DbIntlog::RestoreClause(Clause *pClause)
{
    Add(pClause, 0, 0);
    return TRUE;
}

///////////////////////////////////////
// match elements by kstring and arity
//
int DbIntlog::keymatch(e_hashtable *e1, e_hashtable *e2) const
{
    DbEntry *d1 = (DbEntry*)e1,
            *d2 = (DbEntry*)e2;
    return	d1->funct == d2->funct &&
            (d1->arity == d2->arity || d1->arity == -1 || d2->arity == -1);
}

///////// INTERFACE CONTROL //////////

///////////////////////////////
// enable to change identifier
//
void DbIntlog::SetId(kstring id)
{
    if (MemStoreRef(m_name) == MSR_NULL)
        m_name = id;
}

///////////////////////////////////////////
// start definition of new local interface
//
DbIntlog *DbIntlog::BeginInterface(kstring id)
{
#ifdef _DEBUG
    CCP tstring = id;
#endif

    // check redefinition
    if (IsLocalInterface(id))
        return 0;

    // insert as local type
    DbIntlog *db = GetEngines()->makeDb(id, this);
    m_types.append(db);
    return db;
}

////////////////////////////
// look in local types list
//
DbIntlog *DbIntlog::IsLocalInterface(kstring id) const
{
#ifdef _DEBUG
    CCP tstring = id;
#endif

    slist_iter i(m_types);
    DbIntlog *db;

    while ((db = (DbIntlog*)i.next()) != 0)
        if (db->GetId() == id)
            break;

    return db;
}

//////////////////////////////////////////////
// terminate this interface, back to 'father'
//
DbIntlog *DbIntlog::EndInterface()
{
#ifdef _DEBUG
    CCP ids = m_name;
#endif

    hashtable_iter i(this);

    DbEntry *dbe;
    while ((dbe = (DbEntry*)i.next()) != 0)
        check_inherited_entries(dbe);

    return m_father;
}

///////////////////////////////
// open an inherited interface
//
int DbIntlog::InheritInterface(DbIntlog *db)
{
    if (db == this)
        return 0;

    m_inherited.append(db);

    // check dynamic data
    hashtable_iter id(db);
    DbEntry *dbe;
    while ((dbe = (DbEntry*)id.next()) != 0)
        if (dbe->vProp == DbEntry::dynamic)
        {
            // make a local copy of data
            DbEntry *dbes = make_entry(dbe->funct, dbe->arity);
            dbes->vProp = DbEntry::dynamic;

            // make base class entries point to copied data
            dbe->addVTbl(0)->add(this, dbes);

            const e_DbList *pEntry = dbe->get_first();
            while (pEntry)
            {
                if (	pEntry->type == e_DbList::tClause ||
                        pEntry->type == e_DbList::tShared)
                {
                    // a copy (but avoid deleted clause)
                    e_DbList *pNew = new e_DbList(pEntry->clause);
                    pNew->type = e_DbList::tShared;

                    // tail to copied entries
                    dbes->entries.append(pNew);
                }

                pEntry = pEntry->next();
            }
        }

    return 1;
}

///////////////////////////////////////
// check for inheritance relationship
//
int DbIntlog::IsA(const DbIntlog *dbt) const
{
    if (dbt == this)
        return 1;

    // make a recursive check
    DbInherIter i(this);
    const DbIntlog *db;
    while ((db = i.next()) != 0)
        if (db->IsA(dbt))
            return 1;

    return 0;
}

/////////////////////////////
// search in inheritance list
//
DbEntry* DbIntlog::find_inherited_entry(DbEntry *e, DbInherIter &l) const
{
    DbEntry *dbe;
    DbIntlog *db;

    while ((db = l.next()) != 0)
        if ((dbe = db->isin(e)) != 0)
            return dbe;

    return 0;
}

//////////////////////////////////////////////////
// check if imported/exported from some inherited
//
void DbIntlog::check_inherited_entries(DbEntry *dbe)
{
    DbEntry *ei, *elink = 0;
    DbInherIter l(this);

    while ((ei = find_inherited_entry(dbe, l)) != 0)
    {
        switch (ei->vProp)
        {
        case DbEntry::local:
        case DbEntry::dynamic:
            break;

        case DbEntry::import:
            ei->addVTbl()->add(this, dbe);
            break;

        case DbEntry::exported:
            if (elink == 0)
                dbe->addExtRef(ei);
            else
                elink->addVTbl()->add(this, ei);

            elink = ei;
        }
    }
}

//////////////////////////////////
// seek entry (if !found, insert)
//
DbEntry *DbIntlog::make_entry(kstring f, int arity)
{
    DbEntry e(f, arity), *dbe = isin(&e);

    if (!dbe)
    {
        dbe = new DbEntry(f, arity);
        insert(dbe);
    }
    else if (dbe->arity == -1)
        dbe->arity = arity;

    return dbe;
}

//////////////////////////////////////////////////////
// make an entry, changing her properties as required
//
int DbIntlog::ChangeEntryProperty(kstring sym, int arity, DbEntry::scopemode prop)
{
    DbEntry *e = make_entry(sym, arity);
    if (e)
    {
        check_inherited_entries(e);
        if (e->vProp == DbEntry::local)
        {
            e->vProp = prop;
            if (prop == DbEntry::dynamic)
                e->addVTbl(0);
            return 1;
        }
    }
    return 0;
}

//////////////////////////////////
// list all entries with property
//
void DbIntlog::EntriesWithProperty(DbEntry::scopemode vProp, slist &l) const
{
    if (vProp == DbEntry::local)
    {
        slist_iter i(m_types);
        DbIntlog *db;

        while ((db = (DbIntlog*)i.next()) != 0)
        {
            EntryWithProp *ewp = new EntryWithProp;
            ewp->id = db->GetId();
            ewp->arity = -1;
            l.append(ewp);
        }
    }
    else
    {
        hashtable_iter it(this);
        DbEntry *dbe;

        while ((dbe = (DbEntry*)it.next()) != 0)
            if (dbe->vProp == vProp)
            {
                EntryWithProp *ewp = new EntryWithProp;
                ewp->id = dbe->funct;
                ewp->arity = dbe->arity;
                l.append(ewp);
            }
    }
}

///////////////// DATA STRUCTS //////////////

/////////////////////////////////////
// add a virtual table to references
//
DbVTable* DbEntry::addVTbl(unsigned atpos)
{
    DbListSeek ts(e_DbList::tVTable);
    e_DbList *etbl = (e_DbList *)entries.seekptr(&ts);

    DbVTable *tbl;
    if (!etbl)
    {
        tbl = new DbVTable;
        if (atpos == unsigned(-1))
            entries.append(new e_DbList(tbl));
        else
            entries.insert(new e_DbList(tbl), atpos);
    }
    else
        tbl = etbl->table;

    return tbl;
}

void DbEntry::addExtRef(DbEntry* dbe)
{
    DbListSeek tse(e_DbList::tExtRef, dbe);
    if (entries.seekptr(&tse) == 0)
        entries.append(new e_DbList(dbe));
}

//////////////////////////////////////
// matching conditions
//	use a struct to pass generic data
//
int DbList::match(e_slist *e, void *a) const
{
    DbListSeek *ts = (DbListSeek*)a;
    e_DbList *ebd = (e_DbList*)e;

    return	ts->type == ebd->type &&
            (ts->dptr == 0 || (void*)ebd->extref == ts->dptr);
}

////////////////////////////////////
// append a virtual reference entry
//
void DbVTable::add(DbIntlog *db, DbEntry *ref)
{
    e_VTable *e;
    slist_iter i(this);

    // check to avoid duplicates
    while ((e = (e_VTable *)i.next()) != 0)
        if (e->db == db && e->first == ref)
            return;

    e = new e_VTable;
    e->db = db;
    e->first = ref;
    append(e);
}

///////////////////////
// search in chainings
//
const e_DbList* e_DbList::fix_clause(const DbIntlog *dbs) const
{
    const e_DbList *r = this;

    while (r)
        switch (r->type)
        {
        case tVTable:
            r = r->searchVTbl(dbs);
            break;
        case tExtRef:
            r = r->extref->get_first();
            break;
        case tClause:
        case tShared:
            if (r->clause)	// check if retracted
                return r;
            r = r->next();
            break;
        case tBuiltin:
            return r;
        case tLocData:
            if (r->clause && r->clause->get_db() == dbs)
                return r;
            r = r->next();
        }

    return 0;
}

////////////////////////
// search virtual table
//
const e_DbList* e_DbList::searchVTbl(const DbIntlog *dbs) const
{
    ASSERT(type == tVTable);

    slist_iter i(table);
    DbVTable::e_VTable *et;

    while ((et = (DbVTable::e_VTable *)i.next()) != 0)
        if (dbs->IsA(et->db))
            return et->first->get_first();

    return next();
}

e_DbList::~e_DbList()
{
    if (type == tClause || type == tLocData)
        delete clause;
    else if (type == tVTable)
        delete table;
}

//////////////////////////////////////////
///////////////// ITERATORs //////////////
//////////////////////////////////////////

////////////////////////////////////
// iterator for all matched clauses
//
Clause* DbEntryIter::next()
{
    if (pcc)
    {
        Clause *c = (pcp = pcc)->clause;
        pcc = (e_DbList*)pcc->next()->fix_clause(own);
        return c;
    }

    return 0;
}

DbIdArityIter::DbIdArityIter(DbIntlog *db, kstring funct, int a)
    : hashtable_iter(db, funct)
{
    DbEntry x(funct, arity = a);
    dbe = db->isin(x);
    pcp = dbe? (e_DbList*)dbe->get_first()->fix_clause(own = db) : 0;
}

Clause* DbIdArityIter::next()
{
    Clause *c = 0;
    if (dbe)
    {
        if (pcp)
        {
            c = pcp->clause;
            pcp = (e_DbList*)pcp->next()->fix_clause(own);
        }
        else
            if (arity == -1)
            {}
    }
    return c;
}
