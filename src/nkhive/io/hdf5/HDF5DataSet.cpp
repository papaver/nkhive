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
// HDF5DataSet.cpp
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------

#include <nkbase/Exceptions.h>
#include <nkhive/io/hdf5/HDF5DataSet.h>

BEGIN_NKHIVE_NS

//------------------------------------------------------------------------------
// interface
//------------------------------------------------------------------------------

HDF5DataSet::HDF5DataSet() :
    HDF5Base()
{
}

//------------------------------------------------------------------------------

HDF5DataSet::~HDF5DataSet()
{
    if (isValid()) {
        H5Dclose(m_id);
    }
}

//------------------------------------------------------------------------------

void
HDF5DataSet::create(HDF5Id location_id, const String &name,
                    HDF5Id type_id, HDF5Id space_id)
{
    m_id = H5Dcreate(location_id, name.c_str(), type_id, space_id,
                     H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
}

//------------------------------------------------------------------------------

void
HDF5DataSet::open(HDF5Id location_id, const String &name)
{
    if (isValid()) {
        THROW(Iex::IoExc, "Tried to open data set that has already "
                          "been opened or created");
    }

    m_id = H5Dopen(location_id, name.c_str(), H5P_DEFAULT);
}

//------------------------------------------------------------------------------

void
HDF5DataSet::write(HDF5Id mem_type_id, const void *data) const
{
    HDF5Err res = H5Dwrite(m_id, mem_type_id, 
                           H5S_ALL, H5S_ALL, H5P_DEFAULT, 
                           data);

    if (res < 0) {
        THROW(Iex::IoExc, "Could not write data set with id " << m_id);
    }
}

//------------------------------------------------------------------------------

void
HDF5DataSet::read(HDF5Id mem_type_id, void *data) const
{
    HDF5Err res = H5Dread(m_id, mem_type_id,
                          H5S_ALL, H5S_ALL, H5P_DEFAULT,
                          data); 

    if (res < 0) {
        THROW(Iex::IoExc, "Could not read data set with id " << m_id);
    }
}

//------------------------------------------------------------------------------

END_NKHIVE_NS
