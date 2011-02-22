//------------------------------------------------------------------------------
//
// Copyright (c) 2011, NektarFX, Inc. (http://www.nektarfx.com)
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without modific-
// ation, are permitted provided that the following conditions are met:
// 
//  - Redistributions of source code must retain the above copyright notice, 
//    this list of conditions and the following disclaimer.
//  - Redistributions in binary form must reproduce the above copyright 
//    notice, this list of conditions and the following disclaimer in the 
//    documentation and/or other materials provided with the distribution.
//  - Neither the name of NektarFX, Inc. nor the names of its contributors may 
//    be used to endorse or promote products derived from this software 
//    without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSE-
// QUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE 
// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY 
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH 
// DAMAGE.
// 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// NodeSetIterator.hpp
//------------------------------------------------------------------------------

// no includes allowed

//------------------------------------------------------------------------------

template <typename CellType>
inline 
NodeSetIterator<CellType>::NodeSetIterator(Node<CellType> *node) : 
    m_node(node)
{
    // The current node should never be the fill node. We always look ahead
    // when traversing the tree. A fill node is iterated over using a
    // FilledBoundsIterator.
    assert(!m_node->isFill());

    // Initialize the branch iterator.
    m_branch_iterator = 
        m_node->m_bitfield.setIterator(m_node->m_branches.begin());
}

//------------------------------------------------------------------------------

template <typename CellType>
inline 
NodeSetIterator<CellType>::~NodeSetIterator()
{
}

//------------------------------------------------------------------------------

template <typename CellType>
inline bool
NodeSetIterator<CellType>::atEnd() const
{
    return !m_branch_iterator();
}

//------------------------------------------------------------------------------

template <typename CellType>
inline typename NodeSetIterator<CellType>::const_reference
NodeSetIterator<CellType>::value() const
{
    THROW(Iex::LogicExc, "Cannot get value from NodeSetIterator");
}

//------------------------------------------------------------------------------

template <typename CellType>
inline void
NodeSetIterator<CellType>::next()
{
    ++m_branch_iterator;
}

//------------------------------------------------------------------------------

template <typename CellType>
inline void
NodeSetIterator<CellType>::getCoordinates(index_type &i,
                                          index_type &j,
                                          index_type &k) const
{
    m_branch_iterator.getCoordinates(i, j, k);
}

//------------------------------------------------------------------------------

template <typename CellType>
inline void
NodeSetIterator<CellType>::getCoordinates(index_vec &coords) const
{
    AbstractIterator<value_type>::getCoordinates(coords);
}

//------------------------------------------------------------------------------

template <typename CellType>
inline bool
NodeSetIterator<CellType>::isValueIterator() const
{
    return false;
}

//------------------------------------------------------------------------------

template <typename CellType>
inline Node<CellType>*
NodeSetIterator<CellType>::getNode() const
{
    return m_node;
}

//------------------------------------------------------------------------------

template <typename CellType>
inline const typename Node<CellType>::branch_iterator&
NodeSetIterator<CellType>::getBranchIterator() const
{
    return m_branch_iterator;
}

//------------------------------------------------------------------------------
