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

#ifndef _IAFX_H_
#define _IAFX_H_

#if defined(_IAFX_BUILD)
	#define IAFX_API _declspec(dllexport)
#elif defined(_IAFX_USE)
	#define IAFX_API _declspec(dllimport)
#else
	#define IAFX_API
#endif

#include "term.h"
#include "eng.h"

#define IIFTYPE(type) extern "C" type IAFX_API

// run time interface access
IIFTYPE(void) IAFX_Initialize();
IIFTYPE(void) IAFX_Terminate();

// query interface utilities
IIFTYPE(int) IAFX_QueryString(const char*, EngHandle, Term**, kstr_list*);
IIFTYPE(int) IAFX_QueryTerm(Term, EngHandle, Term*, kstr_list*);

// port to 64 bit: define pointer size adeguately
#define BUILD_64 1

#endif
