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

#include "cin_to_qt.h"
#include <QDebug>

#include <QTimer>
#include <QEventLoop>

inline void do_events(int delay = 1) {
    QEventLoop lp;
    QTimer::singleShot(delay, &lp, &QEventLoop::quit);
    lp.exec();
}

cin_to_qt::cin_to_qt(std::istream &stream, TEXT_WIDGET* text_edit)
    : stream(stream),
      lastCursorSeen(text_edit->textCursor().position())
{
    con_window = text_edit;
    old_buf = stream.rdbuf();
    stream.rdbuf(this);
    /*
    QObject::connect(text_edit, &TEXT_WIDGET::cursorPositionChanged, [this]() {
        auto p = con_window->textCursor().position();
        con_window->setReadOnly(p < lastCursorSeen);
    });
    */
}

cin_to_qt::~cin_to_qt() {
    qDebug() << "cin_to_qt::~cin_to_qt";

    // pass any input left...
    // ...

    stream.rdbuf(old_buf);
}

cin_to_qt::int_type cin_to_qt::underflow() {
    if (gptr() == NULL || gptr() >= egptr()) {
        int gotted = next_char();
        if (gotted == EOF)
            return traits_type::eof();

        *one_char = gotted;
        setg(one_char, one_char, one_char + 1);
        return traits_type::to_int_type(*one_char);
    }
    return traits_type::to_int_type(*one_char);
}

cin_to_qt::int_type cin_to_qt::sync() {
    qDebug() << "cin_to_qt::sync" << lastCursorSeen;
    return 0;
}

int cin_to_qt::next_char() {
_:  if (!saved.empty()) {
        int c = saved[0];
        saved = saved.substr(1);
        return c;
    }

    auto c = con_window->textCursor();

    c.movePosition(c.End);
    lastCursorSeen = c.position();

    for ( ; ; ) {
        c.setPosition(lastCursorSeen);
        c.movePosition(c.End, c.KeepAnchor);
        auto s = c.selectedText();
        if (s.indexOf(QChar(0x2029)) >= 0) {
            saved = s.replace(QChar(0x2029), QChar('\n')).toStdString();
            goto _;
        }
        do_events();
    }
}
