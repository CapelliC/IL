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

////////////////////////////
// operator table handling
//
#include "stdafx.h"
#include "iafx.h"
#include "operator.h"

////////////////////////////////
// append a definition
//	dont' check for duplicates
//
void OperTable::Add(kstring name,
	short prec, Operator::assocTag assoc, Operator::opCodeTag opc)
{
	ASSERT(opc == int(name) || opc == Operator::UserDef);
    push_back(Operator {name, opc, prec, assoc});
}

////////////////////////////////
// search first insertion point
//
Operator* OperTable::IsOp(kstring name) const
{
	if (unsigned(name) < Operator::UserDef)
		return getptr(name);

	// now search in user defined space
	for (unsigned ix = Operator::UserDef; ix < size(); ix++)
		if (getptr(ix)->name == name)
			return getptr(ix);

	return 0;
}

///////////////////////////////////
// search for next insertion point
//
Operator* OperTable::GetNext(Operator* op) const
{
	ASSERT(op && unsigned(op->opCode) < size());

	Operator *opnext = 0;
	if (op->opCode != Operator::UserDef) {
		unsigned ix = op->opCode + 1;

		while (ix < size()) {
			if (CCP(getptr(ix)->name) == CCP(op->name)) {
				opnext = getptr(ix);
				break;
			}
			ix++;
		}
	}

	return opnext;
}
