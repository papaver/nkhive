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
// Stamp.h
//------------------------------------------------------------------------------

#ifndef __NKHIVE_TILING_STAMP_H__
#define __NKHIVE_TILING_STAMP_H__

//-----------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------- 

#include <nkhive/Defs.h>
#include <nkhive/Types.h>
#include <nkhive/util/Bounds3D.h>

//-----------------------------------------------------------------------------
// class declarations
//-----------------------------------------------------------------------------

BEGIN_NKHIVE_NS

/**
 *  The Stamp container abstracts the concept of a buffer used to 
 *  efficiently write data to a volume.  Only a bounds and read accessors
 *  are required.  This allows extending stamps with many implementations.
 */
template <typename T, template <typename> class Source>
class Stamp : public Source<T>
{
    
public:

    //-------------------------------------------------------------------------
    // typedefs
    //-------------------------------------------------------------------------

    typedef Source<T>           parent;
    typedef T                   value_type;
    typedef value_type*         pointer;
    typedef const value_type*   const_pointer;

    //-------------------------------------------------------------------------
    // public interface
    //-------------------------------------------------------------------------
      
    /**
     * Default constructor.
     */
    Stamp();

    /**
     * Destructor. Deallocates any used memory.
     */
    ~Stamp();

    /**
     * Get the value at the given coordinate.
     */
    value_type get(signed_index_type x, 
                   signed_index_type y, 
                   signed_index_type z) const;

    /**
     * Gets the bounds of the stamp.
     */
    signed_index_bounds getBounds() const;

};

END_NKHIVE_NS

//-----------------------------------------------------------------------------
// class implementation
//-----------------------------------------------------------------------------

BEGIN_NKHIVE_NS

#include <nkhive/tiling/Stamp.hpp>

END_NKHIVE_NS

//-----------------------------------------------------------------------------

#endif // __NKHIVE_TILING_STAMP_H__
