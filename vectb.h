
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


#ifndef _VECTB_H_
#define _VECTB_H_

#include <vector>

template <typename T>
class vect : protected std::vector<T> {
public:
    void grow(unsigned dim) {
        for (unsigned c = 0; c < dim; ++c)
            this->push_back(T());
    }
    void setat(unsigned pos, T val) {
        this->at(pos) = val;
    }
    T getat(unsigned pos) const {
        return this->at(pos);
    }
    T* getptr(unsigned pos) const {
        const T* p = &(this->at(pos));
        return const_cast<T*>(p);
    }
    unsigned dim() const { return this->size(); }
};

#endif
