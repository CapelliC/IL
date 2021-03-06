
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


//-----------------
// string storage
//-----------------

#include "iafx.h"
#include "mycont.h"
#include "binstr.h"

MemStorage<CCP> kstring::nameSpace;
hashtable* kstring::strTbl = nullptr;

class e_kstrtable : public e_hashtable {
public:
    e_kstrtable(istream &s) {
        bsread(s, &pstr);
        kstr.m_nameIndex = kstring::nameSpace.Store(pstr);
    }
    e_kstrtable(CCP s) {
        pstr = new char[strlen(s) + 1];
        strcpy(pstr, s);
        kstr.m_nameIndex = kstring::nameSpace.Store(pstr);
    }
    ~e_kstrtable() override;
    CCP getstr() const override {
        return pstr;
    }

    kstring kstr;
    char *pstr;
};
e_kstrtable::~e_kstrtable() {
    delete pstr;
}

kstring::kstring(CCP s, int force) {
    e_kstrtable *e = static_cast<e_kstrtable*>(strTbl->isin(e_hashtable_str(s)));
    if (!e)
        strTbl->insert(e = new e_kstrtable(s));
    else if (force) {
        m_nameIndex = nameSpace.Store(e->pstr);
        return;
    }
    m_nameIndex = e->kstr;
}

void kstring::initMem() {
    assert(!strTbl);
    strTbl = new hashtable(997);
}
void kstring::freeMem() {
    nameSpace.FreeMem();
    strTbl->clear();
    delete strTbl;
    strTbl = nullptr;
}

///////// LIST ////////

unsigned kstr_list::add(kstring s) {
    unsigned ix = search(s);
    if (ix == unsigned(-1))
        ix = append(new kstr_el(s));
    return ix;
}
unsigned kstr_list::addnc(kstring s) {
    return append(new kstr_el(s));
}
kstring kstr_list::get(unsigned p) const {
    kstr_el *e = static_cast<kstr_el*>(slist::get(p));
    assert(e);
    return e->s;
}
unsigned kstr_list::search(kstring s) const {
    slist_iter i(*this);
    kstr_el *e;
    unsigned ix = 0;
    while ((e = static_cast<kstr_el *>(i.next())) != nullptr) {
        if (s == e->s)
            return ix;
        ix++;
    }
    return unsigned(-1);
}

kstr_list_iter::kstr_list_iter(const kstr_list &l)
: slist_iter(l) {
}

kstring kstr_list_iter::next() {
    kstr_list::kstr_el* e = static_cast<kstr_list::kstr_el*>(slist_iter::next());
    return e? e->s : kstring(MSR_NULL);
}

kstr_list::kstr_el::~kstr_el() {
}
