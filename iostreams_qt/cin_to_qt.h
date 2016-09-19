/*
    IL : Intlog Language
    Object Oriented Prolog Project
    Copyright (C) 2016-2016 - Ing. Capelli Carlo

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
#include "text_widget.h"

//-------------------------------
// input buffer from QTextEdit
//
class cin_to_qt : public std::basic_streambuf<char>
{
public:
    cin_to_qt(std::istream &stream, TEXT_WIDGET* text_edit);
    ~cin_to_qt();

protected:
    virtual int_type underflow();
    virtual int_type sync();

private:
    std::istream &stream;
    std::streambuf *old_buf;

    TEXT_WIDGET* con_window;
    int lastCursorSeen;

    /*
    char_type *p_editable_input_area = 0;
    int_type l_editable_input_area;
    //char inputBuffer_[1];
    */
    char one_char[1];
    int next_char();
    std::string saved;
};

#endif
