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

#ifndef _ARGALI_H_
#define _ARGALI_H_

#include "term.h"
#include "query.h"

//--------------------------------------------------
// a class to iterate arguments of form like spy...
//
class ArgALIter
{
public:
	struct aDesc {
		CCP idkey;
		unsigned long tval;
	};

	ArgALIter(Term, aDesc * = 0, unsigned = 0);

	enum aType {
		Key,
		KeyValue,
		KeyErr,
		Pair,
		Single,
		EndArgs
	};

	aType nextarg(unsigned * = 0, Term* = 0);

	virtual int match(CCP key, CCP arg) const {
		return strcmp(key, arg) == 0;
	}

private:
	Term curr;
	Term lptr;
	aDesc *vectkeys;
	unsigned nkeys;

	unsigned search(CCP) const;
	aType ret(aType, unsigned *, unsigned, Term*, Term);
};

//----------------------------------
// iterate a common argument schema
//	identifier
//	identifier / arity
//
class ArgIdArityList : public ArgALIter
{
public:

	// copy term on create
	ArgIdArityList(Term, IntlogExec *, CCP);

	// delete copied term
	~ArgIdArityList();

	// access next arg (0 on fail/EndArgs)
	int next();

	// display error message with some other info
	void errmsg(int = 0, CCP = 0, CCP = 0);

	// nth-argument functor
	kstring funct;

	// nth-argument arity (-1 if none)
	int arity;

	// 1 if OK, 0 else
	int status;

	// track OK arguments
	unsigned counter;

private:
	CCP caller;
	Term root;
};

#endif
