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

#ifndef COUT_TO_QT_H
#define COUT_TO_QT_H

#include <iostream>
#include <streambuf>
#include "text_widget.h"

//----------------------------------------
// output buffer to QTextEdit
// modified hint from Jochen Ulrich: see
//  http://www.archivum.info/qt-interest@trolltech.com/2005-06/00040/Redirecting-std-cout-to-QTextEdit--Using-QTextEdit-as-a-Log-Window.html
//
class cout_to_qt : public std::basic_streambuf<char>
{
public:
    cout_to_qt(std::ostream &stream, TEXT_WIDGET* text_edit);
    ~cout_to_qt();

protected:
    virtual int_type overflow(int_type v);
    virtual std::streamsize xsputn(const char *p, std::streamsize n);

private:
    std::ostream &m_stream;
    std::streambuf *m_old_buf;
    TEXT_WIDGET* log_window;
};

#endif
