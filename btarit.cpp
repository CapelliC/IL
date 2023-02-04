
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


//---------------------
// arithmetic builtins
//---------------------

#include "bterr.h"
#include "qdata.h"
#include "aritval.h"

BtFDecl(is);
BtFDecl(less_);
BtFDecl(great);
BtFDecl(lessequ);
BtFDecl(greatequ);

BuiltIn arithmetic[6] = {
    {"is",      2,  is},
    {":=",      2,  is},
    {"<",       2,  less_},
    {">",       2,  great},
    {"=<",      2,  lessequ},
    {">=",      2,  greatequ}
};

////////////////////////////////
// normalize types by promotion
//
int AritValue::compatible(AritValue &r) {
    if (t == IVAL) {
        if (r.t == DBL) {
            d = Double(i);
            t = DBL;
        }
        else if (r.t != IVAL)
            return 0;
    }
    else if (r.t == IVAL) {
        if (t == DBL) {
            r.d = Double(r.i);
            r.t = DBL;
        }
        else if (t != IVAL)
            return 0;
    }

    return t == r.t;
}

////////////////////////////////////
// evaluate the argument as integer
//
int AritValue::strlist_int(Term v, Int &i) {
    const List &l = v;

    if (l.head().type(f_INT) && l.tail().LNULL()) {
        i = Int(l.head());
        return 1;
    }

    return 0;
}

void AritValue::evalexpr(Term e, IntlogExec *p) {
    t = ERR;

    switch (e.type()) {
    case f_INT:
        i = Int(e);
        t = IVAL;
        break;

    case f_DOUBLE:
        d = Double(e);
        t = DBL;
        break;

    case f_ATOM:
        a = e;
        t = ATOM;
        break;

    case f_VAR: {
        Term v = p->tval(Var(e));
        if (v.type(f_INT)) {
            i = Int(v);
            t = IVAL;
            break;
        }
        if (v.type(f_DOUBLE)) {
            d = Double(v);
            t = DBL;
            break;
        }
        if (v.type(f_ATOM)) {
            a = v;
            t = ATOM;
            break;
        }
        if (v.type(f_VAR|f_STRUCT)) {
            // try to accept expressions
            evalexpr(v = p->copy(e), p);
            v.Destroy();
            break;
        }

        p->BtErr(BTERR_NOT_VALID_VAR);
        break;
    }

    case f_STRUCT: {
        AritValue l, r;

        if (e.is_expr(Operator::SUBUNARY, 1)) {
            evalexpr(e.getarg(0), p);
            if (t == ERR)
                break;

            if (t == IVAL)
                i = -i;
            else if (t == DBL)
                d = -d;
            else {
                p->BtErr(BTERR_INVALID_ARG_TYPE);
                t = ERR;
            }
            break;
        }

        if (e.is_expr(Operator::ADDUNARY, 1)) {
            evalexpr(e.getarg(0), p);
            break;
        }

        l.evalexpr(e.getarg(0), p);
        if (l.t == ERR)
            break;

        r.evalexpr(e.getarg(1), p);
        if (r.t == ERR)
            break;

        // normalize types by promotion
        if (!l.compatible(r)) {
            p->BtErr(BTERR_INVALID_ARG_TYPE);
            break;
        }

        unsigned opc = unsigned(e.get_funct());
        CCP ops = e.get_funct();

        if (l.t == IVAL)
            switch (opc) {
            case Operator::ADD:
                i = l.i + r.i;
                break;
            case Operator::SUB:
                i = l.i - r.i;
                break;
            case Operator::MUL:
                i = l.i * r.i;
                break;
            case Operator::DIV:
                if (r.i == 0) {
                    p->BtErr(BTERR_DIVIDE_BY_ZERO, ops);
                    return;
                }
                i = l.i / r.i;
                break;
            case Operator::MOD:
                if (r.i == 0) {
                    p->BtErr(BTERR_MODULUS_BY_ZERO, ops);
                    return;
                }
                i = l.i % r.i;
                break;
            default:
                p->BtErr(BTERR_INVALID_OPERATOR, ops);
                break;
            }
        else if (l.t == DBL)
            switch (opc) {
            case Operator::ADD:
                d = l.d + r.d;
                break;
            case Operator::SUB:
                d = l.d - r.d;
                break;
            case Operator::MUL:
                d = l.d * r.d;
                break;
            case Operator::DIV:
                if (r.d == 0.0) {
                    p->BtErr(BTERR_DIVIDE_BY_ZERO, ops);
                    return;
                }
                d = l.d / r.d;
                break;
            default:
                p->BtErr(BTERR_INVALID_OPERATOR, ops);
                return;
            }

        t = l.t;
    }
    break;

    case f_LIST:
        if (strlist_int(e, i))
            t = IVAL;
        break;

    default:
        p->BtErr(BTERR_INVALID_ARG_TYPE);
    }
}

// evaluate right argument expression and 'assign' to left argument
BtFImpl(is, t, p) {
    AritValue res;
    res.evalexpr(t.getarg(1), p);

    switch (res.t) {
    case AritValue::ATOM:
    case AritValue::ERR:
        break;
    case AritValue::IVAL:
        return p->unify(t.getarg(0), Term(res.i));
    case AritValue::DBL:
        return p->unify(t.getarg(0), p->save(Term(res.d)));
    }

    return 0;
}

// compare arguments (integers, double or atoms)
//  types must be compatible
static int getargs(TermArgs t, IntlogExec *p, AritValue &l, AritValue &r) {
    l.evalexpr(t.getarg(0), p);
    r.evalexpr(t.getarg(1), p);

    if (!l.compatible(r)) {
        p->BtErr(BTERR_DIFF_ARGS);
        return 0;
    }

    return 1;
}

// functions differ only for operator
#define CmpF(op)\
{   AritValue l, r;\
    if (getargs(t, p, l, r)) {\
        if (l.t == AritValue::IVAL)\
            return l.i op r.i;\
        if (l.t == AritValue::DBL)\
            return l.d op r.d;\
        if (l.t == AritValue::ATOM)\
            return strcmp(l.a, r.a) op 0;\
    } return 0;\
}

// true is a1 < a2
BtFImpl(less_, t, p) CmpF(<)

// true is a1 > a2
BtFImpl(great, t, p) CmpF(>)

// true is a1 <= a2
BtFImpl(lessequ, t, p) CmpF(<=)

// true is a1 >= a2
BtFImpl(greatequ, t, p) CmpF(>=)
