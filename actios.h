
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


#ifndef ACTIOS_H_
#define ACTIOS_H_

#include "iafx.h"
#include "proios.h"
#include "binlib.h"

//------------------------
// a source stream reader
//
class IAFX_API IntlogSourceStream : public IntlogIStream {
public:
    IntlogSourceStream(kstring k, istream *s, BinOFile *b = nullptr);
    ~IntlogSourceStream();

    int ateof() const;
    readResult read();
    int get();
    void close();

private:
    friend class IntlogIOStreams;
    BinOFile *binfile;
};

//-------------------
// binary lib reader
//
class IntlogBinStream : public IntlogIStream {
public:
    IntlogBinStream(BinIFile *s);

    int ateof() const;
    readResult read();
    int get();
    void close();

private:
    BinIFile *bf;
};

#endif
