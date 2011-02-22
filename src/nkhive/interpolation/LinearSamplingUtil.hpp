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
// LinearSamplingUtil.hpp
//------------------------------------------------------------------------------

// no includes allowed

//------------------------------------------------------------------------------
// defines
//------------------------------------------------------------------------------

#define X_SIGN(i) i & 4
#define Y_SIGN(i) i & 2
#define Z_SIGN(i) i & 1

//------------------------------------------------------------------------------
// interface implementation
//------------------------------------------------------------------------------

template <typename T>
inline void
LinearSamplingUtil<T>::getIndexBounds(const vec3d &voxel_coords, 
                                      const vec3i &voxel_indices,
                                      vec3i &min_indices,
                                      vec3i &max_indices)
{
    // establish the index bounds
    for (i32 i = 0; i < 3; ++i) {
        if (round(voxel_coords[i]) > voxel_indices[i]) {
            min_indices[i] = voxel_indices[i];
            max_indices[i] = voxel_indices[i] + 1;
        } else {
            min_indices[i] = voxel_indices[i] - 1;
            max_indices[i] = voxel_indices[i];
        }
    }
}

//------------------------------------------------------------------------------

template < typename T >
inline void
LinearSamplingUtil<T>::collectInterpolants(const vec3i &min_indices, 
                                           const vec3i &max_indices,
                                           volume_ptr volume,
                                           T interpolants[VOXEL_NEIGHBORS])
{
    // fill interpolant array using bit indices
    // 000 (index 0) is -x, -y, -z voxel value
    // 111 (index 7) is +x, +y, +z voxel value 
    for (i32 i = 0; i < VOXEL_NEIGHBORS; ++i) {
        interpolants[i] = volume->get(
            X_SIGN(i) ? max_indices[0] : min_indices[0],
            Y_SIGN(i) ? max_indices[1] : min_indices[1],
            Z_SIGN(i) ? max_indices[2] : min_indices[2]);
    }

}

//------------------------------------------------------------------------------

template < typename T >
inline void
LinearSamplingUtil<T>::computeWeights(const vec3d &voxel_coords, 
                                      const vec3i &min_indices,
                                      const vec3i &,
                                      calc_type weights[VOXEL_NEIGHBORS])
{
    // compute the weights based on normalized voxels
    // TODO: implement vector component wise floor and do this math with vectors
    calc_vec_type w(voxel_coords[0] - (min_indices[0] + calc_type(0.5)),
                    voxel_coords[1] - (min_indices[1] + calc_type(0.5)),
                    voxel_coords[2] - (min_indices[2] + calc_type(0.5)));

    calc_vec_type w_1_minus(calc_type(1.0) - w[0],
                            calc_type(1.0) - w[1],
                            calc_type(1.0) - w[2]);

    // fill the weights array
    for (i32 i = 0; i < VOXEL_NEIGHBORS; ++i) {
        weights[i] = (X_SIGN(i) ? w[0] : w_1_minus[0]) *
                     (Y_SIGN(i) ? w[1] : w_1_minus[1]) *
                     (Z_SIGN(i) ? w[2] : w_1_minus[2]);
    }
}

//------------------------------------------------------------------------------

template <typename T>
template<template <typename> class SetPolicy>
inline void
LinearSamplingUtil<T>::updateValues(const vec3i &min_indices, 
                                    const vec3i &max_indices,
                                    calc_type weights[VOXEL_NEIGHBORS],
                                    volume_ptr volume,
                                    const_reference val,
                                    SetPolicy<T> set_op)
{
    // update the volume with val, weighting it by the
    // given weights
    for (i32 i = 0; i < VOXEL_NEIGHBORS; ++i) {
        volume->update(X_SIGN(i) ? max_indices[0] : min_indices[0], 
                       Y_SIGN(i) ? max_indices[1] : min_indices[1], 
                       Z_SIGN(i) ? max_indices[2] : min_indices[2], 
                       val * weights[i], set_op);
    }
}

//------------------------------------------------------------------------------
// macro cleanup
//------------------------------------------------------------------------------

#undef X_SIGN
#undef Y_SIGN
#undef Z_SIGN

//------------------------------------------------------------------------------
