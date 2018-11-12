
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


#ifndef _MEMSTORE_H_
#define _MEMSTORE_H_

#include "vectb.h"
#include "iafx.h"

typedef unsigned	MemStoreRef;
#define MSR_NULL	MemStoreRef(-1)

////////////////////////////////////////////////////////////
// allocation schema for frequently allocated items
//	dimensions must be power of 2, for fast address compute
//
//  now simplified to vector...
//
template <typename T>
class MemStorage : vect<T>
{
public:

	// insert consecutive uninitialized
    MemStoreRef Reserve(unsigned n = 1) {
        MemStoreRef p = this->dim();
        this->grow(n);
        return p;
    }

	// release used cell
    void Delete(MemStoreRef) {
    }

	// release all memory
    void FreeMem() {
        this->resize(0);
    }

    MemStoreRef Store(const T& v) {
		MemStoreRef r = Reserve();
		*cell(r) = v;
		return r;
	}

	T* operator[](MemStoreRef c) const {
		return cell(c);
	}

protected:

	// get pointer to cell as index
    T* cell(MemStoreRef p) const {
        return this->getptr(p);
    }

	// true if cell is used
    int used(MemStoreRef r) const {
        return r >= 0 && r < this->dim();
    }
};

#endif
