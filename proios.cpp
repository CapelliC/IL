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

///////////////////////////////
// modelling Intlog IO streams
///////////////////////////////

#include "stdafx.h"
#include "proios.h"
#include "binlib.h"
#include "defsys.h"
#include "eng.h"
#include "clause.h"
#include "actios.h"
#include "parse.h"
#include "membuf.h"
/*
#include <io.h>
#include <fcntl.h>
*/

///////////////////////////////////
// static IntlogIOStreams members
//
kstring			IntlogIOStreams::envar;
kstring			IntlogIOStreams::user;
SourceBinaryLib*	IntlogIOStreams::sbl;

//////////////////
// memory cleanup
//
void IntlogIOStreams::closeall()
{
    slist_iter i(opened);

    IntlogStream *s;
    while ((s = (IntlogStream *)i.next()) != 0)
    {
        if (s->md == IntlogStream::input)
        {
            IntlogIStream* i = (IntlogIStream*)s;
            close(i->i, i->name());
        }
        if (s->md == IntlogStream::output)
        {
            IntlogOStream* o = (IntlogOStream*)s;
            close(o->o, o->name());
        }
    }
}

void IntlogIOStreams::ReleaseBinLib()
{
    delete sbl;
    sbl = 0;
}

////////////////////////
// set the input stream
//
int IntlogIOStreams::see(kstring id, int flags)
{
    IntlogStream *s = isin(id, IntlogStream::input);
    if (!s)
    {
        if ((s = create_input_file(id, flags)) == 0)
            return 0;
        opened.insert(s, 0);
    }

    currinp = (IntlogIStream*)s;
    if (currinp->i)
        GetEngines()->get_parser()->SetSource(currinp->i, id, &currinp->t_vars);

    return 1;
}

//////////////////////////
// pop current top stream
//
int IntlogIOStreams::seen()
{
    if (currinp->name() != user)
    {
        // revert to default (name is unique)
        if (currinp->i)
        {
            GetEngines()->get_parser()->PopSource(currinp->i);
            close(currinp->i, currinp->name());
        }
        opened.remove(opened.seek(currinp));

        currinp = (IntlogIStream*)isin(MSR_NULL, IntlogStream::input);
        return 1;
    }

    return 0;
}

/////////////////////////
// set the output stream
//
int IntlogIOStreams::tell(kstring id, int flags)
{
    IntlogStream *s = isin(id, IntlogStream::output);
    if (!s)
    {	// create a new output file
        ostream *o = openout(id, flags);
        if (!o || !o->good())
        {
            delete o;
            return 0;
        }
        opened.insert(s = new IntlogOStream(id, o), 0);
    }

    currout = (IntlogOStream*)s;
    return 1;
}

/////////////////////////////////////////
// close (if !default) the output stream
// and revert to default
//
int IntlogIOStreams::told()
{
    if (currout->name() != user)
    {
        // revert to default (delete unique name)
        close(currout->o, currout->name());
        opened.remove(opened.seek(currout));

        currout = (IntlogOStream*)isin(MSR_NULL, IntlogStream::output);
        return 1;
    }

    return 0;
}

////////////////////////////
// search for an identifier
//
IntlogStream* IntlogIOStreams::isin(kstring id, IntlogStream::mode mode) const
{
    IntlogStream* s;
    slist_iter i(opened);

    while ((s = (IntlogStream*)i.next()) != 0)
        if (((MemStoreRef(id) == MSR_NULL || s->ident == id) && mode == s->md)
                || mode == IntlogStream::none)
            break;

    return s;
}

////////////////////////////
// check for binary library
//
IntlogIStream *IntlogIOStreams::create_input_file(kstring id, int flags)
{
    IntlogIStream *s;
    BinOFile *obf = 0;

    if (sbl)
    {
        BinIFile *ibf = sbl->is_input(id);
        if (ibf != 0)
        {
            s = new IntlogBinStream(ibf);
            ibf->restart();
            openedbin(id);
            return s;
        }
        obf = sbl->is_output(id);
    }

    istream *i = openinp(&id, flags);
    if (!i || !i->good())
    {
        delete i;
        return 0;
    }

    s = new IntlogSourceStream(id, i, obf);
    return s;
}

//////////////////////////
// set binary source file
//
void IntlogIOStreams::CreateBinLib(kstring id)
{
    sbl = new SourceBinaryLib(id, SourceBinaryLib::toWrite);
}

///////////////////////////////////
// set source file from binary lib
//
void IntlogIOStreams::ReadFromBinLib(kstring id)
{
    sbl = new SourceBinaryLib(id, SourceBinaryLib::toRead);
}

////////////////////////////////////////////////
// cat a name file to be saved in binary format
//
void IntlogIOStreams::AddBinLibSourceName(kstring idf)
{
    if (sbl)
        sbl->add_name(idf);
}

//////////////////////////////////////
// make a search of file
//	scanning the environment variable
//
filebuf* IntlogIOStreams::openfile(kstring id, int of, char *paths, char *buf, int maxbuf)
{
    CCP fid = id;
    filebuf *fb = new filebuf;

    if (fb->open(fid, ios::in)) //of))//|ios::nocreate))
    {
        if (buf)
            strncpy(buf, id, maxbuf);
        return fb;
    }
    if ((of & ios::in) == 0)	// open failed: output stream can't be opened
    {
        delete fb;
        return 0;
    }

    if (!paths)
        paths = (char*)CCP(envar);
    if (paths)
    {
        // search all directories enumerated
        char dir[1 << 16];
        basic_istringstream<char> dirs(paths);
        int szdir = sizeof(dir) - strlen(fid) + 1;
        if (szdir > 0)
            while (dirs.getline(dir, szdir, ';'))
            {
                char *pdir = strcat(strcat(dir, "//"), fid);
                if (fb->open(pdir, ios::in))
                {
                    if (buf)
                        strncpy(buf, dir, maxbuf);
                    return fb;
                }
            }
    }

    // not found
    delete fb;
    return 0;
}

membuf *find(kstring id, slist_iter& i)
{
    membuf *mb;
    while ((mb = (membuf*)i.next()) != 0)
        if (mb->id == id)
            return mb;
    return 0;
}

void IntlogIOStreams::DelBuffer(kstring id)
{
    slist_scan s(membuffer);
    membuf *mb = find(id, s);
    if (mb)
        s.delitem();
}
bool IntlogIOStreams::IsBuffer(kstring id) const
{
    slist_iter i(membuffer);
    return find(id, i) != 0;
}
const char* IntlogIOStreams::GetBuffer(kstring id)
{
    slist_iter i(membuffer);
    membuf *mb = find(id, i);
    return mb? mb->mem : 0;
}
void IntlogIOStreams::SetBuffer(kstring name, const char *pMem)
{
    membuf *mb;
    membuffer.append(mb = new membuf);
    mb->id = name;
    mb->mem = (char*)pMem;
}
istream &IntlogIOStreams::inp() const
{
    return *ips()->i;
}
