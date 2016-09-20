
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


#ifndef _SLIST_H_
#define _SLIST_H_

//--------------------
// single linked list
//--------------------

#include "iafx.h"

class slist;
class e_slist;
class slist_iter;
class slist_scan;
class e_slistvptr;
class slistvptr;
class slistvptr_iter;

#define SLIST_INVPOS unsigned(-1)

class e_slist
{
public:
	e_slist() { next = 0; }
	virtual	~e_slist() {}
	unsigned offlast() const;

protected:
	e_slist* next;

        friend class slist;
        friend class slist_iter;
        friend class slist_scan;
};

class IAFX_API slist
{
public:
	slist() { first = 0; }
	virtual ~slist() { clear(); }

	unsigned append(e_slist *);
	void insert(e_slist *, unsigned);
	void clear();
	void remove(unsigned);

	unsigned numel() const;

	// linear searching
	unsigned seek(void*) const;
	e_slist* seekptr(void*) const;
	virtual int match(e_slist *e1, void *e2) const {
		return e1 == e2;
	}

	e_slist* get(unsigned) const;
	e_slist* get_first() const {
		return first;
	}

private:
        friend class slist_iter;
        friend class slist_scan;
	e_slist* first;
};

//------------------
// forward iterator
//------------------
class IAFX_API slist_iter
{
public:
	slist_iter() { e = 0; }
	slist_iter(const slist& sl) { set(sl); }
	slist_iter(const slist* sl) { set(*sl); }

	void set(const slist& sl) { e = sl.first;	}
	void set(const slist* sl) { e = sl? sl->first : 0; }

	e_slist* curr() const { return e; }
	e_slist* next() {
		if (e) {
			e_slist *n = e;
			e = e->next;
			return n;
		}
		return 0;
	}

protected:
	e_slist* e;
};

//-----------------------
// iterator with editing
//-----------------------
class IAFX_API slist_scan : public slist_iter
{
public:

	slist_scan(slist &l) : slist_iter(l), lref(l) {}
	void delitem();
	void swap(e_slist *);

private:
	slist &lref;
};

//-----------------------
// list of void pointers
//-----------------------
class e_slistvptr : public e_slist
{
	void *vptr;
	e_slistvptr(void *p) { vptr = p; }
        friend class slistvptr;
        friend class slistvptr_iter;
};

class IAFX_API slistvptr : public slist
{
public:
	// append a pointer
	void append(void *p) {
		slist::append(new e_slistvptr(p));
	}

	// insert a pointer
	void insert(void *p, unsigned ix) {
		slist::insert(new e_slistvptr(p), ix);
	}

	// return indexed pointer
	void* get(unsigned index) const {
		e_slistvptr* e = (e_slistvptr*)slist::get(index);
		return e? e->vptr : 0;
	}

	// matching condition
	int match(e_slist *e1, void *e2) const {
		return ((e_slistvptr*)e1)->vptr == ((e_slistvptr*)e2)->vptr;
	}
};

class slistvptr_iter
{
public:
	slistvptr_iter() { e = 0; }
	slistvptr_iter(const slistvptr& sl) {
		set(sl);
	}

	void set(const slistvptr& sl) {
		e = (e_slistvptr*)sl.get_first();
	}
	void set(const slistvptr* sl) {
		e = sl? (e_slistvptr*)sl->get_first() : 0;
	}

	void* curr() const {
		return e? e->vptr : 0;
	}
	void* next() {
		if (e) {
			e_slistvptr *n = e;
			e = (e_slistvptr*)e->next;
			return n->vptr;
		}
		return 0;
	}

protected:
	e_slistvptr* e;
};

#endif
