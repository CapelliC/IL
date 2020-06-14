
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


// parser error codes
#define FILE_NOT_FOUND			0
#define EXPECT_POINT			1
#define EXPECT_CLPAR			2
#define EXPECT_CLQUA			3
#define UNEXPECT_TOKEN			4
#define UNEXPECT_OPER			5
#define OPER_REDUCE				6

// scanner error codes
#define UNTERMINATED_STRING		20
#define INVALID_QUOTED_ATOM		21
#define INVALID_CHAR			22
#define EOF_IN_COMMENT			23
#define TOKLEN_OVERFLOW			24
#define INTLEN_OVERFLOW			25
#define HEXLEN_OVERFLOW			26
#define NESTED_COMMENT			27

// semantic clause instance errors
#define BUILTIN_REDEFINITION    30
#define UNUSED_VAR      		31
#define METAVAR_CALL			32
#define ANONYMVAR_CALL			33
#define OR_AS_FUNCTOR			34
#define AND_AS_FUNCTOR			35
#define LIST_AS_FUNCTOR			36
#define INT_AS_FUNCTOR			37
#define REAL_AS_FUNCTOR			38
#define INVALID_DB_PATH			39
#define UNCALL_AS_FUNCTOR		40

// binary library interface
#define BINL_NOT_FOUND			50
#define LOADING_STRINGS			51
#define LOADING_DIRECTORY		52
#define CREATE_OUTPUT_FILE		53
#define WRITE_OUTPUT_FILE		54
#define CANT_REOPEN_TMPFILE		55
#define CANT_OPEN_TMPFILE		56
#define INVALID_BIN_DATA		57
#define INVALID_BIN_OPDEF		58
#define CANT_DELETE_TMPFILE		59

// serialize
#define INVALID_SERIAL_DATA		80
#define SERIAL_FMT_NOT_FOUND	81

