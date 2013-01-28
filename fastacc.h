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

#ifndef _FASTACC_H_
#define _FASTACC_H_

// keep these dimensions to power of two to speed access
#define VCTDIM		16
#define VCTDIM_PATT	0x0F
#define VCTDIM_NBIT	4

#define BLKDIM		32
#define BLKDIM_PATT	0x1F
#define BLKDIM_NBIT	5

#define OPMOD(v, k) ((v) & k##_PATT)
#define OPDIV(v, k) ((v) >> k##_NBIT)
#define OPMUL(v, k) ((v) << k##_NBIT)

// port to 64 bit: define pointer size adeguately
#define BUILD_64 1

#define SL_DOUBLE 3
#if BUILD_64
  #define SL_PTR 3
  #define SL_PTR_P 6
#else
  #define SL_PTR 2
  #define SL_PTR_P 3
#endif

#endif
