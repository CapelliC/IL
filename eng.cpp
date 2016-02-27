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

/////////////////////////////////////
// engine objects mapped to handlers
//
#include "stdafx.h"
#include "query.h"
#include "eng.h"
#include "dbintlog.h"
#include "builtin.h"
#include "parse.h"
#include "defsys.h"
#include <stdarg.h>

/////////////////////////
// initialize environment
//
EngineHandlers::EngineHandlers(ostream *msgStream)
{
	// global string table
	kstring::initMem();

	// local operator table
	OpTbl = new OperTable;
	AddStdOpers(OpTbl);

	// initialize streams
	IntlogIOStreams::setuser(kstring("user"), MSR_NULL);

	// global messages table
	MsgTbl = new MsgTable(msgStream);
	AddStdMsg(MsgTbl);

	// shared parser
	ParseG = new IntlogParser(OpTbl);

	// install system wide builtins
	DbIntlog::_pGlobalBuiltins = new DbIntlog;
	BuiltIn::Initialize(DbIntlog::_pGlobalBuiltins, get_msgtbl());
}

//////////////////////////////
// release global environment
//
EngineHandlers::~EngineHandlers()
{
	IntlogIOStreams::ReleaseBinLib();

	delete MsgTbl;
	delete OpTbl;
	delete ParseG;

	delete DbIntlog::_pGlobalBuiltins;

	clear();
	Term::FreeMem();

	kstring::freeMem();
}

////////////////////
// create an engine
//
EngHandle EngineHandlers::create(DbIntlog *dbowner)
{
	DbIntlog *db = makeDb(dbowner);

	eh *ep = new eh;
	ep->eng = build(db);
	ep->handle = append(ep);

	ep->eng->tell(IntlogIOStreams::name());

	return ep->handle;
}

///////////////////////
// default Db creation
//
DbIntlog *EngineHandlers::makeDb(kstring id, DbIntlog *owner)
{
	return new DbIntlog(id, owner);
}
DbIntlog *EngineHandlers::makeDb(DbIntlog *owner)
{
	return new DbIntlog(owner);
}

/////////////////////
// destroy an engine
//
int EngineHandlers::destroy(EngHandle h)
{
	eh *ep = getH(h);
	if (ep) {
		ep->eng->closeall();
		delete ep->eng->get_db();
		delete ep->eng;
		ep->eng = 0;
		return 1;
	}
	return 0;
}

//////////////////////////
// search data by handler
//
IntlogExec *EngineHandlers::HtoD(EngHandle h) const
{
	eh *ep = getH(h);
	return ep? ep->eng : 0;
}

//////////////////////////
// search handler by data
//
EngHandle EngineHandlers::DtoH(const IntlogExec *e) const
{
	slist_iter i(*this);
	eh *ep;
	while ((ep = (eh*)i.next()) != 0)
		if (ep->eng == e)
			return ep->handle;
	return EH_NULL;
}

////////////////////////
// search data in list
//
EngineHandlers::eh *EngineHandlers::getH(EngHandle h) const
{
	slist_iter i(*this);
	eh *ep;
	while ((ep = (eh*)i.next()) != 0)
		if (ep->handle == h)
			break;
	return ep;
}
EngineHandlers::eh::~eh()
{
	if (eng) {
		eng->closeall();
		delete eng->get_db();
		delete eng;
	}
}

#ifdef _AFXDLL

// system control
EngineHandlers* GetEngines()
{
	extern EngineHandlers* all_engines;
	return all_engines;
}
void SetEngines(EngineHandlers* pEngines)
{
	extern EngineHandlers* all_engines;
	all_engines = pEngines;
}

#endif

////////////////////////////////
// output a message from table
//
void EngineHandlers::ErrMsg(int code, ...)
{
	va_list argptr;
	MsgTable::MsgLink *l = MsgTbl->search(code);

	char buf[512];

	ostream &s = *(MsgTbl->str);
	if (!l)
	{
		s << "message not found: " << code << endl;
		return;
	}

	va_start(argptr, code);
	do
	{
        //_vsnprintf(buf, sizeof buf, l->string, argptr);
        vsprintf(buf, l->string, argptr);
		s << buf << endl;
	}
	while ((l = MsgTbl->search(code, l)) != 0);
}
