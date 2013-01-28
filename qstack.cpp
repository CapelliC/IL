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

//-------------------------
// query evaluation stacks
//-------------------------

#include "stdafx.h"
#include "qdata.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

#ifdef _DEBUG
stkpos countMaxAlloc;
#endif

// release allocated blocks
BaseStack::~BaseStack()
{
	for (unsigned i = 0; i < vtop; i++)
		delete base[i];
}

// allocate top elements
void BaseStack::push(unsigned n)
{
#ifndef WIN32
	if (free >= 0x40000)
		throw "stack overflow";
#endif

	unsigned newfree = free + n;
	unsigned delta = OPDIV(newfree, BLKDIM) - OPDIV(free, BLKDIM);
	
	while (delta--) {
		if (vtop == vdim)
			grow(VCTDIM);
		setat(vtop++, blkalloc(BLKDIM));
	}

	free = newfree;

#ifdef _DEBUG
	if (free > countMaxAlloc)
		countMaxAlloc = free;
#endif

}

// release top element(s)
stkpos BaseStack::pop(unsigned n)
{
	ASSERT(free >= n);

	stkpos oldfree = free;
	free -= n;

	stkpos delta = OPDIV(oldfree, BLKDIM) - OPDIV(free, BLKDIM);
    /*ASSERT(delta >= 0);*/
	while (delta--)
		delete base[--vtop];

	return free;
}

// ensure empty on create
void BaseStack::init()
{
	free = 0;
	vtop = 0;
	grow(VCTDIM);
	setat(vtop++, blkalloc(BLKDIM));
}

//----------------------------
// display current proof tree
//
void ProofStack::show(ostream &o) const
{
	o << "proof stack\n";
	for (stkpos i = 0; i < free; i++) {
		Env *e = get(i);

		o << '\t' << i << ":<";
		if (e->call)
			o << e->call->val();
		o <<
			"> f=" << e->father <<
			",vs=" << e->vspos <<
			",tr=" << e->trail << endl;
	}
}

//-------------------------------------------------
// find base of required, following fathers' chain
//
stkpos ProofStack::baseof(stkpos p) const
{
	if (p == STKNULL)
		p = free - 1;

	Env *s;
	while ((s = get(p))->father != STKNULL)
		p = s->father;

	return p;
}

//--------------------------
// true if f is father on n
//
int ProofStack::isancestor(stkpos f, stkpos n) const
{
	Env *e = get(n);
	while (e->father != STKNULL) {
		if (e->father == f)
			return 1;
		e = get(e->father);
	}
	return 0;
}

//--------------------------------------
// attempt to find env that have p vars
//
const ProofStack::Env* ProofStack::findVarEnv(stkpos p) const
{
	for (stkpos l = free - 1; l > 0; l--)
	{
		Env *e = get(l);
		if (e->vspos == p && e->call)
			return e;

		if (e->father != STKNULL)
		{
			Env *f = get(e->father);
			if (f->vspos == p && e->call)
				return e;
		}
	}

	return 0;
}

//-------------------------------------------
// reserve cells in stack, clearing contents
//
stkpos BindStack::reserve(unsigned n)
{
	stkpos ret = free;

	push(n);
				
	for ( ; n > 0; n--) {
		elbind *e = getptr(free - n);
		e->value = f_NOTERM;
		e->envp = 0;
	}

	return ret;
}

//--------------------------
// display current bindings
//
void BindStack::show(ostream &s) const
{
	s << "bind stack\n";
	for (stkpos i = 0; i < free; i++) {
		elbind *e = getptr(i);
		s << '\t' << i << ':';
		if (e->envp == STKNULL)
			s << '=' << e->share;
		else {
			s << e->envp;
			if (e->value != f_NOTERM)
				s << '=' << Term(e->value);
		}
		s << endl;
	}
}

//--------------------------------------
// return valued var from <off> to base
//
Term BindStack::getvar(stkpos off, stkpos *envp, stkpos *shenv) const
{
	elbind *e;

	while ((e = getptr(off))->envp == STKNULL)
	{
		ASSERT(off != e->share);
		off = e->share;
	}

	Term t = e->value;
	if (!t.type(f_NOTERM))
		*envp = e->envp;
	else
		*shenv = off;

	return t;
}

//----------------------------------
// display current bindings segment
//
void TrailStack::show(ostream &s) const
{
	s << "trail stack\n";
	for (stkpos i = 0; i < free; i++)
		s << '\t' << i << ':' << get(i) << endl;
}
