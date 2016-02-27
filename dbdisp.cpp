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

///////////////////////////
// IntLog database Display
///////////////////////////

#include "stdafx.h"
#include "dbintlog.h"
#include "query.h"

///////// DISPLAY /////////

ostream& operator<<(ostream& s, kstring k)
{
	return s << (unsigned(k) != MSR_NULL? CCP(k) : "\"\"");
}

static int indent_cnt, indent_diff = 2;
static ostream& indent(int mode, ostream &s)
{
	if (mode & DbIntlog::Indent)
		for (int i = 0; i < indent_cnt; i++)
			s << ' ';
	return s;
}
inline void inc()
{
	indent_cnt += indent_diff;
}
inline void dec()
{
	indent_cnt -= indent_diff;
}
inline ostream& inc(int mode, ostream &s)
{
	indent_cnt += indent_diff;
	return indent(mode, s);
}
inline ostream& dec(int mode, ostream &s)
{
	indent_cnt -= indent_diff;
	return indent(mode, s);
}

ostream& e_DbList::Display(ostream& s, int mode) const
{
	static const char* tStr[5] = {
		"Clause", "VTable", "ExtRef", "LocData", "Shared"
	};
	indent(mode, s) << tStr[type];

	int cl = 1;
	if (type == tVTable)
	{
		table->Display(s << '\t', mode);
		cl = 0;
	}
	if (type == tExtRef)
	{
		s << '\t' << extref;
		cl = 0;
	}

	if (cl && (mode & DbIntlog::Clauses) && clause)
		s << '\t' << clause->get_image() << '[' << clause->get_source() << ']';

	return s;
}
ostream& DbVTable::Display(ostream& s, int) const
{
	slist_iter it(this);

	e_VTable *e;
	while ((e = (e_VTable *)it.next()) != 0)
		s << '[' << e->db << ',' << e->first << ']';

	return s;
}
ostream& DbList::Display(ostream& s, int mode) const
{
	slist_iter it(this);
	e_DbList *e;

	while ((e = (e_DbList *)it.next()) != 0)
		e->Display(s, mode) << endl;

	return s;
}
ostream& DbEntry::Display(ostream& s, int mode) const
{
	indent(mode, s) << funct;

	if (mode & DbIntlog::Arity)
		s << '/' << arity;

	s << ' ' << this;

	if (mode & DbIntlog::PropInt)
	{
		s << '(';
		if (vProp == local)		s << "Local ";
		if (vProp == exported)	s << "Export ";
		if (vProp == import)	s << "Import ";
		if (vProp == dynamic)	s << "Dynamic ";
		s << ')';
	}

	s << endl;

	if (mode & DbIntlog::Entries)
	{
		inc();
		entries.Display(s, mode);
		dec();
	}

	return s;
}

ostream& DbIntlog::Display(ostream& s, int mode) const
{
	indent(mode, s) << '{' << m_name << ' ' << this;
	if (mode & Header)
		s << " #entries " << get_nentries() << " #size " << get_vsize();
	s << endl;

	inc();

	if (mode & PropInt)
	{
		indent(mode, s) << "Inherits:";

		DbInherIter it(this);
		DbIntlog *pDb;
		while ((pDb = it.next()) != 0)
			s << ' ' << pDb->m_name;

		s << endl;
	}

	if (mode & Entries)
	{
		indent(mode, s) << "Entries:" << endl;

		hashtable_iter it(this);
		DbEntry *e;

		while ((e = (DbEntry*)it.next()) != 0)
			e->Display(s, mode);
	}

	if (m_types.numel() > 0)
	{
		indent(mode, s) << "Local Types DB" << endl;

		slist_iter dbit(m_types);
		const DbIntlog *dbt;
		while ((dbt = (const DbIntlog *)dbit.next()) != 0)
			dbt->Display(s, mode) << endl;
	}

	if (mode & Recurs)
	{
		DbInherIter it(this);
		DbIntlog *pDb;
		while ((pDb = it.next()) != 0)
			pDb->Display(s, mode);

		s << endl;
	}

	return dec(mode, s) << '}' << endl;
}
