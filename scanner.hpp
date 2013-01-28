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

#ifdef _DEBUG
#define inline
#endif

inline IntlogScanner::~IntlogScanner()
{
	delete buftok;
}
inline const char* IntlogScanner::Name() const
{
	return id;
}

inline const char* IntlogScanner::Image() const
{
	return buftok;
}
inline int IntlogScanner::Row() const
{
	return rowtok;
}
inline int IntlogScanner::Col() const
{
	return coltok;
}
inline int IntlogScanner::Len() const
{
	return buflen - 1;
}

inline void IntlogScanner::tokenFix()
{
	somechange = 0;
}
inline int IntlogScanner::tokenChanged() const
{
	return somechange;
}

inline void IntlogScanner::clear()
{
	buftok[buflen = overflow = 0] = 0;
}
inline void IntlogScanner::nl()
{
	colcnt = 0;
	rowcnt++;
}

inline int IntlogScanner::issign(ch_t c)
{
	return (chclass[c] & Sign);
}
inline int IntlogScanner::issymbody(ch_t c)
{
	return (chclass[c] & SymBody);
}
inline int IntlogScanner::isnumber(ch_t c)
{
	return (chclass[c] & Digit);
}
inline int IntlogScanner::isnumberh(ch_t c)
{
	return (chclass[c] & DigitH);
}
inline int IntlogScanner::isblank(ch_t c)
{
	return (chclass[c] & Blank);
}
inline int IntlogScanner::issymhead(ch_t c)
{
	return (chclass[c] & SymHead);
}
inline int IntlogScanner::isvarhead(ch_t c)
{
	return (chclass[c] & VarHead);
}
inline int IntlogScanner::isused(ch_t c)
{
	return chclass[c];
}
inline void IntlogScanner::ident()
{
	while (issymbody(ch_t(clc)))
		get();
}
inline void IntlogScanner::SetSource(istream *data, kstring fileId)
{
	i = data;
	id = fileId;
	clc = -2;
}
inline IntlogScanner::operator SourcePos() const
{
	return SourcePos(Row(), Col(), Len());
}
