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
// BoundingBox.hpp
//------------------------------------------------------------------------------

// no includes allowed

//------------------------------------------------------------------------------
// typedefs
//------------------------------------------------------------------------------

typedef BoundingBox<f16> BoundingBoxh;
typedef BoundingBox<f32> BoundingBoxf;
typedef BoundingBox<f64> BoundingBoxd;

//------------------------------------------------------------------------------
// interface implementation
//------------------------------------------------------------------------------

template <typename T>
inline
BoundingBox<T>::BoundingBox() :
    Bounds3D<T>(std::numeric_limits<T>::infinity(),
                -std::numeric_limits<T>::infinity())
{
}

//------------------------------------------------------------------------------

template <typename T>
inline
BoundingBox<T>::BoundingBox(value_type min, value_type max) :
    Bounds3D<T>(min, max)
{
}

//------------------------------------------------------------------------------

template <typename T>
inline
BoundingBox<T>::BoundingBox(const_vector_reference min, 
        const_vector_reference max) :
    Bounds3D<T>(min, max)
{
}

//------------------------------------------------------------------------------

template <typename T>
inline bool 
BoundingBox<T>::inRange(const_vector_reference value) 
{
    vector_reference min = Bounds3D<T>::min();
    vector_reference max = Bounds3D<T>::max();
    bool result = (min.x <= value.x) && (value.x <= max.x) && 
                  (min.y <= value.y) && (value.y <= max.y) && 
                  (min.z <= value.z) && (value.z <= max.z);
    return result;
}

//------------------------------------------------------------------------------

template <typename T>
inline typename BoundingBox<T>::vector_type 
BoundingBox<T>::center() 
{
    return Bounds3D<T>::min() + 
           ((Bounds3D<T>::max() - Bounds3D<T>::min()) / T(2));
}

//------------------------------------------------------------------------------

template <typename T>
inline bool
BoundingBox<T>::isValid() 
{
    vector_reference min = Bounds3D<T>::min();
    vector_reference max = Bounds3D<T>::max();

    bool result = (min.x <= max.x) && (min.y <= max.y) && (min.z <= max.z); 
    return result;
}

