
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


#ifndef ARGALI_H_
#define ARGALI_H_

#include "term.h"
#include "query.h"

//--------------------------------------------------
// a class to iterate arguments of form like spy...
//
class ArgALIter {
public:
    struct aDesc {
        CCP idkey;
        TermData tval;
    };

    ArgALIter(Term, aDesc* = nullptr, unsigned = 0);
    virtual ~ArgALIter() {}

    enum aType {
        Key,
        KeyValue,
        KeyErr,
        Pair,
        Single,
        EndArgs
    };

    aType nextarg(unsigned* = nullptr, Term* = nullptr);

    virtual int match(CCP key, CCP arg) const;

protected:
    Term get_root() const;

private:
    Term curr;
    Term lptr;
    aDesc *vectkeys;
    unsigned nkeys;

    unsigned search(CCP) const;
    aType ret(aType, unsigned *, unsigned, Term*, Term);
};

//----------------------------------
// iterate a common argument schema
//  identifier
//  identifier / arity
//
class ArgIdArityList : public ArgALIter {
public:

    // copy term on create
    ArgIdArityList(Term, IntlogExec *, CCP);

    // delete copied term
    virtual ~ArgIdArityList();

    // access next arg (0 on fail/EndArgs)
    int next();

    // display error message with some other info
    void errmsg(int = 0, CCP = nullptr, CCP = nullptr);

    // nth-argument functor
    kstring funct;

    // nth-argument arity (-1 if none)
    int arity;

    // 1 if OK, 0 else
    int status;

    // track OK arguments
    unsigned counter;

private:
    CCP caller;
    Term root;
};

#endif
