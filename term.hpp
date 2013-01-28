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

// dummy (no init)
inline Term::Term()
{
}

// dummy (like copy)
inline Term::Term(TermData d)
{
	m_data = d;
}

// kstring selector
inline Term::Term(Atom a)
{
	m_data = f_ATOM | (a & ~MaskFlags);
}

// short integer
inline Term::Term(Int i)
{
	m_data = f_INT | (i & ~MaskFlags);
}

// variable offset
inline Term::Term(Var v)
{
	m_data = f_VAR | (v & ~MaskFlags);
}

// set to empty or invalid
inline void Term::NoTerm()
{
	m_data = f_NOTERM;
}

inline Term::operator Var() const
{
	ASSERT(type(f_VAR));
	return Var(ivalue());
}
inline Term::operator Int() const
{
	ASSERT(type(f_INT));
	return Int(ivalue());
}
/*
inline Term::operator kstring() const
{
	ASSERT(type(f_ATOM));
	return kstring(ivalue());
}
*/
inline Term::operator CCP() const
{
	ASSERT(type(f_ATOM));
	return kstring(ivalue());
}
inline kstring Term::kstr() const
{
	ASSERT(type(f_ATOM));
	return kstring(ivalue());
}

inline Term::operator TermData() const
{
	return m_data;
}

inline unsigned long Term::type() const
{
	return m_data & MaskFlags;
}
inline unsigned Term::ivalue() const
{
	return unsigned(m_data & ~MaskFlags);
}
inline int Term::type(unsigned long f) const
{
	return (m_data & f)? 1 : 0;
}

// true if NULL list (only!)
inline int Term::LNULL() const
{
	ASSERT(type(f_LIST));
	return m_data == ListNULL;
}

inline int Term::is_expr(Operator::opCodeTag opTag, int arity) const
{
	return int(get_funct()) == opTag && get_arity() == arity;
}

// true if '?-' operator expression
inline int Term::is_query() const
{
	return is_expr(Operator::QUERY, 1);
}

// true if ':-' operator expression
inline int Term::is_rule() const
{
	return is_expr(Operator::RULE);
}

// true if ';' operator expression
inline int Term::is_or() const
{
	return is_expr(Operator::OR);
}

// true if ',' operator expression
inline int Term::is_and() const
{
	return is_expr(Operator::AND);
}

// true if ':-' unary operator expression
inline int Term::is_command() const
{
	return is_expr(Operator::RULEUNARY, 1);
}

// true if atom '!'
inline int Term::is_cut() const
{
    //return type(f_ATOM) && unsigned(get_funct()) == KSTRING_CUT;
    return type(f_ATOM) && int(get_funct()) == KSTRING_CUT;
}

//////////////////////
// SysData interface
//
inline SysData::~SysData()
{
}

inline int SysData::delete_able() const
{
	return 0;
}
inline int SysData::copy_able() const
{
	return 0;
}
inline Term SysData::copy() const
{
	return Int(0);
}
inline int SysData::istype(const char *key) const
{
	return key == m_key.id;
}
inline CCP SysData::gettype() const
{
	return m_key.id;
}
inline void SysData::show(ostream &s) const
{
	s << m_key.id;
}
inline int SysData::show_able() const
{
	return 1;
}
inline int SysData::unify(Term t) const
{
	return SysDataPtr(t)->gettype() == m_key.id;
}
inline ostream& SysData::save(ostream& s)
{
	return s;
}
inline istream& SysData::load(istream& s)
{
	return s;
}

/////////////////////
// list data hiding
//
inline Term List::head() const
{
	return h;
}
inline Term List::tail() const
{
	return t;
}

//////////////////////////
// arguments data hiding
//
inline Term TermArgs::getarg(int n) const
{
#ifdef _DEBUG
	static long counter;
	counter++;
	ASSERT(n >= 0 && n < arity);
#endif
	return args[n];
}
