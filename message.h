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

#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include "iafx.h"

/////////////////////
// a messages table
//
class IAFX_API MsgTable
{
public:
	MsgTable(ostream *);
	~MsgTable();

	int add(int code, const char *msg);
	ostream *setoutput(ostream *);

	struct MsgLink
	{
		const char* string;
		int code;
		MsgLink* link;
	};
	MsgLink* search(int code, MsgLink *from = 0) const;

	ostream* str;
	MsgLink* list;
};

//////////////////////////
// change current stream
//
inline ostream *MsgTable::setoutput(ostream *os)
{
	ostream *s = str;
	str = os;
	return s;
}

#endif
