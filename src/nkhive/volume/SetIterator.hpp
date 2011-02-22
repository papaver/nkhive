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
// SetIterator.hpp
//------------------------------------------------------------------------------

// no includes allowed

//------------------------------------------------------------------------------

template <typename CellType>
inline
SetIterator<CellType>::SetIterator(Node<CellType> *node)
{
    init(node);
}

//------------------------------------------------------------------------------

template <typename CellType>
inline
SetIterator<CellType>::~SetIterator()
{
    destroy();
}

//------------------------------------------------------------------------------

template <typename CellType>
inline bool
SetIterator<CellType>::atEnd() const
{
    // the bottom of the stack cannot iterate forward.
    return top()->atEnd();
}

//------------------------------------------------------------------------------

template <typename CellType>
inline typename SetIterator<CellType>::const_reference
SetIterator<CellType>::value() const
{
    return top()->value();
}

//------------------------------------------------------------------------------

template <typename CellType>
inline void
SetIterator<CellType>::destroy()
{
    typename std::vector<abstract_iterator_pointer>::iterator iter = 
        m_iterators.begin();
    for ( ; iter != m_iterators.end(); ++iter) {
        delete *iter;
    }
    m_iterators.clear();
}

//------------------------------------------------------------------------------

template <typename CellType>
inline void 
SetIterator<CellType>::init(Node<CellType> *node)
{
    destroy();

    // Create the top level node iterator.
    NodeSetIterator<CellType> *iter = new NodeSetIterator<CellType>(node);

    // Initialize the stack with the current iterator.
    m_iterators.push_back(iter);

    // Create the stack of iterators until we get a value iterator.
    createValueIterator(iter);
}

//------------------------------------------------------------------------------

template <typename CellType>
inline void
SetIterator<CellType>::createValueIterator(NodeSetIterator<CellType> *iter)
{
    NodeSetIterator<CellType> *curr_iter = iter;

    // Iterate until we are at a cell or fill node. This is determined as long
    // as we have a valid NodeSetIterator to walk through.
    while (curr_iter && !curr_iter->atEnd()) {
    
        // get the next child iterator down
        abstract_iterator_pointer child_iter = NULL;
        if (createChildIterator(curr_iter, &child_iter)) {
            // Created a child iterator, so we need to reset the current
            // iterator to NULL.
            curr_iter = NULL;
        } else {
            // Otherwise, we created another NodeSetIterator so we need to
            // reset the NodeSetIterator appropriately. 
            curr_iter = dynamic_cast<NodeSetIterator<CellType>*>(child_iter);
            assert(curr_iter != NULL);
        }

        // insert the new child iterator at the top of the stack
        m_iterators.push_back(child_iter);
    }
}

//------------------------------------------------------------------------------

template <typename CellType>
inline void
SetIterator<CellType>::next()
{
    top()->next();

    if (top()->atEnd() && m_iterators.size() > 1) {
        // If the iterator is at the end, then pop from the stack and go to
        // parent. However, we check if the size is greater than one so that we
        // don't pop the top level node iterator.
        pop();
        next();
    } else if (!top()->isValueIterator()) {
        // If we have a valid iterator and it is not a value iterator, then we
        // need to create a stack hierarchy with value iterators.
        NodeSetIterator<CellType> *iter = 
            dynamic_cast<NodeSetIterator<CellType>*>(top());
        assert(iter != NULL);
        createValueIterator(iter);
    }
}

//------------------------------------------------------------------------------

template <typename CellType>
inline void
SetIterator<CellType>::getCoordinates(index_type &i, 
                                      index_type &j, 
                                      index_type &k) const
{
    // TODO: We accumulate offsets here on demand. This can be cached while are
    // building up the iterator stack. 
    index_vec offset(0, 0, 0);

    // Iterate over all the NodeSetIterators and accumulate their offsets.
    for (u32 idx = 0; idx < m_iterators.size() - 1; ++idx) {
        abstract_iterator_pointer iter = m_iterators[idx];
        NodeSetIterator<CellType> *node_iter = 
            dynamic_cast<NodeSetIterator<CellType>*>(iter);

        assert(node_iter != NULL);

        index_type child_dim = node_iter->getNode()->computeChildDim();

        index_vec node_offset(0, 0, 0);
        node_iter->getCoordinates(node_offset);
        node_offset *= child_dim;

        offset += node_offset;
    }

    // Finally, add the offset of the value iterator that is on top of the
    // stack.
    top()->getCoordinates(i, j, k);
    i += offset[0];
    j += offset[1];
    k += offset[2];
}

//------------------------------------------------------------------------------

template <typename CellType>
inline bool
SetIterator<CellType>::isValueIterator() const
{
    return true;
}

//------------------------------------------------------------------------------

template <typename CellType>
inline bool
SetIterator<CellType>::createChildIterator(NodeSetIterator<CellType> *iter, 
                                           abstract_iterator_pointer *child)
{
    // The given NodeSetIterator should never be empty.
    assert(!iter->atEnd());

    // The current node should never be a fill node, as this function assumes
    // that the given node has nodes/cells underneath it.
    Node<CellType> *node = iter->getNode();
    assert(!node->isFill());

    const typename Node<CellType>::branch_iterator& branch_iter = 
        iter->getBranchIterator();

    // If direct descendent of parent, then create the cell iterator..
    if (node->isCellParent()) {
        CellType *cell = branch_iter->cell;

        // A CellSetIterator handles both filled and non-filled cells.
        *child = new CellSetIterator<CellType>(cell);

        return true;
    } else {
        // else, the child is a node.
        Node<CellType> *child_node = branch_iter->node;
        if (child_node->isFill()) {
            // If the child is a FillNode, then create and return a
            // FilledBoundsIterator.
            index_bounds bounds;
            child_node->computeSetBounds(bounds);
            *child = 
                new FilledBoundsIterator<value_type>(bounds, 
                                                     child_node->fillValue());
            return true;
        } else {
            // Otherwise, create a NodeSetIterator for the child node
            *child = new NodeSetIterator<CellType>(child_node);
            return false;
        }
    }

    // Should never get here. Only adding a failsafe return so we don't get
    // compiler warning regarding no return for this function.
    assert(false);
    return false;
}

//------------------------------------------------------------------------------

template <typename CellType>
inline void
SetIterator<CellType>::pop()
{
    abstract_iterator_pointer iter = m_iterators.back();
    m_iterators.pop_back();
    delete iter;
}

//------------------------------------------------------------------------------

template <typename CellType>
inline typename SetIterator<CellType>::abstract_iterator_pointer
SetIterator<CellType>::top() const
{
    abstract_iterator_pointer iter = m_iterators.back();
    return iter;
}

//------------------------------------------------------------------------------
