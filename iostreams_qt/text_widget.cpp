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

#include "text_widget.h"

TEXT_WIDGET::TEXT_WIDGET(QWidget *parent) : TEXT_WIDGET_SUPER(parent) {
    fixedPosition = 0;
}
TEXT_WIDGET::TEXT_WIDGET(const QString &text, QWidget *parent) : TEXT_WIDGET_SUPER(text, parent) {
    fixedPosition = 0;
}

/** strict control on keyboard events required */
void TEXT_WIDGET::keyPressEvent(QKeyEvent *event) {
    using namespace Qt;
    bool ctrl = event->modifiers() == CTRL;
    QTextCursor c = textCursor();
    int cp = c.position(), k = event->key();
    bool accept = true, /*ret = false, down = true,*/ editable = cp >= fixedPosition;

    switch (k) {
    case Key_Home:
        if (!ctrl && cp > fixedPosition) {
            c.setPosition(fixedPosition, (event->modifiers() & SHIFT) ? c.KeepAnchor : c.MoveAnchor);
            setTextCursor(c);
            return;
        }
        break;

    case Key_End:
    case Key_Left:
    case Key_Right:
    case Key_Up:
    case Key_Down:
    case Key_PageUp:
    case Key_PageDown:
        break;

    case Key_Backspace:
        accept = (cp > fixedPosition);
        break;

    default:
        accept = editable || event->matches(QKeySequence::Copy);
    }

    if (accept)
        TEXT_WIDGET_SUPER::keyPressEvent(event);
}

void TEXT_WIDGET::add_string(QString s) {
    auto c = textCursor();
    c.movePosition(c.End);
    c.insertText(s);
    c.movePosition(c.End);
    fixedPosition = c.position();
    setTextCursor(c);
}
