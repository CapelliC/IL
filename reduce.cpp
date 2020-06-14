
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


//-------------------------
// term parsing reduction
//-------------------------

#include "stdafx.h"
#include "reduce.h"
#include "parse.h"
#include "proios.h"

#define DIMGROW 32

ReduceStack::ReduceStack() : stack<RedEl>()
{
	m_vArgs.grow(DIMGROW);
}
ReduceStack::~ReduceStack()
{
}

//-----------------------------------------------
// explicit terminal call
//	if sp.Used(), check for parentesized terms,
//	to avoid losing nesting infos
//
RedEl *ReduceStack::Reduce(bool bUsePar)
{
	RedEl *el0 = get(0), *el1;

el1 = get(1);

	unsigned a_count = 0;
	NodeIndex lastIdx = INVALID_NODE;

	if (bUsePar)
	{
		m_nArgs = 1;

		// count expected arguments list
		for (unsigned i = 0; get(i)->type != RedEl::aMark; i++)
			if ((el1 = get(i))->type == RedEl::aOper && el1->op->opCode == Operator::AND)
				m_nArgs++;

		// allocate required space to arguments
		unsigned cbase = CountArgs();
		while (cbase + m_nArgs > m_vArgs.dim())
		{
			m_vArgs.grow(DIMGROW);
			m_Args = m_vArgs.getptr(cbase);	// resinc on memory realloc
		}

		a_count = m_nArgs;
	}

	while (get(1)->type != RedEl::aMark)
	{
		Operator *op0, *op1;
		int off1 = 1;

		if (el0->is_term())
		{
			el0 = get(1);
			off1++;
		}
		op0 = el0->op;

		if ((el1 = getchk(off1)) != 0)
		{
			if (el1->is_term())
				el1 = getchk(off1 + 1);

			if (el1)
			{
				op1 = el1->op;

				// precedence error
				if (op1->prec < op0->prec)
					return 0;

				// check associativity
				if (	op0->prec  == op1->prec		&&
						op0->assoc != Operator::YFY &&
						op0->assoc != Operator::YFX &&
						op0->assoc != Operator::YF  &&
						op1->assoc != Operator::YFY &&
						op1->assoc != Operator::XFY &&
						op1->assoc != Operator::FY)
					return 0;
			}
		}

		if (bUsePar && el0->type == RedEl::aOper && op0->opCode == Operator::AND)
		{
			RedEl *top = get();

			top->SetPos(m_ft);
			if (lastIdx != INVALID_NODE)
				m_ft.CatBrother(top->idx, lastIdx);
			lastIdx = top->idx;

			m_Args[--a_count] = Term(top->term);
			pop(2);
		}
		else
			reduce(op0); // reduce rightmost operator

		el0 = get(0);
	}

	if (bUsePar)
	{
		RedEl *top = get();

		top->SetPos(m_ft);
		if (lastIdx != INVALID_NODE)
			m_ft.CatBrother(top->idx, lastIdx);

		m_Args[--a_count] = Term(top->term);
		ASSERT(a_count == 0);
	}

	ASSERT(el0->term != f_NOTERM);
	return el0;
}

//--------------------------------------
// this operation could cause reduction
//	of a lower prec term stacked
//
RedEl* ReduceStack::AddOper(Operator *op, SourcePos sp)
{
	Operator *tored;

	while ((tored = canreduce(op)) != 0)
		reduce(tored);

	push(op, sp);
	return get();
}

//--------------------------------------------------------------
// anayze if reduction can take place
//	i.e. see if last operator has lower precedence than <optop>
//
Operator* ReduceStack::canreduce(Operator *optop)
{
	RedEl *el = get();

	if (el->type != RedEl::aMark)
	{
		if (el->is_term())
			el = getchk(1);

		if (el)
		{
			ASSERT(el->type == RedEl::aOper);

			Operator *op = el->op;

			// check prec
			if (op->prec < optop->prec &&
					optop->assoc != Operator::FX &&
					optop->assoc != Operator::FY)
				return op;

			// check associativity
			if (op->prec == optop->prec && (
					optop->assoc == Operator::YFY ||
					optop->assoc == Operator::YFX ||
					optop->assoc == Operator::YF))
				return op;
		}
	}

	return 0;
}

//-----------------------------------------------
// reduce op, using precedente and associativity
//
void ReduceStack::reduce(Operator* op)
{
	Term t;

	switch (op->assoc)
	{
	case Operator::XFX:
	case Operator::XFY:
	case Operator::YFX:
	case Operator::YFY: {
		RedEl *left = get(2), *right = get(0);

		// build node
		NodeIndex root = m_ft.AllocNode();
		SourcePos sp = get(1)->sp;

		left->SetPos(m_ft);
		m_ft.SetSon(root, left->idx);

		right->SetPos(m_ft);
		m_ft.SetBrother(left->idx, right->idx);

		t = Term(op->name, 2);
		t.setarg(0, left->term);
		t.setarg(1, right->term);

		pop(3);
		push(t, m_ft[root].m_data = sp)->idx = root;
		}
		break;

	case Operator::FX:
	case Operator::FY: {

		RedEl *right = get(0);

		// build node
		NodeIndex root = m_ft.AllocNode();
		SourcePos sp = get(1)->sp;

		right->SetPos(m_ft);
		m_ft.SetSon(root, right->idx);

		t = Term(op->name, 1);
		t.setarg(0, right->term);

		pop(2);
		push(t, m_ft[root].m_data = sp)->idx = root;
		}
		break;

	case Operator::XF:
	case Operator::YF: {

		RedEl *left = get(1);

		// build node
		NodeIndex root = m_ft.AllocNode();
		SourcePos sp = get(0)->sp;

		left->SetPos(m_ft);
		m_ft.SetSon(root, left->idx);

		t = Term(op->name, 1);
		t.setarg(0, left->term);

		pop(2);
		push(t, m_ft[root].m_data = sp)->idx = root;
		}
	}
}

//-------------------------------------
// find the bottom mark of current env
//
void ReduceStack::End()
{
	while (get(0)->type != RedEl::aMark)
		pop();

	// restore arguments
	m_nArgs = get(0)->mark;
	m_Args -= m_nArgs;
	ASSERT(CountArgs() < m_vArgs.dim());

	pop();
}

RedEl* ReduceStack::getchk(unsigned off) const
{
	if (off < size())
	{
		RedEl *e = get(off);
		if (e->type != RedEl::aMark)
			return e;
	}

	return 0;
}

int SrcPosTree::DisplayNode(ostream &s, int, const NodeLevel& nl) const
{
	SourcePos sp(GetAt(nl.n)->m_data);

	if (sp.Used())
		s << sp.row() << ',' << sp.col() << ':' << sp.len();
	else
		s << "?";

	s << endl;
	return 0;
}
