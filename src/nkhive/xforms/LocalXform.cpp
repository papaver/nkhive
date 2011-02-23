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
// LocalXform.cpp
//------------------------------------------------------------------------------

#include <assert.h>
#include <nkhive/xforms/LocalXform.h>
#include <nkhive/io/hdf5/HDF5Attribute.h>
#include <nkhive/io/hdf5/HDF5DataType.h>
#include <nkhive/io/hdf5/HDF5DataSpace.h>
#include <nkhive/io/hdf5/HDF5Util.h>

BEGIN_NKHIVE_NS

//------------------------------------------------------------------------------
// interface implementation
//------------------------------------------------------------------------------

LocalXform::LocalXform() :
    m_res(1, 1, 1)
{
}

//------------------------------------------------------------------------------

LocalXform::LocalXform(const vec3d &res) :
    m_res(res)
{
    assert(m_res.x > 0 && m_res.y > 0 && m_res.z > 0);
}

//------------------------------------------------------------------------------

LocalXform::~LocalXform()
{
}

//------------------------------------------------------------------------------

const vec3d&
LocalXform::res() const
{
    return m_res;
}

//------------------------------------------------------------------------------

double
LocalXform::resX() const
{
    return m_res.x;
}

//------------------------------------------------------------------------------

double
LocalXform::resY() const
{
    return m_res.y;
}

//------------------------------------------------------------------------------

double
LocalXform::resZ() const
{
    return m_res.z;
}

//------------------------------------------------------------------------------

void
LocalXform::localToVoxel(const vec3d &l, vec3d &v) const
{
    // Component wise multiplication. 
    v = l * m_res; 
}

//------------------------------------------------------------------------------

vec3d
LocalXform::localToVoxel(const vec3d &l) const
{
    vec3d v(0, 0, 0);
    localToVoxel(l, v);
    return v;
}

//------------------------------------------------------------------------------

void
LocalXform::voxelToLocal(const vec3d &v, vec3d &l) const
{
    // Component-wise division.
    l = v / m_res;
}

//------------------------------------------------------------------------------

vec3d
LocalXform::voxelToLocal(const vec3d &v) const
{
    vec3d l(0, 0, 0);
    voxelToLocal(v, l);
    return l;
}

//------------------------------------------------------------------------------

void
LocalXform::voxelToIndex(const vec3d &v, vec3i &i) const
{
    i.x = (vec3i::BaseType)floor(v.x);
    i.y = (vec3i::BaseType)floor(v.y);
    i.z = (vec3i::BaseType)floor(v.z);
}

//------------------------------------------------------------------------------

vec3i
LocalXform::voxelToIndex(const vec3d &v) const
{
    vec3i i(0, 0, 0);
    voxelToIndex(v, i);
    return i;
}

//------------------------------------------------------------------------------

void
LocalXform::indexToVoxel(const vec3i &i, vec3d &v) const
{
    v.x = (vec3d::BaseType)(i.x);
    v.y = (vec3d::BaseType)(i.y);
    v.z = (vec3d::BaseType)(i.z);
}

//------------------------------------------------------------------------------

vec3d
LocalXform::indexToVoxel(const vec3i &i) const
{
    vec3d v(0, 0, 0);
    indexToVoxel(i, v);
    return v;
}

//------------------------------------------------------------------------------
    
void 
LocalXform::read(std::istream &is) 
{
    is.read((char*)&m_res, sizeof(vec3d));
}

//------------------------------------------------------------------------------

void
LocalXform::read(HDF5Id parent_id)
{
    // get the local xform attribute
    HDF5Attribute local_xform_attr;
    local_xform_attr.open(parent_id, kLocalXformAttr);

    // read the local xform attribute in
    HDF5DataType data_type;
    data_type.getFromAttribute(local_xform_attr.id());

    local_xform_attr.read(data_type.id(), &m_res);
}

//------------------------------------------------------------------------------

void 
LocalXform::write(std::ostream &os) const
{
    os.write((char*)&m_res, sizeof(vec3d));
}

//------------------------------------------------------------------------------

void
LocalXform::write(HDF5Id parent_id) const
{
    // create the data type for the local xform attribute
    HDF5DataType data_type;
    HDF5Size type_dims[] = { 3 };
    data_type.createArray(H5T_NATIVE_DOUBLE, 1, type_dims);

    writeScalarAttribute(parent_id, kLocalXformAttr, 
                         data_type.id(), &m_res[0]);
}

//--------------------------------------------------------------------------
    
bool 
LocalXform::operator==(const LocalXform that) const
{
    return (m_res == that.m_res);
}

//--------------------------------------------------------------------------

bool 
LocalXform::operator!=(const LocalXform that) const
{
    return !(*this == that);
}

//------------------------------------------------------------------------------

END_NKHIVE_NS
