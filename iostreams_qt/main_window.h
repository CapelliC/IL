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

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>

#include "cout_to_qt.h"
#include "cin_to_qt.h"

/** keep informing stream handlers */
class main_window : public QMainWindow
{
    Q_OBJECT
public:
    explicit main_window(QWidget *parent = Q_NULLPTR);
    ~main_window();

signals:

public slots:

protected:
    iostream_widget *ed;
    cin_to_qt *in;
    cout_to_qt *out;
    virtual void closeEvent(QCloseEvent *event);
};

#endif // MAIN_WINDOW_H
