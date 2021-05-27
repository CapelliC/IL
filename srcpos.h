
/*
    IL : Intlog Language
    Object Oriented Prolog Project
    Copyright (C) 1992-2021 - Ing. Capelli Carlo

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


#ifndef SRCPOS_H_
#define SRCPOS_H_

#include <iostream>
#include <cstdint>
using namespace std;

//////////////////////////////////////
// encode position and len in 32 bits
//  16 bits to row number (64K)
//   8 bits to col number
//   8 bits to len
//
class SourcePos {
public:

    typedef uint32_t sp_data;
    static const sp_data sp_data_null = sp_data(-1);

    SourcePos(streampos ipos, unsigned clen);
    SourcePos(unsigned row, unsigned col, unsigned clen);
    SourcePos(sp_data sp = sp_data_null);

    unsigned row() const;
    unsigned col() const;
    unsigned len() const;

    operator sp_data() const;
    bool Used() const;

private:

    enum {
        MASK_ROW = sp_data(0x0000FFFF),
        MASK_COL = sp_data(0x000000FF),
        MASK_LEN = sp_data(0x000000FF)
    };
    enum {
        SHIFT_ROW = 8+8,
        SHIFT_COL = 8,
        SHIFT_LEN = 0
    };

    sp_data m_data;
};

inline SourcePos::SourcePos(sp_data data) {
    m_data = data;
}
inline SourcePos::SourcePos(unsigned row, unsigned col, unsigned len) {
    m_data =
        (sp_data(row & MASK_ROW) << SHIFT_ROW) |
        (sp_data(col & MASK_COL) << SHIFT_COL) |
        (sp_data(len & MASK_LEN) << SHIFT_LEN);
}

inline unsigned SourcePos::row() const {
    return unsigned(m_data >> SHIFT_ROW) & MASK_ROW;
}
inline unsigned SourcePos::col() const {
    return unsigned(m_data >> SHIFT_COL) & MASK_COL;
}
inline unsigned SourcePos::len() const {
    return unsigned(m_data >> SHIFT_LEN) & MASK_LEN;
}
inline SourcePos::operator sp_data() const {
    return m_data;
}
inline bool SourcePos::Used() const {
    return m_data != sp_data_null;
}

#endif
