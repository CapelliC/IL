
/*
    IL : Intlog Language
    Object Oriented Prolog Project
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


#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

#include <assert.h>
#define ASSERT assert

#define BOOL bool
#define UINT unsigned
#define FALSE false
#define TRUE true

#define BASED_CODE
#define DEBUG_NEW new

#define RGB(r, g, b) (r << 16 | g << 8 | b)
#define COLORREF UINT
#define GetRValue(c) ((c >> 16) & 15)
#define GetGValue(c) ((c >> 8) & 15)
#define GetBValue(c) ((c >> 0) & 15)

#define LPCSTR const char*
#define DECLARE_MESSAGE_MAP()
