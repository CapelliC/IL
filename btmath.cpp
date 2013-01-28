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

//--------------
// math builtins
//--------------

#include "stdafx.h"
#include "builtin.h"
#include "qdata.h"
#include "bterr.h"
#include "aritval.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

BtFDecl(btsin);
BtFDecl(btacos);
BtFDecl(btasin);
BtFDecl(btatan);
BtFDecl(btatan2);
BtFDecl(btceil);
BtFDecl(btcos);
BtFDecl(btcosh);
BtFDecl(btexp);
BtFDecl(btfabs);
BtFDecl(btfloor);
BtFDecl(btfmod);
BtFDecl(btlog);
BtFDecl(btlog10);
BtFDecl(btpow);
BtFDecl(btsin);
BtFDecl(btsinh);
BtFDecl(btsqrt);
BtFDecl(bttan);
BtFDecl(bttanh);
BtFDecl(cast);

BuiltIn mathfunctions[20] = {
	{"acos",	2, btacos},
	{"asin",	2, btasin},
	{"atan",	2, btatan},
	{"atan2",	3, btatan2},
	{"ceil",	2, btceil},
	{"cos",		2, btcos},
	{"cosh",	2, btcosh},
	{"exp",		2, btexp},
	{"fabs",	2, btfabs},
	{"floor",	2, btfloor},
	{"fmod",	3, btfmod},
	{"log",		2, btlog},
	{"log10",	2, btlog10},
	{"pow",		3, btpow},
	{"sin",		2, btsin},
	{"sinh",	2, btsinh},
	{"sqrt",	2, btsqrt},
	{"tan",		2, bttan},
	{"tanh",	2, bttanh},
	{"cast",	3, cast}
};

BOOL argin(Term t, IntlogExec *p, Double &x)
{
	AritValue v;
	v.evalexpr(t, p);

	if (v.t == AritValue::DBL)
	{
		x = v.d;
		return 1;
	}
	if (v.t == AritValue::IVAL)
	{
		x = v.i;
		return 1;
	}

	p->BtErr(BTERR_INVALID_ARG_TYPE);
	return 0;
}

BtFImpl(btacos, t, p)
{
	Double x;
	if (argin(t.getarg(0), p, x))
	{
		Double v = acos(x);
		return p->unify(Term(v), t.getarg(1));
	}
	return 0;
}

BtFImpl(btasin, t, p)
{
	Double x;
	if (argin(t.getarg(0), p, x))
	{
		Double v = asin(x);
		return p->unify(Term(v), t.getarg(1));
	}
	return 0;
}
BtFImpl(btatan, t, p)
{
	Double x;
	if (argin(t.getarg(0), p, x))
	{
		Double v = atan(x);
		return p->unify(Term(v), t.getarg(1));
	}
	return 0;
}
BtFImpl(btatan2, t, p)
{
	Double x, y;
	if (argin(t.getarg(0), p, x) && argin(t.getarg(1), p, y))
	{
		Double v = atan2(x, y);
		return p->unify(Term(v), t.getarg(2));
	}
	return 0;
}
BtFImpl(btceil, t, p)
{
	Double x;
	if (argin(t.getarg(0), p, x))
	{
		Double v = ceil(x);
		return p->unify(Term(v), t.getarg(1));
	}
	return 0;
}
BtFImpl(btcos, t, p)
{
	Double x;
	if (argin(t.getarg(0), p, x))
	{
		Double v = cos(x);
		return p->unify(Term(v), t.getarg(1));
	}
	return 0;
}
BtFImpl(btcosh, t, p)
{
	Double x;
	if (argin(t.getarg(0), p, x))
	{
		Double v = cosh(x);
		return p->unify(Term(v), t.getarg(1));
	}
	return 0;
}
BtFImpl(btexp, t, p)
{
	Double x;
	if (argin(t.getarg(0), p, x))
	{
		Double v = exp(x);
		return p->unify(Term(v), t.getarg(1));
	}
	return 0;
}
BtFImpl(btfabs, t, p)
{
	Double x;
	if (argin(t.getarg(0), p, x))
	{
		Double v = fabs(x);
		return p->unify(Term(v), t.getarg(1));
	}
	return 0;
}
BtFImpl(btfloor, t, p)
{
	Double x;
	if (argin(t.getarg(0), p, x))
	{
		Double v = floor(x);
		return p->unify(Term(v), t.getarg(1));
	}
	return 0;
}
BtFImpl(btfmod, t, p)
{
	Double x, y;
	if (argin(t.getarg(0), p, x) && argin(t.getarg(1), p, y))
	{
		Double v = fmod(x, y);
		return p->unify(Term(v), t.getarg(2));
	}
	return 0;
}
BtFImpl(btlog, t, p)
{
	Double x;
	if (argin(t.getarg(0), p, x))
	{
		Double v = log(x);
		return p->unify(Term(v), t.getarg(1));
	}
	return 0;
}
BtFImpl(btlog10, t, p)
{
	Double x;
	if (argin(t.getarg(0), p, x))
	{
		Double v = log10(x);
		return p->unify(Term(v), t.getarg(1));
	}
	return 0;
}
BtFImpl(btpow, t, p)
{
	Double x, y;
	if (argin(t.getarg(0), p, x) && argin(t.getarg(1), p, y))
	{
		Double v = pow(x, y);
		return p->unify(Term(v), t.getarg(2));
	}
	return 0;
}
BtFImpl(btsin, t, p)
{
	Double x;
	if (argin(t.getarg(0), p, x))
	{
		Double v = sin(x);
		return p->unify(Term(v), t.getarg(1));
	}
	return 0;
}
BtFImpl(btsinh, t, p)
{
	Double x;
	if (argin(t.getarg(0), p, x))
	{
		Double v = sinh(x);
		return p->unify(Term(v), t.getarg(1));
	}
	return 0;
}
BtFImpl(btsqrt, t, p)
{
	Double x;
	if (argin(t.getarg(0), p, x))
	{
		Double v = sqrt(x);
		return p->unify(Term(v), t.getarg(1));
	}
	return 0;
}
BtFImpl(bttan, t, p)
{
	Double x;
	if (argin(t.getarg(0), p, x))
	{
		Double v = tan(x);
		return p->unify(Term(v), t.getarg(1));
	}
	return 0;
}
BtFImpl(bttanh, t, p)
{
	Double x;
	if (argin(t.getarg(0), p, x))
	{
		Double v = tanh(x);
		return p->unify(Term(v), t.getarg(1));
	}
	return 0;
}
BtFImpl(cast, t, p)
{
	AritValue v;
	v.evalexpr(t.getarg(0), p);

	if (v.t == AritValue::DBL || v.t == AritValue::IVAL)
	{
		Term q = p->eval_term(t.getarg(1));
		if (q.type(f_ATOM))
		{
			CCP req = q;
			if (!strcmp(req, "integer"))
			{
				if (v.t == AritValue::DBL)
					v.i = Int(v.d);
				return p->unify(Term(v.i), t.getarg(2));
			}
			else if (!strcmp(req, "real"))
			{
				if (v.t == AritValue::IVAL)
					v.d = v.i;
				return p->unify(p->save(Term(v.d)), t.getarg(2));
			}
		}

		p->BtErr(BTERR_INVALID_ARG_TYPE);
	}

	return 0;
}
