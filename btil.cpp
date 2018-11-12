
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


////////////////////////////////////////////////////
// my engine object model
//	use handlers to map engine(s) data to execution
////////////////////////////////////////////////////

#include "stdafx.h"
#include "qdata.h"
#include "builtin.h"
#include "btil.h"
#include "argali.h"
#include "defsys.h"
#include "bterr.h"

/////////////////////////////////////
// MUST be allocated from main()...
/////////////////////////////////////
EngineHandlers *all_engines;

/////////////////////
// scripts interface
/////////////////////

// construction control
BtFDecl(i_begin);
BtFDecl(i_end);

// procedures' properties
BtFDecl(i_inherit);
BtFDecl(i_import);
BtFDecl(i_export);
BtFDecl(i_dynamic);
BtFDecl(i_handler);

// instances control
BtFDecl(i_create);
BtFDecl(i_call);
BtFDecl(i_gcall);
BtFDecl(i_destroy);

// information
BtFDecl(i_isa);
BtFDecl(i_proplist);

BuiltIn interface_obj[13*2-1] = {

	{"i_begin",		1,					i_begin},		{":{",	1,					i_begin},
	{"i_end",		1,					i_end},			{":}",	1,					i_end},
	{"i_inherit",	1,					i_inherit},		{":<",	1,					i_inherit},
	{"i_import",	1,					i_import},		{":/",	1,					i_import},
	{"i_export",	1,					i_export},		{":\\",	1,					i_export},
	{"i_dynamic",	1,					i_dynamic},		{":+",	1,					i_dynamic},
	{"i_handler",	1,					i_handler},		{":$",	1,					i_handler},

	{"i_create",	2,					i_create},		{":@",	2,					i_create},
	{"i_call",		2,					i_call},		{":",	2,					i_call},
	{"i_destroy",	1,					i_destroy},		{":~",	1,					i_destroy},
	{"i_gcall",		2|BuiltIn::retry,	i_gcall},		{":*",	2|BuiltIn::retry,	i_gcall},

	{"i_proplist",	3,					i_proplist},
	{"i_isa",		1,					i_isa},			{":&",	1,					i_isa}
};

///////////////////////////////////////
// engine object code
//	mapping instanced engines to terms
///////////////////////////////////////

static SysData* mkEngObj() { return new EngineObj(EH_NULL); }
SysData::rtinfo EngineObj::rti = { "EngObj", mkEngObj, nullptr };

EngineObj::EngineObj(EngHandle h)
: SysData(rti)
{
	handler = h;
}

void EngineObj::show(ostream &s) const
{
	IntlogExec *eng = all_engines->HtoD(handler);

	if (eng) {
		s << '$' << handler << ':';
		DbIntlog *db = eng->get_db();

		kstring dbid = db->GetId();
		if (unsigned(dbid) != MSR_NULL)
			s << CCP(dbid);

		s << ':';
		DbInherIter ii(db);
        while ((db = ii.next()) != nullptr) {
			dbid = db->GetId();
			if (unsigned(dbid) != MSR_NULL)
				s << CCP(dbid);
			s << ':';
		}
	} else
		s << '~' << handler;
}

Term EngineObj::copy() const
{
	return Term(new EngineObj(handler));
}

int EngineObj::unify(Term t) const
{
	EngineObj *e = get_eng(t);
	return e && e->get_handler() == handler;
}

////////////////////////////////
// access checking Term* values
//
int EngineObj::is_eng(Term t)
{
	return t.type(f_SYSDATA) && SysDataPtr(t)->istype(rti.id);
}
EngineObj* EngineObj::get_eng(Term t)
{
	if (t.type(f_SYSDATA)) {
		SysDataPtr sp = t;
		if (sp->istype(rti.id))
			return (EngineObj*)sp;
	}
    return nullptr;
}

/**************************
	construction control
***************************/

static int chgprop(Term, IntlogExec*, DbEntry::scopemode, CCP);
static DbIntlog *findint(DbIntlog *, kstring);

/////////////////////////////////////////
// i_begin(i_name) (+)
//	insert i_name as local to current DB
//	push empty DB(i_name)
//
BtFImpl(i_begin, a, p)
{
	Term t;
	if (!(t = p->eval_term(a.getarg(0))).type(f_ATOM)) {
		p->BtErr(BTERR_INVALID_ARG_TYPE);
		return 0;
	}

	DbIntlog *db = p->get_db();
	kstring idi = t.kstr();
    if ((db = db->BeginInterface(idi)) == nullptr) {
		p->BtErr(BTERR_CANT_BEGINTERF, CCP(idi));
		return 0;
	}

	p->set_db(db);
	return 1;
}

///////////////////////////////////////////////////
// i_end(i_name) (+)
//	pop current DB (interface became instantiable)
//
BtFImpl(i_end, a, p)
{
	Term t;
	if (!(t = p->eval_term(a.getarg(0))).type(f_ATOM)) {
		p->BtErr(BTERR_INVALID_ARG_TYPE);
		return 0;
	}

	DbIntlog *db = p->get_db();
	kstring dbid = db->GetId(), idi = t.kstr();
    if (dbid != idi || (db = db->EndInterface()) == nullptr) {
		CCP sdb = MemStoreRef(dbid) != MSR_NULL ? dbid : "";
		p->BtErr(BTERR_CANT_ENDINTERF, CCP(idi), sdb);
		return 0;
	}

	p->set_db(db);
	return 1;
}

///////////////////////////////////////////////
// i_inherit(i_name*) (+)
//	store references to inherited interfaces
//
BtFImpl(i_inherit, t, p)
{
	ArgIdArityList i(t.getarg(0), p, "i_inherit");

	while (i.next()) {
		DbIntlog *dbi = findint(p->get_db(), i.funct);
		if (!dbi || !p->get_db()->InheritInterface(dbi)) {
			p->BtErr(BTERR_CANT_INHERIT, CCP(i.funct));
			break;
		}
	}

	return i.status;
}

///////////////////////////////////////
// i_export(p_name*) (+)
//	declare locally defined procs that
//	are externally callable
//
BtFImpl(i_export, t, p)
{
	return chgprop(t.getarg(0), p, DbEntry::exported, "i_export");
}

///////////////////////////////////////////
// i_import(p_name*) (+)
//	declare which externally defined procs
//	will be locally called
//
BtFImpl(i_import, t, p)
{
	return chgprop(t.getarg(0), p, DbEntry::import, "i_import");
}

//////////////////////////////////////////////////
// i_dynamic(p_name*) (+)
//	declare which procs work as per instance data
//
BtFImpl(i_dynamic, t, p)
{
	return chgprop(t.getarg(0), p, DbEntry::dynamic, "i_dynamic");
}

//////////////////////
// i_handler(Handler)
//
BtFImpl(i_handler, t, p)
{
	EngineObj *o = new EngineObj(all_engines->DtoH(p));
	return p->unify(p->save(Term(o)), t.getarg(0));
}

/*************************
	instances control
*************************/

//////////////////////////////////////////
// i_create(i_name, HandleEngine)
//	create a new running interface (+, -)
//
BtFImpl(i_create, t, p)
{
	ASSERT(all_engines);

	ArgIdArityList i(t.getarg(0), p, "i_create");

	if (!p->eval_term(t.getarg(1)).type(f_NOTERM)) {
		p->BtErr(BTERR_INVALID_ARG_TYPE);
		return 0;
	}

	EngHandle h = GetEngines()->create(p->get_db());
	if (h == EH_NULL) {
		p->BtErr(BTERR_CANT_CREATE_ENG);
		return 0;
	}

	IntlogExec *pExec = GetEngines()->HtoD(h);
	DbIntlog *engdb = pExec->get_db(), *dbi;

	// scan all interfaces names
	int errc = 0;
	while (i.next()) {

		// search for declared interface (from current to root)
        if ((dbi = findint(p->get_db(), i.funct)) == nullptr)
			errc = BTERR_CANT_FIND_DB;
		else if (!engdb->InheritInterface(dbi))
			errc = BTERR_CANT_INHERIT;

		if (errc) {
			// on error signal and reset
			p->BtErr(errc, CCP(i.funct));
			all_engines->destroy(h);
			return 0;
		}
	}

	return p->unify(p->save(Term(new EngineObj(h))), t.getarg(1));
}

////////////////////////////////////////////
// i_destroy(HandlerEng) (+)
//	release a previously created interface
//
BtFImpl(i_destroy, t, p)
{
	ASSERT(all_engines);

	Term te = p->eval_term(t.getarg(0));
	EngineObj *e = EngineObj::get_eng(te);
	if (!e) {
		p->BtErr(BTERR_INVALID_ARG_TYPE);
		return 0;
	}

	// explicitly release memory??
	return all_engines->destroy(e->get_handler());
}

////////////////////////////////////////////////////////////
// i_call(HandlerEng, Goal) (+, +)
//	make a call: generate multiple solutions at lower level
//
static int make_call(TermArgs t, IntlogExec *p, int mode)
{
	ASSERT(all_engines);

	if (mode == 1)
        return p->gcall(Term(f_NOTERM), nullptr);

	Term te = p->eval_term(t.getarg(0)),
		 tq = p->copy(t.getarg(1));

	EngineObj *e = EngineObj::get_eng(te);
	IntlogExec *d;
	if (	!e ||
			tq.type(f_NOTERM) ||
            (d = all_engines->HtoD(e->get_handler())) == nullptr) {
		p->BtErr(BTERR_INVALID_ARG_TYPE);
		tq.Destroy();
		return 0;
	}

	return mode == 0? p->gcall(tq, d) : p->call(tq, d);
}

BtFImpl(i_call, t, p)
{
	return make_call(t, p, -1);
}

BtFImpl_R(i_gcall, t, p, r)
{
	return make_call(t, p, r);
}

/******************
	information
******************/

/////////////////////////////////////////
// i_isa(Name, Eng)
//	(+, +) check matching name
//	(-, +) return name of engine
//	(+, -) return next named engine	?
//	(-, -) return next unnamed engine ?
//
BtFImpl(i_isa, t, p)
{
	Term tn = p->eval_term(t.getarg(0)),	// name
		 te = p->eval_term(t.getarg(1));	// engine

	ASSERT(all_engines);

	if (!tn.type(f_ATOM) || !EngineObj::is_eng(te)) {
		p->BtErr(BTERR_INVALID_ARG_TYPE);
		return 0;
	}

//	EngineObj *o = EngineObj::get_eng(te);
	return 0;
}

/////////////////////////////////////////////
// i_proplist(NameInt,Property,List) (+,+,?)
// retrieve property name/arity list
//	for required interface e property
//
BtFImpl(i_proplist, t, p)
{
	Term NameInt = p->eval_term(t.getarg(0)),
		 Prop = p->eval_term(t.getarg(1)),
		 ListProp(ListNULL);
	DbIntlog *db;

	if (	!NameInt.type(f_ATOM) ||
			!Prop.type(f_ATOM) ||
            (db = findint(p->get_db(), NameInt.kstr())) == nullptr)
	{ err:
		p->BtErr(BTERR_INVALID_ARG_TYPE);
		return 0;
	}

	// search for property
	DbEntry::scopemode prop;

	BuiltIn* bt = p->get_db()->is_builtin(Prop, 1);
	if (!bt)
		goto err;

	if (bt->eval == i_export)	prop = DbEntry::exported;
	else
	if (bt->eval == i_import)	prop = DbEntry::import;
	else
	if (bt->eval == i_dynamic)	prop = DbEntry::dynamic;
	else
	if (bt->eval == i_begin)	prop = DbEntry::local;
	else goto err;

	slist lprop;
	db->EntriesWithProperty(prop, lprop);

	// transform the slist in Prolog List
	if (lprop.numel() > 0) {

		Term tail(f_NOTERM);
		slist_iter it(lprop);
		DbIntlog::EntryWithProp *pEntry;
		while ((pEntry = (DbIntlog::EntryWithProp *)it.next()) != 0) {

			// build the name/arity structure
			Term sProp = Term(kstring(Operator::DIV), 2);
			sProp.setarg(0, Term(kstring(pEntry->id)));
			sProp.setarg(1, Term(Int(pEntry->arity)));

			Term elem = Term(sProp, Term(ListNULL));
			if (tail.type(f_NOTERM))
				tail = ListProp = elem;
			else {
				tail.setarg(1, elem);
				tail = elem;
			}
		}

		ListProp = p->save(ListProp);
	}

	return p->unify(t.getarg(2), ListProp);
}

////////// utilities //////////

static DbIntlog *findint(DbIntlog *base, kstring id)
{
	DbIntlog *dbi;
	while (base) {
        if ((dbi = base->IsLocalInterface(id)) != nullptr)
			return dbi;
		base = base->GetFather();
	}
    return nullptr;
}
static int chgprop(Term t, IntlogExec* p, DbEntry::scopemode sc, CCP idbt)
{
	ArgIdArityList i(t, p, idbt);

	while (i.next())
		if (!p->get_db()->ChangeEntryProperty(i.funct, i.arity, sc)) {
			p->BtErr(BTERR_CANT_IMPORT, CCP(i.funct));
			break;
		}

	return i.status;
}
