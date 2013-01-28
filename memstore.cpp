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

#include "stdafx.h"
#include "iafx.h"
#include "memstore.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

MemStorage::MemStorage(int elSize)
{
    ASSERT(size_t(elSize) >= (sizeof(MemStoreRef)>>1));

    m_cellSize = elSize;
    m_vectTop = 0;
    m_firstFree = MSR_NULL;
}

void MemStorage::FreeMem()
{
    while (m_vectTop > 0)
        delete base[--m_vectTop];

    delete[] base;
    base = 0;
    vdim = 0;
}

//////////////////////
// insert a data item
//
MemStoreRef MemStorage::EmptyStore()
{
    if (m_firstFree == MSR_NULL) {
        m_firstFree = OPMUL(m_vectTop, BLKDIM);
        alloc();
    }

    MemStoreRef ff = m_firstFree;
    m_firstFree = *cell(ff);

    return ff;
}

/*
/////////////////////////////////////
// insert consecutive uninitialized
//
MemStoreRef MemStorage::Reserve(unsigned n)
{
 ASSERT(n > 0);

 MemStoreRef lastSkip = MSR_NULL;

 // be shure not empty
 if (m_firstFree == MSR_NULL) {
  m_firstFree = OPMUL(m_vectTop, BLKDIM);
  alloc();
 }

restart:

 // firstC: first of consecutive, follow goto chain
 MemStoreRef firstC = m_firstFree,
    follow = firstC,
    nextc = *cell(follow);

retry:
 // attempt to follow chain for n cells
 for (unsigned c = 1; c < n; c++) {

  if (nextc == MSR_NULL) {

   // at end of vector, with some free cell on top
   *cell(follow) = OPMUL(m_vectTop, BLKDIM);
   alloc();
   goto restart;
  }

  if (nextc != follow + 1) {

   lastSkip = follow;
   firstC = follow = nextc;
   nextc = *cell(follow);
   goto retry;
  }
  nextc = *cell(++follow);
 }

 // n consecutive LOGICAL found, but check for PHISICAL also
 unsigned boundFollow = OPDIV(follow, BLKDIM);
 if (boundFollow != OPDIV(firstC, BLKDIM)) {
  firstC = follow = OPMUL(boundFollow, BLKDIM);
  lastSkip = firstC - 1;
  nextc = *cell(follow);
  goto retry;
 }

 if (firstC == m_firstFree)
  // shift forward free space index
  m_firstFree = *cell(firstC + n - 1);
 else
  // some empty cell skipped
  *cell(lastSkip) = *cell(firstC + n - 1);

 return firstC;
}
*/

///////////////////////
// release a data item
//
void MemStorage::Delete(MemStoreRef c)
{
    ASSERT(c < OPMUL(m_vectTop, BLKDIM));
#if 0
    ASSERT(used(c));
#endif
    *cell(c) = m_firstFree;
    m_firstFree = c;
}

/*
/////////////////////////////
// release consecutive items
//
void MemStorage::DelReserved(MemStoreRef firstCell, unsigned nConsec)
{
 MemStoreRef top = firstCell + nConsec;

 ASSERT(nConsec > 0);
 ASSERT(top <= OPMUL(m_vectTop, BLKDIM));
 ASSERT(used(firstCell));
 ASSERT(used(top - 1));

 for (MemStoreRef c = firstCell; c < top - 1; c++)
  *cell(c) = c + 1;
 *cell(c) = m_firstFree;
 m_firstFree = firstCell;
}
*/

///////////////////////////
// allocate a memory block
//
void MemStorage::alloc()
{
    MemStoreRef r = OPMUL(m_vectTop, BLKDIM);

    if (m_vectTop == vdim)
        grow(VCTDIM);

    //	cout << r << ',' << vdim << ',' << m_vectTop << endl;
    if (r > 64000) {
#ifndef _WINDLL
        cout << "\noverflow" << endl;
        exit(1);
#else
        AfxThrowMemoryException();
#endif
    }

    base[m_vectTop++] = new char[BLKDIM << m_cellSize];

    // initialize free element indexs
    MemStoreRef r1 = OPMUL(m_vectTop, BLKDIM);
    while (r < r1 - 1) {
        *cell(r) = r + 1;
        r++;
    }

    *cell(r) = MSR_NULL;
}

#ifdef _DEBUG

////////////////////////
// search the free list
//
int MemStorage::used(MemStoreRef c) const
{
    /*	MemStoreRef r = m_firstFree;

 while (r != MSR_NULL) {
  if (r == c)
   return 0;
  r = *cell(r);
 }
*/
    return 1;
}

///////////////////////////////
// display the vector contents
//
void MemStorage::Dump(ostream& s) const
{
    MemStoreRef r = OPMUL(m_vectTop, BLKDIM);
    s << "Dumping: dim=" << r << ", firstFree=" << m_firstFree << endl;
    MemStoreRef i = 0, cnt = 0;
    for ( ; i < r; i++) {
        if (used(i)) {
            s << i << "\t:";
            DumpFull(s, i);
            cnt++;
        } else {
            s << i << " *";
            DumpEmpty(s, i);
        }

        s << endl;
    }

    s << "Cells Full: " << cnt << endl << endl;
}

void MemStorage::DumpEmpty(ostream& s, MemStoreRef c) const
{
    s << *cell(c);
}

void MemStorage::DumpFull(ostream &s, MemStoreRef c) const
{
    char *pChar = (char*)cell(c);
    for (int i = 0; i < (1 << m_cellSize); i++)
        s << hex << int(pChar[i]);
}

/////////////////////////////////
// get pointer to cell as index
//
MemStoreRef* MemStorage::cell(MemStoreRef p) const
{
    if (p >= OPMUL(m_vectTop, BLKDIM))
        ASSERT(0);

    return (MemStoreRef*)(
		(char*)getat(OPDIV(p, BLKDIM)) + (OPMOD(p, BLKDIM) << m_cellSize)
                );
}

//////////////////////////////
// destructor release memory
//
MemStorage::~MemStorage()
{
    FreeMem();
}

void MemStorage::MemStorageCheckUsed(MemStoreRef r) const
{
    ASSERT(used(r));
}

#endif
