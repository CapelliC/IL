
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


//--------------------
// resizing hashtable
//--------------------

#include "hasht.h"

#include <QDebug>
#include <cassert>
using namespace std;

//--------------------------
// constructor alloc vector
//
hashtable::hashtable(unsigned size) {
    hvect = new e_hashtable*[vsize = size];
    assert(hvect);
    memset(hvect, 0, sizeof(e_hashtable*) * vsize);
    nentry = 0;
}

//-------------------------------------
// destructor: release all memory used
//
hashtable::~hashtable() {
    clear();
    delete hvect;
    hvect = nullptr;
}

//----------------------------
// release vector and entries
//
void hashtable::clear() {
    for (unsigned c = 0; c < vsize; c++)
        if (hvect[c]) {
            e_hashtable *e = hvect[c], *n;
            while (e) {
                n = e->link;
                delete e;
                e = n;
                nentry--;
            }
            hvect[c] = nullptr;
        }

    assert(nentry == 0);
}

//-----------------------------------------
// inconditionally insert entry into table
//
void hashtable::insert(e_hashtable *data) {
    unsigned h = hash(data->getstr(), vsize);
    nentry++;

    data->link = nullptr;
    e_hashtable *e = hvect[h];
    if (!e)
        hvect[h] = data;
    else {
        while (e->link)
            e = e->link;
        e->link = data;
    }

/*  these (faster) lines reverse order!
    data->link = hvect[h];
    hvect[h] = data;
*/
}

//--------------------------
// release required element
//
void hashtable::remove(e_hashtable* s, int index) {
    unsigned h = hash(s->getstr(), vsize);
    e_hashtable *e = hvect[h], *p = nullptr;

    while (e) {
        if (keymatch(s, e) && index-- == 0) {
            if (!p)
                hvect[h] = e->link;
            else
                p->link = e->link;
            delete e;
            nentry--;
            break;
        }
        e = (p = e)->link;
    }
}

e_hashtable::~e_hashtable() {
}

//--------------------------
// seek a string into table
//
e_hashtable* hashtable::isin(const e_hashtable* s) const {
#if 0
auto t = s->getstr();
auto h = hash(t, vsize);
qDebug() << __FUNCTION__ << ':' << t << h << vsize;
#endif
    e_hashtable* e = hvect[hash(s->getstr(), vsize)];
    while (e) {
        if (keymatch(e, s))
            break;
        e = e->link;
    }

    return e;
}
e_hashtable* hashtable::isin(const e_hashtable& s) const {
    return isin(&s);
}

//-------------------------
// access the next element
//  returning current
//
e_hashtable *hashtable_iter::next() {
    e_hashtable *r = e;

    if (k) {
        if (e) {
            e_hashtable_str ek(k);
            while ((e = e->link) != nullptr)
                if (t->keymatch(&ek, e))
                    break;
        }
    } else {
        if (e)
            e = e->link;
        else {
            while (i < t->vsize && (e = t->hvect[i]) == nullptr)
                i++;
            if (i < t->vsize) {
                r = e;
                e = e->link;
                i++;
            }
        }
    }
    return r;
}

//------------------
// build references
//
void hashtable_iter::init(const char *s) {
    if ((k = s) != nullptr)
        e = t->isin(e_hashtable_str(s));
    else {
        i = 0;
        while (i < t->vsize && (e = t->hvect[i++]) == nullptr)
            ;
    }
}

e_hashtable_str::~e_hashtable_str() {

}
hashtable_str::~hashtable_str() {

}

unsigned hashtable::hash(const char *pp, unsigned d) {
    unsigned ii = 0;
    while (*pp)
        ii = (ii << 1) ^ unsigned(*pp++);
    return ii % d;
}
