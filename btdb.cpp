
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


//--------------------
// database interface
//--------------------

#include "stdafx.h"
#include "defsys.h"
#include "builtin.h"
#include "bterr.h"
#include "qdata.h"
#include "argali.h"
#include "eng.h"

// database control
BtFDecl(consult);
BtFDecl(reconsult);
BtFDecl(listing);
BtFDecl(asserta);
BtFDecl(assertz);
BtFDecl(retract);
BtFDecl(dbdisplay);

BuiltIn database_control[7] = {
	{"retract",		1|BuiltIn::retry,	retract},
	{"consult",		1,	consult},
	{"reconsult",	1,	reconsult},
	{"asserta",		1,	asserta},
	{"assert",		1,	assertz},
	{"listing",		1,	listing},
	{"dbdisplay",	1,	dbdisplay}
};

//--------------------------------------------
// load a file into memory DB attached to <p>
//
BtFImpl(consult, t, p)
{
	Term v = p->eval_term(t.getarg(0));

	if (v.type(f_ATOM))
		return p->use_file(v.get_funct());

	p->BtErr(BTERR_INVALID_ARG_TYPE);
	return 0;
}

//--------------------------------------------
// load a file into memory DB attached to <p>
//
BtFImpl(reconsult, t, p)
{
	Term v = p->eval_term(t.getarg(0));

	if (v.type(f_ATOM))
		return p->reuse_file(v.kstr());

	p->BtErr(BTERR_INVALID_ARG_TYPE);
	return 0;
}

static int dbassert(TermArgs t, IntlogExec *p, int mode)
{
	Term v = p->copy(t.getarg(0));
	if (!v.type(f_NOTERM))
	{
		Clause *c = p->make_clause(v);
		if (c)
		{
			DbIntlog *dbt = p->on_call()->get_clause()->get_db(),
					 *dbs = p->get_db();
			ASSERT(dbt);
			dbt->Add(c, mode, dbs);
			return 1;
		}
	}

	p->BtErr(BTERR_INVALID_ARG_TYPE);
	return 0;
}

//--------------------------------
// insert a fact into DB as first
//
BtFImpl(asserta, t, p)
{
	return dbassert(t, p, 1);
}

//-----------------------
// append a fact into DB
//
BtFImpl(assertz, t, p)
{
	return dbassert(t, p, 0);
}

//-----------------------------------------------
// delete from DB bind to <p> the required entry
//	on retry search subsequent matchs
//

//////////////////////////////////////
// sequential access to clauses in DB
//
class SeqClauseAccess : public BltinData
{
public:
	~SeqClauseAccess();
	int first(IntlogExec*, Term, int);

	Term v;
	DbIntlog *db;
	DbEntryIter i;
};

/////////////////////////////////////
// initialize sequential search data
//
int SeqClauseAccess::first(IntlogExec* p, Term t, int errc)
{
	if ((v = p->copy(t)).type(f_NOTERM))
	{
		p->BtErr(errc);
		return 0;
	}

	// search first entry
	db = p->on_call()->get_clause()->get_db();
	ASSERT(db);

	if (p->find_match(v, i, db))
	{
		p->set_btdata(p->save(this));
		return 1;
	}

	// will not be called again
	return 0;
}
SeqClauseAccess::~SeqClauseAccess()
{
	v.Destroy();
}

BtFImpl_R(retract, t, p, r)
{
	SeqClauseAccess *pd;

	if (!r)
	{
		pd = new SeqClauseAccess;
		if (!pd->first(p, t.getarg(0), BTERR_REQ_INSTANCE_ARG))
		{
			delete pd;
			return 0;
		}
		return pd->db->Del(pd->i);
	}

	// previously try OK, goto next
	pd = (SeqClauseAccess*)p->get_btdata();
	if (p->find_match(pd->v, pd->i))
		return pd->db->Del(pd->i);

	return 0;
}

//--------------------------------
// utility to list clauses images
//
static void show_clause(Clause *c, ostream &s)
{
	Term i = c->get_image();

	if (i.is_rule())
	{
		s << i.getarg(0) << CCP(i.get_funct()) << "\n\t";
		i = i.getarg(1);
		while (i.is_and())
		{
			s << i.getarg(0) << ",\n\t";
			i = i.getarg(1);
		}
	}
	s << i << '.' << endl;
}

//-------------------------------------------------------
// search all matching identifiers in DB attached to <p>
//
BtFImpl(listing, t, p)
{
	ArgIdArityList a(t.getarg(0), p, "listing");

	Clause *c;
	while (a.next())
	{
		DbIdArityIter i(p->get_db(), a.funct, a.arity);
		while ((c = i.next()) != 0)
			show_clause(c, p->out());
	}

	return 1;
}

BtFImpl(dbdisplay, t, p)
{
	Term v = p->eval_term(t.getarg(0));
	if (v.type(f_INT))
	{
		p->get_db()->Display(p->out(), Int(v));
		return 1;
	}

	p->BtErr(BTERR_INVALID_ARG_TYPE, "dbdisplay");
	return 0;
}
