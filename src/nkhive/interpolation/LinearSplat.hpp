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
// LinearSplat.hpp
//------------------------------------------------------------------------------

// no includes allowed

//------------------------------------------------------------------------------

template <typename T, template<typename> class SetPolicy>
inline
LinearSplat<T, SetPolicy>::LinearSplat(volume_ptr v) :
    m_volume(v)
{
}

//------------------------------------------------------------------------------

template <typename T, template<typename> class SetPolicy>
inline
LinearSplat<T, SetPolicy>::~LinearSplat()
{
}

//------------------------------------------------------------------------------

template <typename T, template<typename> class SetPolicy>
inline void
LinearSplat<T, SetPolicy>::splat(double x, double y, double z, 
                                 const_reference val) const
{
    vec3d voxel_coords(x, y, z);
    vec3i voxel_indices;

    // obtain indices from voxel coordinates
    m_volume->voxelToIndex(voxel_coords, voxel_indices);

    // get the voxelindices to interpolate from
    vec3i min_indices, max_indices;
    LinearSamplingUtil<T>::getIndexBounds(voxel_coords, voxel_indices,
                                          min_indices, max_indices);

    // calc interpolation weights
    calc_type weights[VOXEL_NEIGHBORS];
    LinearSamplingUtil<T>::computeWeights(voxel_coords, min_indices,
                                          max_indices, weights);

    // splat the value back into the volume weighting it by the
    // interpolated weights
    SetPolicy<T> set_op;
    LinearSamplingUtil<T>::updateValues(min_indices, max_indices,
                                        weights, m_volume, val, set_op);
}

//------------------------------------------------------------------------------
