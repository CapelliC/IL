
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


#ifndef BTMIX_H_
#define BTMIX_H_

extern BuiltIn mixing[16];

//---------------------------------------
// time representation as user data test
//
class mtime : public SysData
{
public:

	mtime(time_t t = time_t(0));
	void show(ostream &s) const;
	char *format(char *buf, int maxbuf, const char *fmt = 0) const;

	int copy_able() const;
	Term copy() const;

	time_t get() const;
	int delete_able() const;

	static mtime* get_data(Term);

private:
	static rtinfo rti;
	time_t tval;
};

inline mtime::mtime(time_t t)
	: SysData(rti)
{
	tval = t;
}
inline void mtime::show(ostream &s) const
{
	char buf[1<<10];
	s << format(buf, sizeof buf);
}
inline int mtime::copy_able() const
{
	return 1;
}
inline time_t mtime::get() const
{
	return tval;
}
inline int mtime::delete_able() const
{
	return 1;
}

#endif
