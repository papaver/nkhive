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
// BitOps.hpp
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// interface implementation
//------------------------------------------------------------------------------

inline index_type
numBits3D(index_type lg_size)
{
    return 1 << (3 * lg_size);
}

//------------------------------------------------------------------------------

inline index_type
getIndex(index_type i, index_type j, index_type k, 
         index_type w, index_type h, index_type d)
{
    assert((i < w) && (j < h) && (k < d));

    const index_type index = i + (j * w) + (k * (w * h));
    return index;
}
 
//------------------------------------------------------------------------------

inline index_type
getIndex(index_type i, index_type j, index_type k, index_type lg_size)
{
    const index_type row = 1 << lg_size;
    assert((i < row) && (j < row) && (k < row));

    // The row of a cube is 2^lg_size
    // A square face of the cube, is (2^lg_size)^2
    const index_type index = i + (j << lg_size) + (k << (lg_size << 1));
    return index;
}
 
//------------------------------------------------------------------------------

inline void
getCoordinates(index_type index, index_type lg_size, 
               index_type& i, index_type& j, index_type& k) 
{
    const index_type lg_row  = lg_size;
    const index_type lg_page = lg_size << 1;

    k     = index >> lg_page;
    index = moduloLg(index, lg_page);
    j     = index >> lg_row;
    i     = moduloLg(index, lg_row);
}

//------------------------------------------------------------------------------

inline bool
isPow2(index_type v) 
{
    bool is_pow2 = v && !(v & (v - 1));
    return is_pow2;
}

//------------------------------------------------------------------------------

inline index_type
roundPow2(index_type v, index_type pow2) 
{
    assert(isPow2(pow2));

    index_type round = (v + (pow2 - 1)) & ~(pow2 - 1);
    return round;
}

//------------------------------------------------------------------------------

inline index_type
moduloLg(index_type i, index_type lg_divisor)
{
    return i & ((1 << lg_divisor) - 1);
}

//------------------------------------------------------------------------------
