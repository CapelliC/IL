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

#ifndef CIN_TO_QT_H
#define CIN_TO_QT_H

#include <iostream>
#include <streambuf>
#include "iostream_widget.h"

//-------------------------------
// input buffer from QTextEdit
//
class cin_to_qt : public std::basic_streambuf<char>
{
public:
    cin_to_qt(std::istream &stream, iostream_widget* text_edit);
    ~cin_to_qt();

protected:
    virtual int_type underflow();
    virtual int_type sync();

private:
    std::istream &stream;
    std::streambuf *old_buf;

    iostream_widget* con_window;
    int last_cursor_seen;

    char one_char[1];
    int next_char();
    std::string saved;
};

#endif
