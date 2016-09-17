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
*/

#ifndef _ENG_H_
#define _ENG_H_

//---------------------------------
// engine(s) tied in handlers list
//---------------------------------

#include "iafx.h"

// forward classes
class IntlogExec;
class DbIntlog;
class OperTable;
class IntlogParser;
class MsgTable;

typedef unsigned	EngHandle;
#define EH_NULL		unsigned(-1)

#include "kstr.h"

class IAFX_API EngineHandlers : protected slist
{
public:
	EngineHandlers(ostream *msgStream);
	virtual ~EngineHandlers();

	virtual EngHandle create(DbIntlog* = 0);
	virtual int destroy(EngHandle);

	IntlogExec* HtoD(EngHandle) const;
	EngHandle DtoH(const IntlogExec*) const;

	virtual DbIntlog *makeDb(kstring, DbIntlog *);
	virtual DbIntlog *makeDb(DbIntlog *);

	OperTable* get_optbl() const;
	IntlogParser* get_parser() const;
	MsgTable* get_msgtbl() const;

	// display global error messages
	virtual void ErrMsg(int code, ...);

protected:

	virtual IntlogExec* build(DbIntlog*) = 0;

	struct eh : public e_slist {
		IntlogExec*	eng;
		EngHandle	handle;
		~eh();
	};

	eh *getH(EngHandle) const;

	OperTable* OpTbl;
	IntlogParser* ParseG;
	DbIntlog* DbRoot;
	MsgTable* MsgTbl;
};

inline IntlogParser* EngineHandlers::get_parser() const
{
	return ParseG;
}
inline OperTable* EngineHandlers::get_optbl() const
{
	return OpTbl;
}
inline MsgTable* EngineHandlers::get_msgtbl() const
{
	return MsgTbl;
}

// system control
#ifndef _AFXDLL
	extern EngineHandlers* all_engines;
	inline EngineHandlers* GetEngines()	{ return all_engines; }
	inline void SetEngines(EngineHandlers* pEngines) { all_engines = pEngines; }
#else
	extern IAFX_API EngineHandlers* GetEngines();
	extern IAFX_API void SetEngines(EngineHandlers* pEngines);
#endif

#endif
