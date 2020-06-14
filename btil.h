
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


#ifndef BTIL_H_
#define BTIL_H_

#include "eng.h"
#include "iafx.h"

extern BuiltIn interface_obj[13*2-1];

class IAFX_API EngineObj: public SysData
{
public:
	EngineObj(EngHandle);
	EngHandle get_handler() const;

	void show(ostream &) const;

	int copy_able() const { return 1; }
	Term copy() const;
	int delete_able() const { return 1; }
	int unify(Term) const;
	ostream &save(ostream &);
	istream &load(istream &);

	static int is_eng(Term);
	static EngineObj* get_eng(Term);

private:
	static rtinfo rti;
	EngHandle handler;
};

inline EngHandle EngineObj::get_handler() const
{
	return handler;
}
inline ostream &EngineObj::save(ostream &s)
{
	return s << handler;
}
inline istream &EngineObj::load(istream &s)
{
	return s >> handler;
}

#endif
