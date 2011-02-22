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
// BoundingBox.h
//------------------------------------------------------------------------------

#ifndef __NKHIVE_UTIL_BOUNDINGBOX_H__
#define __NKHIVE_UTIL_BOUNDINGBOX_H__

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------

#include <nkhive/util/Bounds3D.h>

//------------------------------------------------------------------------------
// class declaration
//------------------------------------------------------------------------------

BEGIN_NKHIVE_NS

/**
 * This conceptually seperates Bounds3D by initializing the inverse infinity
 * allowing the construction of a bounds by adding data to it.
 */
template <typename T>
class BoundingBox : public Bounds3D<T>
{

public:

    //--------------------------------------------------------------------------
    // typedefs
    //--------------------------------------------------------------------------

    typedef typename Bounds3D<T>::value_type             value_type;
    typedef typename Bounds3D<T>::vector_type            vector_type;
    typedef typename Bounds3D<T>::vector_reference       vector_reference;
    typedef typename Bounds3D<T>::const_vector_reference const_vector_reference;

    //--------------------------------------------------------------------------
    // public interface
    //--------------------------------------------------------------------------

    /** 
     * Constructors. Default constructor creates bounds [0,1].
     */
    BoundingBox();
    BoundingBox(value_type min, value_type max);
    BoundingBox(const_vector_reference min, const_vector_reference max);

    /**
     * Returns true if value is within the bounds.
     */
    bool inRange(const_vector_reference value);

    /**
     * Returns the center of the bounding box.
     */
     vector_type center();

    /**
     * Negetive area indicates an invalid bounding box.
     */
     bool isValid();

};

END_NKHIVE_NS

//-----------------------------------------------------------------------------
// class implementation
//-----------------------------------------------------------------------------

BEGIN_NKHIVE_NS

#include <nkhive/util/BoundingBox.hpp>

END_NKHIVE_NS

//------------------------------------------------------------------------------

#endif // __NKHIVE_UTIL_BOUNDINGBOX_H__
