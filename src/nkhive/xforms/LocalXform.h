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
// LocalXform.h
//------------------------------------------------------------------------------

#ifndef __NKHIVE_XFORMS_LOCAL_XFORM_H__
#define __NKHIVE_XFORMS_LOCAL_XFORM_H__

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------

#include <nkhive/Types.h>
#include <nkhive/io/hdf5/HDF5Base.h>
#include <nkbase/String.h>

//-----------------------------------------------------------------------------
// interface definition
//-----------------------------------------------------------------------------

BEGIN_NKHIVE_NS

/**
 * A local xform for a volume. This is simply a scaling defined by the
 * resolution of the voxels. Note that a translation is not needed here since
 * HiVE volumes support negative indices and boundless grids. As such, no offset
 * of the origin is necessary.
 */
class LocalXform
{

public:

    //--------------------------------------------------------------------------
    // Constructor/Destructor
    //--------------------------------------------------------------------------

    LocalXform();
    LocalXform(const vec3d &scaling);
    ~LocalXform();
    
    //--------------------------------------------------------------------------
    // Accessors.
    //--------------------------------------------------------------------------

    const vec3d& res() const;
    double resX() const;
    double resY() const;
    double resZ() const;

    //--------------------------------------------------------------------------
    // Transformation methods.
    //--------------------------------------------------------------------------

    void localToVoxel(const vec3d &l, vec3d &v) const;
    vec3d localToVoxel(const vec3d &l) const;

    void voxelToLocal(const vec3d &v, vec3d &l) const;
    vec3d voxelToLocal(const vec3d &v) const;

    void voxelToIndex(const vec3d &v, vec3i &i) const;
    vec3i voxelToIndex(const vec3d &v) const;

    void indexToVoxel(const vec3i &i, vec3d &v) const;
    vec3d indexToVoxel(const vec3i &i) const;

    //--------------------------------------------------------------------------
    // I/O Operations.
    //--------------------------------------------------------------------------
    
    void read(std::istream &is);
    void write(std::ostream &os) const;
    void read(HDF5Id parent_id);
    void write(HDF5Id parent_id) const;

    //--------------------------------------------------------------------------
    // Operators.
    //--------------------------------------------------------------------------
    
    bool operator==(const LocalXform that) const;
    bool operator!=(const LocalXform that) const;

protected:

    vec3d m_res;
};

END_NKHIVE_NS

#endif //__NKHIVE_XFORMS_LOCAL_XFORM_H__
