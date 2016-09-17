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

#include "stdafx.h"
#include "iafx.h"
#include "proios.h"
#include "parse.h"
#include "binlib.h"
#include "defsys.h"
#include "eng.h"
#include "clause.h"
#include "actios.h"

		//--------//
		// source //
		//--------//

IntlogSourceStream::IntlogSourceStream(kstring k, istream *s, BinOFile *b)
:	IntlogIStream(k)
{
	binfile = b;
	i = s;
}

IntlogIStream::readResult IntlogSourceStream::read()
{
	IntlogParser *p = GetEngines()->get_parser();

	t_data = Term(f_NOTERM);

	switch (p->getinput()) {

	case IntlogParser::NewClause:
		t_data = p->parsed;
		if (binfile)
			binfile->echo_term(t_data, t_vars);
		return NewClause;

	case IntlogParser::EndFile:
		if (binfile)
			binfile->end_data();
		return EndFile;

	case IntlogParser::RedoCommand:
		return RedoCommand;

	default:
		return Error;
	}
}

IntlogSourceStream::~IntlogSourceStream()
{
}

int IntlogSourceStream::ateof() const
{	// simply check for eof in source stream
	return i->eof();
}

int IntlogSourceStream::get()
{
	return i->get();
}
void IntlogSourceStream::close()
{
}

		//--------//
		// binary //
		//--------//

IntlogBinStream::IntlogBinStream(BinIFile *s)
: IntlogIStream(s->get_id())
{
	bf = s;
	i = 0;
}

int IntlogBinStream::ateof() const
{
	return bf->ateof();
}

IntlogIStream::readResult IntlogBinStream::read()
{
	if (bf->read_term(&t_data, &t_vars))
		return NewClause;
	return Error;
}

int IntlogBinStream::get()
{
	return -1;
}

void IntlogBinStream::close()
{
	bf->end_data();
}
