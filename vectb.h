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

#ifndef _VECTB_H_
#define _VECTB_H_

#define decl_vect(type)					\
class vect##type {					\
public:							\
    void grow(unsigned dim) {				\
        type *basenew = new type[vdim + dim];		\
        memcpy(basenew, base, sizeof(type) * vdim);	\
        delete [] base;					\
        base = basenew;					\
        vdim += dim;					\
    }							\
    void setat(unsigned pos, type val) {		\
        ASSERT(pos < vdim);				\
        base[pos] = val;				\
    }							\
    type getat(unsigned pos) const {			\
        ASSERT(pos < vdim);				\
        return base[pos];				\
    }							\
    type* getptr(unsigned pos) const {			\
        ASSERT(pos < vdim);				\
        return base + pos;				\
    }							\
    unsigned dim() const { return vdim; }		\
    vect##type() { base = 0; vdim = 0; }		\
    ~vect##type() { delete [] base; }			\
protected:						\
    type* base;						\
    unsigned vdim;					\
};

typedef void *vptr;
decl_vect(vptr);

#endif
