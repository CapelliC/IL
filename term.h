
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


#ifndef _TERM_H_
#define _TERM_H_

////////////////////////////////////////
// main data structure
//	mimic the recursive nature of TERMs
////////////////////////////////////////

#include "iafx.h"
#include "operator.h"

class	Term;
class	Struct;
class	List;
class	SysData;
class	TermArgs;
class	TermVisit;

typedef kstring			Atom;
typedef short			Int;
typedef double			Double;
typedef unsigned		Var;
typedef unsigned long	TermData;
typedef SysData*		SysDataPtr;
typedef Struct*			StructPtr;

const TermData
#if BUILD_64
    f_VAR		= 0x8000000000000000,
    f_STRUCT	= 0x4000000000000000,
    f_INT		= 0x2000000000000000,
    f_ATOM		= 0x1000000000000000,
    f_DOUBLE	= 0x0800000000000000,
    f_LIST		= 0x0400000000000000,
    f_SYSDATA	= 0x0200000000000000,
    f_NOTERM	= 0x0100000000000000,
    MaskFlags	= 0xFF00000000000000,
#else
	f_VAR		= 0x80000000,
	f_STRUCT	= 0x40000000,
	f_INT		= 0x20000000,
	f_ATOM		= 0x10000000,
	f_DOUBLE	= 0x08000000,
	f_LIST		= 0x04000000,
	f_SYSDATA	= 0x02000000,
	f_NOTERM	= 0x01000000,
	MaskFlags	= 0xFF000000,
#endif
    ListNULL	= f_LIST|~MaskFlags;

const int KSTRING_LIST	= int(Operator::UserDef);
const int KSTRING_CUT	= KSTRING_LIST + 1;
const Var ANONYM_IX     = Var(~MaskFlags);

///////////////
// term value
//
class IAFX_API Term
{
public:

	Term();				// dummy (random init)
	Term(TermData);		// like a low level copy
	Term(Atom);			// kstring selector
	Term(Int);			// short integer
	Term(Var);			// variable offset
	Term(Double);		// float number

	Term(Atom, int);	// struct (add args later)
	Term(Term, Term);	// list: head and tail
	Term(SysData*);		// system defined data pointer
	Term(CCP, int);		// string to list

	void NoTerm();		// set to empty or invalid

	void Destroy();		// recursive destructor

	// accessors: assert if no type OK
	operator const List&() const;
	operator const Double&() const;
	operator Var() const;
	operator Int() const;
	operator SysData*() const;
	operator TermData() const;

//	operator kstring() const;
	operator CCP() const;
	kstring kstr() const;

	kstring structData(Term**, int *) const;
	TermArgs getargs() const;

	TermData type() const;			// return type bits
	int type(TermData) const;		// check for type bits matching

									// display term
	int		is_expr(Operator::opCodeTag, int = 2) const;

	int		is_query() const;		// true if operator '?-'
	int		is_rule() const;		// true if operator ':-'
	int		is_or() const;			// true if operator ';'
	int		is_and() const;			// true if operator ','
	int		is_command() const;		// true if unary operator ':-'

	int		is_cut() const;			// true if atom '!'
	int		LNULL() const;			// true if NULL list (only!)

	kstring	get_funct() const;		// return functor
	int		get_arity() const;		// num of args (0..N)

	Term	LTerm() const;			// left term of expression
	Term	RTerm() const;			// right term of expression

	Term	getarg(int) const;		// return required arg or 0
	void	setarg(int, Term);		// change required argument

	static void FreeMem();			// static memory release

#ifdef _DEBUG
	static void DumpMem(ostream&, int);	// static memory displayer
#endif

private:
	TermData m_data;

	unsigned ivalue() const;		// return the value part

	static int useOpDecl;			// output flag
/*
	// disable 'standard' memory interface
	void* operator new(size_t);
	void operator delete(void *);
#ifdef _DEBUG
	void* operator new(size_t, const char *, int);
#endif
*/

};

//////////////////////////////
// structure
//	functor and argument list
//	contiguous memory layout
//
class IAFX_API Struct
{
private:
	kstring funct;
	int arity;
	// Term args[arity] : a vector of variable dimension

	#define TermArgsVect(sptr)	((Term*)((char*)sptr + sizeof(Struct)))
        friend class Term;
};

///////////////
// binary list
//
class IAFX_API List
{
public:
	Term head() const;	// head data
	Term tail() const;	// must point to empty list to terminate

private:
	TermData h, t;
        friend class Term;
};

//////////////////////////////////////
// fast acces to arguments of a Term
//	with debug support
//
class IAFX_API TermArgs
{
public:
	Term getarg(int) const;

private:
	Term* args;
#ifdef _DEBUG
	int arity;
#endif

        friend class Term;
};

//////////////////////////////////////
// system defined data
//	derive from this class your data
//
class IAFX_API SysData
{
public:

	struct rtinfo {
		CCP id;
		SysData* (*build)();
		rtinfo* link;
	};

	// an identifying key (supposed unique in the system)
	SysData(rtinfo&);

	// type info access
	int istype(CCP) const;
	CCP gettype() const;

	// enable delete operator on this
	virtual int delete_able() const;

	// release memory
	virtual ~SysData();

	// enable copying (should always be true)
	virtual int copy_able() const;

	// generate a copy of this data
	virtual Term copy() const;

	// enable 'deep' show
	virtual int show_able() const;

	// display data
	virtual void show(ostream &) const;

	// guaranteed a SysData argument: default match names
	virtual int unify(Term) const;

	// defined only in system
	virtual ostream& save(ostream&);
	virtual istream& load(istream&);

//private:

	// identification (auto register in constructor)
	rtinfo&	m_key;
	static rtinfo *reglist;

	friend IAFX_API ostream& operator<<(ostream&, SysData&);
	friend IAFX_API istream& operator>>(istream&, SysData*&);
        friend class Term;
};

// serial data storage
IAFX_API ostream& operator<<(ostream&, SysData&);
IAFX_API istream& operator>>(istream&, SysData*&);

///////////////////////////////////////////
// exec a preorder visit of requested term
//	declare and use a Term stack
//
class TermVisit : protected stack<Term>
{
public:
	// initialize
	TermVisit(Term);

	// next visited node
	Term next();

	// set starting position
	void start(Term);
};

#endif
