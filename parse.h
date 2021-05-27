
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


#ifndef PARSE_H_
#define PARSE_H_

#include "iafx.h"
#include "term.h"
#include "scanner.h"

class ReduceStack;
class SrcPosTree;

//-------------------------------------------------
// top-down recursive parser
//  fetch next clause from stream (0 at EOF or error)
//  use a precedence algorithm to handle operators
//
class IAFX_API IntlogParser {
public:
    IntlogParser(OperTable*);
    ~IntlogParser();

    // set current scanner for current stream
    void SetSource(istream *pStream, kstring kFileid, kstr_list *varList);
    void PopSource(istream *pStream);

    istream *GetSourceStream() const;
    kstring GetSourceName() const;

    // type of input found
    enum objType {
        NewClause,
        RedoCommand,
        EndFile,
        Error
    };
    objType getinput();

    // valorized if getinput() returned NewClause
    Term parsed;
    SrcPosTree& GetSrcPosTree();

    // return operator table
    OperTable* get_ops() const;

    // display message on parsing error with infos on source location
    void err_msg(int, const char * = nullptr) const;

private:

    static const int MaxOpPrec;
    static const int MaxOpCdr;

    // keep active scanner stream
    IntlogScanner* scan;
    kstr_list* var_ids;
    struct iDesc : public e_slist {
        ~iDesc() override;
        istream *pStream;
        kstring kFileid;
        kstr_list *pVars;
        int nRow, nCol, lhChar;
    };
    slist activeScan;

    // hold operators
    OperTable* OpTbl;

    // term reduction stack
    ReduceStack* rs;

    // hold last token code read
    IntlogScanner::Codes curr_tok;

    // read next token
    void advance();

    // return next term read (0 at EOF or error)
    Term term();

    // Term Expr1 | '(' Expr ')' Expr1 | Opre Expr Expr1
    int Expr(int maxLev = MaxOpPrec);

    // Oinf Expr Expr1 | Opos Expr1
    int Expr1(int maxLev = MaxOpPrec);

    // atom '(' Expr ')' | int | var | '[' Expr ']'
    int Termp();

    // check backtracking command
    int checkCmdRedo();

    // parse list data
    int Listd();

    // parse a symbol
    int Symbol();

    // scan operators definition
    Operator* lastOp;
    Operator* GetOp(Operator* = nullptr);
};

inline OperTable* IntlogParser::get_ops() const {
    return OpTbl;
}

inline istream *IntlogParser::GetSourceStream() const {
    return scan->i;
}
inline kstring IntlogParser::GetSourceName() const {
    return scan->id;
}

#endif
