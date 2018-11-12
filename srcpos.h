
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


#ifndef _SRCPOS_H_
#define _SRCPOS_H_

//////////////////////////////////////
// encode position and len in 32 bits
//	14 bits to row number (16K)
//	8 bits to col number (256)
//	10 bits to len (1K)
//
class SourcePos
{
public:

	SourcePos(streampos ipos, unsigned clen);
	SourcePos(unsigned row, unsigned col, unsigned clen);
    SourcePos(long = -1);

	unsigned row() const;
	unsigned col() const;
	unsigned len() const;

	operator long() const;
	bool Used() const;

private:

	enum { MASK_ROW = 0x3FFF,	MASK_COL = 0x00FF,	MASK_LEN = 0x03FF };
	enum { SHIFT_ROW = 18,		SHIFT_COL = 10,		SHIFT_LEN = 0 };

	long m_data;
};

/* don't work with istream (i->tellg() cause character lose...)
inline SourcePos::SourcePos(streampos ipos, unsigned clen)
{
	// leave size up to 1024
	m_data = (ipos << 10) | (clen & 0x3FF);
}
*/
inline SourcePos::SourcePos(long data)
{
	m_data = data;
}
inline SourcePos::SourcePos(unsigned row, unsigned col, unsigned len)
{
	m_data =	(long(row & MASK_ROW) << SHIFT_ROW) |
				(long(col & MASK_COL) << SHIFT_COL) |
				(long(len & MASK_LEN) << SHIFT_LEN);
}

inline unsigned SourcePos::row() const
{
	return unsigned(m_data >> SHIFT_ROW) & MASK_ROW;
}
inline unsigned SourcePos::col() const
{
	return unsigned(m_data >> SHIFT_COL) & MASK_COL;
}
inline unsigned SourcePos::len() const
{
	return unsigned(m_data >> SHIFT_LEN) & MASK_LEN;
}
inline SourcePos::operator long() const
{
	return m_data;
}
inline bool SourcePos::Used() const
{
	return m_data != -1;
}

#endif
