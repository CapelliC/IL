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

#include "stdafx.h"
#include "binstr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

void bswrite(ostream &s, const char *sz)
{
	unsigned ls = strlen(sz);
	bswrite(s, ls);
	s.write(sz, ls);
}

void bsread(istream &s, char *sz, unsigned *lmax)
{
	unsigned ls;
	bsread(s, &ls);
	if (ls < *lmax) {
		s.read(sz, ls);
		sz[*lmax = ls] = 0;
	} else
		s.clear(ios::failbit);
}
void bsread(istream &s, char **sz)
{
	unsigned ls;
	bsread(s, &ls);
	*sz = new char[ls + 1];
	s.read(*sz, ls);
	(*sz)[ls] = 0;
}