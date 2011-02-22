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
// Types.h
//------------------------------------------------------------------------------

#ifndef __NKHIVE_TYPES_H__
#define __NKHIVE_TYPES_H__

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------

#include <memory>

#include <nkhive/Defs.h>
#include <nkbase/Types.h>

//-----------------------------------------------------------------------------
// forward declarations 
//-----------------------------------------------------------------------------

BEGIN_NKHIVE_NS

template <typename T, typename A>
class BitField3D;

template <typename T>
class Bounds3D;

template <typename T>
class BoundingBox;

END_NKHIVE_NS

//-----------------------------------------------------------------------------
// typedefs
//-----------------------------------------------------------------------------

BEGIN_NKHIVE_NS

//- data -----------------------------------------------------------------------

typedef i32                         signed_index_type;
typedef u32                         index_type;

typedef vec3i                       signed_index_vec;
typedef vec3ui                      index_vec;

typedef Bounds3D<signed_index_type> signed_index_bounds;
typedef Bounds3D<index_type>        index_bounds;

typedef BoundingBox<f64>            bounding_box;

typedef std::allocator<index_type>  bitfield_alloc;

typedef BitField3D<index_type, 
                   bitfield_alloc>  bitfield_type;

//-----------------------------------------------------------------------------

END_NKHIVE_NS

#endif // __NKHIVE_TYPES_H__
