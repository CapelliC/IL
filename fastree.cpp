
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


#include "stdafx.h"
#include "fastree.h"
#include <iomanip>

void FastTree::Assign(const FastTree& toCopy)
{
	m_nRoot = toCopy.m_nRoot;
    m_nTop = toCopy.m_nTop;
    ASSERT(false);
	//memcpy(base, toCopy.base, sizeof(NodeDefinition) * m_nTop);
}

// display indented
int FastTree::DisplayNode(ostream &s, int, const NodeLevel &n) const
{
	s << GetAt(n.n)->m_data << endl;
	return 0;
}

void FastTree::Display(ostream &s, unsigned off) const
{
	stack<NodeLevel> v;

	NodeLevel q = { m_nRoot, 0 };
	v.push(q);

	while (v.size() > 0)
	{
		q = v.get();
		v.pop();

		int nLine = 0;
		do
		{
			for (unsigned c = 0; c < q.l * off; c++)
				s << ' ';
		}
		while (DisplayNode(s, nLine++, q));

		PushSons(v, q);
	}
}
void FastTree::DisplayConnected(ostream &s, unsigned char setc[5], unsigned off) const
{
	stack<NodeLevel> snl;

	NodeLevel q = { m_nRoot, 0 };
	snl.push(q);

	while (snl.size() > 0)
	{
		// unstack last node
		q = snl.get();
		snl.pop();

		int nLine = 0;
		do
		{
			// draw higher level connections
			for (unsigned l = 1; l < q.l; l++)
			{
				unsigned h = snl.size();
				for ( ; h >= 1; h--)
				{
					NodeLevel t = snl.get(h - 1);
					if (t.l == l)
						break;
				}

				if (h >= 1)
					s << setc[CH_V];
				else
					s << setc[CH_SP];

				for (unsigned k = 0; k < off; k++)
					s << setc[CH_SP];
			}

			// draw node
			NodeDefinition &nd = *GetAt(q.n);

			if (q.l > 0)
			{
				if (nd.m_nBrother != INVALID_NODE)
					s << setc[CH_VR];
				else
					s << setc[CH_BR];

				for (unsigned k = 0; k < off; k++)
					s << setc[CH_H];
			}
		}
		while (DisplayNode(s, nLine++, q));

		PushSons(snl, q);
	}
}
void FastTree::CharConnections(unsigned char setc[5], selConn select)
{
	setc[CH_SP] = ' ';

	switch (select)
	{
	case V1H1:
		setc[CH_V] = 179;
		setc[CH_H] = 196;
		setc[CH_BR] = 192;
		setc[CH_VR] = 195;
		break;

	case V1H2:
	case V2H1:
	case V2H2:
		break;
	}
}

// reserve a slot to be filled later
NodeIndex FastTree::AllocNode()
{
    if (m_nTop == dim())
        grow(256);
    getptr(m_nTop)->m_nFirstSon = INVALID_NODE;
	getptr(m_nTop)->m_nBrother = INVALID_NODE;
	return m_nTop++;
}

unsigned FastTree::NumSons(NodeIndex father) const
{
	unsigned nsons = 0;

	NodeIndex n = GetAt(father)->m_nFirstSon;
	while (n != INVALID_NODE)
	{
		nsons++;
		n = GetAt(n)->m_nBrother;
	}

	return nsons;
}
NodeIndex FastTree::NthSon(NodeIndex father, unsigned index) const
{
	NodeIndex n = GetAt(father)->m_nFirstSon;

	while (index > 0)
	{
		index--;
		n = GetAt(n)->m_nBrother;
	}

	return n;
}

// push sons in order
void FastTree::PushSons(stack<NodeLevel> &v, const NodeLevel &q) const
{
	for (unsigned z = NumSons(q.n); z > 0; z--)
	{
		NodeLevel h = { NthSon(q.n, z - 1), q.l + 1 };
		v.push(h);
	}
}

// compute dimension from root
unsigned FastTree::GetSize(NodeIndex node) const
{
	unsigned size = 1;

	stack<NodeLevel> v;
	NodeLevel q = { node, 0 };
	v.push(q);

	while (v.size() > 0)
	{
		size++;

		q = v.get();
		v.pop();

		for (NodeIndex z = GetAt(q.n)->m_nFirstSon; z != INVALID_NODE; z = GetAt(z)->m_nBrother)
		{
			NodeLevel h = { z, q.l + 1 };
			v.push(h);
		}
	}

	return size;
}
