
/*
    IL : Intlog Language
    Object Oriented Prolog Project
    Copyright (C) 1992-2021 - Ing. Capelli Carlo

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


#ifndef MEMBUF_H_
#define MEMBUF_H_

#include "iafx.h"
#include "slist.h"
#include "kstr.h"

// in-memory files support
//
struct IAFX_API membuf : public e_slist {
    kstring id;
    char *mem;

    membuf() { mem = nullptr; }
    ~membuf() { delete mem; }
};

extern IAFX_API membuf *find(kstring id, slist_iter& i);

#endif
