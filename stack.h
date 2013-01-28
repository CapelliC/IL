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

#ifndef _STACK_H_
#define _STACK_H_

//-------------------
//		stack
//-------------------

#ifndef _VECTB_H_
#include "vectb.h"
#endif

#define decl_stack(type)					\
class stack##type : protected vect##type {	\
public:										\
	stack##type(unsigned dblk = 64) {		\
		dimblk = dblk;						\
		count = 0;							\
	}										\
	~stack##type() { clear(); }				\
	unsigned size() const { return count; }	\
	void push(type v) {						\
		if (count == vdim) grow(dimblk);	\
		setat(count++, v);					\
	}										\
	type get(unsigned off = 0) const {		\
		return getat(count - 1 - off);		\
	}										\
	type* getptr(unsigned off = 0) const {	\
		return vect##type::					\
			getptr(count - 1 - off);		\
	}										\
	virtual void pop(unsigned n = 1) {		\
		ASSERT(n <= count);					\
		count -= n;							\
	}										\
	virtual void clear() { pop(count); }	\
protected:									\
	type* reserve() {						\
		if (count++ == vdim) grow(dimblk);	\
		return vect##type::getptr(count-1);	\
	}										\
	unsigned dimblk;						\
	unsigned count;							\
};

decl_stack(vptr);

#endif
