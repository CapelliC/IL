/*
    IL : Prolog interpreter
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

/*
	Intlog Language
	Object Oriented Prolog Project
	Ing. Capelli Carlo - Brescia

	intlog tokens scanner
*/

#include "stdafx.h"
#include "defsys.h"
#include "scanner.h"
#include "parsemsg.h"
#include "eng.h"
#include <string.h>
#include <ctype.h>

// initialize basic scanner status
//
IntlogScanner::IntlogScanner(int maxbuf)
{
	i = 0;
	id = MSR_NULL;
	rowcnt = colcnt = 0;
	rowtok = coltok = -1;

	buftok = new char[bufmax = maxbuf + 1];
	buftok[buflen = 0] = 0;

	clc = -2;
	initchclass();

	m_bTranslateAnsiSeq = TRUE;
}
IntlogScanner::~IntlogScanner()
{
    delete buftok;
}

// get current token and position to next
//
IntlogScanner::Codes IntlogScanner::Next()
{
	if (clc == -2)
		get();

	clear();
	addchar();

	for ( ; ; )
	{
		rowtok = rowcnt + 1;
		coltok = colcnt;

		switch (clc)
		{
		case -1:	// end-of-file
			clc = -2;
			return retc(EOSTREAM);

		case '\n':	// blanks
		case '\r':
		case ' ':
		case '\t':
			while (isblank(ch_t(clc)))
				get();
			retc(BLANKS);
			clear();
			addchar();
			break;

		case '_':	// variable
			if (issymbody(ch_t(get())))
			{
				ident();
				return retc(VAR_N);
			}
			return retc(VAR_A);

		case '.':
			get();
			return retc(POINT);

		case '"':
			return seq(STRING, UNTERMINATED_STRING);

		case '\'':
			return seq(ATOM_Q, INVALID_QUOTED_ATOM);

		case '/':
			if (get() == '/')
				comment();
			else if (clc == '*')
				C_comment();
			else
				return retc(Codes('/'));
			break;

		case '%':
			comment();
			break;

		default:

			if (issymhead(ch_t(clc)))
			{
				ident();
				return retc(ATOM_N);
			}

			if (isvarhead(ch_t(clc)))
			{
				ident();
				return retc(VAR_N);
			}

			if (isnumber(ch_t(clc)))
				return number();

			if (issign(ch_t(clc)))
			{
				while (issign(ch_t(get())))
					;
				return retc(ATOM_S);
			}

			if (isused(ch_t(clc)))
			{
				Codes c = Codes(clc);
				get();
				return retc(c);
			}

			ErrMsg(INVALID_CHAR);
			get();
			clear();
			addchar();
		}
	}
}

// scan a C like comment
//
void IntlogScanner::C_comment()
{
	addchar();
	retc(MCOMM_BEGIN);
	clear();

	int sl = Row(), exp = 0, emb = 0;

	for ( ; ; )
	{
		if (get() == '*')
		{
			if (emb)
			{
				ErrMsg(NESTED_COMMENT);
				C_comment();
			}
			else
				exp = 1;
		}
		else if (clc == '/')
		{
			if (exp)
				break;
			exp = 0;
			emb = 1;
		}
		else if (clc == -1)
		{
			ErrMsg(EOF_IN_COMMENT, sl);
			break;
		}
		else if (clc == '\n')
		{
			addchar();
			retc(MCOMM_LINE);
			clear();
		}
		else
			emb = exp = 0;
	}

	addchar();
	retc(MCOMM_END);

	clear();
	get();
}

// scan one line comment
//
void IntlogScanner::comment()
{
	while ((clc = i->get()) != '\n' && clc != -1)
		addchar();
	addchar();
	retc(SCOMM_LINE);

	clear();
	if (clc == '\n')
	{
		nl();
		addchar();
		addchar();
		retc(BLANKS);
		clear();
	}

	get();
}

// scan a number and return code
//
IntlogScanner::Codes IntlogScanner::number()
{
	Codes c = INTEGER;

	while (isnumber(ch_t(get())))
		;

	if (clc == '.')
	{
		if (isnumber(ch_t(get())))
		{
			c = DOUBLE;
			while (isnumber(ch_t(get())))
				;
		}
		else
			backc('.');
	}
	else if (clc == 'x' || clc == 'X')
	{
		if (buflen == 2 && buftok[0] == '0')
		{
			while (isnumberh(ch_t(get())))
				;
			c = INTHEX;
			if (buflen > 4+2)
				ErrMsg(HEXLEN_OVERFLOW, 4);
		}
		else
			backc(clc);
	}
	else if (Len() >= 5 && strncmp(buftok, "32768", Len()) > 0)
	{
		ErrMsg(INTLEN_OVERFLOW);
		c = DOUBLE;
	}

	return retc(c);
}

long IntlogScanner::GetHexNum() const
{
	CCP pNum = Image() + Len() - 1;
	long cv = 0;
	int shift = 0, n;

	while (isnumberh(ch_t(n = *pNum)))
	{
		if (n >= '0' && n <= '9')
			n -= '0';
		else if (n >= 'A' && n <= 'Z')
			n = n - 'A' + 10;
		else
			n = n - 'a' + 10;

		cv |= (n << shift);
		shift += 4;
		pNum--;
	}

	return cv;
}

void IntlogScanner::ErrMsg(int code, int numl) const
{
	if (MemStoreRef(id) != MSR_NULL)
		GetEngines()->ErrMsg(code, CCP(id), Row(), Col(), numl);
}

void IntlogScanner::addchar()
{
	if (buflen < bufmax)
		buftok[buflen++] = (char)clc;
	else if (!overflow)
	{
		ErrMsg(TOKLEN_OVERFLOW, bufmax);
		overflow = 1;
	}
}

// parse a quoted string
//
IntlogScanner::Codes IntlogScanner::seq(Codes rc, int errc)
{
	int qchar = clc;

	while (get() != qchar && clc != '\n' && clc != -1)
		if (clc == '\\')
			if (m_bTranslateAnsiSeq)
			{
				// apply reduced ANSI C escape sequences translation
				get();

				switch (clc) {
				case 'a': clc = '\a'; goto setup;
				case 'b': clc = '\b'; goto setup;
				case 'f': clc = '\f'; goto setup;
				case 'n': clc = '\n'; goto setup;
				case 'r': clc = '\r'; goto setup;
				case 't': clc = '\t'; goto setup;
				case 'v': clc = '\v';
				case '?':
				case '\'':
				case '\\':
				case '\"':
				setup:
					buflen--;
					buftok[buflen - 1] = char(clc);
					break;

				case 'x':
				default:
					if (clc >= '0' && clc <= '7')
                    {

                    }
				}
			}

	if (clc != qchar)
		ErrMsg(errc);
	else
		get();

	return retc(rc);
}

void IntlogScanner::backc(int c)
{
	i->putback(char(clc));
	buflen--;
	ASSERT(buflen >= 0);

	if (clc == '\n')
		rowcnt--;
	else
		colcnt--;

	clc = c;
}

IntlogScanner::ch_t IntlogScanner::chclass[256];
void IntlogScanner::initchstr(pcch_t s, int bit)
{
	while (*s)
		chclass[*s++] |= bit;
}
void IntlogScanner::initchset(ch_t from, ch_t to, int bit)
{
	while (int(from) <= int(to))
		chclass[from++] |= bit;
}

void IntlogScanner::initchclass()
{
	if (chclass['+'] == 0)	// check if already initialized
	{
		initchset(' ', '~', Ascii);
		initchstr(pcch_t(" \t\n\r"), Blank);
		initchstr(pcch_t("+-*/=^<>~:.?@#$\\&{}`"), Sign);

		chclass['_'] |= SymBody;
		initchset('A', 'Z', SymBody|VarHead);
		initchset('a', 'z', SymBody|SymHead);
		initchset('0', '9', SymBody|Digit|DigitH);
		initchset('A', 'F', DigitH);
		initchset('a', 'f', DigitH);

		// italian chars
		static ch_t
			ANSI_set[] = {133,138,130,141,149,151,0},
			WIND_set[] = "àèéìòù"; // keept accented letters, QtCreator complains about invalid encodings
		initchstr(ANSI_set, SymBody|SymHead);
		initchstr(WIND_set, SymBody|SymHead);
	}
}

// advance lookahead character in stream
//
int IntlogScanner::get()
{
	if ((clc = i->get()) == '\n')
		nl();
	else
		colcnt++;
	addchar();

	return clc;
}

// get atom correcting for '.,.,.'
//
kstring IntlogScanner::GetAtom(Codes toktype)
{
	if (toktype == ATOM_Q)
	{
		buftok[buflen - 2] = 0;
		return kstring(buftok + 1);
	}

	return kstring(buftok);
}

// setup data ready for get
//
IntlogScanner::Codes IntlogScanner::retc(Codes c)
{
	buftok[buflen - 1] = 0;
	somechange = 1;
	useInput(c);
	return c;
}

// hpp

#define inline

inline const char* IntlogScanner::Name() const
{
	return id;
}

inline const char* IntlogScanner::Image() const
{
	return buftok;
}
inline int IntlogScanner::Row() const
{
	return rowtok;
}
inline int IntlogScanner::Col() const
{
	return coltok;
}
inline int IntlogScanner::Len() const
{
	return buflen - 1;
}

inline void IntlogScanner::tokenFix()
{
	somechange = 0;
}
inline int IntlogScanner::tokenChanged() const
{
	return somechange;
}

inline void IntlogScanner::clear()
{
	buftok[buflen = overflow = 0] = 0;
}
inline void IntlogScanner::nl()
{
	colcnt = 0;
	rowcnt++;
}

inline int IntlogScanner::issign(ch_t c)
{
	return (chclass[c] & Sign);
}
inline int IntlogScanner::issymbody(ch_t c)
{
	return (chclass[c] & SymBody);
}
inline int IntlogScanner::isnumber(ch_t c)
{
	return (chclass[c] & Digit);
}
inline int IntlogScanner::isnumberh(ch_t c)
{
	return (chclass[c] & DigitH);
}
inline int IntlogScanner::isblank(ch_t c)
{
	return (chclass[c] & Blank);
}
inline int IntlogScanner::issymhead(ch_t c)
{
	return (chclass[c] & SymHead);
}
inline int IntlogScanner::isvarhead(ch_t c)
{
	return (chclass[c] & VarHead);
}
inline int IntlogScanner::isused(ch_t c)
{
	return chclass[c];
}
inline void IntlogScanner::ident()
{
	while (issymbody(ch_t(clc)))
		get();
}
inline void IntlogScanner::SetSource(istream *data, kstring fileId)
{
	i = data;
	id = fileId;
	clc = -2;
}
inline IntlogScanner::operator SourcePos() const
{
	return SourcePos(Row(), Col(), Len());
}
