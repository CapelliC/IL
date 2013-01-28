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

#ifndef _FASTREE_H_
#define _FASTREE_H_

//////////////////////////////////
// fast tree construction,
//	to be used when parsing terms
//	to store source positions
//

#ifndef _STACK_H_
#include "stack.h"
#endif

#ifndef _IAFX_H_
#include "iafx.h"
#endif

// forward decl
class FastTree;
class stackNodeLevel;
struct NodeLevel;

// data types
typedef unsigned long NodeData;
typedef unsigned NodeIndex;

#define INVALID_NODE NodeIndex(-1)

// vector elements
class NodeDefinition
{
public:

    // store your data (term...)
    NodeData m_data;

private:

    // tree construction info
    NodeIndex	m_nFirstSon,
    m_nBrother;

    friend class FastTree;
};

decl_vect(NodeDefinition);

class IAFX_API FastTree : vectNodeDefinition
{
public:

    FastTree();
    FastTree(const FastTree&);
    FastTree& operator=(const FastTree&);

    // formatted display indented
    virtual int DisplayNode(ostream &s, int nline, const NodeLevel & nd) const;
    void Display(ostream &s, unsigned off = 1) const;
    void DisplayConnected(ostream &s, unsigned char connLines[5], unsigned off = 1) const;

    // type connection character options
    enum selConn { V1H1, V1H2, V2H1, V2H2 };
    enum selChar { CH_V, CH_H, CH_BR, CH_VR, CH_SP };
    static void CharConnections(unsigned char connLines[5], selConn select = V1H1);

    // release all elements indexs
    void RemoveAll();

    // access a node (debug validate index)
    NodeDefinition &operator[](NodeIndex allocated);
    NodeDefinition *GetAt(NodeIndex allocated) const;

    // tree construction
    void SetSon(NodeIndex father, NodeIndex firstSon);
    void CatSon(NodeIndex father, NodeIndex firstSon);
    void AddSon(NodeIndex father, NodeIndex newSon);
    void SetBrother(NodeIndex firstSon, NodeIndex secSon);
    void CatBrother(NodeIndex firstSon, NodeIndex secSon);
    void SetRoot(NodeIndex root);
    NodeIndex UnlinkBrother(NodeIndex son);

    // reserve a slot to be given to GetAt, SetSon, CatSon, ...
    NodeIndex AllocNode();

    // tree visit
    NodeIndex Root() const;
    NodeIndex FirstSon(NodeIndex father) const;
    NodeIndex LastSon(NodeIndex father) const;
    NodeIndex NextSon(NodeIndex brother) const;
    unsigned NumSons(NodeIndex father) const;
    NodeIndex NthSon(NodeIndex father, unsigned index) const;
    void PushSons(stackNodeLevel &v, const NodeLevel &q) const;

    // compute dimension from required node
    unsigned GetSize(NodeIndex node) const;
    unsigned TotalSize() const;

protected:

    void Assign(const FastTree& toCopy);

    // save free allocation
    NodeIndex m_nTop;
    NodeIndex m_nRoot;

};

// used to fast visit term
struct NodeLevel
{
    NodeIndex n;
    unsigned l;
};

decl_vect(NodeLevel);
decl_stack(NodeLevel);

#ifndef _DEBUG
#include "fastree.hpp"
#endif

#endif
