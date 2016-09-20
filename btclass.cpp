
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


//------------------------------
// term classification builtins
//------------------------------

#include "stdafx.h"
#include "builtin.h"
#include "qdata.h"
#include <math.h>

BtFDecl(atom);
BtFDecl(integer);
BtFDecl(real);
BtFDecl(number);
BtFDecl(atomic);
BtFDecl(var);
BtFDecl(nonvar);

BuiltIn classification[7] = {
	{"atom",	1,	atom},
	{"integer",	1,	integer},
	{"real",	1,	real},
	{"number",	1,	number},
	{"atomic",	1,	atomic},
	{"var",		1,	var},
	{"nonvar",	1,	nonvar}
};

BtFImpl(atom, t, p)
{
	return p->eval_term(t.getarg(0)).type(f_ATOM);
}

BtFImpl(integer, t, p)
{
	return p->eval_term(t.getarg(0)).type(f_INT);
}
BtFImpl(real, t, p)
{
	return p->eval_term(t.getarg(0)).type(f_DOUBLE);
}
BtFImpl(number, t, p)
{
	return p->eval_term(t.getarg(0)).type(f_INT|f_DOUBLE);
}

BtFImpl(atomic, t, p)
{
	return p->eval_term(t.getarg(0)).type(f_ATOM|f_INT|f_DOUBLE);
}

BtFImpl(var, a, p)
{
	Term t = a.getarg(0);
	return t.type(f_VAR) && p->tval(Var(t)).type(f_NOTERM);
}

BtFImpl(nonvar, a, p)
{
	return !var(a, p, 0);
}
