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

#include "cout_to_qt.h"

cout_to_qt::cout_to_qt(std::ostream &stream, iostream_widget* text_edit)
    : m_stream(stream)
{
    log_window = text_edit;
    m_old_buf = stream.rdbuf();
    stream.rdbuf(this);
}
cout_to_qt::~cout_to_qt() {
    m_stream.rdbuf(m_old_buf);
}
cout_to_qt::int_type cout_to_qt::overflow(int_type v) {
    log_window->add_string(QString(QChar(v)));
    return v;
}
std::streamsize cout_to_qt::xsputn(const char *p, std::streamsize n) {
    log_window->add_string(QString::fromStdString(std::string(p, p + n)));
    return n;
}
