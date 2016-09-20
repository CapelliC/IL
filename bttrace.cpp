
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


#include "stdafx.h"
#include "defsys.h"
#include "qdata.h"
#include "tracer.h"
#include "builtin.h"
#include "bterr.h"
#include "argali.h"

//-------------------------------
// interface to builtin debugger
//-------------------------------

BtFDecl(trace);
BtFDecl(notrace);
BtFDecl(spy);
BtFDecl(nospy);
BtFDecl(debugging);
BtFDecl(nodebug);
BtFDecl(tracemode);

static int setspy(IntlogExec *, Term, int);

BuiltIn tracing_control[7] = {
	{"trace",		0, trace},
	{"notrace",		0, notrace},
	{"spy",			1, spy},
	{"nospy",		1, nospy},
	{"debugging",	0, debugging},
	{"nodebug",		0, nodebug},
	{"tracemode",	1, tracemode}
};

//--------------------------
// enable extensive tracing
//
BtFImpl_P1(trace, p)
{
	p->tracer()->trace_on();
	return 1;
}

//----------------------------
// disable extensive tracing
//
BtFImpl_P1(notrace, p)
{
	p->tracer()->trace_off();
	return 1;
}

//-------------------------------
// create spy point specifier(s)
//
BtFImpl(spy, t, p)
{
	return setspy(p, t.getarg(0), 1);
}

//-------------------------------
// remove spy point specifier(s)
//
BtFImpl(nospy, t, p)
{
	return setspy(p, t.getarg(0), 0);
}

//-------------------------
// require spy points list
//
BtFImpl_P1(debugging, p)
{
	p->tracer()->debugging(p->out());
	return 1;
}

//-----------------------
// remove all spy points
//
BtFImpl_P1(nodebug, p)
{
	p->tracer()->nodebug();
	return 1;
}

//-----------------
// set tracer mode
//
BtFImpl(tracemode, t, p)
{
	Term cp = p->copy(t.getarg(0)), a;

	static ArgALIter::aDesc keys[4] = {
		{ "ext",	f_INT },
		{ "file",	f_ATOM },
		{ "on",		f_NOTERM },
		{ "off",	f_NOTERM }
	};
	ArgALIter it(cp, keys, sizeof(keys) / sizeof(keys[0]));
	unsigned ixk;

	for ( ; ; ) {
		switch (it.nextarg(&ixk, &a)) {

		case ArgALIter::KeyValue:
			if (ixk == 0) {
				Int i = Int(a);
				if (i > 0)
					p->tracer()->etrace_on();
				else
					p->tracer()->etrace_off();
			}
			if (ixk == 1) {
				ofstream *f = new ofstream(a);
				if (f->good()) //is_open())
					p->tracer()->setoutput(f);
				else {
					delete f;
					goto err;
				}
			}
			break;

		case ArgALIter::Key:
			if (ixk == 2)
				p->tracer()->trace_on();
			if (ixk == 3)
				p->tracer()->trace_off();
			break;

		case ArgALIter::EndArgs:
			cp.Destroy();
			return 1;

		default:
			goto err;
		}
	}

err:
	p->BtErr(BTERR_INVALID_ARG_TYPE);
	cp.Destroy();

	return 0;
}

//-------------------------
// scan argument specifier and
//
static int setspy(IntlogExec *p, Term t, int on)
{
	Term cp = p->copy(t), a, r;
	ArgALIter it(cp, 0, 0);

	for ( ; ; ) {
		kstring funct;
		int arity = -1;

		switch (it.nextarg(0, &a)) {

		case ArgALIter::Pair:
			if (!(r = a.getarg(1)).type(f_INT))
				goto err;

			arity = Int(r);
			a = a.getarg(0);

			// fall through!

		case ArgALIter::Single:
			if (!a.type(f_ATOM))
				goto err;
			funct = a.kstr();
			break;

		default:
			goto err;

		case ArgALIter::EndArgs:
			cp.Destroy();
			return 1;
		}

		p->tracer()->spy(funct, on, arity);
	}

err:
	p->BtErr(BTERR_INVALID_ARG_TYPE);
	cp.Destroy();
	return 0;
}
