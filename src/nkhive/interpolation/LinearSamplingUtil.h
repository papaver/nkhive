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
// LinearSamplingUtil.h
//------------------------------------------------------------------------------

#ifndef __NKHIVE_INTERPOLATION_LINEARSAMPLINGUTIL_H__
#define __NKHIVE_INTERPOLATION_LINEARSAMPLINGUTIL_H__

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
 * Defines helper functions for doing linear sampling operations, e.g.,
 * interpolation and splatting.
 */
template <typename T>
class LinearSamplingUtil
{

public:

    //--------------------------------------------------------------------------
    // typedefs
    //--------------------------------------------------------------------------

    typedef T                                      value_type;
    typedef T&                                     reference;
    typedef const T&                               const_reference;

    typedef typename CalcType<T>::calc_type        calc_type;
    typedef typename CalcType<T>::calc_vec_type    calc_vec_type;

    typedef typename Volume<T>::shared_ptr         volume_ptr;

    //--------------------------------------------------------------------------
    // public interface
    //--------------------------------------------------------------------------

    /** 
     * Get the indices for the voxels bounding the input point. 
     */
    static void getIndexBounds(const vec3d &voxel_coords, 
                               const vec3i &voxel_indices, 
                               vec3i &min_indices, vec3i &max_indices);

    /** 
     * Collect interpolant values from 1-ring neighborhood of sample input. 
     */
    static void collectInterpolants(const vec3i &min_indices, 
                                    const vec3i &max_indices, 
                                    volume_ptr volume,
                                    T interpolants[VOXEL_NEIGHBORS]);

    /**
     * Compute the weights for the interpolation. 
     */
    static void computeWeights(const vec3d &voxel_coords, 
                               const vec3i &min_indices,
                               const vec3i &max_indices, 
                               calc_type weights[VOXEL_NEIGHBORS]);

    /**
     * Update the volume with value splatted across interpolants
     */
    template < template <typename> class SetPolicy >
    static void updateValues(const vec3i &min_indices, 
                             const vec3i &max_indices, 
                             calc_type weights[VOXEL_NEIGHBORS],
                             volume_ptr volume,
                             const_reference val,
                             SetPolicy<T> set_op);


private:

    //--------------------------------------------------------------------------
    // internal typedefs.
    //--------------------------------------------------------------------------

    //--------------------------------------------------------------------------
    // internal methods.
    //--------------------------------------------------------------------------

    //--------------------------------------------------------------------------
    // friends.
    //--------------------------------------------------------------------------

    //--------------------------------------------------------------------------
    // members.
    //--------------------------------------------------------------------------
};

END_NKHIVE_NS

//-----------------------------------------------------------------------------
// class implementation
//-----------------------------------------------------------------------------

BEGIN_NKHIVE_NS

#include <nkhive/interpolation/LinearSamplingUtil.hpp>

END_NKHIVE_NS

//------------------------------------------------------------------------------

#endif // __NKHIVE_INTERPOLATION_LINEARSAMPLINGUTIL_H__
