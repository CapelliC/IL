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

#ifndef IOSTREAM_WIDGET_H
#define IOSTREAM_WIDGET_H

#include <QPlainTextEdit>
#define IOSTREAM_WIDGET_BASE QPlainTextEdit

class iostream_widget : public IOSTREAM_WIDGET_BASE {

    Q_OBJECT

public:

    explicit iostream_widget(QWidget *parent = Q_NULLPTR);
    explicit iostream_widget(const QString &text, QWidget *parent = Q_NULLPTR);

    void add_string(QString s);

protected:

    /** strict control on keyboard events required */
    virtual void keyPressEvent(QKeyEvent *event);

    /** flushing buffer */
    virtual void timerEvent(QTimerEvent *event);

private:

    int fixedPosition;
    QString buffer;
    void setup();

public slots:

    void out_string();
};

#endif // IOSTREAM_WIDGET_H
