
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


#include "defsys.h"
#include "bterr.h"
#include "argali.h"
#include "qdata.h"
#include "eng.h"

//--------------------------
// initialize iterator data
//
ArgALIter::ArgALIter(Term arg, aDesc *keys, unsigned nks) {
    vectkeys = keys;
    nkeys = nks;
    if (arg.type(f_LIST)) {
        lptr = arg;
        if (!lptr.LNULL())
            curr = lptr.getarg(0);
        else
            curr.NoTerm();
    } else {
        lptr.NoTerm();
        curr = arg;
    }
}
int ArgALIter::match(CCP key, CCP arg) const {
    return strcmp(key, arg) == 0;
}

//---------------------------------------------------
// get current argument and advance to next (if any)
//  if a pair atom / term is found, check keys
//  if an atom is found, check keys
//
ArgALIter::aType ArgALIter::nextarg(unsigned *ixkey, Term* argp) {
    if (curr.type(f_NOTERM))
        return EndArgs;

    Term tk;
    unsigned ik;

    if (curr.is_expr(Operator::DIV)) {

        if ((tk = curr.getarg(0)).type(f_ATOM)) {
            if ((ik = search(tk)) != unsigned(-1)) {

                TermData tt = vectkeys[ik].tval;
                if (tt == f_NOTERM || (tk = curr.getarg(1)).type(tt))
                    return ret(KeyValue, ixkey, ik, argp, curr.getarg(1));

                return ret(KeyErr, ixkey, ik, argp, curr.getarg(1));
             }
        }

        return ret(Pair, ixkey, unsigned(-1), argp, curr);

    } else if (curr.type(f_ATOM))
        if ((ik = search(curr)) != unsigned(-1))
            return ret(Key, ixkey, ik, argp, curr);

    return ret(Single, ixkey, unsigned(-1), argp, curr);
}

//----------------------------------------
// returning argument: check for pointers
//  and advance to next arg
//
ArgALIter::aType ArgALIter::ret(aType rc,
    unsigned *ptrix, unsigned ix, Term *ptrarg, Term arg) {
    if (ptrix)
        *ptrix = ix;
    if (ptrarg)
        *ptrarg = arg;

    curr.NoTerm();
    if (lptr.type(f_LIST) && !lptr.LNULL()) {
        lptr = lptr.getarg(1);
        if (lptr.type(f_LIST) && !lptr.LNULL())
            curr = lptr.getarg(0);
    }

    return rc;
}

//-------------------------
// linear search in vector
//
unsigned ArgALIter::search(CCP arg) const {
    for (unsigned k = 0; k < nkeys; k++)
        if (match(vectkeys[k].idkey, arg))
            return k;

    return unsigned(-1);
}

Term ArgALIter::get_root() const {
    if (lptr.type(f_LIST))
        return lptr;
    return curr;
}

//---------------------
// copy term on create
//
ArgIdArityList::ArgIdArityList(Term t, IntlogExec *p, CCP id)
    : ArgALIter(p->copy(t)) {
    caller = id;
    status = 1;
    counter = 0;

    root = get_root();
    if (root.type(f_NOTERM))
        errmsg();
}

//--------------------
// delete copied term
//
ArgIdArityList::~ArgIdArityList() {
    root.Destroy();
}

//-------------------------------------
// access next arg (0 on fail/EndArgs)
//
int ArgIdArityList::next() {
    Term a;

    switch (nextarg(nullptr, &a)) {

    case Key:
    case KeyValue:
    case KeyErr:
        break;

    case Pair:
        if (!a.getarg(0).type(f_ATOM|f_INT)) {
            errmsg();
            break;
        }
        funct = a.getarg(0).kstr();
        arity = Int(a.getarg(1));
        counter++;
        return 1;

    case Single:
        if (!a.type(f_ATOM)) {
            errmsg();
            break;
        }
        arity = -1;
        funct = a.kstr();
        counter++;
        return 1;

    case EndArgs:
        if (counter == 0)
            errmsg();
    }

    return 0;
}

//--------------------------------
// show error message on argument
//
void ArgIdArityList::errmsg(int code, CCP s1, CCP s2) {
    if (!code)
        code = BTERR_INVALID_ARG_TYPE;
    GetEngines()->ErrMsg(code, caller, s1, s2);
    status = 0;
}
