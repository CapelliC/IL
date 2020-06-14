
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


#ifndef DLIST_H_
#define DLIST_H_

//--------------------
// double linked list
//--------------------

class e_dlist;
class dlist;
class dlist_iter;

class e_dlist {
    friend class dlist;
    friend class dlist_iter;

    e_dlist* next;
    e_dlist* prev;

public:
    e_dlist() { next = prev = 0; }
    virtual ~e_dlist() {}
};

class dlist {

    friend	class dlist_iter;
    e_dlist*	first;
    e_dlist*	last;

public:

    void	insert(e_dlist *e, int pos = -1);
    void	remove(int pos);
    void	clear();
    e_dlist*	get(int pos);

    unsigned	numel() const;

    dlist() { first = last = 0; }
    ~dlist() { clear(); }
};

class dlist_iter {

    dlist*	l;
    e_dlist*	e;

public:
    dlist_iter(dlist& dl) {
        first(dl);
    }
    void first(dlist& dl) {
        l = &dl; e = l->first;
    }
    void last(dlist& dl) {
        l = &dl; e = l->last;
    }

    e_dlist* next() {
        if (e) {
            e_dlist *n = e;
            e = e->next;
            return n;
        }
        return 0;
    }
    e_dlist* prev() {
        if (e) {
            e_dlist *n = e;
            e = e->prev;
            return n;
        }
        return 0;
    }
};

#endif
