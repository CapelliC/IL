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

//-----------------------
// default messages/data
//-----------------------

#include "stdafx.h"
#include "defsys.h"
#include "parsemsg.h"

//----------------
// setup messages
//
void AddStdMsg(MsgTable *tbl)
{
	tbl->add(FILE_NOT_FOUND,		"file '%s' non trovato");
	tbl->add(EXPECT_POINT,			"%s(%d) atteso '.'");
	tbl->add(EXPECT_CLQUA,			"%s(%d) atteso ']'");
	tbl->add(EXPECT_CLPAR,			"%s(%d) atteso ')'");
	tbl->add(UNEXPECT_TOKEN,		"%s(%d) inatteso '%s'");
	tbl->add(UNEXPECT_OPER,			"%s(%d) operatore '%s' inatteso");
	tbl->add(OPER_REDUCE,			"%s(%d) riduzione espressione");

	tbl->add(UNTERMINATED_STRING,	"%s(%d) stringa non terminata");
	tbl->add(INVALID_QUOTED_ATOM,	"%s(%d) 'atomo' non terminato");
	tbl->add(INVALID_CHAR,			"%s(%d) carattere non valido");
	tbl->add(EOF_IN_COMMENT,		"%s(%d) EOF in commento");
	tbl->add(TOKLEN_OVERFLOW,		"%s(%d) superata lunghezza massima (%d)");
	tbl->add(HEXLEN_OVERFLOW,		"%s(%d) superata lunghezza massima esadecimale (%d)");
	tbl->add(INTLEN_OVERFLOW,		"%s(%d) conversione oltre lunghezza massima");
	tbl->add(NESTED_COMMENT,		"%s(%d) commento annidato");

	tbl->add(BUILTIN_REDEFINITION,	"%s(%d) predicato di sistema");
	tbl->add(UNUSED_VAR,			"%s(%d) variabile '%s' non usata");
	tbl->add(METAVAR_CALL,			"%s(%d) metavariabile: usare call(%s)");
	tbl->add(ANONYMVAR_CALL,		"%s(%d) chiamata di variabile anonima");

	tbl->add(OR_AS_FUNCTOR,			"%s(%d) ';' come funtore");
	tbl->add(AND_AS_FUNCTOR,		"%s(%d) ',' come funtore");
	tbl->add(LIST_AS_FUNCTOR,		"%s(%d) lista come funtore");
	tbl->add(INT_AS_FUNCTOR,		"%s(%d) numero come funtore");
	tbl->add(REAL_AS_FUNCTOR,		"%s(%d) numero come funtore");
	tbl->add(INVALID_DB_PATH,		"%s(%d) database non trovato");
	tbl->add(UNCALL_AS_FUNCTOR,		"%s(%d) elemento non chiamabile");

	tbl->add(BINL_NOT_FOUND,		"%s: libreria non trovata");
	tbl->add(LOADING_STRINGS,		"%s: tabella stringhe non corretta");
	tbl->add(LOADING_DIRECTORY,		"%s: tabella File non corretta");
	tbl->add(CREATE_OUTPUT_FILE,	"%s: non posso creare il file");
	tbl->add(WRITE_OUTPUT_FILE,		"%s: errore nella scrittura");
	tbl->add(CANT_OPEN_TMPFILE,		"%s: non posso aprire il temp. '%s'");
	tbl->add(CANT_REOPEN_TMPFILE,	"%s: non posso riaprire il temp. '%s'");
	tbl->add(INVALID_BIN_DATA,		"%s: dati non validi");
	tbl->add(INVALID_BIN_OPDEF,		"%s: operatore '%s' non valido");
	tbl->add(CANT_DELETE_TMPFILE,	"%s: temp. '%s' non eliminabile");

	tbl->add(INVALID_SERIAL_DATA,	"formato seriale non valido");
	tbl->add(SERIAL_FMT_NOT_FOUND,	"formato seriale '%s' non trovato");
}

//------------------------------------------------------------------------
// install standard operators
//	for convenction on constant mapping of kstring/functors_operators
//	the call order MUST follow that of Operator::tag as declared
//	in operator.h
//
void AddStdOpers(OperTable *tbl)
{
	tbl->Add(kstring(":-"),		255,	Operator::XFX,	Operator::RULE);
	tbl->Add(kstring("?-"),		255,	Operator::FX,	Operator::QUERY);
	tbl->Add(kstring(";"),		254,	Operator::XFY,	Operator::OR);
	tbl->Add(kstring(","),		253,	Operator::XFY,	Operator::AND);
	tbl->Add(kstring("spy"),	240,	Operator::FX,	Operator::SPY);
	tbl->Add(kstring("nospy"),	240,	Operator::FX,	Operator::NOSPY);
	tbl->Add(kstring("not"),	60,		Operator::FX,	Operator::NOT);
	tbl->Add(kstring("is"),		40,		Operator::XFX,	Operator::IS);
	tbl->Add(kstring(":="),		40,		Operator::XFX,	Operator::ASS);
	tbl->Add(kstring("=.."),	40,		Operator::XFX,	Operator::UNIV);
	tbl->Add(kstring("="),		40,		Operator::XFX,	Operator::EQ);
	tbl->Add(kstring("\\="),	40,		Operator::XFX,	Operator::NE);
	tbl->Add(kstring("<"),		40,		Operator::XFX,	Operator::LT);
	tbl->Add(kstring("=<"),		40,		Operator::XFX,	Operator::LE);
	tbl->Add(kstring(">="),		40,		Operator::XFX,	Operator::GE);
	tbl->Add(kstring(">"),		40,		Operator::XFX,	Operator::GT);
	tbl->Add(kstring("=="),		40,		Operator::XFX,	Operator::STRICTEQ);
	tbl->Add(kstring("\\=="),	40,		Operator::XFX,	Operator::STRICTNE);
	tbl->Add(kstring("-"),		31,		Operator::YFX,	Operator::SUB);
	tbl->Add(kstring("+"),		31,		Operator::YFX,	Operator::ADD);
	tbl->Add(kstring("/"),		21,		Operator::YFX,	Operator::DIV);
	tbl->Add(kstring("*"),		21,		Operator::YFX,	Operator::MUL);
	tbl->Add(kstring("mod"),	11,		Operator::XFX,	Operator::MOD);
	tbl->Add(kstring("^"),		10,		Operator::XFY,	Operator::POT);
	tbl->Add(kstring("\\"),		50,		Operator::XFY,	Operator::PATHNAME);

	tbl->Add(kstring(":-", 1),	255,	Operator::FX,	Operator::RULEUNARY);
	tbl->Add(kstring("-", 1),	31,		Operator::FX,	Operator::SUBUNARY);
	tbl->Add(kstring("+", 1),	31,		Operator::FX,	Operator::ADDUNARY);

	tbl->Add(kstring(":{", 1),	31,		Operator::XF,	Operator::I_BEGIN);
	tbl->Add(kstring(":}", 1),	31,		Operator::XF,	Operator::I_END);
	tbl->Add(kstring(":<", 1),	31,		Operator::FX,	Operator::I_INHERIT);
	tbl->Add(kstring(":/", 1),	31,		Operator::FX,	Operator::I_IMPORT);
	tbl->Add(kstring(":\\", 1),	31,		Operator::FX,	Operator::I_EXPORT);
	tbl->Add(kstring(":+", 1),	31,		Operator::FX,	Operator::I_DYNAMIC);
	tbl->Add(kstring(":$", 1),	31,		Operator::FX,	Operator::I_HANDLER);

	tbl->Add(kstring(":@", 1),	31,		Operator::XFX,	Operator::I_CREATE);
	tbl->Add(kstring(":", 1),	241,	Operator::XFX,	Operator::I_CALL);
	tbl->Add(kstring(":*", 1),	241,	Operator::XFX,	Operator::I_GCALL);
	tbl->Add(kstring(":~", 1),	31,		Operator::FX,	Operator::I_DESTROY);

	tbl->Add(kstring(":^", 1),	31,		Operator::XFX,	Operator::I_PROPLIST);
	tbl->Add(kstring(":&", 1),	31,		Operator::XFX,	Operator::I_ISA);

	kstring kl("."), kc("!");
}
