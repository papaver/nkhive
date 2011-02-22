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
// Bounds3D.hpp
//------------------------------------------------------------------------------

// no includes allowed

//------------------------------------------------------------------------------

template <typename T>
inline
Bounds3D<T>::Bounds3D() :
    m_min(0), 
    m_max(1)
{
}

//------------------------------------------------------------------------------

template <typename T>
inline
Bounds3D<T>::Bounds3D(value_type min, value_type max) :
    m_min(min), 
    m_max(max)
{
}

//------------------------------------------------------------------------------

template <typename T>
inline
Bounds3D<T>::Bounds3D(const_vector_reference min, const_vector_reference max) :
    m_min(min), 
    m_max(max)
{
}

//------------------------------------------------------------------------------

template <typename T>
inline
Bounds3D<T>::~Bounds3D()
{
}

//------------------------------------------------------------------------------

template <typename T>
inline typename Bounds3D<T>::const_vector_reference
Bounds3D<T>::min() const
{
    return m_min;
}

//------------------------------------------------------------------------------

template <typename T>
inline typename Bounds3D<T>::const_vector_reference
Bounds3D<T>::max() const
{
    return m_max;
}

//------------------------------------------------------------------------------

template <typename T>
inline typename Bounds3D<T>::vector_reference
Bounds3D<T>::min()
{
    return m_min;
}

//------------------------------------------------------------------------------

template <typename T>
inline typename Bounds3D<T>::vector_reference
Bounds3D<T>::max()
{
    return m_max;
}

//------------------------------------------------------------------------------

template <typename T>
inline typename Bounds3D<T>::vector_type
Bounds3D<T>::nbl() const
{
    // near bottom left (min)
    return min();
}

//------------------------------------------------------------------------------

template <typename T>
inline typename Bounds3D<T>::vector_type
Bounds3D<T>::nbr() const
{
    // near bottom right
    return vector_type(max().x, min().y, min().z);
}

//------------------------------------------------------------------------------

template <typename T>
inline typename Bounds3D<T>::vector_type
Bounds3D<T>::ntr() const
{
    // near top right
    return vector_type(max().x, max().y, min().z);
}

//------------------------------------------------------------------------------

template <typename T>
inline typename Bounds3D<T>::vector_type
Bounds3D<T>::ntl() const
{
    // near top left
    return vector_type(min().x, max().y, min().z);
}

//------------------------------------------------------------------------------

template <typename T>
inline typename Bounds3D<T>::vector_type
Bounds3D<T>::ftl() const
{
    // far top left
    return vector_type(min().x, max().y, max().z);
}

//------------------------------------------------------------------------------

template <typename T>
inline typename Bounds3D<T>::vector_type
Bounds3D<T>::ftr() const
{
    // far top right (max)
    return max();
}

//------------------------------------------------------------------------------

template <typename T>
inline typename Bounds3D<T>::vector_type
Bounds3D<T>::fbr() const
{
    // far bottom right
    return vector_type(max().x, min().y, max().z);
}

//------------------------------------------------------------------------------

template <typename T>
inline typename Bounds3D<T>::vector_type
Bounds3D<T>::fbl() const
{
    // far bottom left
    return vector_type(min().x, min().y, max().z);
}

//------------------------------------------------------------------------------

template <typename T>
inline typename Bounds3D<T>::vector_type 
Bounds3D<T>::get(u8 corner) const
{
    typedef vector_type (Bounds3D<T>::*get_func)() const;

    // jump table mapping each corner to its get function
    static get_func s_get_callbacks[CORNERS] = {
        &Bounds3D<T>::ftr,
        &Bounds3D<T>::ntr,
        &Bounds3D<T>::fbr,
        &Bounds3D<T>::nbr,
        &Bounds3D<T>::ftl,
        &Bounds3D<T>::ntl,
        &Bounds3D<T>::fbl,
        &Bounds3D<T>::nbl,
    };

    // validate corner
    assert(corner <= CORNERS);

    // call get for requested corner
    return (this->*s_get_callbacks[corner])();
}

//------------------------------------------------------------------------------

template <typename T>
inline void 
Bounds3D<T>::setNBL(const_vector_reference nbl)
{
    min() = nbl;
}

//------------------------------------------------------------------------------

template <typename T>
inline void 
Bounds3D<T>::setNBR(const_vector_reference nbr)
{
    max().x = nbr.x; min().y = nbr.y; min().z = nbr.z;
}

//------------------------------------------------------------------------------

template <typename T>
inline void 
Bounds3D<T>::setNTR(const_vector_reference ntr)
{
    max().x = ntr.x; max().y = ntr.y; min().z = ntr.z;
}

//------------------------------------------------------------------------------

template <typename T>
inline void 
Bounds3D<T>::setNTL(const_vector_reference ntl)
{
    min().x = ntl.x; max().y = ntl.y; min().z = ntl.z;
}

//------------------------------------------------------------------------------

template <typename T>
inline void 
Bounds3D<T>::setFTL(const_vector_reference ftl)
{
    min().x = ftl.x; max().y = ftl.y; max().z = ftl.z;
}

//------------------------------------------------------------------------------

template <typename T>
inline void 
Bounds3D<T>::setFTR(const_vector_reference ftr)
{
    max() = ftr;
}

//------------------------------------------------------------------------------

template <typename T>
inline void 
Bounds3D<T>::setFBR(const_vector_reference fbr)
{
    max().x = fbr.x; min().y = fbr.y; max().z = fbr.z;
}

//------------------------------------------------------------------------------

template <typename T>
inline void 
Bounds3D<T>::setFBL(const_vector_reference fbl)
{
    min().x = fbl.x; min().y = fbl.y; max().z = fbl.z;
}

//------------------------------------------------------------------------------

template <typename T>
inline void 
Bounds3D<T>::set(const_vector_reference v, u8 corner)
{
    typedef void (Bounds3D<T>::*set_func)(const_vector_reference);

    // jump table mapping each corner to its set function
    static set_func s_set_callbacks[CORNERS] = {
        &Bounds3D<T>::setFTR,
        &Bounds3D<T>::setNTR,
        &Bounds3D<T>::setFBR,
        &Bounds3D<T>::setNBR,
        &Bounds3D<T>::setFTL,
        &Bounds3D<T>::setNTL,
        &Bounds3D<T>::setFBL,
        &Bounds3D<T>::setNBL,
    };

    // validate corner
    assert(corner <= CORNERS);

    // call set for requested corner
    (this->*s_set_callbacks[corner])(v);
}

//------------------------------------------------------------------------------

template <typename T>
inline void 
Bounds3D<T>::clampNBL(const_vector_reference nbl)
{
    min().x = std::max(min().x, nbl.x);
    min().y = std::max(min().y, nbl.y);
    min().z = std::max(min().z, nbl.z);
}

//------------------------------------------------------------------------------

template <typename T>
inline void 
Bounds3D<T>::clampNBR(const_vector_reference nbr)
{
    max().x = std::min(max().x, nbr.x); 
    min().y = std::max(min().y, nbr.y); 
    min().z = std::max(min().z, nbr.z);
}

//------------------------------------------------------------------------------

template <typename T>
inline void 
Bounds3D<T>::clampNTR(const_vector_reference ntr)
{
    max().x = std::min(max().x, ntr.x); 
    max().y = std::min(max().y, ntr.y); 
    min().z = std::max(min().z, ntr.z);
}

//------------------------------------------------------------------------------

template <typename T>
inline void 
Bounds3D<T>::clampNTL(const_vector_reference ntl)
{
    min().x = std::max(min().x, ntl.x); 
    max().y = std::min(max().y, ntl.y); 
    min().z = std::max(min().z, ntl.z);
}

//------------------------------------------------------------------------------

template <typename T>
inline void 
Bounds3D<T>::clampFTL(const_vector_reference ftl)
{
    min().x = std::max(min().x, ftl.x); 
    max().y = std::min(max().y, ftl.y); 
    max().z = std::min(max().z, ftl.z);
}

//------------------------------------------------------------------------------

template <typename T>
inline void 
Bounds3D<T>::clampFTR(const_vector_reference ftr)
{
    max().x = std::min(max().x, ftr.x);
    max().y = std::min(max().y, ftr.y);
    max().z = std::min(max().z, ftr.z);
}

//------------------------------------------------------------------------------

template <typename T>
inline void 
Bounds3D<T>::clampFBR(const_vector_reference fbr)
{
    max().x = std::min(max().x, fbr.x); 
    min().y = std::max(min().y, fbr.y); 
    max().z = std::min(max().z, fbr.z);
}

//------------------------------------------------------------------------------

template <typename T>
inline void 
Bounds3D<T>::clampFBL(const_vector_reference fbl)
{
    min().x = std::max(min().x, fbl.x); 
    min().y = std::max(min().y, fbl.y); 
    max().z = std::min(max().z, fbl.z);
}

//------------------------------------------------------------------------------

template <typename T>
inline void 
Bounds3D<T>::clamp(const_vector_reference v, u8 corner)
{
    typedef void (Bounds3D<T>::*clamp_func)(const_vector_reference);

    // jump table mapping each corner to its clamp function
    static clamp_func s_clamp_callbacks[CORNERS] = {
        &Bounds3D<T>::clampFTR,
        &Bounds3D<T>::clampNTR,
        &Bounds3D<T>::clampFBR,
        &Bounds3D<T>::clampNBR,
        &Bounds3D<T>::clampFTL,
        &Bounds3D<T>::clampNTL,
        &Bounds3D<T>::clampFBL,
        &Bounds3D<T>::clampNBL,
    };

    // validate corner
    assert(corner <= CORNERS);

    // call clamp for requested corner
    (this->*s_clamp_callbacks[corner])(v);
}

//------------------------------------------------------------------------------

template <typename T>
inline void
Bounds3D<T>::setExtrema(const_vector_reference a, const_vector_reference b)
{
    min().x = std::min(a.x, b.x);
    min().y = std::min(a.y, b.y);
    min().z = std::min(a.z, b.z);
    max().x = std::max(a.x, b.x);
    max().y = std::max(a.y, b.y);
    max().z = std::max(a.z, b.z);
}

//------------------------------------------------------------------------------

template <typename T>
inline void
Bounds3D<T>::updateExtrema(value_type i, value_type j, value_type k)
{
    if (i < m_min.x) m_min.x = i;
    if (j < m_min.y) m_min.y = j;
    if (k < m_min.z) m_min.z = k;
    if (i > m_max.x) m_max.x = i;
    if (j > m_max.y) m_max.y = j;
    if (k > m_max.z) m_max.z = k;
}

//------------------------------------------------------------------------------

template <typename T>
inline void
Bounds3D<T>::updateExtrema(const_vector_reference v)
{
    updateExtrema(v.x, v.y, v.z);
}

//------------------------------------------------------------------------------

template <typename T>
inline void
Bounds3D<T>::updateExtrema(const Bounds3D<T> &bounds)
{
    if (bounds.min().x < m_min.x) m_min.x = bounds.min().x;
    if (bounds.min().y < m_min.y) m_min.y = bounds.min().y; 
    if (bounds.min().z < m_min.z) m_min.z = bounds.min().z;
    if (bounds.max().x > m_max.x) m_max.x = bounds.max().x;
    if (bounds.max().y > m_max.y) m_max.y = bounds.max().y;
    if (bounds.max().z > m_max.z) m_max.z = bounds.max().z;
}

//------------------------------------------------------------------------------

template <typename T>
inline void
Bounds3D<T>::translate(const_vector_reference offset)
{
    translateMin(offset);
    translateMax(offset);
}

//------------------------------------------------------------------------------

template <typename T>
inline void
Bounds3D<T>::translateMax(const_vector_reference offset)
{
    m_max += offset;
}

//------------------------------------------------------------------------------

template <typename T>
inline void
Bounds3D<T>::translateMin(const_vector_reference offset)
{
    m_min += offset;
}

//-----------------------------------------------------------------------------

template <typename T>
inline bool
Bounds3D<T>::intersects(Bounds3D<T> &that) const
{
    bool result = (min().x < that.max().x) && (that.min().x < max().x) && 
                  (min().y < that.max().y) && (that.min().y < max().y) && 
                  (min().z < that.max().z) && (that.min().z < max().z);
    return result;
}

//-----------------------------------------------------------------------------

template <typename T>
inline bool
Bounds3D<T>::contains(Bounds3D<T> &that) const
{
    bool result = (min().x <= that.min().x) && (that.max().x <= max().x) && 
                  (min().y <= that.min().y) && (that.max().y <= max().y) && 
                  (min().z <= that.min().z) && (that.max().z <= max().z);
    return result;
}

//-----------------------------------------------------------------------------

template <typename T>
inline Bounds3D<T> 
Bounds3D<T>::intersection(Bounds3D<T> &that) const
{
    Bounds3D<T> bounds;
    bounds.min().x = std::max(m_min.x, that.min().x);
    bounds.max().x = std::min(m_max.x, that.max().x);
    bounds.min().y = std::max(m_min.y, that.min().y);
    bounds.max().y = std::min(m_max.y, that.max().y);
    bounds.min().z = std::max(m_min.z, that.min().z);
    bounds.max().z = std::min(m_max.z, that.max().z);

    // check if intersections exists
    if (bounds.max().x <= bounds.min().x ||
        bounds.max().y <= bounds.min().y ||
        bounds.max().z <= bounds.min().z) {
        const vector_type zero(0);
        return Bounds3D<T>(zero, zero);
    }

    return bounds;
}

//------------------------------------------------------------------------------

template <typename T>
inline void 
Bounds3D<T>::swap(Bounds3D &that)
{
    std::swap(min(), that.min());
    std::swap(max(), that.max());
}

//------------------------------------------------------------------------------

template <typename T>
inline bool 
Bounds3D<T>::inRange(const_vector_reference value) 
{
    bool result = (m_min.x <= value.x) && (value.x < m_max.x) && 
                  (m_min.y <= value.y) && (value.y < m_max.y) && 
                  (m_min.z <= value.z) && (value.z < m_max.z);
    return result;
}

//-----------------------------------------------------------------------------

template <typename T>
inline std::string 
Bounds3D<T>::toString() const
{
    std::stringstream sstream; 
    sstream << "[" << min().x << ", " << min().y << ", " << min().z << "] / [" <<
                      max().x << ", " << max().y << ", " << max().z << "]";
    return sstream.str();
}

