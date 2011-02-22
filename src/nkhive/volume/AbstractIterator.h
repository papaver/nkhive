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
// AbstractIterator.h
//------------------------------------------------------------------------------

#ifndef __NKHIVE_VOLUME_ABSTRACT_ITERATOR_H__
#define __NKHIVE_VOLUME_ABSTRACT_ITERATOR_H__

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------

#include <nkhive/Defs.h>
#include <nkhive/Types.h>

//------------------------------------------------------------------------------
// class definition
//------------------------------------------------------------------------------

BEGIN_NKHIVE_NS

/** 
 * Defines an abstract iterator concept that can be used to iterate over values
 * in the structure. T defines the value type the iterator is iterating over.
 */
template <typename T>
class AbstractIterator 
{

public:

    //--------------------------------------------------------------------------
    // typedefs
    //--------------------------------------------------------------------------

    typedef std::forward_iterator_tag       iterator_category;
    typedef T                               value_type;
    typedef value_type&                     reference;
    typedef const value_type&               const_reference;
    typedef value_type*                     pointer;

    /**
     * Constructors and destructors.
     */
    AbstractIterator();
    virtual ~AbstractIterator();

    //--------------------------------------------------------------------------
    // Common iterator operations.
    //--------------------------------------------------------------------------

    /**
     * Test to see if the iterator is at it's end. 
     */
    virtual bool atEnd() const = 0;
    
    /**
     * Return the value of the iterator.
     */
    virtual const_reference value() const = 0;

    /**
     * Advance the iterator forward.
     */
    virtual void next() = 0; 

    /**
     * Get index coordinates of the current iterator position
     */
    virtual void getCoordinates(index_type &i, 
                                index_type &j, 
                                index_type &k) const = 0;
    void getCoordinates(index_vec &coords) const;

    /**
     * Return true if the iterator can return a value. False otherwise. 
     */
    virtual bool isValueIterator() const = 0; 
 
};

END_NKHIVE_NS

//-----------------------------------------------------------------------------
// class implementation
//-----------------------------------------------------------------------------

BEGIN_NKHIVE_NS

#include <nkhive/volume/AbstractIterator.hpp>

END_NKHIVE_NS

#endif // __NKHIVE_VOLUME_ABSTRACT_ITERATOR_H__
