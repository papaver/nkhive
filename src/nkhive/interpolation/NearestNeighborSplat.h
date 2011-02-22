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
// NearestNeighborSplat.h
//------------------------------------------------------------------------------

#ifndef __NKHIVE_INTERPOLATION_NEARESTNEIGHBORSPLAT_H__
#define __NKHIVE_INTERPOLATION_NEARESTNEIGHBORSPLAT_H__

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------

#include <nkbase/BinaryOps.h>
#include <nkhive/Defs.h>
#include <nkhive/volume/Volume.h>

//------------------------------------------------------------------------------
// forward declarations
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// class definition
//------------------------------------------------------------------------------

BEGIN_NKHIVE_NS

template <typename T, template<typename> class SetPolicy = set_op >
class NearestNeighborSplat
{

public:

    //--------------------------------------------------------------------------
    // typedefs
    //--------------------------------------------------------------------------
    
    typedef T                                      value_type;
    typedef T&                                     reference;
    typedef const T&                               const_reference;
    typedef typename Volume<T>::shared_ptr         volume_ptr;

    //--------------------------------------------------------------------------
    // public interface
    //--------------------------------------------------------------------------

    /**
     * Constructors and destructors.
     */
    NearestNeighborSplat(volume_ptr v);
    ~NearestNeighborSplat();

    /**
     * Splats the given value at a the given coordinate using the current
     * policy and splatting scheme. In this case just finds the nearest
     * neighbor and splats into that voxel.
     */
    void splat(double x, double y, double z, const_reference result) const;

private:

    //--------------------------------------------------------------------------
    // internal typedefs.
    //--------------------------------------------------------------------------

    //--------------------------------------------------------------------------
    // internal methods.
    //--------------------------------------------------------------------------

    //--------------------------------------------------------------------------
    // friends.
    //--------------------------------------------------------------------------

    //--------------------------------------------------------------------------
    // members.
    //--------------------------------------------------------------------------

    volume_ptr m_volume;
};

END_NKHIVE_NS

//-----------------------------------------------------------------------------
// class implementation
//-----------------------------------------------------------------------------

BEGIN_NKHIVE_NS

#include <nkhive/interpolation/NearestNeighborSplat.hpp>

END_NKHIVE_NS

//------------------------------------------------------------------------------

#endif // __NKHIVE_INTERPOLATION_NEARESTNEIGHBORSPLAT_H__
