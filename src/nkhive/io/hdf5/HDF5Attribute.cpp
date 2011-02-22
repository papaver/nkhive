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
// HDF5Attribute.cpp
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------

#include <nkbase/Exceptions.h>
#include <nkhive/io/hdf5/HDF5Attribute.h>

BEGIN_NKHIVE_NS

//------------------------------------------------------------------------------
// interface implementation
//------------------------------------------------------------------------------

HDF5Attribute::HDF5Attribute():
    HDF5Base()
{
}

//------------------------------------------------------------------------------

HDF5Attribute::~HDF5Attribute()
{
    if (isValid()) {
        // release in memory
        H5Aclose(m_id);
    }
}

//------------------------------------------------------------------------------

void
HDF5Attribute::create(const HDF5Id location_id, const String &name, 
                      const HDF5Id type_id, const HDF5Id space_id)
{
    // create the attribute in memory
    m_id = H5Acreate(location_id, name.c_str(), type_id, space_id, 
                     H5P_DEFAULT, H5P_DEFAULT);
}

//------------------------------------------------------------------------------

void 
HDF5Attribute::open(const HDF5Id location_id, const String &name)
{
    if (isValid()) {
        THROW(Iex::IoExc, "Tried to open attribute that has already "
                          "been opened or created");
    }
    
    m_id = H5Aopen(location_id, name.c_str(), H5P_DEFAULT);
}

//------------------------------------------------------------------------------

void
HDF5Attribute::read(HDF5Id mem_type_id, void *buf) const
{
    // make sure it's a valid attribute id
    if (!isValid()) {
        THROW(Iex::IoExc, "Trying to read invalid attribute");
    }

    // read in the attribute
    HDF5Err res = H5Aread(m_id, mem_type_id, buf);

    // check if read was successful
    if (res < 0) {
        char attrName[256];
        H5Aget_name(m_id, 256, attrName);
        THROW(Iex::IoExc, "Couldn't read attribute " << attrName);
    }
}

//------------------------------------------------------------------------------

void
HDF5Attribute::write(const HDF5Id mem_type_id, const void *attribute_data) const
{
    i32 res = H5Awrite(m_id, mem_type_id, attribute_data);
    if (res < 0) {
        char attrName[256];
        H5Aget_name(m_id, 256, attrName);
        THROW(Iex::IoExc, "Could not write attribute" << attrName);
    }
}

//------------------------------------------------------------------------------

END_NKHIVE_NS
