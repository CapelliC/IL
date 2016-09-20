
/*
    IL : Intlog Language
    Object Oriented Prolog Project
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


//-----------------
// term operations
//-----------------

#include "stdafx.h"
#include "iafx.h"
#include "term.h"
#include "eng.h"

/*
void* Term::operator new(size_t)
{
	ASSERT(0);
	return 0;
}
*/

///////////////////////
// memory vector pools
///////////////////////

static MemStorage<List> 		m_lists;
static MemStorage<StructPtr>	m_structs;
static MemStorage<Double>		m_doubles;
static MemStorage<SysDataPtr>	m_sysd;

// default to write fmt
int Term::useOpDecl = 1;

void Term::FreeMem()
{
	m_doubles	.FreeMem();
	m_structs	.FreeMem();
	m_lists		.FreeMem();
	m_sysd		.FreeMem();
}

#ifdef _DEBUG
void Term::DumpMem(ostream& s, int el)
{
	if (el & 0x01) { s << "double "; m_doubles.Dump(s); }
	if (el & 0x02) { s << "struct "; m_structs.Dump(s); }
	if (el & 0x04) { s << "lists "; m_lists.Dump(s); }
	if (el & 0x08) { s << "sysd "; m_sysd.Dump(s); }
}
#endif

// float number
Term::Term(Double d)
{
	m_data = f_DOUBLE | m_doubles.Store(d);
}

// struct (no args yet)
Term::Term(Atom f, int arity)
{
	ASSERT(arity > 0);

	StructPtr sp = StructPtr(new char[sizeof(Struct) + sizeof(Term) * arity]);
	sp->funct = f;
	sp->arity = arity;
	m_data = f_STRUCT | m_structs.Store(sp);
}

// a list
Term::Term(Term h, Term t)
{
	List l;
	l.h = h;
	l.t = t;

	m_data = f_LIST | m_lists.Store(l);
}

// system defined data
Term::Term(SysData* d)
{
	m_data = f_SYSDATA | m_sysd.Store(d);
}

//------------------------------------------------------
// transform string <yytext> of lenght <yyleng> to list
//
Term::Term(CCP yytext, int yyleng)
{
	if (yyleng > 0) {	// not empty

		MemStoreRef f = m_lists.EmptyStore();
		m_data = f_LIST|f;
		List *l = m_lists[f];

		int cc = 0;
		for ( ; cc < yyleng - 1; cc++) {
			l->h = Term(Int(yytext[cc]));
			l->t = f_LIST|(f = m_lists.EmptyStore());
			l = m_lists[f];
		}
		l->h = Term(static_cast<Int>(yytext[cc]));
		l->t = ListNULL;

	} else
		m_data = ListNULL;
}

//----------------------
// recursive destructor
//
void Term::Destroy()
{
	MemStoreRef iv;		// MemStorage position (if applicable)

	switch (type()) {

	case f_STRUCT: {
		Struct *s = *m_structs[iv = ivalue()];
		for (int i = 0; i < s->arity; i++)
			TermArgsVect(s)[i].Destroy();
		delete s;
		m_structs.Delete(iv);
		}
		break;

	case f_LIST:/*
		if (m_data != ListNULL) {
			List *l = m_lists[iv = ivalue()];
			Term(l->t).Destroy();
			Term(l->h).Destroy();
			m_lists.Delete(iv);
		}
		*/
		{ Term td(m_data);
		while (!td.LNULL()) {
			List *l = m_lists[iv = td.ivalue()];
			Term(l->h).Destroy();

			td = Term(l->t);
			m_lists.Delete(iv);

			if (!td.type(f_LIST))
			{
				td.Destroy();
				break;
			}
		}}
		break;

	case f_DOUBLE:
		m_doubles.Delete(ivalue());
		break;

	case f_SYSDATA: {
		SysData *s = *m_sysd[iv = ivalue()];
		if (s->delete_able())
			delete s;
		m_sysd.Delete(iv);
		}
	}
}

////////////////////////////
// change required argument
//
void Term::setarg(int n, Term t)
{
	if (type(f_STRUCT)) {
		Struct* s = *m_structs[ivalue()];
		ASSERT(n < s->arity);
		TermArgsVect(s)[n] = t;
	} else if (type(f_LIST)) {
		List *l = m_lists[ivalue()];
		if (n == 0)
			l->h = t;
		else {
			ASSERT(n == 1);
			l->t = t;
		}
	}
}

////////////////////////////////
// iterative visit of term tree
//
TermVisit::TermVisit(Term t)
{
	push(t);
}
void TermVisit::start(Term t)
{
	clear();
	push(t);
}
Term TermVisit::next()
{
	if (size()) {
		Term top = get();
		pop();

		if (top.type(f_STRUCT|f_LIST))
			for (int n = top.get_arity() - 1; n >= 0; n--)
				push(top.getarg(n));

		return top;
	}

	return Term(f_NOTERM);
}

//////////////////////
// memory data hiding
//////////////////////

Term::operator const List&() const
{
	ASSERT(type(f_LIST));
	return *m_lists[ivalue()];
}
Term::operator const Double&() const
{
	ASSERT(type(f_DOUBLE));
	return *m_doubles[ivalue()];
}
Term::operator SysData*() const
{
	ASSERT(type(f_SYSDATA));
	return *m_sysd[ivalue()];
}

////////////////////////////////
// optimized struct data access
//
kstring Term::structData(Term** pArgs, int *pArity) const
{
	ASSERT(type(f_STRUCT));
	Struct *s = *m_structs[ivalue()];
	*pArgs = TermArgsVect(s);
	*pArity = s->arity;
	return s->funct;
}

//////////////////
// return functor
//
kstring Term::get_funct() const
{
	if (type(f_ATOM))
		return kstring(ivalue());
	if (type(f_STRUCT))
		return (*m_structs[ivalue()])->funct;
	if (type(f_LIST))
		return kstring(KSTRING_LIST);

	return kstring(MSR_NULL);
}

////////////////
// return arity
//
int	Term::get_arity() const
{
	if (type(f_STRUCT))
		return (*m_structs[ivalue()])->arity;

	if (type(f_LIST) && !LNULL())
		return 2;

	return 0;
}

///////////////////
// return argument
//
Term Term::getarg(int n) const
{
	if (type(f_STRUCT)) {
		Struct *s = *m_structs[ivalue()];
		ASSERT(n < s->arity);
		return TermArgsVect(s)[n];
	}

	ASSERT(type(f_LIST) && !LNULL());
	ASSERT(n == 0 || n == 1);

	List *l = m_lists[ivalue()];
	return n == 0? l->h : l->t;
}

////////////////////////////////
// initialize arguments pointer
//
TermArgs Term::getargs() const
{
	TermArgs a;

	a.args = 0;
#ifdef _DEBUG
	a.arity = 0;
#endif

	if (type(f_STRUCT)) {
		Struct* s = *m_structs[ivalue()];
		a.args = TermArgsVect(s);
#ifdef _DEBUG
		a.arity = s->arity;
#endif
	} else if (type(f_LIST)) {
		if (ivalue() != MSR_NULL) {
			List *l = m_lists[ivalue()];
			a.args = (Term*)(&l->h);
#ifdef _DEBUG
			a.arity = 2;
#endif
		}
	}

	return a;
}

//// HPP

#define inline

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

	return args ? args[n] : Term(f_NOTERM);
}
