
/*
    IL : Intlog Language
    Object Oriented Prolog Project
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


////////////////////////
// metalogical builtins
////////////////////////
#include "stdafx.h"
#include "defsys.h"
#include "builtin.h"
#include "qdata.h"
#include "dbintlog.h"
#include "bterr.h"

BtFDecl(clause);
BtFDecl(functor);
BtFDecl(arg);
BtFDecl(name);
BtFDecl(univ);
BtFDecl(freeze);
BtFDecl(melt);

BuiltIn metalogical[7] = {
	{"clause",	2|BuiltIn::retry,	clause},
	{"functor",	3,	functor},
    {"arg",		3|BuiltIn::retry,	arg},
	{"name",	2,	name},
	{"=..",		2,	univ},
	{"freeze",	2,	freeze},
	{"melt",	2,	melt}
};

///////////////////////////////////////////////
// partial implementation, need p->unbind(pos)
//	also found this bug: a free,unique variable, not anonymus,
//	in body cause a out-of-space reference
//
class trackClause : public BltinData
{
public:
	~trackClause() {
		v.Destroy();
	}
	DbEntryIter i;
	Term v;
};

BtFImpl_R(clause, t, p, retry)
{
	trackClause *pd;

	if (!retry) {
		pd = new trackClause;
        if (((pd->v = p->copy(t.getarg(0))).type(f_NOTERM)) || pd->v.type(f_VAR)) {
			p->BtErr(BTERR_REQ_INSTANCE_ARG);
			delete pd;
			return 0;
		}
		p->set_btdata(p->save(pd));
		p->get_db()->Search(pd->v, pd->i, p->on_call()->get_clause()->get_db());
	} else
		pd = (trackClause*)p->get_btdata();

	// scan searching for a head match
	Term factbody = Term(kstring("true"));
	Clause*	c = pd->i.next();

	while (c) {

		// make head & body unification
		Term head = c->get_image(),
			 body = factbody;

#if 0 //def _DEBUG
char buf[1024];	// assertion failed: see above!
ostrstream(buf, sizeof buf) << teWrite(head, p) << ends;
#endif

		if (c->is_rule()) {
			body = head.getarg(1);
			head = head.getarg(0);
		}

		if (p->unify_gen(t.getarg(0), head)) {

			if (p->unify_gen(t.getarg(1), body))
				return 1;

			// unbind first unification
		}

		c = pd->i.next();
	}

	return 0;
}


/////////////////////////////////////
// retrieve functor or create struct
//
BtFImpl(functor, t, p)
{
	Term s = p->eval_term(t.getarg(0));

	if (s.type(f_STRUCT|f_ATOM)) {	// first usage mode: unify

		kstring funct = s.get_funct();
		int arity = s.get_arity();

		return	p->unify(t.getarg(1), Term(funct)) &&
				p->unify(t.getarg(2), Term(Int(arity)));
	}

	// second usage mode: create
	Term f = p->eval_term(t.getarg(1)),
		 n = p->eval_term(t.getarg(2));

	if (!f.type(f_ATOM) || !n.type(f_INT) || Int(n) < 0) {
		p->BtErr(BTERR_INVALID_ARG_TYPE);
		return 0;
	}

	if (Int(n) > 0) {	// create a struct with args uninstanced vars
		Term c = Term(f.kstr(), Int(n));
		for (int i = 0; i < Int(n); i++)
			c.setarg(i, Term(ANONYM_IX));
		return p->unify(t.getarg(0), p->save(c));

	}

	return p->unify(t.getarg(0), f);
}

//////////////////////////////////////
// find nth argument of compound term
//	behaves like SWI-Prolog arg
//  second arg MUST be instanced
//
struct arg_pos : public BltinData
{
    arg_pos() : pos(0) {}
    int pos;
    ~arg_pos() override;
};
arg_pos::~arg_pos() {}

BtFImpl_R(arg, t, p, r)
{
    Term a = p->eval_term(t.getarg(1));
    if (!r) {

        if (!a.type(f_STRUCT|f_LIST))
            return p->BtErr(BTERR_INVALID_ARG_TYPE);

        Term n = p->eval_term(t.getarg(0));
        if (n.type(f_INT)) {
            Int nn = Int(n);
            if (nn > 0 && nn <= a.get_arity())
                return p->unify(a.getarg(nn - 1), t.getarg(2));
            return 0;
        }

        if (!n.type(f_NOTERM))
            return p->BtErr(BTERR_INVALID_ARG_TYPE);

        p->set_btdata(new arg_pos);
    }

    auto g = dynamic_cast<arg_pos*>(p->get_btdata());

    int rc = g->pos < a.get_arity();
    if (rc) {
        rc = p->unify(t.getarg(0), Int(g->pos + 1));
        if (rc)
            rc = p->unify(a.getarg(g->pos), t.getarg(2));
        g->pos++;
    }
    return rc;
}

///////////////////////////////////////////////
// make list of integer from atom or viceversa
//
BtFImpl(name, t, p)
{
	Term a = p->eval_term(t.getarg(0));
	if (!a.type(f_NOTERM)) {

		if (!a.type(f_ATOM)) {
			p->BtErr(BTERR_INVALID_ARG_TYPE);
			return 0;
		}

		// make list
		CCP str = a;
		return p->unify(Term(str, strlen(str)), t.getarg(1));
	}

	Term ls = p->copy(t.getarg(1)), l = ls;
	if (l.type(f_NOTERM)) {
		p->BtErr(BTERR_REQ_INSTANCE_ARG);
		return 0;
	}
	if (!l.type(f_LIST)) {
		p->BtErr(BTERR_REQ_LIST_ARG);
		l.Destroy();
		return 0;
	}

	char buf[256 + 1];
	int i = 0;
	for ( ; !l.LNULL() && i < 256; i++) {
		Term h = l.getarg(0);
		if (!h.type(f_INT) || !(l = l.getarg(1)).type(f_LIST)) {
			p->BtErr(BTERR_REQ_LIST_ARG);
			ls.Destroy();
			return 0;
		}
		buf[i] = char(Int(h));
	}
	ls.Destroy();

	buf[i] = 0;
	return p->unify(t.getarg(0), Term(kstring(buf)));
}

//////////////////////////////////////
// unification with arbitrary clauses
//
BtFImpl(univ, t, p)
{
	Term x = p->eval_term(t.getarg(0)), u1, tgen;

	if (x.type(f_ATOM|f_STRUCT|f_LIST)) {

		// decompose data, instantiating list
		x = p->copy(t.getarg(0));
		kstring k = x.get_funct();

		Term tailFollow = tgen = Term(Term(k), Term(ListNULL));
		int arity = x.get_arity();
		for (int i = 0; i < arity; i++) {
			Term tl(x.getarg(i), Term(ListNULL));
			tailFollow.setarg(1, tl);
			tailFollow = tl;
			x.setarg(i, Int(0));	// avoid shared memory
		}

		u1 = t.getarg(1);

	} else {

		// analyze list, to build structure/atom
		if (!(x = p->copy(t.getarg(1))).type(f_LIST)) {
			p->BtErr(BTERR_REQ_LIST_ARG);
			x.Destroy();
			return 0;
		}

		Term head = x.getarg(0),
			 tail = x.getarg(1);
		if (!head.type(f_ATOM)) {
			p->BtErr(BTERR_INVALID_ARG_TYPE);
			x.Destroy();
			return 0;
		}

		// build the structure
		short n = 0, i;

		// count args
		while (tail.type(f_LIST) && !tail.LNULL()) {
			tail = tail.getarg(1);
			n++;
		}

		if (n == 0)	// build atom
			tgen = Term(head);
		else {		// build structure
			tgen = Term(head.kstr(), n);
			tail = x.getarg(1);
			for (i = 0; i < n; i++) {
				tgen.setarg(i, tail.getarg(0));
				tail.setarg(0, Term(TermData(0)));	// avoid shared memory
				tail = tail.getarg(1);
			}
		}

		u1 = t.getarg(0);
	}

	// release tmp memory
	x.Destroy();

	// unify args
	return p->unify_abs(u1, p->save(tgen));
}

/////////////////////////////////
// freeze(TermUnbound, TermBound)
//
// replace all free vars with text constants
//	should use unredable or hidden atoms, p->save(them)
//	and search when melt() required
//
BtFImpl(freeze, r, p)
{
	Term c = p->copy(r.getarg(0));
	Atom idVar("%VAR%");

	stack<Term> st;
	st.push(c);

	for ( ; st.size() > 0; )
	{
		Term t = st.get();
		st.pop();

		if (t.type(f_STRUCT|f_LIST))
		{
			int arity = t.get_arity();
			for (int ixson = 0; ixson < arity; ixson++)
			{
				Term son = t.getarg(ixson);
				if (son.type(f_VAR))
				{
					Term tVar(idVar, 1);
					tVar.setarg(0, Int(Var(son)));
					t.setarg(ixson, tVar);
				}
				else
					st.push(son);
			}
		}
	}

	return p->unify(p->save(c), r.getarg(1));
}
BtFTBD(melt)
