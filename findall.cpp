
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


#include "qdata.h"
#include "builtin.h"
#include "unify.h"

#include <QDebug>
#include <QTime>

/*
    findall : Clocksin, Mellish PROLOG implementation

findall(X,G,_):-
    asserta(found('$mark')),
    call(G),
    asserta(found(X)),
    fail.
findall(_,_,N) :-
    collect_found([],N),
collect_found(S,L) :-
    getnext(X),
    !,
    collect_found([X|S],L).
collect_found(L,L).
getnext(X) :-
    retract(found(X)),
    !,
    X \= '$mark'.
*/
int IntlogExec::findall(Term X, Term G, Term L) {
    //qDebug() << "findall" << QTime::currentTime();
    Term C = copy(G),
         N = ListNULL, V;

    OffVars oG(C);

    DbIntlog *dbc = ps->topget()->call->get_clause()->get_db();
    if (dbc == nullptr)
        dbc = db;

    Clause c(C, nullptr, dbc);
    c.set_nvars(oG.nvars());

    // correct immediate assert() behaviour,
    //  due to test of src in Clause::Clause, cyclic reference error
    c.set_source(kstring("findall"));

    stkpos  pX = ps->topget()->vspos,
            tc = ps->curr_dim(),
            tp = ts->curr_dim();

    // count new variables required
    unsigned nnv = 0, offv = vs->curr_dim() - pX;

    int qr = query(&c);
    if (qr == 1) {
        stkpos qp = ts->curr_dim();
        for (Var v = 0; v < oG.nvars(); v++) {
            Var bv = oG.new2old(Var(v));
            ts->bind(bv);
            vs->setshare(bv, ps->get(tc)->vspos + v);
        }

        V = copy_term(X, pX);
        unbind(qp);

        OffVars oV(V, offv);
        nnv += oV.nvars();

        Term S(V, N);
        N = S;

        while ((qr = query()) == 1) {
            qp = ts->curr_dim();
            for (Var v = 0; v < oG.nvars(); v++) {
                Var bv = oG.new2old(Var(v));
                ts->bind(bv);
                vs->setshare(bv, ps->get(tc)->vspos + v);
            }

            V = copy_term(X, pX);
            unbind(qp);

            oV.SetTerm(V, offv + nnv);
            nnv += oV.nvars();

            S.setarg(1, Term(V, Term(ListNULL)));
            S = S.getarg(1);
        }
    }

    if (qr == -1)
        return 0;

    unbind(tp);
    ps->pop();  // release dummy top node

    // allocate space to new vars
    vs->reserve(nnv);

    //qDebug() << "findall unify" << QTime::currentTime();

    UnifyStack us(vs, ts);
    auto rc = us.work(L, pX, save(N), pX);
    //qDebug() << "findall done" << QTime::currentTime() << "with" << rc;
    return rc;
}

int IntlogExec::setof(Term, Term, Term) //(Term V, Term L, Term P)
{
    return 0;
}
int IntlogExec::bagof(Term, Term, Term) // (Term V, Term L, Term P)
{
    return 0;
}
