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

#include <QtCore/QCoreApplication>
#include "stdafx.h"
#include "iafx.h"
#include "constr.h"
#include "qdata.h"
#include "parse.h"
#include <sstream>

static int runquery(IntlogExec *, const char *);
static void load_args(char *argfile);
static void createBinLib(IntlogExec *eng, int &arg, int argc, char **argv);

#define Msg_FileNotFound "File Not Found"
#define Msg_BadLibName "Bad Lib Name"

int main(int argc, char **argv)
{
    QCoreApplication a(argc, argv);

    SetEngines(new ConsoleEngines);
    IntlogExec *eng = GetEngines()->HtoD(GetEngines()->create());

    if (argc == 1)
        eng->use_file("user");

    for (int arg = 1; arg < argc; arg++)
    {
        char *parg = argv[arg];

        if (parg[0] == '-')
        {
            switch (parg[1])
            {
            case 'u':
                eng->use_file("user");
                break;

            case 'c':
                createBinLib(eng, arg, argc, argv);
                break;

            case 'b':
                eng->ReadFromBinLib(kstring(parg + 2));
                break;

            case 'q':
                runquery(eng, parg + 2);
            }
        }
        else
            eng->use_file(kstring(parg));
    }
    delete GetEngines();

    return a.exec();
}

void load_args(char *argfile)
{
    ifstream f(argfile);
    if (f)
    {
        char buf[128];
        while (f >> buf)
            IntlogIOStreams::AddBinLibSourceName(kstring(buf));
    }
    else
        cerr << Msg_FileNotFound << ':' << argfile << endl;
}

void createBinLib(IntlogExec *eng, int &arg, int argc, char **argv)
{
    char *libname = argv[arg++] + 2;
    if (*libname)
    {
        eng->CreateBinLib(kstring(libname));
        while (arg < argc)
        {
            char *parg = argv[arg];
            if (parg[0] == '-')
                break;
            if (parg[0] == '@')
                load_args(parg + 1);
            else
                eng->AddBinLibSourceName(kstring(parg));
            arg++;
        }
    }
    else
        cerr << Msg_BadLibName << endl;
}

static int query(IntlogExec *eng, Term tquery, kstr_list *var_ids, Term *v, int nv)
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

int runquery(IntlogExec *ile, const char *src)
{
    int rc = -1;

    Term *var_val = 0;
    kstr_list var_ids;

    // prepare input source stream
    IntlogParser* parser = GetEngines()->get_parser();
    istringstream srcstream(src);

    // save status
    parser->SetSource(&srcstream, "-q", &var_ids);

    // submit source line: if parse ok, query
    if (parser->getinput() == IntlogParser::NewClause)
    {
        Term tparsed = parser->parsed;

        int nv = var_ids.numel();
        if (nv > 0)
            var_val = new Term[nv];

        rc = query(ile, tparsed, &var_ids, var_val, var_ids.numel());

        delete [] var_val;
        tparsed.Destroy();
    }

    // some error occurred
    return rc;
}
