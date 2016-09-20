
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


#ifndef _PROIOS_H_
#define _PROIOS_H_

//---------------------------------------------------------------
// Intlog named streams model:
//
//	high level constructs for Term(s) read and a polymorphic
//	data interface, that can fetch data from source stream files
//	or binary format libraries.
//
// These libraries are fully transparent and built 'at once',
//	saving data while source input file are scanned.
//---------------------------------------------------------------

#include "iafx.h"
#include "term.h"

class IntlogExec;	// forward decl
class IntlogStream;	// a named stream
class IntlogIStream;	// for input
class IntlogOStream;	// for output
class IntlogIOStreams;	// IO modelling
class SourceBinaryLib;	// binary files libraries

//////////////////////////////////////
// a named stream for input or output
//
class IAFX_API IntlogStream : e_slist
{
public:

    // keep identifier
    kstring name() const {
        return ident;
    }

    enum mode { input, output, none };
    mode get_mode() const {
        return md;
    }

    virtual void close() {}

protected:

    IntlogStream(kstring id, mode m){
        ident = id;
        md = m;
    }

    kstring ident;
    mode md;

    friend class IntlogIOStreams;
};

/////////////////////////////////////////
// for input: derive actual term readers
//
class IntlogIStream : public IntlogStream
{
public:

    virtual int ateof() const = 0;

    // term reading
    enum readResult {
        NewClause,
        RedoCommand,
        EndFile,
        Error
    };
    virtual readResult read() = 0;

    // these keep data after a read OK
    Term t_data;
    kstr_list t_vars;

    virtual int get() = 0;

protected:

    IntlogIStream(kstring id) : IntlogStream(id, input)
    {
    }

    istream *i;

    friend class IntlogIOStreams;
};

///////////////////////////
// for output: dummy class
//
class IntlogOStream : public IntlogStream
{
    IntlogOStream(kstring id, ostream* s) : IntlogStream(id, output)
    {
        o = s;
    }

    ostream* o;
    friend class IntlogIOStreams;
};

///////////////////////////////////////
// a model for prolog named streams
//	mimic te same calls used in prolog
//
class IAFX_API IntlogIOStreams
{
public:

    IntlogIOStreams() {
        currinp = 0;
        currout = 0;
    }

    void closeall();

    // console identify
    static kstring name() {
        return user;
    }

    static void setuser(kstring console, kstring searchpath) {
        user = console;
        envar = searchpath;
    }

    // input control
    int seeing(kstring id) const {
        return id == currinp->ident;
    }

    int see(kstring name, int flags = 0);
    int seen();

    IntlogIStream *ips() const{
        return currinp;
    }

    istream& inp() const;

    // find by name
    IntlogStream *isin(kstring ident, IntlogStream::mode = IntlogStream::none) const;

    // output control
    int telling(kstring id) const {
        return id == currout->ident;
    }

    int tell(kstring name, int flags = 0);
    int told();

    IntlogOStream *ops() const {
        return currout;
    }

    ostream& out() const {
        return *currout->o;
    }


    // in-memory files support
    void NewBuffer(kstring name);
    void DelBuffer(kstring name);
    bool IsBuffer(kstring name) const;
    const char *GetBuffer(kstring name);
    void SetBuffer(kstring name, const char *value);

    // binary library control
    static void CreateBinLib(kstring);
    static void AddBinLibSourceName(kstring);
    static void ReadFromBinLib(kstring);
    static void ReleaseBinLib();

    // open file buf (avoid a problem arise from WINDLL streams...)
    static filebuf *openfile(kstring id, int of, char *path = 0, char *buf = 0, int maxbuf = -1);
    static kstring envar;
    static void setenvar(kstring var) {
        envar = var;
    }
    static kstring getenvar() {
        return envar;
    }

protected:

    // keep the name of default IO streams (console)
    static kstring user;

    // open/close the stream as input
    virtual istream *openinp(kstring* name, int flags) = 0;
    virtual void close(istream*, kstring) = 0;

    // open/close the stream as output
    virtual ostream *openout(kstring name, int flags) = 0;
    virtual void close(ostream*, kstring) = 0;

    // current IO ports
    IntlogIStream *currinp;
    IntlogOStream *currout;

    // local opened streams
    slist opened;
    void closestream(IntlogStream *s);

    IntlogIStream *create_input_file(kstring ident, int flags);
    virtual void openedbin(kstring) {}

    // keep memory buffers
    slist membuffer;

    // binary lib control
    static SourceBinaryLib*	sbl;
};

#endif
