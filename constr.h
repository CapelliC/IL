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

#ifndef _CONSTR_H_
#define _CONSTR_H_

////////////////////////////////
// define console IO interface
////////////////////////////////

#include "eng.h"
#include "query.h"

class ConsoleEngines : public EngineHandlers
{
public:
	ConsoleEngines();
protected:
	virtual IntlogExec *build(DbIntlog* pDb);
};

class ConsoleExec : public IntlogExec
{
public:
	ConsoleExec(DbIntlog *pDb);

protected:

	virtual ostream *openout(kstring id, int flags);
	virtual void close(ostream* s, kstring id);

	virtual istream *openinp(kstring *, int flags);
	virtual void close(istream*, kstring id);
};

#endif
