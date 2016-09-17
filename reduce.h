/*
    IL : Prolog interpreter
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

/*
	Intlog Language
	Object Oriented Prolog Project
	Ing. Capelli Carlo - Brescia
*/

#ifndef _REDUCE_H_
#define _REDUCE_H_

//--------------------------------
// operators reduction 'stack'
//	carry on operations as needed
//	to parse prolog expressions
//--------------------------------

#include "iafx.h"
#include "term.h"
#include "fastree.h"
#include "srcpos.h"

class IAFX_API SrcPosTree : public FastTree
{
public:
	virtual int DisplayNode(ostream &s, int, const NodeLevel & nd) const;
};

class IntlogParser;
class ReduceStack;

//------------------------------
// a reduction element:
//	can be a term or expression
//
class RedEl
{
public:

	enum etype
	{
		aTerm,	// term reference
		aOper,	// operator *
		aMark	// begin reduction save args placeholder
	}
		type;

	union
	{
		TermData	term;	// reduced term tree
		Operator*	op;		// pointer to subsequent tree 'node'
		unsigned	mark;	// keep saved arguments
	};

	// source position in mapping tree
	NodeIndex idx;	// keep tree node 'pointer'
	SourcePos sp;	// save parsed pos
	void SetPos(FastTree&);

	int is_term() const;
};

//---------------------------------------
// shift/reduction operation
//	parsing operations must be bracketed
//	with Begin()... End()
//
class ReduceStack : stack<RedEl>
{
friend class IntlogParser;

public:

	// only a parser utility
	ReduceStack();
    virtual ~ReduceStack();

	// begin a section
	void Begin();

	RedEl* AddOper(Operator *op, SourcePos sp);
	RedEl* AddTerm(Term t, SourcePos sp);
	RedEl* Reduce(bool bUsePar = FALSE);

	// terminate a section
	void End();

	// reset state
	void Clear();

	// keep ',' separed arguments count (structs' arity and lists' heads)
	unsigned m_nArgs;
	Term *m_Args;

	// build tree contextually to parsing
	SrcPosTree m_ft;

private:

	RedEl* push(Term td, SourcePos sp);
	void push(unsigned nArgs);
	void push(Operator* op, SourcePos sp);

	Operator* canreduce(Operator *);
	void reduce(Operator *);

	RedEl* get(unsigned = 0) const;
	RedEl* getchk(unsigned = 0) const;

	// actual arguments (m_Args will point to its elements)
	vect<Term> m_vArgs;
	unsigned CountArgs() const;
};

inline void ReduceStack::Clear()
{
	// empty arguments vector
	m_nArgs = 0;
	m_Args = m_vArgs.getptr(0);

	pop(size());
	m_ft.RemoveAll();
}
inline void ReduceStack::Begin()
{
	push(m_nArgs);

	// fix to start of available space
	m_Args += m_nArgs;
	m_nArgs = 0;
}

// insert a term (can only follow an operator)
inline RedEl* ReduceStack::AddTerm(Term t, SourcePos sp)
{
	push(t, sp);
	return get();
}

inline unsigned ReduceStack::CountArgs() const
{
	return m_Args - m_vArgs.getptr(0);
}
inline int RedEl::is_term() const
{
	return type == aTerm;
}

inline RedEl* ReduceStack::get(unsigned off) const
{
	return getptr(off);
}

inline void ReduceStack::push(unsigned cArgs)
{
	RedEl* e = reserve();

	e->type = RedEl::aMark;
	e->mark = cArgs;

#ifdef _DEBUG
	e->sp = SourcePos(-1);
#endif
	e->idx = INVALID_NODE;
}
inline RedEl* ReduceStack::push(Term t, SourcePos sp)
{
	RedEl* e = reserve();

	e->type = RedEl::aTerm;
	e->term = t;

	e->sp = sp;
	e->idx = INVALID_NODE;

	return e;
}
inline void ReduceStack::push(Operator* op, SourcePos sp)
{
	RedEl* e = reserve();

	e->type = RedEl::aOper;
	e->op = op;

	e->sp = sp;
	e->idx = INVALID_NODE;
}

inline void RedEl::SetPos(FastTree& ft)
{
	if (idx == INVALID_NODE)
		ft[idx = ft.AllocNode()].m_data = sp;
}

#endif
