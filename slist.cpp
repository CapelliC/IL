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

//--------------------
// single linked list
//--------------------

#include "stdafx.h"
#include "iafx.h"
#include "slist.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//-------------------------------
// count elements to end of list
//
unsigned e_slist::offlast() const
{
	unsigned c = 0;
	e_slist *n = next;

	while (n) {
		c++;
		n = n->next;
	}
	
	return c;
}

//-------------------------
// release all list memory
//
void slist::clear()
{
	e_slist *e = first, *n = 0;
	while (e) {
		n = e->next;
		delete e;
		e = n;
	}
	first = 0;
}

//----------------------------
// append element to end list
//
unsigned slist::append(e_slist *e)
{
	unsigned ix = 0;

	if (!first)
		first = e;
	else {
		e_slist *l = first;
		ix++;
		while (l->next) {
			ix++;
			l = l->next;
		}
		l->next = e;
	}

	return ix;
}

//-------------------------------------
// insert at required position
//	if out of bounds insert to nearest
//
void slist::insert(e_slist *e, unsigned pos)
{
	if (!first)
		first = e;
	else {
		e_slist *l = first, *prev = 0;
		while (l->next && pos--) {
			prev = l;
			l = l->next;
		}
		e->next = l;
		if (!prev)
			first = e;
		else
			prev->next = e;
	}
/*	if (!first)
		first = e;
	else {
		e_slist *l = first;
		while (l->next && pos--)
			l = l->next;
		e->next = l;
		if (l == first)
			first = e;
	}
*/
}

//-------------------------
// remove required element
//
void slist::remove(unsigned ix)
{
	ASSERT(ix < numel());
	e_slist *e = first, *p = 0;

	while (ix > 0) {
		e = (p = e)->next;
		ix--;
	}
	if (p)
		p->next = e->next;
	else
		first = e->next;
	delete e;
}

//-------------------------------
// search required element
//	return index or SLIST_INVPOS
//
unsigned slist::seek(void *e) const
{
	register e_slist *s = first;
	register unsigned ix = 0;

	while (s) {
		if (match(s, e))
			return ix;
		s = s->next;
		ix++;
	}

	return SLIST_INVPOS;
}

//----------------------------
// search required element
//	return pointer to element
//
e_slist* slist::seekptr(void* e) const
{
	register e_slist *s = first;

	while (s) {
		if (match(s, e))
			return s;
		s = s->next;
	}

	return 0;
}

//---------------------------
// count numbers of elements
//
unsigned slist::numel() const
{
	unsigned count = 0;
	e_slist *l = first;
	while (l) {
		count++;
		l = l->next;
	}
	return count;
}

//-----------------------------
// return indexed element or 0
//
e_slist* slist::get(unsigned index) const
{
	register e_slist* e = first;

	while (e && index--)
		e = e->next;
	return e;
}

//-------------------------------
// release the item under cursor
//
void slist_scan::delitem()
{
	e_slist *n = lref.first, *p = 0;
	
	// seek current, fixing previous
	while (n) {
		if (n->next == e)
			break;
		n = (p = n)->next;
	}

	ASSERT(n);
	
	if (!p)
		lref.first = n->next;
	else
		p->next = n->next;

	delete n;
}
