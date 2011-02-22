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
// FilledBoundsIterator.h
//------------------------------------------------------------------------------

#ifndef __NKHIVE_VOLUME_FILLED_BOUNDS_ITERATOR_H__
#define __NKHIVE_VOLUME_FILLED_BOUNDS_ITERATOR_H__

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------

#include <nkhive/Defs.h>
#include <nkhive/Types.h>
#include <nkhive/util/Bounds3D.h>

#include <nkhive/bitfields/BitOps.h>

//------------------------------------------------------------------------------
// forward declarations
//------------------------------------------------------------------------------

#ifdef UNITTEST
template <typename T>
class TestAbstractIterators; 
#endif

//------------------------------------------------------------------------------
// class definition
//------------------------------------------------------------------------------

BEGIN_NKHIVE_NS

/** 
 * Defines an iterator that emulates iterating over a single fill value over a
 * fixed index bounds. 
 */
template <typename T>
class FilledBoundsIterator : public AbstractIterator<T>
{
public:

    //--------------------------------------------------------------------------
    // typedefs
    //--------------------------------------------------------------------------

    typedef typename AbstractIterator<T>::iterator_category  iterator_category;
    typedef typename AbstractIterator<T>::value_type         value_type;
    typedef typename AbstractIterator<T>::const_reference    const_reference;

    /**
     * Constructors and destructors. This iterator is constructed using a 3D
     * bounds and the fill value used in those bounds.
     */
    FilledBoundsIterator(const index_bounds &bounds, const_reference v);
    virtual ~FilledBoundsIterator();

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

protected:

private:

    //--------------------------------------------------------------------------
    // private typedefs
    //--------------------------------------------------------------------------

#ifdef UNITTEST
    template <typename U>
    friend class ::TestAbstractIterators;
#endif // UNITTEST

    //--------------------------------------------------------------------------
    // members 
    //--------------------------------------------------------------------------

    /** 
     * Store the linear index of the current iterator and the end point of the
     * iterator.
     */
    index_type m_curr;
    index_type m_end;

    /**
     * Lg size of the dimensions of the bounds being iterated on
     */
    index_type m_lg_size;

    /**
     * offset of the (0,0,0) corner of the bounds for returning
     * correct coordinates
     */
    index_vec m_offset;

    /** 
     * The fill value the bounds is filled with.
     */
    value_type m_fill;
};

END_NKHIVE_NS

//-----------------------------------------------------------------------------
// class implementation
//-----------------------------------------------------------------------------

BEGIN_NKHIVE_NS

#include <nkhive/volume/FilledBoundsIterator.hpp>

END_NKHIVE_NS

#endif //__NKHIVE_VOLUME_FILLED_BOUNDS_ITERATOR_H__
