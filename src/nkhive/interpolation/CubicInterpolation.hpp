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
// CubicInterpolation.hpp
//------------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// definitions
//-----------------------------------------------------------------------------

#define GET_INDEX_2D(i, j) j*4 + i

//-----------------------------------------------------------------------------
// class implementation
//-----------------------------------------------------------------------------

template < typename T >
inline
CubicInterpolation<T>::CubicInterpolation() :
    m_volume()
{
}

//-----------------------------------------------------------------------------

template < typename T >
inline
CubicInterpolation<T>::CubicInterpolation(volume_ptr volume) :
    m_volume(volume)
{
}

//-----------------------------------------------------------------------------

template < typename T >
inline
CubicInterpolation<T>::CubicInterpolation(const CubicInterpolation &that) :
    m_volume(that.m_volume)
{
}

//-----------------------------------------------------------------------------

template < typename T >
inline void
CubicInterpolation<T>::interp(double x, double y, double z, 
                              reference result) const
{
    vec3d local_coords(x, y, z);
    vec3d voxel_coords;
    vec3i voxel_indices;

    // got from local coordinate system to voxel coordinate system
    m_volume->localToVoxel(local_coords, voxel_coords);

    // obtain indices from voxel coordinates
    m_volume->voxelToIndex(voxel_coords, voxel_indices);

    // get the voxelindices to interpolate from
    vec3i min_indices, max_indices;
    getIndexBounds(voxel_coords, voxel_indices, min_indices, max_indices);

    // collect the 64 indices that will be used for the cells 
    // involved in the interpolation
    calc_type interpolants[64];
    collectInterpolants(min_indices, max_indices, interpolants);
  
    // transform to parameterized coordinates
    // for each axis: 
    //      x:    0.0       1.0       2.0       3.0       4.0
    // values:          0         1         2         3
    //             |----*----|----*----|----*----|----*----|
    //                            ^         ^
    //  param:                   t=0       t=1
    calc_type norm_coords[3];
    for (i32 i = 0; i < 3; ++i) {
        norm_coords[i] = (voxel_coords[i] - (min_indices[i] + 1.5))/
                            (max_indices[i] - min_indices[i] - 2.0);
    }
       
    calc_type interp_result; 
    interpolate3d(interpolants, norm_coords, interp_result);
    result = T(interp_result);
}

//-----------------------------------------------------------------------------
// internal methods
//-----------------------------------------------------------------------------

template < typename T >
inline void
CubicInterpolation<T>::getIndexBounds(const vec3d &voxel_coords, 
                                      const vec3i &voxel_indices,
                                      vec3i &min_indices,
                                      vec3i &max_indices) const
{
    // establish the index bounds
    for (i32 i = 0; i < 3; ++i) {
        if (round(voxel_coords[i]) > voxel_indices[i]) {
            min_indices[i] = voxel_indices[i] - 1;
            max_indices[i] = voxel_indices[i] + 2;
        } else {
            min_indices[i] = voxel_indices[i] - 2;
            max_indices[i] = voxel_indices[i] + 1;
        }
    }
}

//-----------------------------------------------------------------------------

template < typename T >
inline void
CubicInterpolation<T>::collectInterpolants(const vec3i &min_indices, 
                                           const vec3i &,
                                           calc_type interpolants[64]) const
{
    // fill 64 interpolate values from a 4x4x4 subvolume
    for (i32 i = 0; i < 64; ++i) {
        index_type v_i, v_j, v_k;
        getCoordinates(i, 2, v_i, v_j, v_k);

        interpolants[i] = m_volume->get(v_i + min_indices[0], 
                                        v_j + min_indices[1], 
                                        v_k + min_indices[2]); 
    }
}

//-----------------------------------------------------------------------------

template < typename T >
inline void
CubicInterpolation<T>::interpolate1d(const calc_type interpolants[4], 
                                     const calc_type &t, 
                                     calc_type &result) const
{
    interp_hermite(interpolants, t, result);
}

//-----------------------------------------------------------------------------

template < typename T >
inline void
CubicInterpolation<T>::interpolate2d(const calc_type interpolants[16], 
                                     const calc_type coord[2], 
                                     calc_type &result) const
{
    calc_type interpolants_x[4];

    for (i32 x_i = 0; x_i < 4; ++x_i) {
        calc_type interpolants_y[4];
        for (i32 y_i = 0; y_i < 4; ++y_i) {
            interpolants_y[y_i] = interpolants[GET_INDEX_2D(x_i,y_i)];
        }

        // interpolate row
        interpolate1d(interpolants_y, coord[1], interpolants_x[x_i]);
    }

    interpolate1d(interpolants_x, coord[0], result);
}

//-----------------------------------------------------------------------------

template < typename T >
inline void
CubicInterpolation<T>::interpolate3d(const calc_type interpolants[64], 
                                     const calc_type coord[3], 
                                     calc_type &result) const
{
    // log size of this 4x4x4 interpolation cube is 2
    index_type lg_size = 2;

    calc_type interpolants_1d[4];

    // first perform 4 bicubic interpolations on xy planes
    // arranged this way for cache friendliness
    for (i32 z_i = 0; z_i < 4; ++z_i) {
        calc_type interpolants_2d[16];
        for (i32 y_i = 0; y_i < 4; ++y_i) {
            for (i32 x_i = 0; x_i < 4; ++x_i) {
                interpolants_2d[GET_INDEX_2D(x_i,y_i)] = 
                                interpolants[getIndex(x_i,y_i,z_i,lg_size)];
             }
        }

        calc_type coord_2d[2];
        coord_2d[0] = coord[0];
        coord_2d[1] = coord[1];
        interpolate2d(interpolants_2d, coord_2d, interpolants_1d[z_i]);
    }

    // now interpolate across z axis
    interpolate1d(interpolants_1d, coord[2], result);
}     

//-----------------------------------------------------------------------------

template < typename T >
inline void
CubicInterpolation<T>::interp_catmull_rom(const calc_type interpolants[4], 
                                          const calc_type &t, 
                                          calc_type &result) const
{
    double c = (2 * interpolants[1]);
    double c1 = (-1*interpolants[0] + interpolants[2]);
    double c2 = (2*interpolants[0] - 5*interpolants[1] + 
                 4*interpolants[2] - interpolants[3]);
    double c3 = (-1 * interpolants[0] + 3 * interpolants[1] - 
                 3 * interpolants[2] + interpolants[3]);
    // catmull rom cubic spline interpolation
    result = 0.5 * ( c + t * (c1 + t * (c2 + t * c3)));
}

//-----------------------------------------------------------------------------

template < typename T >
inline void
CubicInterpolation<T>::interp_hermite(const calc_type interpolants[4], 
                                      const calc_type &t, 
                                      calc_type &result) const
{
    // calculate tangents
    // assuming t is from interpolant[1] to interpolant[2]
    calc_type m0 = 0.5 * ((interpolants[2] - interpolants[1]) + 
                          (interpolants[1] - interpolants[0]));
    calc_type m1 = 0.5 * ((interpolants[3] - interpolants[2]) + 
                          (interpolants[2] - interpolants[1]));
    
    calc_type t3 = t * t * t;
    calc_type t2 = t * t;

    result = ((2.0 * t3) - (3.0 * t2) + 1.0) * interpolants[1] + 
             (t3 - (2.0 * t2) + t) * m0 + 
             ((3.0 * t2) - (2.0 * t3)) * interpolants[2] +
             (t3 - t2) * m1;
}

//------------------------------------------------------------------------------
