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

#ifndef _SyntaxColor_h_
#define _SyntaxColor_h_

#include "scanner.h"

#define SCAN_BUFFER_LEN 1024

class SyntaxColor : public IntlogScanner
{
public:

    SyntaxColor(ostream &f);
    void Format(istream *ps, bool bDefTable, int nHeight = -1);

    void OpenTable();
    void SetHeight(int h);
    void CloseTable();

    ostream &m_file;

    // user configurable colors data handling
    static void GetDataToSave(ostream &s);
    static bool SetSaveToData(istream &s);

protected:

    enum colorTag {
        Atom,
        Number,
        Var,
        String,
        Comment,
        SepChar,
        __maxTag__
    };
    virtual void useInput(Codes c);
    void color(colorTag c);
    void body();
    void enter(colorTag c);

    // user configurable data handling
    struct aColor {
        const char *userId;
        COLORREF value;
    };
    static aColor _colorsDef[__maxTag__];
    static string _colorsStr;
    static void Table2String();
};

inline SyntaxColor::SyntaxColor(ostream &file)
    : m_file(file),
      IntlogScanner(SCAN_BUFFER_LEN)
{
}
inline void SyntaxColor::CloseTable()
{
    m_file << '}';
}

#endif
