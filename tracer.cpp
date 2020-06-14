
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


//---------------------------
// trace/spy model interface
//---------------------------

#include "stdafx.h"
#include "qdata.h"
#include "tracer.h"

//////////////////////////
// get the display string
//
static const char* action_str[4] = {
    "CALL", "FAIL", "EXIT", "REDO"
};

///////////////
// init status
//
ProofTracer::ProofTracer(IntlogExec *p)
    : hashtable_str(20)
{
    eng = p;
    str = 0;
    mode = 0;
    invocnum = STKNULL;
}
ProofTracer::~ProofTracer()
{
    delete str;
}

/////////////////////////////
// insert/delete a spy point
//
void ProofTracer::spy(const char* id, int on, int arity, int stopmode)
{
    SpyEntry *s = (SpyEntry*)isin(e_hashtable_str(id));

    if (s)
    {
        if (!on)
            remove(s);
    }
    else if (on)
    {
        s = new SpyEntry();
        s->id = id;
        s->arity = arity;
        s->stopmode = stopmode;

        insert(s);
    }
}

//////////////////////////
// display all spy points
//
void ProofTracer::debugging(ostream &s) const
{
    hashtable_iter it(this);

    SpyEntry *e;
    while ((e = (SpyEntry *)it.next()) != 0)
        s << CCP(e->id) << '/' << e->arity << endl;
}

/////////////////////////////////////////
// a call: give term a invocation number
//
int ProofTracer::call(stkpos p, CallData* t)
{
    if (p == 1)
    {
        called.clear();
        invocnum = 1;
    }

    SpyCall *s = new SpyCall();
    s->invoc = invocnum++;
    s->tcall = t;
    s->pos = p;
    called.insert(s, 0);

    return work(p, t, Call);
}

//////////////////////////////////////
// display fail
//	delete from called list the term
//
int ProofTracer::fail(stkpos p, CallData* t)
{
    int rc = work(p, t, Fail);

    slist_scan it(called);
    SpyCall *e;

    while ((e = (SpyCall*)it.next()) != 0)
        if (TermData(e->tcall) == TermData(t) && p == e->pos) {
            it.delitem();
            break;
        }

    return rc;
}

//////////////////////////////////////
// display infos associated to action
//
int ProofTracer::work(stkpos p, CallData* t, Action act)
{
    SpyEntry *pEntry = 0;

    if ((mode & AllEntries) ||
        (pEntry = (SpyEntry*)isin(e_hashtable_str(t->val().get_funct()))) != 0)
    {
        SpyCall *c = findinvoc(t, p);
        if (c)
            showentry(c, pEntry, act, str? *str : eng->out());
    }

    return 0;
}

////////////////////
// display an entry
//
ostream& ProofTracer::fmtentry(ostream &os, SpyCall *c) const
{
    Env *e = eng->ps->get(c->pos);
    e = eng->ps->get(e->father);
    ASSERT(e);

    teWrite w(c->tcall->val(), eng, e->vspos);
    return os << w;
}
void ProofTracer::showentry(SpyCall *c, SpyEntry*, Action act, ostream &os)
{
    if (c->invoc != unsigned(-1))
        os << '(' << c->invoc << ") ";

    fmtentry(os << action_str[act] << ':', c) << endl;
    if (mode & Extended)
    {
        IntlogExec::statusmode sm =	IntlogExec::statusmode(
                    IntlogExec::Proof|
                    IntlogExec::Vars|
                    IntlogExec::Trail);
        eng->showstatus(os, sm);
    }
}

////////////////////////////////////////
// find the proof tree offset of <pos>
//
stkpos ProofTracer::findoffset(stkpos pos) const
{
    Env *ec = eng->ps->get(pos);
    int off = 0;

    while (ec->father != STKNULL)
    {
        off++;
        ec = eng->ps->get(ec->father);
    }

    return off;
}

///////////////////////////////////////
// find the invocation number of term
//
SpyCall* ProofTracer::findinvoc(CallData *t, stkpos p) const
{
    slist_iter it(called);

    SpyCall *e;
    while ((e = (SpyCall*)it.next()) != 0)
        if (e->tcall == t && e->pos == p)
            return e;

    return 0;
}

////////////////////////////////////////////
// if nothing more to do,
//	release interface to speed up execution
//
void ProofTracer::trace_off()
{
    if (get_nentries() > 0)
        mode &= ~AllEntries;
    else
    {
        eng->tr = 0;
        delete this;
    }
}

#define GetEnv(Index) ((const Env *)GetAt(Index)->m_data)

// make proof formatted
FmtProofTree::FmtProofTree(const ProofStack *ps)
{
    m_ps = ps;

    // dummy root to connect all sub proof trees
    SetRoot(AllocNode());
    GetAt(m_nRoot)->m_data = 0;

    // build actual sub trees
    for (stkpos i = 0; i < ps->curr_dim(); i++)
    {
        const Env *e = ps->get(i);

        NodeIndex n = AllocNode();
        GetAt(n)->m_data = NodeData(e);

        if (e->father != STKNULL)
            AddSon(FindNode(e->father), n);
        else	 // connect to dummy root
            AddSon(Root(), n);
    }
}

int FmtProofTree::DisplayNode(ostream &s, int, const NodeLevel & nd) const
{
    const Env *e = GetEnv(nd.n);
    if (e)
    {
        if (e->call)
        {
            teWrite w(e->call->val());
            s << w;
        }
        else
            s << '?';
    }
    else
        s << '/';

    s << endl;
    return 0;
}

// find node, really!
NodeIndex FmtProofTree::FindNode(stkpos pNode) const
{
    const Env *e = m_ps->get(pNode);

    for (NodeIndex i = 0; i < m_nTop; i++)
        if (GetEnv(i) == e)
            return i;

    ASSERT(FALSE);
    return INVALID_NODE;
}
/*
 const Env *GetEnv(NodeIndex Index) const;
const Env *FmtProofTree::GetEnv(NodeIndex Index) const;
{
 return (const Env *)GetAt(Index)->m_data;
}
*/
