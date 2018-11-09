
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
#include "bterr.h"
#include "qdata.h"

#include <dirent.h>
#include <unistd.h>

BtFDecl(cd);
BtFDecl(ls);
BtFDecl(pwd);
BtFDecl(between);
BtFDecl(nth1);
BtFDecl(nth0);
BtFDecl(length);


/*
std::array<BuiltIn, 3> btswi0 = {
    {"cd",  1,   cd},
    {"ls",  1,   ls},
    {"pwd", 0,   pwd}
};
*/
BuiltIn btswi0[7] = {
    {"cd",      1,   cd},
    {"ls",      1,   ls},
    {"pwd",     0,   pwd},
    {"between", 3|BuiltIn::retry, between},
    {"nth1",    3|BuiltIn::retry, nth1},
    {"nth0",    3|BuiltIn::retry, nth0},
    {"length",  2|BuiltIn::retry, length},
};

BtFImpl(cd, t, p) {
    auto a = t.getarg(0);
    if (a.type() != f_NOTERM) {
        CCP q = p->eval_term(a);
        if (q && chdir(q) == 0)
            return 1;
    }
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
    char buf[1 << 12];
    if (char *d = getcwd(buf, sizeof buf)) {
        p->out() << d << endl;
        return 1;
    }
    return 0;
}

struct between_range : public BltinData
{
    int a, b;
    ~between_range() override;
};
between_range::~between_range() {}

BtFImpl_R(between, t, p, r) {
    between_range *pd;
    if (!r)
	{
        auto
            a = p->eval_term(t.getarg(0)),
            b = p->eval_term(t.getarg(1)),
            c = p->eval_term(t.getarg(2));
        if (a.type(f_INT) && b.type(f_INT)) {
            pd = new between_range;
            pd->a = a;
            pd->b = b;
            return p->unify(c, c);
        }
        p->BtErr(BTERR_INVALID_ARG_TYPE, "between");
        return 0;
	}
    return 0;
}

BtFTBD(nth1)
BtFTBD(nth0)

BtFImpl_R(length, t, p, r) {
    auto list = p->eval_term(t.getarg(0)), len = p->eval_term(t.getarg(1));
    return p->eval_term(t.getarg(0)).type(f_ATOM|f_INT|f_DOUBLE);
    //if (list.type())
    //auto v = list.operator Var();
}
