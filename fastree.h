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

#ifndef _FASTREE_H_
#define _FASTREE_H_

//////////////////////////////////
// fast tree construction,
//	to be used when parsing terms
//	to store source positions
//

#include "stack.h"
#include "iafx.h"

// forward decl
class FastTree;
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
    NodeIndex m_nFirstSon, m_nBrother;

    friend class FastTree;
};

class IAFX_API FastTree : vect<NodeDefinition>
{
public:

    FastTree() {
        m_nRoot = INVALID_NODE;
        m_nTop = 0;
    }

    FastTree(const FastTree& toCopy) {
        Assign(toCopy);
    }

    FastTree& operator=(const FastTree& toCopy) {
        Assign(toCopy);
        return *this;
    }
    virtual ~FastTree() {}

    // formatted display indented
    virtual int DisplayNode(ostream &s, int nline, const NodeLevel & nd) const;
    void Display(ostream &s, unsigned off = 1) const;
    void DisplayConnected(ostream &s, unsigned char connLines[5], unsigned off = 1) const;

    // type connection character options
    enum selConn { V1H1, V1H2, V2H1, V2H2 };
    enum selChar { CH_V, CH_H, CH_BR, CH_VR, CH_SP };
    static void CharConnections(unsigned char connLines[5], selConn select = V1H1);

    // release all elements indexs
    void RemoveAll() {
        m_nRoot = INVALID_NODE;
        m_nTop = 0;
    }

    // access a node (debug validate index)
    NodeDefinition &operator[](NodeIndex allocated) {
        return *GetAt(allocated);
    }

    NodeDefinition *GetAt(NodeIndex allocated) const {
        ASSERT(/*allocated >= 0 && */allocated < m_nTop);
        return getptr(allocated);
    }

    // tree construction
    void SetSon(NodeIndex father, NodeIndex firstSon) {
        ASSERT(GetAt(father)->m_nFirstSon == INVALID_NODE);
        ASSERT(GetAt(firstSon)->m_nBrother == INVALID_NODE);
        GetAt(father)->m_nFirstSon = firstSon;
    }

    void CatSon(NodeIndex father, NodeIndex firstSon) {
        ASSERT(GetAt(father)->m_nFirstSon == INVALID_NODE);
        ASSERT(GetAt(firstSon) != 0);
        GetAt(father)->m_nFirstSon = firstSon;
    }

    void AddSon(NodeIndex father, NodeIndex newSon) {
        NodeIndex lastBrother = LastSon(father);
        if (lastBrother != INVALID_NODE)
            CatBrother(lastBrother, newSon);
        else
            CatSon(father, newSon);
    }

    void SetBrother(NodeIndex firstSon, NodeIndex secSon) {
        ASSERT(GetAt(firstSon)->m_nBrother == INVALID_NODE);
        ASSERT(GetAt(secSon)->m_nBrother == INVALID_NODE);
        GetAt(firstSon)->m_nBrother = secSon;
    }

    void CatBrother(NodeIndex firstSon, NodeIndex secSon) {
        ASSERT(GetAt(firstSon)->m_nBrother == INVALID_NODE);
        ASSERT(GetAt(secSon) != 0);
        GetAt(firstSon)->m_nBrother = secSon;
    }

    void SetRoot(NodeIndex root) {
        ASSERT(m_nRoot == INVALID_NODE);
        ASSERT(GetAt(root) != 0);
        m_nRoot = root;
    }

    NodeIndex UnlinkBrother(NodeIndex son) {
        NodeIndex brother = GetAt(son)->m_nBrother;
        GetAt(son)->m_nBrother = INVALID_NODE;
        return brother;
    }

    // reserve a slot to be given to GetAt, SetSon, CatSon, ...
    NodeIndex AllocNode();

    // tree visit
    NodeIndex Root() const {
        return m_nRoot;
    }

    NodeIndex FirstSon(NodeIndex father) const {
        return GetAt(father)->m_nFirstSon;
    }

    NodeIndex LastSon(NodeIndex father) const {
        NodeIndex s = FirstSon(father), l = INVALID_NODE;

        while (s != INVALID_NODE)
        {
            l = s;
            s = GetAt(s)->m_nBrother;
        }

        return l;
    }

    NodeIndex NextSon(NodeIndex brother) const {
        return GetAt(brother)->m_nBrother;
    }

    unsigned NumSons(NodeIndex father) const;
    NodeIndex NthSon(NodeIndex father, unsigned index) const;
    void PushSons(stack<NodeLevel> &v, const NodeLevel &q) const;

    // compute dimension from required node
    unsigned GetSize(NodeIndex node) const;
    unsigned TotalSize() const {
        return m_nTop;
    }

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

#endif
