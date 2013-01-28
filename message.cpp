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
#include "message.h"
#include "eng.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

// global messages table
//MsgTable *MsgTbl;

MsgTable::MsgTable(ostream *s)
{
	list = 0;
	str = s;
}

MsgTable::~MsgTable()
{
	MsgLink *l;

	while (list)
	{
		l = list->link;
		delete list;
		list = l;
	}
}

//--------------------------
// insert a message to list
//	don't check duplicates
//
int MsgTable::add(int code, const char *msg)
{
	MsgLink *l = new MsgLink;
	l->code = code;
	l->string = msg;
	l->link = list;
	list = l;

	return 0;
}

//////////////////////////////////////////
// search a code, starting from requested
//
MsgTable::MsgLink* MsgTable::search(int code, MsgLink *l) const
{
	l = l? l->link : list;

	while (l)
	{
		if (l->code == code)
			break;
		l = l->link;
	}

	return l;
}
