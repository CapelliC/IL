
/*
    IL : Intlog Language
    Object Oriented Prolog Project
    Copyright (C) 1992-2020 - Ing. Capelli Carlo

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

class IntlogSourceStream;	// source file input
class Scanner;				// source binded
class IntlogBinStream;		// library binary input
class BinOFile;				// binary file to echo while reading
class BinIFile;				// binary input file from library

//------------------------
// a source stream reader
//
class IAFX_API IntlogSourceStream : public IntlogIStream
{
public:
    IntlogSourceStream(kstring, istream*, BinOFile * = 0);
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
class IntlogBinStream : public IntlogIStream
{
public:
    IntlogBinStream(BinIFile *);

    int ateof() const;
    readResult read();
    int get();
    void close();

private:
    BinIFile *bf;
};

#endif
