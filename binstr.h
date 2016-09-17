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

#ifndef _BINSTR_H_
#define _BINSTR_H_

void bswrite(ostream &, unsigned);
void bswrite(ostream &, short);
void bswrite(ostream &, long);
void bswrite(ostream &, float);
void bswrite(ostream &, double);
void bswrite(ostream &, const char *);

void bsread(istream &, unsigned *);
void bsread(istream &, short *);
void bsread(istream &, long *);
void bsread(istream &, float *);
void bsread(istream &, double *);
void bsread(istream &, char *, unsigned*);
void bsread(istream &, char **);

//#define TYPE_WRITE_1(x) (unsigned char *)x
#define TYPE_WRITE_1(p) (const char *)(p)	//PVOID(p) // 
#define TYPE_READ_1(p)	(char*)(p)	//PVOID(p) //

inline void bswrite(ostream &s, unsigned p)
{
	s.write(TYPE_WRITE_1(&p), sizeof(p));
}
inline void bswrite(ostream &s, short p)
{
	s.write(TYPE_WRITE_1(&p), sizeof(p));
}
inline void bswrite(ostream &s, long p)
{
	s.write(TYPE_WRITE_1(&p), sizeof(p));
}
inline void bswrite(ostream &s, float p)
{
	s.write(TYPE_WRITE_1(&p), sizeof(p));
}
inline void bswrite(ostream &s, double p)
{
	s.write(TYPE_WRITE_1(&p), sizeof(p));
}


inline void bsread(istream &s, unsigned *p)
{
	s.read(TYPE_READ_1(p), sizeof(*p));
}
inline void bsread(istream &s, short *p)
{
	s.read(TYPE_READ_1(p), sizeof(*p));
}
inline void bsread(istream &s, long *p)
{
	s.read(TYPE_READ_1(p), sizeof(*p));
}
inline void bsread(istream &s, float *p)
{
	s.read(TYPE_READ_1(p), sizeof(*p));
}
inline void bsread(istream &s, double *p)
{
	s.read(TYPE_READ_1(p), sizeof(*p));
}

#endif
