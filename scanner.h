/*
    IL : Prolog interpreter
    Copyright (C) 1992-2012 - Ing. Capelli Carlo

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

/*
 Intlog Language
 Object Oriented Prolog Project
 Ing. Capelli Carlo - Brescia
*/

#ifndef _SCANNER_H_
#define _SCANNER_H_

#ifndef _IAFX_H_
#include "iafx.h"
#endif

#ifndef _SRCPOS_H_
#include "srcpos.h"
#endif

class IntlogParser;

//-----------------------
// Intlog tokens scanner
//
class IAFX_API IntlogScanner
{
public:
    IntlogScanner(int);
    ~IntlogScanner();

    void SetSource(istream *data, kstring fileId = kstring(MSR_NULL));
    operator SourcePos() const;

    enum Codes {
        ATOM_N = 256,	// atom 'normal'
        ATOM_S,		// signed
        ATOM_Q,		// quoted
        INTEGER,	// integer numeric sequence
        INTHEX,		// hexadecimal numeric sequence
        DOUBLE,		// numeric sequence floating point
        VAR_N,		// symbol variable
        VAR_A,		// anonym variable
        STRING,		// sequence of characters
        POINT,		// a single '.'
        EOSTREAM,	// end of stream

        // blanks, never returned by Next
        MCOMM_BEGIN,	// begin multi line comment
        MCOMM_LINE,	// line, followed by \n
        MCOMM_END,	// end multi line comment
        SCOMM_LINE,	// a comment line, followed by \n
        BLANKS		// spaces, tabs, \n
    };
    Codes Next();

    // current token properties
    const char*	Image()	const;
    int	Len() const;
    int	Row() const;
    int Col() const;

    // display error message in standard format
    void ErrMsg(int, int = 0) const;
    const char*	Name() const;

    // enable to sense if some input read between operations
    void tokenFix();
    int tokenChanged() const;

    // compute current buffer value
    long GetHexNum() const;

    // get atom correcting for '.,.,.'
    kstring GetAtom(Codes toktype);

    // enable full 8 bit range processing
    typedef unsigned char ch_t;
    typedef unsigned char* pcch_t;

protected:

    friend class IntlogParser;

    istream* i;		// input stream
    kstring id;		// file name

    int clc;		// current lookahead character
    int	rowcnt;		// actual stream row counter
    int	colcnt;		// actual stream column counter
    int	rowtok;		// last token row counter
    int	coltok;		// last token column counter

    char* buftok;	// current token image
    int buflen;		// current token lenght
    int bufmax;		// max buffer lenght
    int overflow;	// flag for buffer overflow
    int somechange;	// flag for external control on changed data

    // character classification
    static ch_t chclass[256];
    enum {
        SymBody	= 0x01,
        SymHead	= 0x02,
        VarHead	= 0x04,
        Sign	= 0x08,
        Digit	= 0x10,
        DigitH	= 0x20,
        Blank	= 0x40,
        Ascii	= 0x80
    };
    static void initchclass();
    static void initchstr(pcch_t, int);
    static void initchset(ch_t from, ch_t to, int bit);
    static int issign(ch_t);	// true for sign chars
    static int issymbody(ch_t);	// true for symbols body chars
    static int issymhead(ch_t);	// true for first symbol char
    static int isvarhead(ch_t);	// true for first symbol variable
    static int isnumber(ch_t);	// true for digits
    static int isnumberh(ch_t);	// true for hexadecimal digits
    static int isblank(ch_t);	// true for blanks
    static int isused(ch_t);	// character has some class

    int	get();			// advance lookahead character in stream
    void addchar();		// add a char to current token image
    void backc(int);		// insert char back in stream
    void clear();		// reset after last token used
    void nl();			// a nl in source: change line counter...
    Codes retc(Codes);		// fixup buffer and return to caller

    void comment();		// scan one line comment
    void C_comment();		// scan a C comment
    void ident();		// scan a prolog identifier
    Codes number();		// scan a number

    // scan a sequence embedded as string
    Codes seq(Codes, int);

    // enable formatting...
    virtual void useInput(Codes /*c*/){}
    BOOL m_bTranslateAnsiSeq;
};

#ifndef _DEBUG
#include "scanner.hpp"
#endif

#endif
