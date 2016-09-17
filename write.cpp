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

#include "stdafx.h"
#include "qdata.h"
#include "eng.h"

int IntlogExec::writeMode = UseOpDecl|NameVars|NumberVars;

///////////////////////////////////////////
// could be called also on NULL engine...
//
ostream& IntlogExec::write(Term t, ostream &s, stkpos p) const
{
	switch (t.type())
	{
	case f_INT:
		s << Int(t);
		break;

	case f_DOUBLE:
		s << Double(t);
		break;

	case f_ATOM:
		s << CCP(t);
		break;

	case f_VAR:
		writeVar(t, s, p);
		break;

	case f_STRUCT:
		writeStruct(t, s, p);
		break;

	case f_LIST:
		writeList(t, s, p);
		break;

	case f_SYSDATA: {
		SysData *d = t;
		if (d->show_able())
			d->show(s);
		}
		break;

	default:
		ASSERT(0);
	}

	return s;
}

ostream& IntlogExec::display(Term t, ostream &s, stkpos p)
{
	int saveMode = writeMode;

	writeMode &= ~UseOpDecl;
	write(t, s, p);
	writeMode = saveMode;

	return s;
}

ostream& IntlogExec::writeVar(Var v, ostream &s, stkpos p) const
{
	if (v != ANONYM_IX)
	{
		if (this)
		{
			if (p == STKNULL)
				p = ps->topget()->vspos;

			stkpos p1;
			Term tv = vs->getvar(v + p, &p1, &p1);

			if (!tv.type(f_NOTERM))
				write(tv, s, p1);
			else
			{
				// look for var in stack with clauses
				if (writeMode & NameVars)
				{
					const Env *e = ps->findVarEnv(p);
					if (e)
					{
						kstring id;

						if (e->call->get_type() == CallData::DBPRED)
						{
							if (e->dbpos)
								id = e->dbpos->get()->get_varid(v);
						}
						else
							id = e->call->get_clause()->get_varid(v);

						if (unsigned(id) != MSR_NULL)
							s << CCP(id);
					}
				}
				if (writeMode & NumberVars)
					s << '_' << p1;
			}
		}
		else
			s << '_' << v;
	}
	else
		s << '_';

	return s;
}

ostream& IntlogExec::writeStruct(Term t, ostream &s, stkpos p) const
{
	CCP funct = t.get_funct();
	int arity = t.get_arity();

	if (writeMode & UseOpDecl)
	{
		OperTable* opt = GetEngines()->get_optbl();
		Operator* op = opt->IsOp(funct);

		while (op)
		{
			switch (op->assoc)
			{
			case Operator::XF:
			case Operator::YF:
				if (arity == 1)
					return write(t.getarg(0), s, p) << ' ' << funct;
				break;

			case Operator::FX:
			case Operator::FY:
				if (arity == 1)
					return write(t.getarg(0), s << funct << ' ', p);
				break;

			default:
				if (arity == 2)
				{
					write(t.getarg(0), s, p) << ' ' << funct << ' ';
					return write(t.getarg(1), s, p);
				}
			}

			op = opt->GetNext(op);
		}
	}

	s << funct << '(';
	for (int i = 0; i < arity; i++)
	{
		write(t.getarg(i), s, p);
		if (i < arity - 1)
			s << ',';
	}
	return s << ')';
}

ostream& IntlogExec::writeList(Term t, ostream &s, stkpos p) const
{
	s << '[';

	if (!t.LNULL())
		for ( ; ; )
		{
			// output head
			write(t.getarg(0), s, p);

			// use tail
			t = t.getarg(1);

			if (t.type(f_VAR) && this && Var(t) != ANONYM_IX && p != STKNULL)
			{
				stkpos p1;
				Term tv = vs->getvar(Var(t) + p, &p1, &p1);
				if (!tv.type(f_NOTERM))
				{
					t = tv;
					p = p1;
				}
			}

			if (!t.type(f_LIST))
			{
				write(t, s << '|', p);
				break;
			}

			if (t.LNULL())
				break;

			s << ',';
		}

	return s << ']';
}
