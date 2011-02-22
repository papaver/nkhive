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
// Bounds3D.h
//------------------------------------------------------------------------------

#ifndef __NKHIVE_UTIL_BOUNDS3D_H__
#define __NKHIVE_UTIL_BOUNDS3D_H__

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------

#include <cassert>
#include <limits>
#include <string>
#include <sstream>
#include <nkhive/Defs.h>
#include <nkhive/Types.h>

//------------------------------------------------------------------------------
// class declaration
//------------------------------------------------------------------------------

BEGIN_NKHIVE_NS

/**
 * Defines a generic container for holding a bounding box. Essentially a
 * storage for the lower-left-hand-corner and the upper-right-hand-corner of
 * a bounding box.
 */
template <typename T>
class Bounds3D
{

public:

    //--------------------------------------------------------------------------
    // typedefs
    //--------------------------------------------------------------------------

    typedef T                                   value_type;
    typedef typename Vec3Type<T>::type          vector_type;
    typedef vector_type&                        vector_reference;
    typedef const vector_type&                  const_vector_reference;
    typedef std::numeric_limits<value_type>     limits;

    //--------------------------------------------------------------------------
    // enums
    //--------------------------------------------------------------------------
    
    enum Corners
    {
        FTR = 0,  //  1,  1,  1 -> 000 ->  0
        NTR = 1,  //  1,  1, -1 -> 001 ->  1
        FBR = 2,  //  1, -1,  1 -> 010 ->  2
        NBR = 3,  //  1, -1, -1 -> 011 ->  3 
        FTL = 4,  // -1,  1,  1 -> 100 ->  4 
        NTL = 5,  // -1,  1, -1 -> 101 ->  5 
        FBL = 6,  // -1, -1,  1 -> 110 ->  6 
        NBL = 7,  // -1, -1, -1 -> 111 ->  7 
    };

    enum { CORNERS = 8 };

    //--------------------------------------------------------------------------
    // public interface
    //--------------------------------------------------------------------------

    /** 
     * Constructors. Default constructor creates bounds [0, 1].
     */
    Bounds3D();
    Bounds3D(value_type min, value_type max);
    Bounds3D(const_vector_reference min, const_vector_reference max);
    ~Bounds3D();

    /**
     * Getter methods.
     */
    vector_reference min();
    vector_reference max();

    const_vector_reference min() const; 
    const_vector_reference max() const;

    vector_type nbl() const;  // near bottom left  (min)
    vector_type nbr() const;  // near bottom right
    vector_type ntr() const;  // near top    right
    vector_type ntl() const;  // near top    left
    vector_type ftl() const;  // far  top    left
    vector_type ftr() const;  // far  top    right (max)
    vector_type fbr() const;  // far  bottom right
    vector_type fbl() const;  // far  bottom left

    /**
     * Convenience function for calling the appropriate get function 
     * given the corner enum.
     */
    vector_type get(u8 corner) const;   

    /**
     * Setter methods.
     */
    void setNBL(const_vector_reference nbl);  
    void setNBR(const_vector_reference nbr); 
    void setNTR(const_vector_reference ntr); 
    void setNTL(const_vector_reference ntl); 
    void setFTL(const_vector_reference ftl); 
    void setFTR(const_vector_reference ftr); 
    void setFBR(const_vector_reference fbr); 
    void setFBL(const_vector_reference fbl); 

    /**
     * Convenience function for calling the appropriate set function 
     * given the corner enum.
     */
    void set(const_vector_reference v, u8 corner);  

    /**
     * Clamps the given corner by the point.
     */
    void clampNBL(const_vector_reference nbl);  
    void clampNBR(const_vector_reference nbr); 
    void clampNTR(const_vector_reference ntr); 
    void clampNTL(const_vector_reference ntl); 
    void clampFTL(const_vector_reference ftl); 
    void clampFTR(const_vector_reference ftr); 
    void clampFBR(const_vector_reference fbr); 
    void clampFBL(const_vector_reference fbl); 

    /**
     * Convenience function for calling the appropriate clamp function 
     * given the corner enum.
     */
    void clamp(const_vector_reference v, u8 corner);  

    /**
     * Set the bounds to encapsulate the given points.
     */
    void setExtrema(const_vector_reference a, const_vector_reference b);

    /**
     * Given the indices i,j,k update the bounds to the extrema of the current 
     * min and max values of the bounds. If the i,j,k are less than min, the
     * new min will be set to i,j,k. If the i,j,k are greater than max, the 
     * new max will be set to i,j,k.
     */
    void updateExtrema(value_type i, value_type j, value_type k);
    void updateExtrema(const_vector_reference v);

    /**
     * Update the bounds to the extrema relative to the given bounds.
     */
    void updateExtrema(const Bounds3D<T> &bounds);

    /**
     * Translate the two corners of the bounding box with the given vector.
     */
    void translate(const_vector_reference offset);

    /**
     * Translate the max corner by the given vector.
     */
    void translateMax(const_vector_reference offset);

    /**
     * Translate the min corner by the given vector.
     */
    void translateMin(const_vector_reference offset);

    /**
     * Returns true if the two bounds intersect.
     */
    bool intersects(Bounds3D<T> &that) const;

    /**
     * Returns true this bounds contains that bounds.
     */
    bool contains(Bounds3D<T> &that) const;

    /**
     * Returns the intersection of the two bounds.
     */
    Bounds3D<T> intersection(Bounds3D<T> &that) const;

    /**
     * Returns true if value is within the bounds.
     */
    bool inRange(const_vector_reference value);

    /**
     * Swaps the contents of the bounds.
     */
    void swap(Bounds3D &that);

    /**
     * Returns string representation of bounds.
     */
    std::string toString() const;

protected: 

    //--------------------------------------------------------------------------
    // members.
    //--------------------------------------------------------------------------

    vector_type m_min;
    vector_type m_max;

};

END_NKHIVE_NS

//-----------------------------------------------------------------------------
// class implementation
//-----------------------------------------------------------------------------

BEGIN_NKHIVE_NS

#include <nkhive/util/Bounds3D.hpp>

END_NKHIVE_NS

//------------------------------------------------------------------------------

#endif // __NKHIVE_UTIL_BOUNDS3D_H__
