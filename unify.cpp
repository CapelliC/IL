
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


#include "stdafx.h"
#include "qdata.h"
#include "unify.h"

//-----------------------------------------------------
// generate MGU of terms t1, t2
//	return 0 if failure (t1, t2 don't unify)
//
// the simple stacked, abstract algorithm is keep from
//	'The Art Of Prolog' by Sterling & Shapiro
//
int UnifyStack::work(Term t1, stkpos envp1, Term t2, stkpos envp2)
{
	termPair *tp;

	for ( ; ; )
	{
		if (t1.type(f_VAR))			// here be dragons!
		{
			if (Var(t1) == ANONYM_IX)
				goto next;
			stkpos ix1 = Var(t1) + envp1;
			if ((t1 = vs->getvar(ix1, &envp1, &ix1)).type(f_NOTERM))
			{
				if (t2.type(f_VAR))
				{
					if (Var(t2) == ANONYM_IX)
						goto next;
					ts->bind(ix1);
					stkpos ix2 = Var(t2) + envp2;
					if ((t2 = vs->getvar(ix2, &envp2, &ix2)).type(f_NOTERM))
					{
						if (ix1 != ix2)
							vs->setshare(ix1, ix2);
					}
					else
						vs->setvar(ix1, t2, envp2);
				}
				else
				{
					ts->bind(ix1);
					vs->setvar(ix1, t2, envp2);
				}
				goto next;
			}
		}

		if (t2.type(f_VAR))
		{
			if (Var(t2) == ANONYM_IX)
				goto next;
			stkpos ix2 = Var(t2) + envp2;
			if ((t2 = vs->getvar(ix2, &envp2, &ix2)).type(f_NOTERM)) {
				ts->bind(ix2);
				vs->setvar(ix2, t1, envp1);
				goto next;
			}
		}

		if (!t2.type(t1.type()))
			return 0;

		switch (t1.type())
		{

		case f_ATOM:
		case f_INT:
			if (TermData(t1) != TermData(t2))
				return 0;
			break;

		case f_DOUBLE:
			if (Double(t1) != Double(t2))
				return 0;
			break;

		case f_STRUCT:
		{
			Term *pa1, *pa2;
			int na1, na2;
			if (t1.structData(&pa1, &na1) != t2.structData(&pa2, &na2) ||
				na1 != na2)
				return 0;

			for (int i = na1 - 1; i >= 0; i--)
			{
				tp = push();
				tp->t1 = pa1[i];
				tp->i1 = envp1;
				tp->t2 = pa2[i];
				tp->i2 = envp2;
			}
			check_overflow();
		}
			break;

		case f_LIST:
			if (t1.LNULL() && t2.LNULL())
				goto next;
			if (!t1.LNULL() && !t2.LNULL())
			{
				const List& l1 = t1, &l2 = t2;

				tp = push();
				tp->t1 = l1.tail();
				tp->i1 = envp1;
				tp->t2 = l2.tail();
				tp->i2 = envp2;

				tp = push();
				tp->t1 = l1.head();
				tp->i1 = envp1;
				tp->t2 = l2.head();
				tp->i2 = envp2;

				check_overflow();
			}
			else
				return 0;
			break;

		case f_SYSDATA:
			if (!SysDataPtr(t1)->unify(t2))
				return 0;
			break;

		default:
			ASSERT(0);
		}

	next:
		if (free == 0)
			return 1;

		tp = v + --free;

		t1 = tp->t1;
		envp1 = tp->i1;
		t2 = tp->t2;
		envp2 = tp->i2;
	}
}

UnifyStack::UnifyStack(BindStack *pvs, TrailStack *pts)
{
	vs = pvs;
	ts = pts;
	free = 0;
}

///////////////////////////////////////////
// attempt unification of terms t1 & t2
//	access top node environment variables
//	used by builtins to instance arguments
//
int	IntlogExec::unify(Term t1, Term t2)
{
	stkpos vartop = ps->topget()->vspos;
	int tpos = ts->curr_dim(), retc;

	UnifyStack us(vs, ts);
	if ((retc = us.work(t1, vartop, t2, vartop)) == 0)
		unbind(tpos);

	return retc;
}
int	IntlogExec::unify_abs(Term t1, Term t2)
{
	stkpos tpos = ts->curr_dim(), retc;

	UnifyStack us(vs, ts);
	if ((retc = us.work(t1, ps->topget()->vspos, t2, 0)) == 0)
		unbind(tpos);

	return retc;
}

////////////////////////////
// check for generated vars
//
int IntlogExec::unify_gen(Term inStack, Term withVars)
{
	Term c = save(copy(withVars));
	OffVars of(c);
	int tpos = ts->curr_dim(), retc;

	UnifyStack us(vs, ts);
	if ((retc = us.work(inStack, ps->topget()->vspos, withVars, vs->reserve(of.nvars()))) == 0)
	{
		vs->pop(of.nvars());
		unbind(tpos);
	}

	return retc;
}

/*
int IntlogExec::unify(	stkpos envp1, TermArgs pa1,
						stkpos envp2, TermArgs pa2, int n)
{
	if (n > 0) {
		UnifyStack us(vs, ts);
		for (int i = n - 1; i > 0; i--) {
			UnifyStack::termPair *tp = us.push();
			tp->t1 = pa1.getarg(i);
			tp->i1 = envp1;
			tp->t2 = pa2.getarg(i);
			tp->i2 = envp2;
		}
		return us.work(pa1.getarg(0), envp1, pa2.getarg(0), envp2);
	}

	return 1;
}
*/
