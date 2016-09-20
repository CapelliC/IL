
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


//-------------------
// execution control
//-------------------

#include "stdafx.h"
#include "defsys.h"
#include "qdata.h"
#include "builtin.h"
#include "bterr.h"

BtFDecl(true_rep);
BtFDecl(fail);
BtFDecl(call);
BtFDecl(not_);
BtFDecl(findall);
BtFDecl(setof);
BtFDecl(bagof);

BuiltIn execution_control[8] = {
    {"repeat",	0|BuiltIn::retry,	true_rep},
    {"true",	0,					true_rep},
    {"fail",	0,					fail},
    {"call",	1,					call},
    {"not",		1,					not_},
    {"findall",	3,					findall},
    {"setof",	3,					setof},
    {"bagof",	3,					bagof}
};

BtFImpl_NP(true_rep)
{
    return 1;
}

BtFImpl_NP(fail)
{
    return 0;
}

/////////////////////////////////////
// make a call of argument predicate
//
BtFImpl(call, t, p)
{
    Term arg = p->copy(t.getarg(0));
    if (arg.type(f_NOTERM|f_VAR))
    {
        p->BtErr(BTERR_INVALID_ARG_TYPE);
        return 0;
    }
    return p->call(arg);
}

/////////////////////////
// check for failed call
//
BtFImpl(not_, t, p)
{
    Term arg = p->copy(t.getarg(0));
    if (arg.type(f_NOTERM|f_VAR))
    {
        p->BtErr(BTERR_INVALID_ARG_TYPE);
        return 0;
    }
    return !p->call(arg);
}

////////////////////////////
// route request to engine
//
BtFImpl(findall, t, p)
{
    return p->findall(t.getarg(0), t.getarg(1), t.getarg(2));
}
BtFTBD(setof)
BtFTBD(bagof)
#if 0
BtFImpl(setof, t, p)
{
    return p->setof(t.getarg(0), t.getarg(1), t.getarg(2));
}
BtFImpl(bagof, t, p)
{
    return p->bagof(t.getarg(0), t.getarg(1), t.getarg(2));
}
#endif
