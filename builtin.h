
/*
    IL : Intlog Language
    Object Oriented Prolog Project
    Copyright (C) 1992-2021 - Ing. Capelli Carlo

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


#ifndef BUILTIN_H_
#define BUILTIN_H_

#include "iafx.h"
#include "term.h"
#include "message.h"
#include "dbintlog.h"

class IntlogExec;

//----------------------
// user defined entries
//
struct BuiltIn {
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
#define BtFDecl(f)  static  int f(TermArgs, IntlogExec *, short)
#define BtFDeclP(f)         int f(TermArgs, IntlogExec *, short)

// useful implementation declarator macro
#define BtFImpl(f,t,p)      static int f(TermArgs t, IntlogExec *p, short)
#define BtFImpl_P1(f,p)     static int f(TermArgs,   IntlogExec *p, short)
#define BtFImpl_R(f,t,p,r)  static int f(TermArgs t, IntlogExec *p, short r)
#define BtFImpl_NP(f)       static int f(TermArgs,   IntlogExec *, short)

// instance not yet implemented function
#define BtFTBD(f) \
static int f(TermArgs, IntlogExec *p, short) {  \
        p->BtErr(BTERR_UNIMPLEMENTED);              \
        return 0;                                   \
    }

//////////////////////
// builtin predicates
//////////////////////
extern BuiltIn IO_oriented[22];
extern BuiltIn execution_control[8];
extern BuiltIn tracing_control[7];
extern BuiltIn database_control[7];
extern BuiltIn metalogical[7];
extern BuiltIn comparison[4];
extern BuiltIn arithmetic[6];
extern BuiltIn classification[7];
extern BuiltIn mathfunctions[20];

#endif
