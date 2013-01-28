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

#ifdef _DEBUG
#define inline
#endif

inline DbIntlog::~DbIntlog()
{
}
inline const e_DbList *e_DbList::next() const
{
	return (const e_DbList *)e_slist::next;
}

inline e_DbList::e_DbList(Clause *c)
{
	type = tClause;
	clause = c;
}
inline e_DbList::e_DbList(DbVTable *t)
{
	type = tVTable;
	table = t;
}
inline e_DbList::e_DbList(const DbEntry *e)
{
	type = tExtRef;
	extref = e;
}
inline e_DbList::e_DbList(BuiltIn *bt)
{
	type = tBuiltin;
	bltin = bt;
}

inline e_DbList *DbList::seekptr(void* e) const
{
	return (e_DbList *)slist::seekptr(e);
}
inline unsigned DbList::seek(void *e) const
{
	return slist::seek(e);
}

inline DbEntry* DbIntlog::isin(DbEntry *e) const
{
	return (DbEntry*)hashtable::isin((e_hashtable*)e);
}
inline DbEntry* DbIntlog::isin(DbEntry &e) const
{
	return (DbEntry*)hashtable::isin((e_hashtable*)&e);
}

inline DbInherIter::DbInherIter(const DbIntlog *db)
: slistvptr_iter(db->m_inherited)
{}

inline DbIntlog* DbInherIter::next()
{
	return (DbIntlog*)slistvptr_iter::next();
}

inline Clause* DbEntryIter::curr() const
{
	return pcp? pcp->clause : 0;
}
inline DbIntlog* DbEntryIter::owner() const
{
	return own;
}


///////////////////////////////////////
// return first clause/VTable in chain
//
inline e_DbList *DbEntry::get_first() const
{
	return (e_DbList*)entries.get_first();
}

////////////////////////////////////////
// fetch next element in procedure list
//
inline const e_DbList *e_DbList::succ(const DbIntlog *dbs) const
{
	ASSERT(this);
	return next()->fix_clause(dbs);
}

////////////////////////////////
// get current procedure clause
//
inline const Clause* e_DbList::get() const
{
	ASSERT(type == tClause || type == tLocData || type == tShared || type == tBuiltin);
	return clause;
}

inline const char* DbEntry::getstr() const
{
	return funct;
}


////////////////////////////////////////////
// get first clause in procedure list entry
//
inline const e_DbList* DbIntlog::StartProc(const DbEntry *dbe) const
{
#ifdef _DEBUG
	CCP tstring = dbe->funct;
#endif

	return dbe->get_first()->fix_clause(this);
}
/////////////////////////
// return DB identifier
//
inline kstring DbIntlog::GetId() const
{
	return m_name;
}

inline DbIntlog *DbIntlog::GetFather() const
{
	return m_father;
}

inline DbEntry::DbEntry(kstring f, int a)
{
	funct = f;
	arity = a;
	vProp = local;
}
