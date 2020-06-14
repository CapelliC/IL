
/*
    IL : Intlog Language
    Object Oriented Prolog Project
    Copyright (C) 1992-2020 - Ing. Capelli Carlo

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


#include "stdafx.h"
#include "builtin.h"
#include "query.h"
#include "btmix.h"
#include "btil.h"
#include "bterr.h"
#include "dbintlog.h"

#include "btswi0.h"

DbIntlog* DbIntlog::_pGlobalBuiltins = nullptr;

///////////////////////////////
// scan vector adding to table
//
void DbIntlog::addtable(BuiltIn* bt, int vectsize)
{
	for (int i = 0; i < vectsize; i++, bt++)
	{
		int nargs = bt->args & ~BuiltIn::retry;

		if (is_builtin(bt->ident, nargs))
			GetEngines()->ErrMsg(BTERR_REDEFINED, bt->ident);
		else
		{
			DbEntry *dbe = new DbEntry(bt->ident, nargs);
			insert(dbe);
			dbe->entries.append(new e_DbList(bt));
		}
	}
}

////////////////////////////////
// check for existance in table
//
BuiltIn* DbIntlog::is_builtin(const char *id, int arity) const
{
	DbEntry dbe(id, arity);

	const DbIntlog *dbi = this;
	while (dbi)
	{
		DbEntry *e = dbi->isin(&dbe);
		if (e && e->entries.numel() > 0)
		{
			e_DbList *g = (e_DbList *)e->entries.get_first();
			if (g->type == e_DbList::tBuiltin)
				return g->bltin;
		}

		dbi = dbi->m_father;
	}

	return _pGlobalBuiltins != this?
		_pGlobalBuiltins->is_builtin(id, arity) : 0;
}

////////////////////////////////////////
// setup messages and standard builtins
//
void BuiltIn::Initialize(DbIntlog* pDB, MsgTable *msg)
{
	msg->add(BTERR_REQ_INT_ARG,		"atteso argomento 'integer'");
	msg->add(BTERR_NOT_VALID_VAR,	"argomento variabile non valido");
	msg->add(BTERR_INVALID_OPERATOR,"operatore aritmetico '%s' non valido");
	msg->add(BTERR_DIFF_ARGS,		"differente tipo argomenti");
	msg->add(BTERR_INVALID_ARG_TYPE,"tipo argomento non valido");
	msg->add(BTERR_REQ_LIST_ARG,	"richiesto argomento 'lista'");
	msg->add(BTERR_REQ_INSTANCE_ARG,"richiesto argomento istanziato");
	msg->add(BTERR_REDEFINED,		"ridefinito");
	msg->add(BTERR_UNIMPLEMENTED,	"non implementato");
	msg->add(BTERR_ONLY_DOS,		"definito solo in DOS");

	msg->add(BTERR_DIVIDE_BY_ZERO,	"divisione per 0");
	msg->add(BTERR_MODULUS_BY_ZERO,	"modulo di 0");
	msg->add(BTERR_DIFF_EXPR_TYPES,	"differenti tipi espressione");
	msg->add(BTERR_UNDEFINED,		"elemento indefinito");

	msg->add(BTERR_CANT_IMPORT,		"'%s' non importabile");
	msg->add(BTERR_CANT_EXPORT,		"'%s' non esportabile");
	msg->add(BTERR_CANT_BEGINTERF,	"'%s' non apribile");
	msg->add(BTERR_CANT_ENDINTERF,	"annidamento su '%s' (atteso '%s')");
	msg->add(BTERR_CANT_INHERIT,	"'%s' interfaccia indefinita");
	msg->add(BTERR_CANT_CREATE_ENG,	"istanza non creabile");
	msg->add(BTERR_CANT_FIND_DB,	"non trovo '%s'");
	msg->add(BTERR_CANT_DESTROY_ENG,"errore eliminazione");
	msg->add(BTERR_CANT_DYNAMIC,	"'%s' non ok");

	#define addt(t)	pDB->addtable(t, sizeof(t)/sizeof(t[0]))

	addt(IO_oriented);
	addt(execution_control);
	addt(database_control);
	addt(tracing_control);
	addt(metalogical);
	addt(comparison);
	addt(arithmetic);
	addt(classification);
	addt(mixing);
	addt(interface_obj);
	addt(mathfunctions);

    //#define adda(t)	pDB->addtable(t.begin(), t.size())
    addt(btswi0);
}
