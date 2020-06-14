
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


#ifndef HASHT_H_
#define HASHT_H_

//--------------------------------
// hash tables with overflow area
//--------------------------------
class e_hashtable;
class e_hashtable_str;
class hashtable;
class hashtable_str;
class hashtable_iter;

#include "iafx.h"

//-------------------------------------
// a table entry
//	derive object from this base class,
//	to be inserted into hashtable
//
class e_hashtable
{
public:
    virtual ~e_hashtable() {}

    // must return a string pointer
    virtual const char*	getstr() const = 0;

private:
    friend class hashtable;
    friend class hashtable_iter;

    // keep matched keys
    e_hashtable* link;
};

//---------------------------------
// specialized version for strings
//
class e_hashtable_str : public e_hashtable
{
public:
    const char *getstr() const { return str; }
    e_hashtable_str(const char *s) { str = s; }
    e_hashtable_str() {}
private:
    const char *str;
};

//-------------------------------
// hash table with overflow area
//
class IAFX_API hashtable
{
public:
    hashtable(unsigned initsize = 997);
    virtual ~hashtable();

    // return 0 if not found
    e_hashtable* isin(const e_hashtable *key) const;
    e_hashtable* isin(const e_hashtable &key) const;

    // insert entry
    void insert(e_hashtable *data);

    // release element (index is occurrence, 0 first)
    void remove(e_hashtable *key, int index = 0);

    // release all entries memory
    void clear();

    // redim hashing vector (useful when nentries approach size)
    void resize(unsigned n = 997);

    // return current vector size
    unsigned get_vsize() const {
        return vsize;
    }

    // return number of stored entries
    unsigned get_nentries() const {
        return nentry;
    }

protected:

    // comparison mode (for strings)
    virtual int keymatch(const e_hashtable *e1, const e_hashtable *e2) const {
        return !strcmp(e1->getstr(), e2->getstr());
    }

private:

    // compute hash code (see Bjarne Stroustrup, pag 81)
    unsigned hash(const char *pp, unsigned d) const {
        register unsigned ii = 0;
        while (*pp)
            ii = (ii << 1) ^ *pp++;
        return ii % d;
    }

    // keep entry vector
    e_hashtable** hvect;

    // vector current size
    unsigned vsize;

    // number of entries allocated
    unsigned nentry;

    friend class hashtable_iter;
};

//----------------------------------
// simply compare directly pointers
//
class hashtable_str : public hashtable
{
public:
    hashtable_str(unsigned insz) : hashtable(insz) {}
    int keymatch(const e_hashtable *e1, const e_hashtable *e2) const {
        return e1->getstr() == e2->getstr();
    }
};

//----------------------
// iterator for entries
//
class IAFX_API hashtable_iter
{
public:
    hashtable_iter(const hashtable *ht, const char *s = 0) {
        t = ht;
        init(s);
    }
    void init(const char *s = 0);
    e_hashtable *next();
    e_hashtable *curr() { return e; }

protected:
    e_hashtable *e;
    const char *k;
    unsigned i;
    const hashtable* t;
};

#endif
