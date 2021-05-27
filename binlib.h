
/*
    IL : Intlog Language
    Object Oriented Prolog Project
    Copyright (C) 1992-2021 - Ing. Capelli Carlo

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


#ifndef BINLIB_H_
#define BINLIB_H_

#include "term.h"

// defined classes
class SourceBinaryLib;
class BinFile;
class BinIFile;
class BinOFile;

//-----------------------
// an element of library
//
class BinFile : public e_slist {
public:
    kstring get_id() const;
    virtual void end_data() = 0;

protected:
    BinFile(kstring, SourceBinaryLib *);
    BinFile();              // for file data only

    // directory data
    kstring id;             // source identifier
    unsigned nobjects;      // counter of file elements

    /*
     streampos begdata;     // track begin data in library file
     streampos enddata;     // file dimension
    */

    streamoff begdata;      // track begin data in library file
    streamoff enddata;      // file dimension

    SourceBinaryLib *plib;  // owner of this file
};

//-----------
// for input
//
class BinIFile : public BinFile {
public:
    BinIFile(SourceBinaryLib *);

    int read_term(Term *, kstr_list *);
    int ateof() const;
    virtual void end_data();
    void restart();

private:
    int read_term(Term *);
    int read_op(Operator **);
    kstring readkstr();
    streampos fcursor;
};

//----------------------------------------------
// building an output section
//  rely on a temporary file, to avoid problems
//  in input data sequencing
//
class BinOFile : public BinFile {
public:
    BinOFile(kstring, SourceBinaryLib *);
    ~BinOFile();

    int echo_term(Term, kstr_list &);
    virtual void end_data();

private:
    ostream* out;
    char tfn[L_tmpnam];
    enum {
        created, dumping, done, saved, err
    } status;

    int echo_term(Term);
    int save_op(Operator *);
    void writekstr(kstring);

    friend class SourceBinaryLib;
    void cat_data(ostream &);
    void write_dir(ostream &) const;
};

//----------------------------------------------
// support binary data format
//  a library is a list of sources, i.e.
//  a set of files, each with its clauses saved
//
class SourceBinaryLib : slist {
public:
    enum openMode {
        toRead, toWrite
    };
    SourceBinaryLib(kstring, openMode);
    ~SourceBinaryLib();

    void add_name(kstring);
    BinIFile *is_input(kstring) const;
    BinOFile *is_output(kstring) const;

    int making() const;
    int extracting() const;

private:
    friend class BinIFile;
    friend class BinOFile;

    kstring idfile;

    int match(e_slist *, void *) const;
    BinFile* is_name(kstring) const;
    int chkerr(int, ios * = nullptr, CCP = nullptr) const;

    // on output
    kstr_list kstrl;
    void check_done_out();

    // on input
    istream *inp;
    kstring *kstrv;
    unsigned maxks;
    void load_directory();
    void check_done_inp();
};

/*************
 INLINEs
**************/

inline int SourceBinaryLib::making() const {
    return inp == nullptr;
}
inline int SourceBinaryLib::extracting() const {
    return inp != nullptr;
}

#endif
