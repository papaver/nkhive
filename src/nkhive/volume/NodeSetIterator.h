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
// NodeSetIterator.h
//------------------------------------------------------------------------------

#ifndef __NKHIVE_VOLUME_NODE_SET_ITERATOR_H__
#define __NKHIVE_VOLUME_NODE_SET_ITERATOR_H__

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------

#include <nkhive/Defs.h>
#include <nkhive/Types.h>
#include <nkhive/volume/CellSetIterator.h>
#include <nkhive/volume/FilledBoundsIterator.h>
#include <nkhive/volume/Node.h>

//------------------------------------------------------------------------------
// forward declarations
//------------------------------------------------------------------------------

BEGIN_NKHIVE_NS

//------------------------------------------------------------------------------
// class definition
//------------------------------------------------------------------------------

/**
 * This iterator traverses the branches contained in a given node. It does not
 * return a value.
 */
template <typename CellType>
class NodeSetIterator : public AbstractIterator<typename CellType::value_type>
{
public:

    //--------------------------------------------------------------------------
    // typedefs
    //--------------------------------------------------------------------------

    typedef typename CellType::value_type                   value_type;
    typedef typename CellType::const_reference              const_reference;
    typedef std::forward_iterator_tag                       iterator_category;

    /**
     * Constructors and destructors.
     */
    NodeSetIterator(Node<CellType> *n);
    virtual ~NodeSetIterator();

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

    void getCoordinates(index_vec &coords) const;

    /**
     * Return true if the iterator can return a value. False otherwise. 
     */
    virtual bool isValueIterator() const;

    //--------------------------------------------------------------------------
    // Other members.
    //--------------------------------------------------------------------------

    Node<CellType>* getNode() const;

    const typename Node<CellType>::branch_iterator& getBranchIterator() const;

protected:

private:

    //--------------------------------------------------------------------------
    // private typedefs
    //--------------------------------------------------------------------------

    typedef 
    AbstractIterator<typename CellType::value_type>* abstract_iterator_pointer;

    //--------------------------------------------------------------------------
    // internal methods.
    //--------------------------------------------------------------------------

    //--------------------------------------------------------------------------
    // members 
    //--------------------------------------------------------------------------

    /**
     * Store the node we are iterating over.
     */
    Node<CellType> *m_node;

    /** 
     * Store the current branch_iterator of the node we are iterating over.
     * This allows us to iterate over existing branches in the node.
     */
    typename Node<CellType>::branch_iterator m_branch_iterator;
};

END_NKHIVE_NS

//-----------------------------------------------------------------------------
// class implementation
//-----------------------------------------------------------------------------

BEGIN_NKHIVE_NS

#include <nkhive/volume/NodeSetIterator.hpp>

END_NKHIVE_NS

#endif //__NKHIVE_VOLUME_NODE_SET_ITERATOR_H__
