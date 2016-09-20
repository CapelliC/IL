
/*
    IL : Intlog Language
    Object Oriented Prolog Project
    Copyright (C) 1992-2016 - Ing. Capelli Carlo

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


#include "stdafx.h"
#include "btswi0.h"
#include "qdata.h"

#include <dirent.h>
#include <stdio.h>

BtFDecl(cd);
BtFDecl(ls);
BtFDecl(pwd);

BuiltIn btswi0[3] = {
    {"cd",  0,   cd},
//    {"cd",  1,   cd},
    {"ls",  0,   ls},
    {"pwd", 0,   pwd},
};

BtFImpl(cd, t, p) {
    //if (t.getarg(0))
        //;
    return 0;
}
BtFImpl_P1(ls, p) {
    if (DIR *d = opendir(".")) {
        struct dirent *dir;
        while ((dir = readdir(d)) != nullptr)
            p->out() << dir->d_name << endl;
        closedir(d);
        return 1;
    }
    return 0;
}
BtFImpl_P1(pwd, p) {
    return 0;
}
