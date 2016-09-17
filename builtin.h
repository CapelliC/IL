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

#ifndef _BUILTIN_H_
#define _BUILTIN_H_

#include "iafx.h"
#include "term.h"
#include "message.h"

class IntlogExec;

//----------------------
// user defined entries
//
struct BuiltIn
{
	// matching functor/identifier string
	CCP ident;

	// arity|retry
	short args;
	enum { retry = 0x80 };

	// evaluator (need variable instance access)
	int (*eval)(TermArgs, IntlogExec *, short);

	static void Initialize(DbIntlog* pDB, MsgTable *pMsgTbl);
};

// useful prototype declarator macro
#define BtFDecl(f)	static	int f(TermArgs, IntlogExec *, short)
#define BtFDeclP(f)			int f(TermArgs, IntlogExec *, short)

// useful implementation declarator macro
#define BtFImpl(f,t,p)		static int f(TermArgs t, IntlogExec *p, short)
#define BtFImpl_P1(f,p)		static int f(TermArgs,   IntlogExec *p, short)
#define BtFImpl_R(f,t,p,r)	static int f(TermArgs t, IntlogExec *p, short r)
#define BtFImpl_NP(f)		static int f(TermArgs,   IntlogExec *, short)

// instance not yet implemented function
#define BtFTBD(f)	static int f(TermArgs, IntlogExec *p, short) {	\
						p->BtErr(BTERR_UNIMPLEMENTED);				\
						return 0;									\
					}

#endif
