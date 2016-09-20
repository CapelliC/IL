
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


//-----------------------
// IO oriented operation
//-----------------------

#include "stdafx.h"
#include "defsys.h"
#include "builtin.h"
#include "qdata.h"
#include "eng.h"
#include "bterr.h"
#include "aritval.h"

BtFDecl(tell);
BtFDecl(telling);
BtFDecl(told);
BtFDecl(see);
BtFDecl(seeing);
BtFDecl(seen);
BtFDecl(nl);
BtFDecl(get1);
BtFDecl(get0);
BtFDecl(skip);
BtFDecl(tab);
BtFDecl(put);
BtFDecl(putn);
BtFDecl(display);
BtFDecl(pwrite);
BtFDecl(writeln);
BtFDecl(pread);
BtFDecl(opbt);
BtFDecl(tokenize);

BtFDecl(set_buffer);
BtFDecl(get_buffer);
BtFDecl(del_buffer);

BuiltIn IO_oriented[22] = {
	{"tell",		1,	tell},
	{"telling",		1,	telling},
	{"told",		0,	told},
	{"see",			1,	see},
	{"seeing",		1,	seeing},
	{"seen",		0,	seen},
	{"tab",			1,	tab},
	{"put",			1,	put},
	{"putn",		2,	putn},
	{"display",		1,	display},
	{"write",		1,	pwrite},
	{"writeln",		1,	writeln},
	{"read", 		1,	pread},
	{"nl",			0,	nl},
	{"get",			1,	get1},
	{"get0",		1,	get0},
	{"skip",		1,	skip},
	{"op",			3,	opbt},
	{"tokenize",	2,	tokenize},


	{"set_buffer",	1, set_buffer},
	{"get_buffer",	2, get_buffer},
	{"del_buffer",	1, del_buffer}
};

//----------------------------------
// get required argument (type INT)
//
static int getint(Term t, IntlogExec *p, Int &v)
{
	Term e = p->eval_term(t);
	if (e.type(f_INT))
	{
		v = Int(e);
		return 1;
	}
	if (e.type(f_LIST) && AritValue::strlist_int(t, v))
		return 1;

	p->BtErr(BTERR_REQ_INT_ARG);
	return 0;
}

//----------------------------------
// check argument and call function
//
#define See		0
#define Seeing	1
#define Tell	2
#define Telling	3
static int work(TermArgs ta, IntlogExec *p, int opc)
{
	if (opc == See || opc == Tell)
	{
		// need instantiated arg
		Term t = p->copy(ta.getarg(0)), l;
		int flags = 0;
		kstring a;

		if (t.type(f_ATOM))
			a = t.kstr();
		else if (t.is_expr(Operator::DIV) && (l = t.getarg(0)).type(f_ATOM))
		{
			a = l.kstr();
			Term r = t.getarg(1);
			if (r.type(f_ATOM) && !strcmp(r, "binary"))
				flags = ios::binary;
		}
		else
		{
			p->BtErr(BTERR_INVALID_ARG_TYPE);
			t.Destroy();
			return 0;
		}

		t.Destroy();
		return opc == See? p->see(a, flags) : p->tell(a, flags);
	}

	// accept any arg to unify
	kstring idstream =
		opc == Seeing? p->ips()->name() : p->ops()->name();

	return p->unify(Term(idstream), ta.getarg(0));
}

//----------------------
// change output stream
//
BtFImpl(tell, t, p)
{
	return work(t, p, Tell);
}

//---------------------
// check output stream
//
BtFImpl(telling, t, p)
{
	return work(t, p, Telling);
}

//---------------------
// reset output stream
//
BtFImpl_P1(told, p)
{
	return p->told();
}

//---------------------
// change input stream
//
BtFImpl(see, t, p)
{
	return work(t, p, See);
}

//--------------------
// check input stream
//
BtFImpl(seeing, t, p)
{
	return work(t, p, Seeing);
}

//--------------------
// reset input stream
//
BtFImpl_P1(seen, p)
{
	return p->seen();
}

//--------------------------
// get first not white char
//
BtFImpl(get1, t, p)
{
	int c = p->ips()->get();
	while (c < ' ' && c > '~' && c != EOF)	// query to scanner!
		c = p->ips()->get();

	return p->unify(t.getarg(0), Term(Int(c)));
}

//----------------
// get first char
//
BtFImpl(get0, t, p)
{
	Term c(Int(p->ips()->get()));
	return p->unify(t.getarg(0), c);
}

//-------------------------------
// skip to the next matched char
//
BtFImpl(skip, t, p)
{
	Term c = p->eval_term(t.getarg(0));
	if (!c.type(f_INT))
	{
		p->BtErr(BTERR_INVALID_ARG_TYPE);
		return 0;
	}

	while (p->ips()->get() != Int(c))
		if (p->ips()->ateof())
			return 0;

	return 1;
}

//----------------
// write n spaces
//
BtFImpl(tab, t, p)
{
	Int v;
	if (getint(t.getarg(0), p, v))
	{
		ostream &s = p->out();
		while (v--)
			s << ' ';
		return s.good();
	}
	return 0;
}

//-------------------
// write a character
//
BtFImpl(put, t, p)
{
	Int v;

	if (getint(t.getarg(0), p, v))
		return (p->out() << char(v)).good();

	return 0;
}

//---------------------------
// write N times a character
//
BtFImpl(putn, t, p)
{
	Int c, n;
	if (getint(t.getarg(0), p, c) && getint(t.getarg(1), p, n))
	{
		ostream &s = p->out();
		while (n--)
			s << (char)c;
		return s.good();
	}
	return 0;
}

//------------------
// write a new line
//
BtFImpl_P1(nl, p)
{
/*	ostream &s = p->out();
	s << '\n'; //endl;
	return s.good();
*/	return (p->out() << endl).good();
}

//--------------------------------------
// write term, ignoring operators decls
//
BtFImpl(display, t, p)
{
	return p->display(t.getarg(0), p->out()).good();
}

//-------------
// write terms
//
BtFImpl(pwrite, t, p)
{
	return p->write(t.getarg(0), p->out()).good();
}

//-----------------------
// write a list of terms
//
BtFImpl(writeln, t, p)
{
	Term v = p->copy(t.getarg(0)), c = v;

	while (v.type() == f_LIST && !v.LNULL())
	{
		const List &l = v;
		p->out() << l.head();
		v = l.tail();
	}
	c.Destroy();

	return 1;
}

//------------------------
// read a term from input
//
BtFImpl(pread, t, p)
{
	Term pt = p->readTerm();
	if (pt.type(f_NOTERM))
	{
		if (!p->ips()->ateof())
			return 0;
		pt = Term(kstring("$end_of_file"));
	}

	if (pt.type(f_STRUCT|f_LIST|f_DOUBLE))
		pt = p->save(pt);

	return p->unify_gen(t.getarg(0), pt);
}

//-------------------------------
// insert a operator declaration
//
BtFImpl(opbt, t, p)
{
	Term	prec = p->eval_term(t.getarg(0)),
			apos = p->eval_term(t.getarg(1)),
			name = p->eval_term(t.getarg(2));

	// check binding
	if (!prec.type(f_INT) || !apos.type(f_ATOM) || !name.type(f_ATOM))
	{
		p->BtErr(BTERR_INVALID_ARG_TYPE);
		return 0;
	}

	// check range
	int iPrec = prec;
	if (iPrec <= 0 || iPrec >= 256)
	{
		p->BtErr(BTERR_INVALID_ARG_TYPE);
		return 0;
	}

	// check associativity/position
	kstring kpos = apos.kstr();
	static struct
	{
		const char*			image;
		Operator::assocTag	assoc;
	}
		vect[8] =
	{
		{"fx",	Operator::FX},
		{"xfx",	Operator::XFX},
		{"xf",	Operator::XF},
		{"fy",	Operator::FY},
		{"yfy",	Operator::YFY},
		{"yf",	Operator::YF},
		{"yfx",	Operator::YFX},
		{"xfy",	Operator::XFY}
	};
	int i = 0;
	for ( ; i < 8; i++)
		if (!strcmp(vect[i].image, kpos))
			break;
	if (i == 8)
	{
		p->BtErr(BTERR_INVALID_ARG_TYPE);
		return 0;
	}

	// check redefinition
	OperTable *tbl = GetEngines()->get_optbl();
	Operator *op = tbl->IsOp(name.kstr());
	if (op)
	{
		if (op->prec != iPrec || op->assoc != vect[i].assoc)
		{
			p->BtErr(BTERR_INVALID_ARG_TYPE);
			return 0;
		}

		// already defined with same properties
		return 1;
	}

	tbl->Add(	name.kstr(), short(iPrec),
				vect[i].assoc, Operator::UserDef);

	return 1;
}

// fetch last opened memory file
//
BtFImpl(get_buffer, t, p)
{
	Term name = p->eval_term(t.getarg(0));
	if (!name.type(f_ATOM) || !p->IsBuffer(name.kstr()))
	{
		p->BtErr(BTERR_INVALID_ARG_TYPE);
		return 0;
	}

	kstring k = p->GetBuffer(name.kstr());
	return p->unify(t.getarg(1), Term(k));
}
BtFImpl(set_buffer, t, p)
{
	Term name = p->eval_term(t.getarg(0));

	if (!name.type(f_ATOM))
	{
		p->BtErr(BTERR_INVALID_ARG_TYPE);
		return 0;
	}

	p->SetBuffer(name.kstr(), 0);
	return 1;
}
BtFImpl(del_buffer, t, p)
{
	Term name = p->eval_term(t.getarg(0));

	if (!name.type(f_ATOM))
	{
		p->BtErr(BTERR_INVALID_ARG_TYPE);
		return 0;
	}

	p->DelBuffer(name.kstr());
	return 1;
}

// get tokens up to end-of-file
//
#include "scanner.h"

Term tokenizeQualified(IntlogScanner &s)
{
	Term ltokens(f_NOTERM), lscan;
	kstring toksym("token");

	for ( ; ; )
	{
		const char *funct = 0;
		Term value;

		switch (s.Next()) {

		case IntlogScanner::ATOM_N:
		case IntlogScanner::ATOM_S:
		case IntlogScanner::ATOM_Q:
		case IntlogScanner::POINT:
		default:
			funct = "atom";
			value = Term(s.Image());
			break;
		case IntlogScanner::INTEGER:
		case IntlogScanner::INTHEX:
			funct = "integer";
			value = Term(Int(atol(s.Image())));
			break;
		case IntlogScanner::DOUBLE:
			funct = "double";
			value = Term(atof(s.Image()));
			break;
		case IntlogScanner::VAR_N:
		case IntlogScanner::VAR_A:
			funct = "var";
			value = Term(s.Image());
			break;
		case IntlogScanner::STRING:
			funct = "string";
			value = Term(s.Image(), s.Len());
			break;
		case IntlogScanner::EOSTREAM:
			return ltokens;
		}

		Term token(toksym, 2);
		token.setarg(0, kstring(funct));
		token.setarg(1, value);

		Term l(token, Term(ListNULL));
		if (TermData(ltokens) == f_NOTERM)
		{
			ltokens = l;
			lscan = ltokens;
		}
		else
		{
			lscan.setarg(1, l);
			lscan = lscan.getarg(1);
		}
	}
}
Term tokenizeBasic(IntlogScanner &s)
{
	Term ltokens(f_NOTERM), lscan;

	for ( ; ; )
	{
		Term value;

		switch (s.Next()) {

		case IntlogScanner::ATOM_N:
		case IntlogScanner::ATOM_S:
		case IntlogScanner::ATOM_Q:
		case IntlogScanner::VAR_N:
		case IntlogScanner::VAR_A:
		case IntlogScanner::POINT:
		default:
			value = Term(s.Image());
			break;
		case IntlogScanner::INTEGER:
		case IntlogScanner::INTHEX:
			value = Term(Int(atol(s.Image())));
			break;
		case IntlogScanner::DOUBLE:
			value = Term(atof(s.Image()));
			break;
		case IntlogScanner::STRING:
			value = Term(s.Image(), s.Len());
			break;
		case IntlogScanner::EOSTREAM:
			return ltokens;
		}

		Term l(value, Term(ListNULL));
		if (TermData(ltokens) == f_NOTERM)
		{
			ltokens = l;
			lscan = ltokens;
		}
		else
		{
			lscan.setarg(1, l);
			lscan = lscan.getarg(1);
		}
	}
}

BtFImpl(tokenize, t, p)
{
	IntlogScanner s(256);
	s.SetSource(&p->inp());

	Term ltokens;

	Term sel = p->eval_term(t.getarg(1));
	if (sel.type(f_ATOM) && !strcmp(sel, "qualified"))
		ltokens = tokenizeQualified(s);
	else
		ltokens = tokenizeBasic(s);

	return p->unify(p->save(ltokens), t.getarg(0));
}
