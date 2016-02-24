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

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

///////////////////////
// memory vector pools
///////////////////////
MemStorageDecl(Double,		SL_DOUBLE);
MemStorageDecl(StructPtr,	SL_PTR);
MemStorageDecl(List,		SL_PTR_P);
MemStorageDecl(SysDataPtr,	SL_PTR);

static MemStorage_List 			m_lists;
static MemStorage_StructPtr		m_structs;
static MemStorage_Double		m_doubles;
static MemStorage_SysDataPtr	m_sysd;

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
		delete vptr(s);
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