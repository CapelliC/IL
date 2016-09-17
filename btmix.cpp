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

///////////////////////////////
// mixing of useful predicates
//	system level interface
///////////////////////////////

#include "stdafx.h"
#include "defsys.h"
#include "builtin.h"
#include "btmix.h"
#include "qdata.h"
#include "bterr.h"
#include "argali.h"
#include "eng.h"
#include <time.h>

#ifdef _DOS
#include <dos.h>
#include <conio.h>
#endif

#ifdef _WINDOWS
#include <windows.h>
#endif

BtFDecl(genrand);

BtFDecl(listfiles);
BtFDecl(os_command);
BtFDecl(os_name);
BtFDecl(os_mem);
BtFDecl(os_interrupt);

BtFDecl(showstatus);
BtFDecl(listbltins);

BtFDecl(pkbhit);
BtFDecl(pgetch);
BtFDecl(pgetche);
BtFDecl(pflush);

BtFDecl(timecurr);
BtFDecl(timeform);
BtFDecl(timediff);

BuiltIn mixing[15] = {
	{"random",		3,	genrand},

	{"listfiles",	3,	listfiles},
	{"os_command",	1,	os_command},
	{"os_name",		1,	os_name},
	{"os_mem",		1,	os_mem},
	{"os_interrupt",3,	os_interrupt},

	{"showstatus",	1,	showstatus},
	{"builtins",	0,	listbltins},

	{"kbhit",		0,	pkbhit},
	{"getch",		1,	pgetch},
	{"getche",		1,	pgetche},
	{"flush",		0,	pflush},

	{"timecurr",	1,	timecurr},
	{"timeform",	3,	timeform},
	{"timediff",	3,	timediff}
};

///////////////////////////////////////////////
// generate a random number in specified range
//
BtFImpl(genrand, t, p)
{
	Term	tl = p->eval_term(t.getarg(0)),
			th = p->eval_term(t.getarg(1)),
			rv = t.getarg(2);
	Int tli, thi;

	if (	!tl.type(f_INT) || !th.type(f_INT) ||
			(thi = Int(th)) - (tli = Int(tl)) == 0 ||
			!rv.type(f_VAR)) {
		p->BtErr(BTERR_INVALID_ARG_TYPE);
		return 0;
	}

	static time_t seed;
	if (!seed)
		srand((unsigned int)(seed = time(0)));

	return p->unify(rv, Term(Int(tli + rand() % (thi - tli))));
}

/////////////////////
// call a OS command
//
BtFImpl(os_command, t, p)
{
	Term c = p->eval_term(t.getarg(0));

	if (!c.type(f_ATOM)) {
		p->BtErr(BTERR_INVALID_ARG_TYPE);
		return 0;
	}

#if defined(_DOS)

	return system(kstring(c))? 0 : 1;

//#elif defined(WIN32)
#elif 1

	return 0;

#else

	extern bool WaitTaskExecution(UINT ntc);
	char cmdBuf[256];
    //_snprintf(cmdBuf, sizeof cmdBuf, "COMMAND.COM /c %s", CCP(kstring(c)));
    snprintf(cmdBuf, sizeof cmdBuf, "COMMAND.COM /c %s", CCP(kstring(c)));

	UINT ntc = GetNumTasks();
	UINT rc = WinExec(cmdBuf, SW_SHOWNORMAL);
	if (rc >= 32)
		WaitTaskExecution(ntc);
	return rc < 32? 0 : 1;

#endif
}

/////////////////////
// get OS identifier
//
BtFImpl(os_name, t, p)
{
#if defined(_DOS)
	kstring os_id("dos");
#elif defined(_WINDOWS)
	kstring os_id("windows");
#elif defined(WIN32)
	kstring os_id("win32");
#else
    kstring os_id("linux");
#endif

	return p->unify(Term(os_id), t.getarg(0));
}

BtFTBD(os_interrupt)
#if 0
BtFImpl(os_interrupt, t, p)
{
	Term inum = p->eval_term(t.getarg(0)),
		 ireg = p->copy(t.getarg(1)),
		 oreg = t.getarg(2);

	if (!inum.type(f_INT) || !ireg.type(f_LIST))
	{ err:
		p->BtErr(BTERR_INVALID_ARG_TYPE);
		ireg.Destroy();
		return 0;
	}

	// fill input registers
	union REGS inregs, outregs;
	Term ir = ireg;
	int r = 0;
	while (!ir.LNULL()) {
		Term rv = ir.getarg(0);
		if (rv.type(f_INT))
			switch (r) {
			case 0: inregs.x.ax = Int(rv); break;
			case 1: inregs.x.bx = Int(rv); break;
			case 2: inregs.x.cx = Int(rv); break;
			case 3: inregs.x.dx = Int(rv); break;
			case 4: inregs.x.si = Int(rv); break;
			case 5: inregs.x.di = Int(rv); break;
			case 6: inregs.x.cflag = Int(rv); break;
			default: goto err;
			}
		else if (!rv.type(f_VAR) || Var(rv) != ANONYM_IX)
			goto err;

		if (!(ir = ir.getarg(1)).type(f_LIST))
			goto err;
		r++;
	}

	// release tmp copy
	ireg.Destroy();

	// actual call
	int86(Int(inum), &inregs, &outregs);

	// check if some retun required
	if (oreg.type(f_VAR) && Var(oreg) == ANONYM_IX)
		return 1;

	// build the result list
	Term tail, lreg;
	tail = lreg = Term(Term(Int(outregs.x.ax)), Term(ListNULL));
	for (r = 1; r < 7; r++) {
		Int v;
		switch (r) {
		case 1: v = outregs.x.bx; break;
		case 2: v = outregs.x.cx; break;
		case 3: v = outregs.x.dx; break;
		case 4: v = outregs.x.si; break;
		case 5: v = outregs.x.di; break;
		case 6: v = outregs.x.cflag; break;
		}
		tail.setarg(1, Term(Term(v), Term(ListNULL)));
		tail = tail.getarg(1);
	}

	return p->unify(oreg, p->save(lreg));
}
#endif

////////////////////////
// get available memory
//
BtFTBD(os_mem)
#if 0
BtFImpl(os_mem, t, p)
{
	return p->unify(t.getarg(0), Term(Int(-1)));
}
#endif

/////////////////////////////////////
// search all files matching pattern
//
BtFTBD(listfiles)
#if 0
BtFImpl(listfiles, t, p)
{
	Term patt = p->eval_term(t.getarg(0));

	if (!patt.type(f_ATOM)) {
		p->BtErr(BTERR_INVALID_ARG_TYPE);
		return 0;
	}

	unsigned attr = 0;

	// scan required properties	identity list
	static ArgALIter::aDesc vect[] = {
		{"arch",	f_NOTERM},
		{"hidden",	f_NOTERM},
		{"normal",	f_NOTERM},
		{"rdonly",	f_NOTERM},
		{"subdir",	f_NOTERM},
		{"system",	f_NOTERM},
		{"volid",	f_NOTERM}
	};
	ArgALIter a(t.getarg(1), vect, 7);

	unsigned ix;
	while (a.nextarg(&ix) != ArgALIter::EndArgs)
		switch (ix) {
			case 0:	attr |= _A_ARCH; break;
			case 1:	attr |= _A_HIDDEN; break;
			case 2:	attr |= _A_NORMAL; break;
			case 3:	attr |= _A_RDONLY; break;
			case 4:	attr |= _A_SUBDIR; break;
			case 5:	attr |= _A_SYSTEM; break;
			case 6:	attr |= _A_VOLID; break;
			default:
				p->BtErr(BTERR_INVALID_ARG_TYPE);
				return 0;
		};

	struct _find_t f;
	if (_dos_findfirst(kstring(patt), attr, &f) == 0) {

		// build the files list (at least one)
		Term ll = Term(Term(kstring(f.name)), Term(ListNULL)), follow = ll;

		while (_dos_findnext(&f) == 0)
			if (attr == 0 || (f.attrib & attr) == attr) {
				Term tail = Term(Term(kstring(f.name)), Term(ListNULL));
				follow.setarg(1, tail);
				follow = tail;
			}

		return p->unify(ll, t.getarg(2));
	}

	// no matches
	return p->unify(Term(ListNULL), t.getarg(2));
}
#endif

//////////////////
// display status
//
BtFImpl(showstatus, t, p)
{
	static ArgALIter::aDesc vect[] = {
		{"base",	f_ATOM},
		{"proof",	f_ATOM},
		{"vars",	f_ATOM},
		{"trail",	f_ATOM}
	};
	ArgALIter a(t.getarg(0), vect, 4);

	unsigned ix;
	int st = 0;

	while (a.nextarg(&ix) != ArgALIter::EndArgs)
		switch (ix) {
		case 0: st |= IntlogExec::Base; break;
		case 1: st |= IntlogExec::Proof; break;
		case 2: st |= IntlogExec::Vars; break;
		case 3: st |= IntlogExec::Trail; break;
		}

	p->showstatus(p->out(), IntlogExec::statusmode(st));
	return 1;
}

////////////////////////
// display all builtins
//
BtFTBD(listbltins)
#if 0
BtFImpl_P1(listbltins, p)
{
    hashtable_iter it(GetEngines()->get_bttbl());
	e_bttable *e;

	while ((e = (e_bttable*)it.next()) != 0) {
		const BuiltIn* bt = e->get_bt();
		ostream &s = p->out();
		s << bt->ident;
		if (bt->nargs != -1)
			s << " / " << int(bt->nargs);
		if (bt->retry)
			s << " *";
		s << endl;
	}

	return 1;
}
#endif

//////////////////////////////////
// true if a keyboard key pressed
//
BtFImpl_P1(pkbhit, p)
{
#ifdef _DOS
	return kbhit()? 1 : 0;
#else
	p->BtErr(BTERR_ONLY_DOS);
	return 0;
#endif
}

//////////////////////////////
// return current pressed key
//
#ifdef _DOS
BtFImpl(pgetch, t, p)
{
	return p->unify(t.getarg(0), Term(Int(getch())));
}
#else
BtFImpl_P1(pgetch, p)
{
	p->BtErr(BTERR_ONLY_DOS);
	return 0;
}
#endif

////////////////////////////////////////
// return current pressed key with echo
//
#ifdef _DOS
BtFImpl(pgetche, t, p)
{
	return p->unify(t.getarg(0), Term(Int(getche())));
}
#else
BtFImpl_P1(pgetche, p)
{
	p->BtErr(BTERR_ONLY_DOS);
	return 0;
}
#endif

/////////////////////////
// flushes output stream
//
BtFImpl_P1(pflush, p)
{
	return p->out().flush().good();
}

//////////////////////////
// catch the current time
//
BtFImpl(timecurr, t, p)
{
	Term tv = p->eval_term(t.getarg(0));
	if (!tv.type(f_NOTERM)) {
		p->BtErr(BTERR_INVALID_ARG_TYPE);
		return 0;
	}
	mtime* pmt = new mtime(time(0));
	return p->unify(p->save(Term(pmt)), t.getarg(0));
}

//////////////////////////////////
// format the time saving as atom
//
BtFImpl(timeform, t, p)
{
	mtime*	mt = mtime::get_data(p->eval_term(t.getarg(0)));
	Term	tf = p->eval_term(t.getarg(1));

	if (!mt || !tf.type(f_ATOM)) {
		p->BtErr(BTERR_INVALID_ARG_TYPE);
		return 0;
	}
	char buf[100];

	mt->format(buf, sizeof buf, tf);
	return p->unify(t.getarg(2), Term(kstring(buf)));
}

////////////////////////////////////////////////////
// compute the difference in seconds
//	between two times and unify with third argument
//
BtFImpl(timediff, t, p)
{
	mtime *mt1 = mtime::get_data(p->eval_term(t.getarg(0))),
		  *mt2 = mtime::get_data(p->eval_term(t.getarg(1)));

	if (!mt1 || !mt2) {
		p->BtErr(BTERR_INVALID_ARG_TYPE);
		return 0;
	}

	double dp =	difftime(mt1->get(), mt2->get());
	return p->unify(t.getarg(2), p->save(Term(dp)));
}

static SysData* mkmtime() { return new mtime(); }
SysData::rtinfo mtime::rti = { "time", mkmtime, 0 };

mtime* mtime::get_data(Term t)
{
	if (t.type(f_SYSDATA)) {
		SysDataPtr sp = t;
		if (sp->istype(rti.id))
			return (mtime*)sp;
	}

	return 0;
}
Term mtime::copy() const
{
	return Term(new mtime(tval));
}

#if defined(_WINDLL) && !defined(WIN32)
extern size_t strftime(char *buf, size_t dim, const char *fmt, const struct tm *t);
#endif

char *mtime::format(char *buf, int maxbuf, const char *fmt) const
{
	struct tm t = *localtime(&tval);
	if (!fmt)
		fmt = "%H:%M %d/%m/%Y";
	if (strftime(buf, maxbuf, fmt, &t) == 0)
		buf[0] = 0;
	return buf;
}
