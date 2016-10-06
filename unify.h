
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


#ifndef _UNIFY_H_
#define _UNIFY_H_

#include <stdexcept>

//////////////////////////
// Unification data stack
//

#define MaxUnifyStack 128

class UnifyStack
{
public:

	// access to vars
	UnifyStack(BindStack *, TrailStack *);

	// a unified pair
	struct termPair {
		Term	t1, t2;
		stkpos	i1, i2;
	};

	// use this to initialize
	termPair* push();

	// carry on evaluation (note: variables binded!)
	int work(Term, stkpos, Term, stkpos);

	// empty storage
	void clear();

	// on overflow abort execution
	void check_overflow() const;

private:
	termPair v[MaxUnifyStack];
	int free;

	BindStack *vs;
	TrailStack *ts;
};

inline UnifyStack::termPair *UnifyStack::push()
{
	return v + free++;
}

inline void UnifyStack::clear()
{
	free = 0;
}

inline void UnifyStack::check_overflow() const
{
    if (free >= MaxUnifyStack)
        throw std::range_error("unification stack overflow");
}

#endif
