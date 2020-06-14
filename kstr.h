
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


#ifndef KSTR_H_
#define KSTR_H_

/////////////////////////////////////////////////////////////
// constant strings storage:
//	note that this data can only grow, during program usage
//	strings are represented by their 'nominal index'
//

#include "iafx.h"
#include "slist.h"
#include "hasht.h"
#include "memstore.h"

// shorthand POD string
typedef const char* CCP;

class kstring;
class kstr_list;
class kstr_list_iter;
class e_kstrtable;

class IAFX_API kstring
{
public:
    kstring(MemStoreRef = MSR_NULL);
    kstring(CCP, int = 0);

    operator CCP() const;
    operator unsigned() const;

    int operator==(kstring) const;
    int operator!=(kstring) const;

    static void initMem();
    static void freeMem();
private:
    MemStoreRef m_nameIndex;

    static MemStorage<CCP> nameSpace;
    static hashtable* strTbl;
    friend class e_kstrtable;
};

/////////////////////////
// constant strings list
//
class IAFX_API kstr_list : public slist
{
public:

    // no duplicates
    unsigned add(kstring);

    // unconditionally add
    unsigned addnc(kstring);

    // find inserted, if any
    unsigned search(kstring) const;

    // return element
    kstring get(unsigned) const;

private:
    struct kstr_el : public e_slist {
        kstr_el(kstring p) { s = p; }
        kstring s;
    };
    friend class kstr_list_iter;
};

///////////////////////////////////////
// iterator for strings stored in list
//
class kstr_list_iter : slist_iter
{
public:
    kstr_list_iter(const kstr_list &);
    kstring next();
};

/**************
 INLINES
***************/

inline kstring::kstring(MemStoreRef u)
{
    m_nameIndex = u;
}
inline kstring::operator CCP() const
{
    return m_nameIndex != MSR_NULL? *nameSpace[m_nameIndex] : 0;
}
inline kstring::operator unsigned() const
{
    return m_nameIndex;
}
inline int kstring::operator==(kstring s) const
{
    return m_nameIndex == s.m_nameIndex;
}
inline int kstring::operator!=(kstring s) const
{
    return m_nameIndex != s.m_nameIndex;
}

#endif
