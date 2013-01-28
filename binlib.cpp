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

///////////////////////
// binary library data
///////////////////////

#include "stdafx.h"
#include "iafx.h"
#include "binlib.h"
#include "defsys.h"
#include "eng.h"
#include "binstr.h"
#include "parsemsg.h"
#include "proios.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

////////////////////////////////////////////
// on read use already stored files
// on write register each file to be stored
//
SourceBinaryLib::SourceBinaryLib(kstring id, openMode mode)
{
    idfile = id;
    kstrv = 0;
    inp = 0;

    if (mode == toRead)
    {
        filebuf *pbuf = IntlogIOStreams::openfile(id, ios::in|ios::binary);
        if (!pbuf)
        {
            GetEngines()->ErrMsg(BINL_NOT_FOUND, CCP(id));
            return;
        }

        inp = new istream(pbuf);
        if (chkerr(BINL_NOT_FOUND))
            return;

        load_directory();
    }
}

SourceBinaryLib::~SourceBinaryLib()
{
    delete [] kstrv;
    delete inp;
}

////////////////////////////////////
// load strings and names from file
//
void SourceBinaryLib::load_directory()
{
    // load strings in global table
    //	and keep pointers in vector
    bsread(*inp, &maxks);
    kstrv = new kstring[maxks];

    char bufstr[1024];
    for (unsigned ixs = 0; ixs < maxks; ixs++)
    {
        unsigned maxbuf = sizeof(bufstr);
        bsread(*inp, bufstr, &maxbuf);
        if (chkerr(LOADING_STRINGS))
            return;
        kstrv[ixs] = kstring(bufstr);
    }

    // read files directory
    unsigned nfiles;
    bsread(*inp, &nfiles);
    while (nfiles-- > 0)
    {
        BinIFile *f = new BinIFile(this);
        if (chkerr(LOADING_DIRECTORY))
            return;
        append(f);
    }
}

//////////////////////////////////////////////
// check stream status and show error message
//
int SourceBinaryLib::chkerr(int errcode, ios *s, CCP info) const
{
    if (!s)
        s = inp;
    if (s && s->rdstate() != ios::goodbit)
    {
        GetEngines()->ErrMsg(errcode, CCP(idfile), info);
        return 1;
    }
    return 0;
}

///////////////////////////////////
// method to match entries by name
//
int SourceBinaryLib::match(e_slist *e, void *p) const
{
    BinFile *f = (BinFile *)e;
    return CCP(f->get_id()) == CCP(p);
}

///////////////////////////////////////////
// register a file to be echoed in library
//
void SourceBinaryLib::add_name(kstring idf)
{
    if (!is_name(idf))
    {
        BinOFile *f = new BinOFile(idf, this);
        append(f);
        kstrl.add(idf);	// will use short form in directory...
    }
}
BinFile *SourceBinaryLib::is_name(kstring id) const
{
    return (BinFile*)seekptr((void*)CCP(id));
}

////////////////////////////////
// see if all output files done
//	if so, create the lib
//
void SourceBinaryLib::check_done_out()
{
    ASSERT(inp == 0);

    // search if some has more to do
    slist_iter i(this);
    BinOFile *of;
    while ((of = (BinOFile *)i.next()) != 0)
        if (of->status != BinOFile::done)
            return;

    // all done: create the output file library
    ofstream libfile(idfile, ios::out|ios::binary);
    if (chkerr(CREATE_OUTPUT_FILE, &libfile))
        return;

    // dump strings
    bswrite(libfile, kstrl.numel());
    kstr_list_iter strl(kstrl);
    kstring cs;
    while (CCP(cs = strl.next()) != 0)
        bswrite(libfile, CCP(cs));

    // save files directory
    bswrite(libfile, numel());
    streampos begdir = libfile.tellp();
    i.set(this);
    while ((of = (BinOFile *)i.next()) != 0)
        of->write_dir(libfile);

    // cat all files data
    i.set(this);
    while ((of = (BinOFile *)i.next()) != 0)
        of->cat_data(libfile);

    // rewrite directory with updated locations
    libfile.seekp(begdir);
    i.set(this);
    while ((of = (BinOFile *)i.next()) != 0)
        of->write_dir(libfile);

    chkerr(WRITE_OUTPUT_FILE, &libfile);

    // release memory used
    clear();
    kstrl.clear();
}

///////////////////////////////
// see if all input files used
//	if so, release memory
//
void SourceBinaryLib::check_done_inp()
{
    if (inp)
    {
        // search if some has more to do
        slist_iter i(this);
        BinIFile *f;
        while ((f = (BinIFile *)i.next()) != 0)
            if (!f->ateof())
                return;

        // all used: release memory
        delete inp;
        inp = 0;
        delete [] kstrv;
        kstrv = 0;
    }
}

///////////////////////////
// input file construction
//
BinIFile::BinIFile(SourceBinaryLib *l)
{
    plib = l;

    // read directory data
    id = readkstr();

#ifdef _DEBUG
    CCP ids = id;
#endif

#if 0
    bsread(*l->inp, &begdata);
    bsread(*l->inp, &enddata);
    bsread(*l->inp, &nobjects);
#else
    assert(false);
#endif

    // set location of next term read
    fcursor = begdata;
}

///////////////////////////////////////
// reload data of a clause
//	term structure and variables names
//
int BinIFile::read_term(Term *t, kstr_list *var_ids)
{
    if (plib->inp->tellg() != fcursor)
        plib->inp->seekg(fcursor);

    istream &s = *plib->inp;
    var_ids->clear();
    if (read_term(t))
    {
        unsigned nv;
        bsread(s, &nv);
        while (nv-- > 0)
        {
            kstring vid = readkstr();
            var_ids->addnc(vid);
        }
        fcursor = s.tellg();
    }

    return s.good();
}

////////////////////////////////////
// recursive read of term structure
//
int BinIFile::read_term(Term *t)
{
    istream &is = *plib->inp;
    char tc = char(is.get());
    TermData tt = TermData(tc) << 24;

    switch (tt)
    {
    case f_ATOM: {
        Atom a = readkstr();
        *t = Term(a);
    }
    break;

    case f_VAR: {
        Var v;
        bsread(is, &v);
        *t = Term(v);
    }
    break;

    case f_INT: {
        Int iv;
        bsread(is, &iv);
        *t = Term(iv);
    }
    break;

    case f_DOUBLE: {
        Double d;
        bsread(is, &d);
        *t = Term(d);
    }
    break;

    case f_STRUCT: {
        kstring funct = readkstr();

        Int arity;
        bsread(is, &arity);

        // seek for operator strings (change before to construct)
        OperTable *pTbl = GetEngines()->get_optbl();
        Operator *op = pTbl->IsOp(funct);
        if (op)
        {
            int nArgs =	op->assoc == Operator::XF ||
                    op->assoc == Operator::FX ||
                    op->assoc == Operator::YF ||
                    op->assoc == Operator::FY		? 1 : 2;
            if (nArgs != arity)
                op = pTbl->GetNext(op);
        }
        if (op)
            funct = op->name;

        *t = Term(funct, arity);
        for (int ix = 0; ix < arity; ix++)
        {
            Term arg;
            if (!read_term(&arg))
                return 0;

            t->setarg(ix, arg);
        } }
    break;

    case f_LIST: {
        Term fh, ft;
        read_term(&fh);
        read_term(&ft);
        *t = Term(fh, ft);
    }
    break;

    case f_LIST|f_NOTERM:
        *t = Term(ListNULL);
        break;

    default:
        ASSERT(0);
    }

    return plib->inp->rdstate() == ios::goodbit;
}

/////////////////////////
// read a stored strings
//
kstring BinIFile::readkstr()
{
    unsigned ix;
    bsread(*plib->inp, &ix);

    if (ix < plib->maxks)
        return kstring(plib->kstrv[ix]);

    plib->inp->clear(ios::failbit);
    plib->chkerr(INVALID_BIN_DATA);
    return kstring(MSR_NULL);
}

///////////////////////
// reset input pointer
//
void BinIFile::restart()
{
    fcursor = begdata;
}

////////////////////////////////////////
// output constructor
//	keep in a temporary file the data,
//	to be appended at library when done
//
BinOFile::BinOFile(kstring id, SourceBinaryLib *l)
    : BinFile(id, l)
{
    tmpnam(tfn);
    out = 0;
    status = created;
}
BinOFile::~BinOFile()
{
    if (out)
    {
        delete out;
        remove(tfn);
    }
}

////////////////
// release file
//
void BinOFile::end_data()
{
    delete out;
    out = 0;
    status = done;
    plib->check_done_out();
}

////////////////////////////////////////////////
// save term data and following variables names
//
int BinOFile::echo_term(Term t, kstr_list& var_ids)
{
    if (!out)
    {
        if (status == created)
        {
            // create file only when needed
            out = new ofstream(tfn, ios::out|ios::binary);
            status = plib->chkerr(CANT_OPEN_TMPFILE, out, tfn)? err : dumping;
        }
        else
            return 0;
    }

    if (status == dumping && echo_term(t))
    {
        // save variables names
        bswrite(*out, var_ids.numel());
        kstr_list_iter i(var_ids);
        kstring s;
        while (CCP(s = i.next()) != 0)
            writekstr(s);

        nobjects++;
    }

    return out->rdstate() == ios::goodbit;
}

////////////////////////////////////////////
// recursive dumping of data to binary file
//
int BinOFile::echo_term(Term t)
{
    // save type tag
    TermData tt = t;
    if (tt == ListNULL)
        tt |= f_NOTERM;
    out->put(char(tt >> 24));

    switch (t.type())
    {
    case f_ATOM:
        writekstr(t.kstr());
        break;

    case f_INT:
        bswrite(*out, Int(t));
        break;

    case f_VAR:
        bswrite(*out, Var(t));
        break;

    case f_DOUBLE:
        bswrite(*out, Double(t));
        break;

    case f_STRUCT: {
        writekstr(t.get_funct());
        Int arity = t.get_arity();
        bswrite(*out, arity);
        for (int i = 0; i < arity; i++)
            echo_term(t.getarg(i));
    }
    break;

    case f_LIST:
        if (!t.LNULL())
        {
            echo_term(t.getarg(0));
            echo_term(t.getarg(1));
        }
        break;

    default:
        ASSERT(0);
    }

    return out->rdstate() == ios::goodbit;
}

////////////////////////////
// store a string reference
//
void BinOFile::writekstr(kstring s)
{
    bswrite(*out, plib->kstrl.add(s));
}

////////////////////////////////////
// append temporary file to library
//	save data start position
//
void BinOFile::cat_data(ostream &slib)
{
    ASSERT(out == 0);

    begdata = slib.tellp();

    ifstream myf(tfn, ios::in|ios::binary);
    if (plib->chkerr(CANT_REOPEN_TMPFILE, &myf, tfn))
        return;

    //unsigned char buf[256];
    char buf[256];
    do
    {
        myf.read(buf, sizeof(buf));
        slib.write(buf, myf.gcount());
    }
    while (myf.rdstate() == ios::goodbit);
    myf.close();

    enddata = slib.tellp();

    if (remove(tfn))
        GetEngines()->ErrMsg(CANT_DELETE_TMPFILE, plib->idfile, tfn);

    status = saved;
}

///////////////////////////////////
// write data to library directory
//
void BinOFile::write_dir(ostream &slib) const
{
#if 0
    ASSERT(out == 0);
    bswrite(slib, plib->kstrl.add(id));
    bswrite(slib, begdata);
    bswrite(slib, enddata);
    bswrite(slib, nobjects);
#else
    assert(false);
#endif
}
