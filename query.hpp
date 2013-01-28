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

#ifdef _DEBUG
#define inline
#endif

inline DbIntlog* IntlogExec::get_db() const
{
    return db;
}
inline void IntlogExec::set_db(DbIntlog* d)
{
    db = d;
}
inline Term IntlogExec::eval_term(Term t) const
{
    return t.type(f_VAR)? tval(Var(t)) : t;
}

// more Efficient Term Display
inline teWrite::teWrite(Term toWrite, IntlogExec *pExec, stkpos stkPos)
{
    m_toWrite = toWrite;
    m_pExec = pExec;
    m_stkPos = stkPos;
}
inline ostream& operator<<(ostream& s, teWrite& data)
{
    return data.m_pExec->write(data.m_toWrite, s, data.m_stkPos);
}
inline ostream& operator<<(ostream& s, Term t)
{
    teWrite w(t, 0);
    return s << w;
}
