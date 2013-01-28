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

#ifdef _DEBUG
#define inline
#endif

inline FastTree::FastTree()
{	
	m_nRoot = INVALID_NODE;
	m_nTop = 0;
}
inline void FastTree::RemoveAll()
{
	m_nRoot = INVALID_NODE;
	m_nTop = 0;
}
inline NodeIndex FastTree::Root() const
{
	return m_nRoot;
}
inline unsigned FastTree::TotalSize() const
{
	return m_nTop;
}

inline FastTree::FastTree(const FastTree& toCopy)
{
	Assign(toCopy);
}
inline FastTree& FastTree::operator=(const FastTree& toCopy)
{
	Assign(toCopy);
	return *this;
}
inline NodeDefinition *FastTree::GetAt(NodeIndex allocated) const
{
    ASSERT(/*allocated >= 0 && */allocated < m_nTop);
	return base + allocated;
}
inline NodeIndex FastTree::NextSon(NodeIndex brother) const
{
	return GetAt(brother)->m_nBrother;
}
inline NodeDefinition &FastTree::operator[](NodeIndex allocated)
{
	return *GetAt(allocated);
}
inline NodeIndex FastTree::FirstSon(NodeIndex father) const
{
	return GetAt(father)->m_nFirstSon;
}
inline NodeIndex FastTree::LastSon(NodeIndex father) const
{
	NodeIndex s = FirstSon(father), l = INVALID_NODE;

	while (s != INVALID_NODE)
	{
		l = s;
		s = GetAt(s)->m_nBrother;
	}

	return l;
}
inline void FastTree::SetSon(NodeIndex father, NodeIndex firstSon)
{
	ASSERT(GetAt(father)->m_nFirstSon == INVALID_NODE);
	ASSERT(GetAt(firstSon)->m_nBrother == INVALID_NODE);
	GetAt(father)->m_nFirstSon = firstSon;
}
inline void FastTree::CatSon(NodeIndex father, NodeIndex firstSon)
{
	ASSERT(GetAt(father)->m_nFirstSon == INVALID_NODE);
	ASSERT(GetAt(firstSon) != 0);
	GetAt(father)->m_nFirstSon = firstSon;
}
inline void FastTree::AddSon(NodeIndex father, NodeIndex newSon)
{
	NodeIndex lastBrother = LastSon(father);
	if (lastBrother != INVALID_NODE)
		CatBrother(lastBrother, newSon);
	else
		CatSon(father, newSon);
}
inline void FastTree::SetBrother(NodeIndex firstSon, NodeIndex secSon)
{
	ASSERT(GetAt(firstSon)->m_nBrother == INVALID_NODE);
	ASSERT(GetAt(secSon)->m_nBrother == INVALID_NODE);
	GetAt(firstSon)->m_nBrother = secSon;
}
inline void FastTree::CatBrother(NodeIndex firstSon, NodeIndex secSon)
{
	ASSERT(GetAt(firstSon)->m_nBrother == INVALID_NODE);
	ASSERT(GetAt(secSon) != 0);
	GetAt(firstSon)->m_nBrother = secSon;
}
inline void FastTree::SetRoot(NodeIndex root)
{
	ASSERT(m_nRoot == INVALID_NODE);
	ASSERT(GetAt(root) != 0);
	m_nRoot = root;
}
inline NodeIndex FastTree::UnlinkBrother(NodeIndex son)
{
	NodeIndex brother = GetAt(son)->m_nBrother;
	GetAt(son)->m_nBrother = INVALID_NODE;
	return brother;
}
