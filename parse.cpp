
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


//------------------------------------------------------
// parse required file (recursive descendent algorithm)
//
//  use a generalized term form (i.e. an expression)
//  and resolve precedence/associativity
//
// Ing. CapCar - HOME 1992
//------------------------------------------------------

#include "defsys.h"
#include "parse.h"
#include "proios.h"
#include "parsemsg.h"
#include "reduce.h"
#include "eng.h"

const int IntlogParser::MaxOpPrec = 255;
const int IntlogParser::MaxOpCdr = 240;

//////////////////////////
// constructor/destructor
//
IntlogParser::IntlogParser(OperTable* opt) {
    OpTbl   = opt;
    rs      = new ReduceStack;
    scan    = new IntlogScanner(256);
    var_ids = nullptr;
}
IntlogParser::~IntlogParser() {
    delete rs;
    delete scan;
}

/////////////////////////////
// push current stream state
//
void IntlogParser::SetSource(istream *pStream, kstring kFileid, kstr_list *pVars) {
    assert(pStream != nullptr);
    assert(MemStoreRef(kFileid) != MSR_NULL);
    assert(pVars != nullptr);

    iDesc *pDesc = new iDesc;

    pDesc->nRow = scan->rowcnt;
    pDesc->nCol = scan->colcnt;
    pDesc->lhChar = scan->clc;
    pDesc->pStream = scan->i;
    pDesc->kFileid = scan->id;
    pDesc->pVars = var_ids;

    activeScan.insert(pDesc, 0);

    var_ids = pVars;
    scan->i = pStream;
    scan->id = kFileid;
    scan->clc = -2;
    scan->rowcnt = 0;
    scan->colcnt = 0;
}

////////////////////////////////
// a request to delete a file
//  that have not seen the EOF!
//
void IntlogParser::PopSource(istream *pStream) {
    assert(pStream != nullptr);
    if (scan->i == pStream) {
        // resume last scanner status
        iDesc *pDesc = static_cast<iDesc *>(activeScan.get_first());
        assert(pDesc);

        var_ids = pDesc->pVars;
        scan->i = pDesc->pStream;
        scan->id = pDesc->kFileid;
        scan->clc = pDesc->lhChar;
        scan->rowcnt = pDesc->nRow;
        scan->colcnt = pDesc->nCol;

        activeScan.remove(0);
    }
}

///////////////////////////
// return clause read or 0
//
IntlogParser::objType IntlogParser::getinput() {
    var_ids->clear();

    advance();
    if (curr_tok == IntlogScanner::EOSTREAM) {
        PopSource(scan->i);
        return EndFile;
    }

    if (checkCmdRedo())
        return RedoCommand;

    if (!(parsed = term()).type(f_NOTERM))
        return NewClause;

    return Error;
}

/////////////////////////////////////
// parse next term into current file
//
Term IntlogParser::term() {
    Term line(f_NOTERM);
    RedEl *r;


    rs->Clear();
    rs->Begin();

    if (Expr()) {
        while (curr_tok != IntlogScanner::POINT && curr_tok != IntlogScanner::EOSTREAM)
            advance();
        goto stop;
    }

    if (curr_tok != IntlogScanner::POINT) {
        err_msg(EXPECT_POINT);
        while (curr_tok != IntlogScanner::POINT && curr_tok != IntlogScanner::EOSTREAM)
            advance();
        goto stop;
    }

    if ((r = rs->Reduce()) != nullptr) {
        line = Term(r->term);
        r->SetPos(rs->m_ft);
        rs->m_ft.SetRoot(r->idx);
    }
    else
        err_msg(OPER_REDUCE);

stop:
    rs->End();
    return line;
}

/////////////////////////////
// infix/postfix expressions
//
int IntlogParser::Expr(int maxLev) {
    if (curr_tok == '(') {
        int rc = 0;
        RedEl *r = nullptr;

        advance();

        rs->Begin();

        if ((rc = Expr(maxLev)) == 0) {
            if (curr_tok != ')') {
                err_msg(EXPECT_CLPAR);
                rc = 1;
            }
            else {
                advance();
                r = rs->Reduce();
            }
        }

        rs->End();

        if (r) {
            RedEl *n = rs->AddTerm(Term(r->term), r->sp);
            n->idx = r->idx;

            rc = Expr1(maxLev);
        }
        else {
            err_msg(OPER_REDUCE);
            rc = 1;
        }

        return rc;
    }

    Operator *op = GetOp();
    if (op) {
        do
            if (op->assoc == Operator::FX || op->assoc == Operator::FY) {
                rs->AddOper(op, *scan);
                advance();
                return Expr(maxLev) || Expr1(maxLev);
            }
        while ((op = GetOp(op)) != nullptr);

        err_msg(UNEXPECT_OPER);
        return 1;
    }

    return Termp() || Expr1(maxLev);
}

//////////////////////////////////
// get the right (following) part
//
int IntlogParser::Expr1(int MaxLev) {
    Operator *op = GetOp();

    if (op)
        switch (op->assoc) {
        case Operator::FX:
        case Operator::FY:
            break;

        case Operator::XFX:
        case Operator::XFY:
        case Operator::YFX:
        case Operator::YFY:
            rs->AddOper(op, *scan);
            advance();
            return Expr(MaxLev) || Expr1(MaxLev);

        case Operator::YF:
        case Operator::XF:
            rs->AddOper(op, *scan);
            advance();
            return Expr1(MaxLev);
        }

    return 0;
}

////////////////////////
// get a recursive term
//
int IntlogParser::Termp() {
    Term tVal;

    switch (curr_tok) {

    // atomic
    case IntlogScanner::INTEGER:
        tVal = Term(Int(atoi(scan->Image())));
        break;

    case IntlogScanner::INTHEX:
        tVal = Term(Int(scan->GetHexNum()));
        break;

    case IntlogScanner::DOUBLE:
        tVal = Term(atof(scan->Image()));
        break;

    // named variable
    case IntlogScanner::VAR_N:
        tVal = Term(Var(var_ids->add(kstring(scan->Image()))));
        break;

    // anonym variable
    case IntlogScanner::VAR_A:
        tVal = Term(ANONYM_IX);
        break;

    // transform to list of int
    case IntlogScanner::STRING:
        tVal = Term(scan->Image() + 1, scan->Len() - 2);
        break;

    // list
    case IntlogScanner::OPEN_SQ:
        return Listd();

    // atom: possibly an operator
    case IntlogScanner::ATOM_N:
    case IntlogScanner::ATOM_S:
    case IntlogScanner::ATOM_Q:
    case IntlogScanner::FENCE:
        return Symbol();

    default:
        err_msg(UNEXPECT_TOKEN);
        return 1;
    }

    rs->AddTerm(tVal, *scan);
    advance();

    return 0;
}

//////////////////////////////////
// parse a symbol
//  (possibly a structure functor)
//
int IntlogParser::Symbol() {
    assert(GetOp() == nullptr);

    SourcePos sp = *scan;
    kstring kf = scan->GetAtom(curr_tok);
    advance();

    if (curr_tok == '(')    // a structure
    {
        advance();

        rs->Begin();

        int rc;
        if ((rc = Expr()) == 0) {
            if (curr_tok != ')') {
                err_msg(EXPECT_CLPAR);
                rc = 1;
            }
            else
                advance();
        }

        Term e(f_NOTERM);
        RedEl *r = nullptr;
        if (!rc && (r = rs->Reduce(true)) != nullptr) {
            e = Term(kf, int(rs->m_nArgs));
            for (unsigned i = 0; i < rs->m_nArgs; i++)
                e.setarg(int(i), rs->m_Args[i]);
        }

        rs->End();

        if (!e.type(f_NOTERM)) {
            RedEl *top = rs->AddTerm(e, sp);
            top->SetPos(rs->m_ft);
            rs->m_ft.CatSon(top->idx, r->idx);
        }
        else if (rc == 0) {
            err_msg(OPER_REDUCE);
            rc = 1;
        }

        return rc;
    }
    else
        rs->AddTerm(Term(kf), sp)->SetPos(rs->m_ft);

    return 0;
}

///////////////////
// parse list data
//
int IntlogParser::Listd() {
    SourcePos sp(*scan);
    advance();
    FastTree &ft = rs->m_ft;

    if (curr_tok != ']') {
        Term tln;
        int rc;

        RedEl *rh, *rt = nullptr;
        NodeIndex ih = INVALID_NODE, it = INVALID_NODE;

        rs->Begin();

        if ((rc = Expr()) == 0) {
            rh = rs->Reduce(true);
            Term th(f_NOTERM),  // [X,Y,...
                 tt(ListNULL);  // |X]

            if (!rh) {
                err_msg(OPER_REDUCE);
                rc = 1;
            }
            else {
                th = Term(rh->term);
                ih = rh->idx;
            }

            if (!rc && curr_tok == '|') {
                advance();
                rs->Begin();

                if ((rc = Expr(MaxOpCdr)) == 0) {
                    if ((rt = rs->Reduce()) == nullptr) {
                        err_msg(OPER_REDUCE);
                        rc = 1;
                    }
                    else {
                        rt->SetPos(ft);
                        tt = Term(rt->term);
                        it = rt->idx;
                    }
                }
                if (rc || !rt)
                    th.Destroy();

                rs->End();
            }

            if (!rc && curr_tok != ']') {
                err_msg(EXPECT_CLQUA);
                rc = 1;
            }

            if (!rc)    // convert head ',' expression to list
            {
                Term tailFollow = tln = Term(rs->m_Args[0], Term(ListNULL));
                for (unsigned i = 1; i < rs->m_nArgs; i++) {
                    Term tl(rs->m_Args[i], Term(ListNULL));
                    tailFollow.setarg(1, tl);
                    tailFollow = tl;
                }
                tailFollow.setarg(1, tt);
            }
        }

        rs->End();

        if (rc)
            return 1;

        rh = rs->AddTerm(tln, sp);
        rh->SetPos(ft);

        NodeIndex irh = rh->idx, fb = INVALID_NODE;
        while (ih != INVALID_NODE) {
            NodeIndex ib = ft.UnlinkBrother(ih);
            ft.SetSon(irh, ih);

            if (ib != INVALID_NODE) {
                fb = ft.AllocNode();
                ft[fb].m_data = SourcePos();
                ft.SetBrother(ih, fb);

                irh = fb;
            }

            fb = ih;
            ih = ib;
        }

        if (rt) {
            assert(it != NodeIndex(-1));
            ft.SetBrother(fb, it);
        }
    }
    else
        rs->AddTerm(Term(ListNULL), sp)->SetPos(ft);

    advance();
    return 0;
}

///////////////////////////////////////////////////
// scan current symbol, checking for operator
//  track nesting of files due to consult(), see()..
//
void IntlogParser::advance() {
    if (!scan) {
        curr_tok = IntlogScanner::EOSTREAM;
        return;
    }

    curr_tok = scan->Next();
}

////////////////////////////
// check for a redo command
//
int IntlogParser::checkCmdRedo() {
    Operator* op = GetOp();
    return op && op->opCode == Operator::OR;
}

/////////////////////////////////
// get current operator
//  or next for multiple defined
//
Operator* IntlogParser::GetOp(Operator *curr) {
    if (!curr) {
        if (scan->tokenChanged()) {
            scan->tokenFix();
            return lastOp = OpTbl->IsOp(kstring(scan->Image()));
        }
        return lastOp;
    }
    return OpTbl->GetNext(curr);
}

//////////////////////////////////////////////
// display error message in 'standard' format
//
void IntlogParser::err_msg(int code, const char *s) const {
    if (scan) {
        if (!s)
            s = scan->Image();
        GetEngines()->ErrMsg(code, scan->Name(), scan->Row(), s);
    }
}

// decouple reduce/fasttree from parser
SrcPosTree& IntlogParser::GetSrcPosTree() {
    return rs->m_ft;
}

IntlogParser::iDesc::~iDesc() {}
