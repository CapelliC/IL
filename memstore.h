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

#ifndef _MEMSTORE_H_
#define _MEMSTORE_H_

#ifndef _VECTB_H_
#include "vectb.h"
#endif

#ifndef _FASTACC_H_
#include "fastacc.h"
#endif

#ifndef _IAFX_H_
#include "iafx.h"
#endif

typedef unsigned	MemStoreRef;
#define MSR_NULL	MemStoreRef(-1)

////////////////////////////////////////////////////////////
// allocation schema for frequently allocated items
//	dimensions must be power of 2, for fast address compute
//
class IAFX_API MemStorage : vectvptr
{
public:
	// insert element at free location (grow if needed)
	MemStoreRef EmptyStore();

	// insert consecutive uninitialized
	MemStoreRef Reserve(unsigned = 1);

	// release used cell
	void Delete(MemStoreRef);

	// release consecutive cells
	void DelReserved(MemStoreRef, unsigned);

	// release all memory
	void FreeMem();

#ifdef _DEBUG
	void Dump(ostream&) const;
	virtual void DumpEmpty(ostream&, MemStoreRef) const;
	virtual void DumpFull(ostream&, MemStoreRef) const;
	void MemStorageCheckUsed(MemStoreRef) const;
#else
	#define MemStorageCheckUsed(c)
#endif

protected:

	// dimension of data item: elSize = log2(sizeof(type)), ceiled
	MemStorage(int elSize);
	~MemStorage();

	// get pointer to cell as index
	MemStoreRef* cell(MemStoreRef) const;

	// true if cell is used
	int used(MemStoreRef) const;

private:

	// current allocation dim
	unsigned m_vectTop;

	// free items linked list pointer
	MemStoreRef m_firstFree;

	// an element cell size
	int m_cellSize;

	// allocate space for vector
	void alloc();
};

#ifndef _DEBUG

/////////////////////////////////
// get pointer to cell as index
//
inline MemStoreRef* MemStorage::cell(MemStoreRef p) const
{
	return (MemStoreRef*)(
		(char*)getat(OPDIV(p, BLKDIM)) + (OPMOD(p, BLKDIM) << m_cellSize)
	);
}

//////////////////////////////
// destructor release memory
//
inline MemStorage::~MemStorage()
{
	FreeMem();
}

#endif

//////////////////////////////////////////
// template macro: instance specific code
// of type required
//
#define MemStorageDecl(type, elsize)		\
class IAFX_API MemStorage_##type			\
:	public MemStorage						\
{ public:									\
	MemStorage_##type()						\
		 : MemStorage(elsize){}				\
	MemStoreRef Store(const type& v) {		\
		MemStoreRef r = EmptyStore();		\
		*((type*)cell(r)) = v;				\
		return r;							\
	}										\
	type* operator[](MemStoreRef c) const {	\
		MemStorageCheckUsed(c);				\
		return (type*)cell(c);				\
	}										\
};

#endif
