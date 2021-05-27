
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

#ifndef OPERATOR_H_
#define OPERATOR_H_


////////////////////////////////////////////
// Operators table elements
//  declare predefined tag
//  allowing for insertion of user defined
//

#include "mycont.h"

class Operator {
public:

    enum opCodeTag {    // code
        RULE,
        QUERY,
        OR,
        AND,
        SPY,
        NOSPY,
        NOT,
        IS,
        ASS,
        UNIV,
        EQ,
        NE,
        LT,
        LE,
        GE,
        GT,
        STRICTEQ,
        STRICTNE,
        SUB,
        ADD,
        DIV,
        MUL,
        MOD,
        POT,
        PATHNAME,

        RULEUNARY,  // these alias name
        SUBUNARY,
        ADDUNARY,

        I_BEGIN,
        I_END,
        I_INHERIT,
        I_IMPORT,
        I_EXPORT,
        I_DYNAMIC,
        I_HANDLER,

        I_CREATE,
        I_CALL,
        I_GCALL,
        I_DESTROY,

        I_PROPLIST,
        I_ISA,

        UserDef     // defined via op/3
    };

    enum assocTag {     // associativity
        XF, FX, YF, FY, XFX, YFY, XFY, YFX
    };

    kstring     name;       // user input string
    opCodeTag   opCode;     // code
    short       prec;       // precedence
    assocTag    assoc;      // associativity
};

/////////////////////////////////////////////
// mainly, an operator is checked using IsOp
//
class OperTable : vect<Operator> {
public:
    Operator* IsOp(kstring) const;
    Operator* GetNext(Operator*) const;
    void Add(kstring, short, Operator::assocTag, Operator::opCodeTag);
};

#endif
