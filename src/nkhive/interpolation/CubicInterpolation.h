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
// CubicInterpolation.h
//------------------------------------------------------------------------------

#ifndef __NKHIVE_INTERPOLATION_CUBIC_INTERPOLATION_H__
#define __NKHIVE_INTERPOLATION_CUBIC_INTERPOLATION_H__

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------

#include <nkhive/volume/Volume.h>
#include <nkhive/Types.h>

//-----------------------------------------------------------------------------
// forward declarations
//-----------------------------------------------------------------------------

BEGIN_NKHIVE_NS

END_NKHIVE_NS

//-----------------------------------------------------------------------------
// class definition
//-----------------------------------------------------------------------------

BEGIN_NKHIVE_NS

template < typename T >
class CubicInterpolation
{
public:

    //-------------------------------------------------------------------------
    // typedefs
    //-------------------------------------------------------------------------

    typedef T                                      value_type;
    typedef T&                                     reference;
    typedef const T&                               const_reference;

    typedef typename CalcType<T>::calc_type        calc_type;
    typedef typename CalcType<T>::calc_vec_type    calc_vec_type;

    typedef typename Volume<T>::shared_ptr         volume_ptr;
    
    //-------------------------------------------------------------------------
    // public interface
    //-------------------------------------------------------------------------

    /**
     * default constructor
     */
    CubicInterpolation();

    /**
     * parameterized constructor
     */ 
    CubicInterpolation(volume_ptr volume);

    /**
     * copy constructor
     */
    CubicInterpolation(const CubicInterpolation &that);

    /**
     * evaluate at a point
     */
    void interp(double x, double y, double z, 
                reference result) const;

private:

    //-------------------------------------------------------------------------
    // internal methods
    //-------------------------------------------------------------------------
  
    /**
     * get the indices of voxels bounding the input point
     */
    void getIndexBounds(const vec3d &voxel_coords, const vec3i &voxel_indices,
                        vec3i &min_indices, vec3i &max_indices) const;

    /**
     * collect interpolant values from 1-ring neighbourhood of sample point
     */
    void collectInterpolants(const vec3i &min_indices, 
                             const vec3i &max_indices, 
                             calc_type interpolants[64]) const;

    /**
     * cubic interpolation in 1 dimension
     */
    void interpolate1d(const calc_type interpolants[4],
                       const calc_type &t,
                       calc_type &result) const;

    /**
     * bicubic interpolation
     */
    void interpolate2d(const calc_type interpolants[16],
                       const calc_type coords[2],
                       calc_type &result) const;

    /**
     * tricubic interpolation
     */
    void interpolate3d(const calc_type interpolants[64],
                       const calc_type coords[3],
                       calc_type &result) const;

    /**
     * cubic interpolation in 1 dimension using catmull-rom
     */
    void interp_catmull_rom(const calc_type interpolants[4],
                            const calc_type &t,
                            calc_type &result) const;

    /**
     * cubic interpolation in 1 dimension using cubic hermite
     */
    void interp_hermite(const calc_type interpolants[4],
                        const calc_type &t,
                        calc_type &result) const;

    //-------------------------------------------------------------------------
    // members
    //-------------------------------------------------------------------------
   
    volume_ptr m_volume; 

    //-------------------------------------------------------------------------
    // typedefs
    //-------------------------------------------------------------------------

#ifdef UNITTEST
    template<typename U>
    friend class TestInterpolation;
#endif // UNITTEST

};

END_NKHIVE_NS

//-----------------------------------------------------------------------------
// class implementation
//-----------------------------------------------------------------------------

BEGIN_NKHIVE_NS

#include <nkhive/interpolation/CubicInterpolation.hpp>

END_NKHIVE_NS

#endif //__NKHIVE_INTERPOLATION_CUBIC_INTERPOLATION_H__
