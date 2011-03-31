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
// LinearInterpolation.hpp
//------------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// class implementation
//-----------------------------------------------------------------------------

template < typename T >
inline
LinearInterpolation<T>::LinearInterpolation() :
    m_volume()
{
}

//-----------------------------------------------------------------------------

template < typename T >
inline
LinearInterpolation<T>::LinearInterpolation(volume_ptr volume) :
    m_volume(volume)
{
}

//-----------------------------------------------------------------------------

template < typename T >
inline
LinearInterpolation<T>::LinearInterpolation(const LinearInterpolation &that) :
    m_volume(that.m_volume)
{
}

//-----------------------------------------------------------------------------

template < typename T >
inline void
LinearInterpolation<T>::interp(double x, double y, double z,
                               reference result) const
{
    vec3d voxel_coords(x, y, z);
    vec3i voxel_indices;

    // obtain indices from voxel coordinates
    m_volume->voxelToIndex(voxel_coords, voxel_indices);

    // get the index to voxel mapping offset
    vec3d kernel_offset = m_volume->kernelOffset();

    // get the voxelindices to interpolate from
    vec3i min_indices, max_indices;
    LinearSamplingUtil<T>::getIndexBounds(voxel_indices,
                                          min_indices, max_indices);

    // collect the 8 indices that will be used for the cells 
    // involved in the interpolation
    T interpolants[VOXEL_NEIGHBORS];
    LinearSamplingUtil<T>::collectInterpolants(min_indices, max_indices,
                                               m_volume, interpolants);
   
    // calc interpolation weights
    calc_type weights[VOXEL_NEIGHBORS];
    LinearSamplingUtil<T>::computeWeights(voxel_coords, kernel_offset, 
                                          min_indices, weights);

    // interpolate value
    calc_type interim_result(0);
    // this should be done with a dot product for large vectors
    for (i32 i = 0; i < VOXEL_NEIGHBORS; ++i) {
        interim_result += interpolants[i] * weights[i];
    }

    result = interim_result;
}

//------------------------------------------------------------------------------
