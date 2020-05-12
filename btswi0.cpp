
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
    {"length_",  2|BuiltIn::retry, length},
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
    between_range(int a, int b) : a(a), b(b) {}
    int a, b;
    ~between_range() override;
};
between_range::~between_range() {}

// test
// ?- between(1,3,X).
// ?- findall(a(X,Y), (between(1,3,X), between(1,X,Y)), L).
BtFImpl_R(between, t, p, r) {
    if (!r) {
        auto
            a = p->eval_term(t.getarg(0)),
            b = p->eval_term(t.getarg(1));
        if (a.type(f_INT) && b.type(f_INT)) {
            Int A = Int(a), B = Int(b);
            auto c = p->eval_term(t.getarg(2));
            if (c.type(f_NOTERM)) {
                p->set_btdata(p->save(new between_range(A, B)));
                return p->unify(t.getarg(2), a);
            }
            if (c.type(f_INT)) {
                Int C = Int(c);
                return C >= A && C <= B;
            }
        }
        p->BtErr(BTERR_INVALID_ARG_TYPE, "between");
        return 0;
	}
    else {
        auto pd = dynamic_cast<between_range*>(p->get_btdata());
        if (++pd->a <= pd->b)
            return p->unify(Int(pd->a), t.getarg(2));
    }

    return 0;
}

BtFTBD(nth1)
BtFTBD(nth0)

struct length_gen : public BltinData
{
    length_gen(int length) : length(length) {}
    long length;
    int list(IntlogExec *p, Term inStack, Term tLen) const {
        auto lvar = Term(ListNULL);
        for (auto clen = length; clen > 0; clen--)
            lvar = Term(Term(ANONYM_IX), lvar);
        return p->unify(tLen, Int(length)) && p->unify_gen(inStack, lvar);
    }
    ~length_gen() override;
};
length_gen::~length_gen() {}

// ?- length([1,2,3],X).
// ?- length(L,3).
// ?- length(X,Y).
BtFImpl_R(length, t, p, r) {
    auto A1 = t.getarg(0), A2 = t.getarg(1);
    if (!r) {
        auto list = p->eval_term(A1);
        if (list.type(f_LIST)) {
            int len = 0;
            while (!list.LNULL()) {
                ++len;
                list = p->eval_term(list.getarg(1));
            }
            return p->unify(Int(len), A2);
        }
        if (list.type(f_NOTERM)) {
            auto len = p->eval_term(A2);
            if (len.type(f_INT)) {
                return length_gen(len).list(p, A1, A2);
            }
        }
        if (A2.type(f_VAR)) {
            auto g = new length_gen(0);
            p->set_btdata(p->save(g));
            return g->list(p, A1, A2);
        }
        p->BtErr(BTERR_INVALID_ARG_TYPE, "length");
        return 0;
    }

    auto g = dynamic_cast<length_gen*>(p->get_btdata());
    g->length++;
    return g->list(p, A1, A2);
}
