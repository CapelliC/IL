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

#ifndef _ARITVAL_H_
#define _ARITVAL_H_

#include "term.h"

class IntlogExec;

// recursive visit of expression tree
//	return type of subexpression or ERR if some error found
//
struct AritValue
{
	union {
		Int		i;
		Double	d;
		CCP		a;
	};
	enum {
		IVAL, DBL, ATOM, ERR
	} t;

	void evalexpr(Term e, IntlogExec *p);
	int compatible(AritValue &r);
	static int strlist_int(Term v, Int &i);
};

#endif
