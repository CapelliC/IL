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

#include "stdafx.h"
#include <sstream>
#include "iafx.h"
#include "qdata.h"
#include "parse.h"

#ifdef _AFXDLL
#include "afxdllx.h"
static AFX_EXTENSION_MODULE NEAR extensionDLL = { 0, 0 };
#endif

static int runquery(IntlogExec *, Term, kstr_list *, Term*, int);

#ifdef _AFXDLL
extern "C" int CALLBACK LibMain(HINSTANCE hInstance, WORD, WORD, LPSTR)
{
	AfxInitExtensionModule(extensionDLL, hInstance);
	return 1;
}
#endif

/////////////////////////////////
// init DLL: if no engPtr given
//	rely on default IO handlers
//
IIFTYPE(void) IAFX_Initialize()
{
#ifdef _AFXDLL
	// create a new CDynLinkLibrary
	new CDynLinkLibrary(extensionDLL);
	TRACE("IAFX_Initialize\n");
#endif

	// force linkage
	SetEngines(0);
}

IIFTYPE(void) IAFX_Terminate()
{
}

/////////////////////////////////////////////////////////////////
// attempt a parse and call
//	if query succeed, copy vars values to v[] (up to nv elements)
//	you must guarantee that nv >= 'num of vars in src'
//
IIFTYPE(int) IAFX_QueryString(const char *src, EngHandle h, Term **var_val, kstr_list *var_ids)
{
	int rc = -1;	// if some error occurs
	IntlogExec *ile = GetEngines()->HtoD(h);

	if (ile != 0)
	{
		// prepare input source stream
		IntlogParser* parser = GetEngines()->get_parser();
        istringstream srcstream(src);

		// save status
		parser->SetSource(&srcstream, "QueryString", var_ids);

		// submit source line: if parse ok, query
		if (parser->getinput() == IntlogParser::NewClause)
		{
			Term tparsed = parser->parsed;

			int nv = var_ids->numel();
			if (nv > 0)
				*var_val = new Term[nv];

			rc = runquery(ile, tparsed, var_ids, *var_val, var_ids->numel());

			tparsed.Destroy();
		}
	}

	// some error occurred
	return rc;
}

//////////////////////////////////////////
// run query with some interface handling
//
IIFTYPE(int) IAFX_QueryTerm(Term toQuery, EngHandle h, Term *v, kstr_list *vars)
{
	IntlogExec *ile = GetEngines()->HtoD(h);
	return ile? runquery(ile, toQuery, vars, v, vars->numel()) : -1;
}

//////// LOCALs //////////

static int runquery(
	IntlogExec *eng, Term tquery, kstr_list *var_ids,
	Term *v, int nv)
{
	for (int j = 0; j < nv; j++)
		v[j] = Term(f_NOTERM);

	// save current status
	ProofStatus ps(eng);
	stkpos cvbase = ps.pdim();

	Clause q(tquery, var_ids, eng->get_db());
	int rc = eng->query(&q);

	if (rc) // query succed: copy variables to caller
		for (int i = 0; i < nv; i++)
			v[i] = eng->value(Var(i), cvbase);

	q.no_image();
	ps.restore();

	return rc;
}
