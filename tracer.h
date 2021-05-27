
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


#ifndef TRACER_H_
#define TRACER_H_

//---------------------------------------
// Bird tracing model
//  define a slightly extended interface
//  to hook in system debug
//---------------------------------------

#include "mycont.h"
#include <iostream>
#include "query.h"

class SpyEntry;
class SpyCall;
class ProofTracer;
class IntlogExec;
class CallData;
class BindStack;
class TrailStack;
class ProofStack;

class IAFX_API ProofTracer : public hashtable_str {
public:

    ProofTracer(IntlogExec *);
    virtual ~ProofTracer();

    // basic tracing interface
    virtual int call(stkpos, CallData*);
    virtual int exit(stkpos, CallData*);
    virtual int redo(stkpos, CallData*);
    virtual int fail(stkpos, CallData*);

    // action describe
    enum Action {
        Call, Fail, Exit, Redo
    };

    // basic control interface
    enum {
        AllEntries  = 0x01,
        Extended    = 0x02,
        NoVars      = 0x04
    };
    virtual void trace_on();
    virtual void trace_off();
    virtual void etrace_on();
    virtual void etrace_off();

    virtual void spy(const char*, int, int = -1, int = 0);
    virtual void debugging(ostream &) const;
    virtual void nodebug();
    virtual void setoutput(ostream *);

protected:

    // back pointer to traced engine
    IntlogExec *eng;

    // tracer status data
    ostream *str;
    int mode;
    slist called;
    stkpos invocnum;

    virtual int work(stkpos, CallData*, Action);
    virtual void showentry(SpyCall *, SpyEntry *, Action, ostream&);

    // access infos to display in IntlogExec
    const BindStack* GetBindStack() const;
    const TrailStack* GetTrailStack() const;
    const ProofStack* GetProofStack() const;

    ostream& fmtentry(ostream &, SpyCall *) const;
    stkpos findoffset(stkpos) const;
    SpyCall* findinvoc(CallData*, stkpos) const;
};

inline void ProofTracer::setoutput(ostream *s) {
    if (str)
        delete str;
    str = s;
}
inline void ProofTracer::trace_on() {
    mode |= AllEntries;
}
inline void ProofTracer::etrace_on() {
    mode |=  Extended;
}
inline void ProofTracer::etrace_off() {
    mode &= ~Extended;
}
inline void ProofTracer::nodebug() {
    clear();
}

//-------------------------------
// display exit (solution found)
//
inline int ProofTracer::exit(stkpos p, CallData* t) {
    return work(p, t, Exit);
}

//-----------------------------
// display redo (backtracking)
//
inline int ProofTracer::redo(stkpos p, CallData* t) {
    return work(p, t, Redo);
}

//---------------------------------------
// access infos to display in IntlogExec
//
inline const BindStack* ProofTracer::GetBindStack() const {
    return eng->vs;
}
inline const TrailStack* ProofTracer::GetTrailStack() const {
    return eng->ts;
}
inline const ProofStack* ProofTracer::GetProofStack() const {
    return eng->ps;
}

//--------------------
// keep spyed entries
//--------------------

class SpyEntry : public e_hashtable {
public:
    ~SpyEntry() override;

    const char* getstr() const override;
    const char* id;
    int arity;
    int stopmode;
};

inline const char* SpyEntry::getstr() const {
    return id;
}

//-------------------
// keep called terms
//-------------------

class SpyCall : public e_slist {
public:
    unsigned invoc;
    CallData* tcall;
    stkpos pos;
};

//-------------------
// display formatted
//-------------------

#include "fastree.h"

class IAFX_API FmtProofTree : public FastTree {
public:

    FmtProofTree(const ProofStack *ps);

    virtual int DisplayNode(ostream &s, int, const NodeLevel & nd) const;
    NodeIndex FindNode(stkpos) const;

    const ProofStack *m_ps;
};

#endif
