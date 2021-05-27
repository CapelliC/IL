
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

#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

#include "iafx.h"
#include "constr.h"
#include "membuf.h"

////////////////////////////////
// define console IO interface
////////////////////////////////

ConsoleEngines::ConsoleEngines()
    : EngineHandlers(&cout) {
}
IntlogExec* ConsoleEngines::build(DbIntlog *pDb) {
    return new ConsoleExec(pDb);
}

ConsoleExec::ConsoleExec(DbIntlog *pDb)
    : IntlogExec(pDb) {
}

///////////////////////////////
// create the stream as output
//
ostream *ConsoleExec::openout(kstring id, int) {
    if (id == user)
        return &cout;

    slist_iter i(membuffer);
    if (find(id, i))
        return new basic_ostringstream<char>();

    return new ofstream(CCP(id));
}

/////////////////////////////////////////////
// create the stream as input from disk file
//
istream *ConsoleExec::openinp(kstring* id, int flags) {
    if (*id == user)
        return &cin;

    if (envar == kstring(MSR_NULL)) {
        const char *pSearch = getenv("ILOCSRC");
        if (pSearch)
            envar = kstring(pSearch);
    }

    filebuf *fb = openfile(*id, flags|ios::in);
    if (!fb) {
        char buf[1 << 16];
        kstring id1(strncat(strncpy(buf, *id, sizeof buf), ".il", sizeof buf));
        if ((fb = openfile(id1, flags|ios::in)) != nullptr)
            *id = id1;
    }

    istream *s = nullptr;
    if (fb) {
        s = new istream(fb);
        //s->delbuf(1);
    }
    return s;
}

void ConsoleExec::close(ostream *s, kstring n) {
    if (n != user) {
        slist_iter i(membuffer);
        membuf *mb = find(n, i);

        if (mb) {
            auto *os = static_cast<basic_ostringstream<char>*>(s);
            *os << ends;

            delete mb->mem;
            mb->mem = strdup(os->rdbuf()->str().c_str());
        }

        delete s;
    }
}
void ConsoleExec::close(istream *s, kstring n) {
    if (n != user)
        delete s;
}
