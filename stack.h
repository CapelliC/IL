
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


#ifndef STACK_H_
#define STACK_H_

//-------------------
//		stack
//-------------------

#include "vectb.h"

template <class T>
class stack : protected vect<T> {
public:
	stack(unsigned dimblk = 64) : dimblk(dimblk), count(0) { }
	virtual ~stack() { clear(); }

	unsigned size() const { return count; }
	void push(T v) {
		if (count == size()) this->grow(dimblk);
		this->setat(count++, v);
	}
	T get(unsigned off = 0) const {
		return this->getat(count - 1 - off);
	}
	T* getptr(unsigned off = 0) const {
		return vect<T>::getptr(count - 1 - off);
	}
	virtual void pop(unsigned n = 1) {
		ASSERT(n <= count);
        ASSERT(n <= vect<T>::size());
		count -= n;
	}
    virtual void clear() { pop(count); }

protected:
	T* reserve() {
		if (count++ == vect<T>::size())
            this->grow(dimblk);
		return getptr();
	}
	unsigned dimblk;
	unsigned count;
};

#endif
