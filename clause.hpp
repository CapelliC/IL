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

inline int Clause::is_fact() const
{
	return !image.is_rule() && !image.is_query() && !image.is_command();
}
inline int Clause::is_rule() const
{
	return image.is_rule();
}
inline int Clause::is_query() const
{
	return image.is_query();
}
inline int Clause::is_command() const
{
	return image.is_command();
}
inline Term Clause::get_head() const
{
	ASSERT(!is_query());
	return image.is_rule()? image.getarg(0) : image;
}
inline Term Clause::get_image() const
{
	return image;
}
inline CallData* Clause::get_body() const
{
	return body;
}
inline kstring Clause::get_source() const
{
	return src;
}
inline void Clause::set_source(kstring newsrc)
{
	src = newsrc;
}
inline TermArgs Clause::h_args() const
{
	ASSERT(!is_query());
	return argp;
}
inline int Clause::h_arity() const
{
	ASSERT(!is_query());
	return arity;
}
inline unsigned Clause::get_nvars() const
{
	return nvars;
}
inline void Clause::no_image()
{
	image = Term(f_NOTERM);
}
inline DbIntlog* Clause::get_db() const
{
	return db;
}
inline void Clause::set_db(DbIntlog *owner)
{
	db = owner;
}
inline void Clause::set_nvars(unsigned nv)
{
	ASSERT(varids == 0);
	nvars = nv;
}
inline int Clause::is_ok() const
{
	return !get_image().type(f_NOTERM);
}

/////// CALLDATA ///////

inline Term CallData::val() const
{
	return owner->cd_image(this);
}
inline int CallData::is_last() const
{
	return link? 0 : 1;
}
inline CallData *CallData::next() const
{
	return link;
}
inline CallData::cdtype CallData::get_type() const
{
	return type;
}
inline CallData* CallData::get_orelse() const
{
	ASSERT(type == DISJUNCT);
	return orelse;
}
inline BuiltIn* CallData::get_builtin() const
{
	ASSERT(type == BUILTIN);
	return bltin;
}
inline DbEntry* CallData::get_dbe() const
{
	ASSERT(type == DBPRED);
	return entry;
}
inline const Clause* CallData::get_clause() const
{
	return owner;
}
inline TermArgs CallData::args() const
{
	return argp;
}

inline SrcPosTree* Clause::GetSrcPosTree() const
{
	return spt;
}

inline CallData::CallData(Clause *c)
{
	owner = c;
	link = 0;
	spos = INVALID_NODE;
}
inline NodeIndex CallData::srcpos() const
{
	return spos;
}
