
/*
    IL : Intlog Language
    Object Oriented Prolog Project
    Copyright (C) 1992-2020 - Ing. Capelli Carlo

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


//---------------------
// comparison builtins
//---------------------

#include "stdafx.h"
#include "builtin.h"
#include "qdata.h"
#include "defsys.h"
#include "bterr.h"

BtFDecl(equ);
BtFDecl(nequ);
BtFDecl(equequ);
BtFDecl(nequequ);

BuiltIn comparison[4] = {
	{"=",		2,	equ},
	{"\\=",		2,	nequ},
	{"==",		2,	equequ},
	{"\\==",	2,	nequequ}
};

BtFImpl(equ, t, p)
{
	return p->unify(t.getarg(0), t.getarg(1));
}
BtFImpl(nequ, t, p)
{
	return !p->unify(t.getarg(0), t.getarg(1));
}

BtFImpl(equequ, t, p)
{
	Term t0 = p->copy(t.getarg(0)), k0,
		 t1 = p->copy(t.getarg(1)), k1;

	TermVisit v0(t0),
			  v1(t1);

	int rc = 0;

	do
		if (TermData(k0 = v0.next()) != TermData(k1 = v1.next()))
			goto ret;
	while (!k0.type(f_NOTERM));

	rc = k1.type(f_NOTERM);

ret:
	t0.Destroy();
	t1.Destroy();

	return rc;
}
BtFImpl(nequequ, t, p)
{
	return equequ(t, p, 0);
}
