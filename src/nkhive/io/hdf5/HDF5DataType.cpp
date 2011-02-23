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
// HDF5DataType.cpp
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------

#include <nkbase/Exceptions.h>
#include <nkhive/io/hdf5/HDF5DataType.h>

BEGIN_NKHIVE_NS

//------------------------------------------------------------------------------
// implementation
//------------------------------------------------------------------------------


HDF5DataType::HDF5DataType() : 
    HDF5Base()
{
}

//------------------------------------------------------------------------------

HDF5DataType::~HDF5DataType()
{
    if (isValid()) {
        H5Tclose(m_id);
    }
}

//------------------------------------------------------------------------------

void
HDF5DataType::create(H5T_class_t type_class, size_t size)
{
    m_id = H5Tcreate(type_class, size);
    if (!isValid()) {
        THROW(Iex::TypeExc, "Couldn't create data type");
    }
}

//------------------------------------------------------------------------------

void
HDF5DataType::copyFromExisting(HDF5Id existing_type_id)
{
    m_id = H5Tcopy(existing_type_id);
    if (!isValid()) {
        THROW(Iex::TypeExc, "Couldn't copy from type " << existing_type_id);
    }
}

//------------------------------------------------------------------------------

void
HDF5DataType::createArray(HDF5Id base_type_id, u32 rank, const HDF5Size dims[])
{
    m_id = H5Tarray_create(base_type_id, rank, dims);
    if (!isValid()) {
        THROW(Iex::TypeExc, "Couldn't create array data type");
    }
}

//------------------------------------------------------------------------------

void
HDF5DataType::enumInsert(const char *name, void *value) const
{
    H5Tenum_insert(m_id, name, value);
}

//------------------------------------------------------------------------------

void
HDF5DataType::compoundInsert(const String &member_name, size_t offset, 
                             HDF5Id member_type_id) const
{
    H5Tinsert(m_id, member_name.c_str(), offset, member_type_id);
}

//------------------------------------------------------------------------------

bool
HDF5DataType::open(HDF5Id location_id, const String &name)
{
    m_id = H5Topen(location_id, name.c_str(), H5P_DEFAULT);
    return (isValid());
}

//------------------------------------------------------------------------------

void
HDF5DataType::getFromAttribute(HDF5Id attr_id)
{
    m_id = H5Aget_type(attr_id);
    if (!isValid()) {
        THROW(Iex::IoExc, "Couldn't get type from attribute " << attr_id);
    }
}

//------------------------------------------------------------------------------

void
HDF5DataType::getFromDataSet(HDF5Id dataset_id)
{
    m_id = H5Dget_type(dataset_id);
    if (!isValid()) {
        THROW(Iex::IoExc, "Couldn't get type from dataset " << dataset_id);
    }
}

//------------------------------------------------------------------------------

void
HDF5DataType::setOpaqueTag(String &tag) const
{
    HDF5Err ret = H5Tset_tag(m_id, tag.c_str());
    if (ret < 0) {
        THROW(Iex::IoExc, "Couldn't set tag " << tag.c_str() << 
                          " on attribute " << m_id);
    }
}

//------------------------------------------------------------------------------

void
HDF5DataType::getOpaqueTag(String &str) const
{
    char *ret = H5Tget_tag(m_id);
    if (ret == NULL) {
       THROW(Iex::IoExc, "Couldn't get tag on attribute " << m_id);
    } 

    str = ret;
    delete ret;
}

//------------------------------------------------------------------------------

void
HDF5DataType::commit(HDF5Id location_id, const String &name) const
{
    HDF5Err ret = H5Tcommit(location_id, name.c_str(), m_id, 
                           H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    if (ret < 0) {
        THROW(Iex::IoExc, "Couldn't commid data type " << m_id << " to file");
    }
}

//------------------------------------------------------------------------------

END_NKHIVE_NS
