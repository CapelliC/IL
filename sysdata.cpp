/*
    IL : Prolog interpreter
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

/*
	Intlog Language
	Object Oriented Prolog Project
	Ing. Capelli Carlo - Brescia
*/

#include "stdafx.h"
#include "defsys.h"
#include "parsemsg.h"
#include "eng.h"

/////// SYSDATA ////////

IAFX_API ostream& operator<<(ostream& s, SysData& d)
{
	s << d.m_key.id << '/'; d.save(s) << ':';
	return s;
}
IAFX_API istream& operator>>(istream& s, SysData*& d)
{
	char k[20], c;

	if (s.get(k, sizeof(k), '/') >> c && c == '/') {
		SysData::rtinfo *p = SysData::reglist;
		for ( ; p; p = p->link)
			if (!strcmp(k, p->id)) {
				d = p->build();
				d->load(s) >> c;
				if (c != ':')
					GetEngines()->ErrMsg(INVALID_SERIAL_DATA);
				break;
			}
		if (!p)
			GetEngines()->ErrMsg(SERIAL_FMT_NOT_FOUND, k);
	}

	return s;
}

SysData::rtinfo *SysData::reglist;

SysData::SysData(rtinfo &key)
: m_key(key)
{
	for (rtinfo *p = reglist; p; p = p->link)
		if (p->id == key.id)
			return;

	// chain type descriptor
	key.link = reglist;
	reglist = &key;
}
