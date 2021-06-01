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

#include <QApplication>
#include <QMainWindow>
#include <QMessageBox>

#include "iafx.h"
#include "constr.h"
#include "qdata.h"
#include "parse.h"

#include "main_window.h"
#include "deblog.h"

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    main_window mw;
    mw.show();
//deblog dl(string(argv[0]) + ".log");

    try {
        SetEngines(new ConsoleEngines);
        IntlogExec *eng = GetEngines()->HtoD(GetEngines()->create());

        for (int arg = 1; arg < argc; arg++) {
            CCP parg = argv[arg];

            if (parg[0] == '-') {
                switch (parg[1]) {
                case 'e':
                    IntlogIOStreams::envar = kstring(parg + 2);
                    break;
                case 'u':
                    eng->use_file("user");
                    break;
                case 'q':
                    eng->runquery(parg + 2);
                }
            }
            else
                eng->use_file(kstring(parg));
        }

        if (argc == 1)
            eng->use_file("user");

        delete GetEngines();
    }
    catch(std::exception &e) {
        QMessageBox::critical(&mw, "IL exception", e.what());
    }
}
