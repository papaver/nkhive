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
// SetIterator.h
//------------------------------------------------------------------------------

#ifndef __NKHIVE_VOLUME_SETITERATOR_H__
#define __NKHIVE_VOLUME_SETITERATOR_H__

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------

#include <nkhive/Defs.h>
#include <nkhive/Types.h>
#include <nkhive/volume/CellSetIterator.h>
#include <nkhive/volume/FilledBoundsIterator.h>
#include <nkhive/volume/NodeSetIterator.h>
#include <nkhive/volume/Node.h>

//------------------------------------------------------------------------------
// forward declarations
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// class definition
//------------------------------------------------------------------------------

BEGIN_NKHIVE_NS

/**
 * This iterator encapsulates other iterators and traverses down a node
 * hierarchy until it reaches a cell or fill node. Then it iterates over values
 * in the cell or fill node. 
 */
template <typename CellType>
class SetIterator : public AbstractIterator<typename CellType::value_type>
{

public:

    //--------------------------------------------------------------------------
    // typedefs
    //--------------------------------------------------------------------------
    
    typedef typename CellType::value_type                   value_type;
    typedef typename CellType::const_reference              const_reference;
    typedef std::forward_iterator_tag                       iterator_category;

    //--------------------------------------------------------------------------
    // public interface
    //--------------------------------------------------------------------------

    /**
     * Constructors and destructors.
     */
    SetIterator(Node<CellType> *n);
    virtual ~SetIterator();

    //--------------------------------------------------------------------------
    // Common iterator operations.
    //--------------------------------------------------------------------------

    /**
     * Test to see if the iterator is at it's end. 
     */
    virtual bool atEnd() const;
    
    /**
     * Return the value of the iterator.
     */
    virtual const_reference value() const;

    /**
     * Advance the iterator forward.
     */
    virtual void next(); 

    /**
     * Get index coordinates of the current iterator position
     */
    virtual void getCoordinates(index_type &i, 
                                index_type &j, 
                                index_type &k) const;

    /**
     * Return true if the iterator can return a value. False otherwise. 
     */
    virtual bool isValueIterator() const;

private:

    //--------------------------------------------------------------------------
    // internal typedefs.
    //--------------------------------------------------------------------------

    typedef 
    AbstractIterator<typename CellType::value_type>* abstract_iterator_pointer;

    //--------------------------------------------------------------------------
    // internal methods.
    //--------------------------------------------------------------------------

    /**
     * Initialize the hierarchy iterator stack given the root node.
     */
    void init(Node<CellType> *n);

    /**
     * Destroys all allocated iterators.
     */
    void destroy();

    /**
     * Creates a hierarchy of iterators until we reach a value iterator (e.g.,
     * CellSetIterator, FilledBoundsIterator) starting from the given
     * NodeSetIterator.
     */
    void createValueIterator(NodeSetIterator<CellType> *iter);

    /**
     * Creates the child iterator from the given NodeSetIterator. Returns true
     * if the created child iterator is a value iterator (e.g., CellSetIterator,
     * FilledBoundsIterator) and false otherwise.
     */
    bool createChildIterator(NodeSetIterator<CellType> *iter, 
                             abstract_iterator_pointer *child_iter);

    /**
     * Does a pop operation on the top of the stack. This will also free the
     * memory of the allocated iterator on the top of that stack.
     */
    void pop();

    /**
     * Returns the iterator at the top of the stack. 
     */
    abstract_iterator_pointer top() const;

    //--------------------------------------------------------------------------
    // friends.
    //--------------------------------------------------------------------------

    //--------------------------------------------------------------------------
    // members.
    //--------------------------------------------------------------------------

    /**
     * Store the stack of iterators that we are traversing over the tree
     * hierarchy. The root of the traversal stack is located at index 0. The
     * top of the stack is located at index size - 1.
     */
    std::vector<abstract_iterator_pointer> m_iterators;
};

END_NKHIVE_NS

//-----------------------------------------------------------------------------
// class implementation
//-----------------------------------------------------------------------------

BEGIN_NKHIVE_NS

#include <nkhive/volume/SetIterator.hpp>

END_NKHIVE_NS

//------------------------------------------------------------------------------

#endif // __NKHIVE_VOLUME_SETITERATOR_H__
