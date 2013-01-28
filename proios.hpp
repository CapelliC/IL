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

/////////// IntlogIOStreams INLINEs ///////////

inline kstring IntlogIOStreams::name()
{
	return user;
}
inline void IntlogIOStreams::setuser(kstring console, kstring searchpath)
{
	user = console;
	envar = searchpath;
}
inline IntlogIOStreams::IntlogIOStreams()
{
	currinp = 0;
	currout = 0;
}
inline int IntlogIOStreams::telling(kstring id) const
{
	return id == currout->ident;
}
inline int IntlogIOStreams::seeing(kstring id) const
{
	return id == currinp->ident;
}
inline IntlogIStream *IntlogIOStreams::ips() const
{
	return currinp;
}
inline ostream& IntlogIOStreams::out() const
{
	return *currout->o;
}
inline IntlogOStream *IntlogIOStreams::ops() const
{
	return currout;
}
inline void IntlogIOStreams::setenvar(kstring var)
{
	envar = var;
}
inline kstring IntlogIOStreams::getenvar()
{
	return envar;
}
inline void IntlogIOStreams::openedbin(kstring)
{
}

/////////// IntlogStream INLINEs ///////////

inline IntlogStream::IntlogStream(kstring id, mode m)
{
	ident = id;
	md = m;
}
inline IntlogStream::mode IntlogStream::get_mode() const
{
	return md;
}
inline kstring IntlogStream::name() const
{
	return ident;
}

/////////// Intlog I/O Stream INLINEs ///////////

inline IntlogIStream::IntlogIStream(kstring id)
: IntlogStream(id, input)
{
}

inline IntlogOStream::IntlogOStream(kstring id, ostream* s)
: IntlogStream(id, output)
{
	o = s;
}
