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
// FilledBoundsIterator.hpp
//------------------------------------------------------------------------------

// no includes allowed

//------------------------------------------------------------------------------

template <typename T>
inline 
FilledBoundsIterator<T>::FilledBoundsIterator(const index_bounds &bounds,
                                              const_reference v) : 
    m_curr(0),
    m_fill(v)
{
    // validate bounds
    assert(bounds.max().x > bounds.min().x);
    assert(bounds.max().y > bounds.min().y);
    assert(bounds.max().z > bounds.min().z);

    // compute the linear range.
    vec3ui diff = bounds.max() - bounds.min();
    m_end = diff.x * diff.y * diff.z;

    // set the offset of the lower corner for getCoordinates
    m_offset = bounds.min();

    // we assume a cube bounds object and power of 2 dims
    assert(diff.x == diff.y && diff.y == diff.z);
    assert(isPow2(diff.x));

    // store the lg of the dimension
    // getLastSetBitIndex is 1 based???
    m_lg_size = getLastSetBitIndex(diff.x) - 1;
}

//------------------------------------------------------------------------------

template <typename T>
inline 
FilledBoundsIterator<T>::~FilledBoundsIterator()
{
}

//------------------------------------------------------------------------------

template <typename T>
inline bool
FilledBoundsIterator<T>::atEnd() const
{
    return m_curr == m_end;
}

//------------------------------------------------------------------------------

template <typename T>
inline typename FilledBoundsIterator<T>::const_reference
FilledBoundsIterator<T>::value() const
{
    return m_fill;
}

//------------------------------------------------------------------------------

template <typename T>
inline void
FilledBoundsIterator<T>::next()
{
    ++m_curr;
}

//------------------------------------------------------------------------------

template <typename T>
inline void
FilledBoundsIterator<T>::getCoordinates(index_type &i,
                                        index_type &j,
                                        index_type &k) const
{
    NKHIVE_NS::getCoordinates(m_curr, m_lg_size, i, j, k);

    i += m_offset[0];
    j += m_offset[1];
    k += m_offset[2];
}

//------------------------------------------------------------------------------

template <typename T>
inline bool
FilledBoundsIterator<T>::isValueIterator() const
{
    return true;
}

//------------------------------------------------------------------------------
