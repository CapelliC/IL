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

#include "cin_to_qt.h"
#include <QDebug>
#include <QTimer>
#include <QEventLoop>

inline void do_events(int delay = 1) {
    QEventLoop lp;
    QTimer::singleShot(delay, &lp, &QEventLoop::quit);
    lp.exec();
}

cin_to_qt::cin_to_qt(std::istream &stream, iostream_widget* text_edit)
    : stream(stream),
      last_cursor_seen(text_edit->textCursor().position())
{
    con_window = text_edit;
    old_buf = stream.rdbuf();
    stream.rdbuf(this);

    QObject::connect(text_edit, &iostream_widget::destroyed, [this]() {
        qDebug() << "cin_to_qt:destroyed";
        con_window = nullptr;
    });
}

cin_to_qt::~cin_to_qt() {
    // should pass any input left ?
    stream.rdbuf(old_buf);
}

cin_to_qt::int_type cin_to_qt::underflow() {
    if (gptr() == nullptr || gptr() >= egptr()) {
        int c = next_char();
        if (c == EOF)
            return traits_type::eof();
        *one_char = static_cast<char>(c);
        setg(one_char, one_char, one_char + 1);
        return traits_type::to_int_type(*one_char);
    }
    return traits_type::to_int_type(*one_char);
}

cin_to_qt::int_type cin_to_qt::sync() {
    qDebug() << "cin_to_qt::sync" << last_cursor_seen;
    return 0;
}

int cin_to_qt::next_char() {

_:  if (!con_window)
        return EOF;

    if (!saved.empty()) {
        int c = saved[0];
        saved = saved.substr(1);
        return c;
    }

    // flush output
    con_window->out_string();

    auto c = con_window->textCursor();

    c.movePosition(c.End);
    last_cursor_seen = c.position();

    for ( ; ; ) {
        if (!con_window)
            return EOF;
        c.setPosition(last_cursor_seen);
        c.movePosition(c.End, c.KeepAnchor);
        auto s = c.selectedText();
        if (s.indexOf(QChar(0x2029)) >= 0) {
            saved = s.replace(QChar(0x2029), QChar('\n')).toStdString();
            goto _;
        }
        do_events();
    }
}
