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
// HDF5DataType.h
//------------------------------------------------------------------------------

#ifndef __NKHIVE_IO_HDF5_HDF5DATATYPE_H__
#define __NKHIVE_IO_HDF5_HDF5DATATYPE_H__

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------

#include <nkbase/String.h>
#include <nkhive/io/hdf5/HDF5Base.h>

//------------------------------------------------------------------------------
// interface
//------------------------------------------------------------------------------

BEGIN_NKHIVE_NS

class HDF5DataType : public HDF5Base
{

public:

    //--------------------------------------------------------------------------
    // constructors/destructors
    //--------------------------------------------------------------------------

    HDF5DataType();
    ~HDF5DataType();

    //--------------------------------------------------------------------------
    // public interface
    //--------------------------------------------------------------------------
    
    void create(H5T_class_t type_class, size_t size);

    void copyFromExisting(HDF5Id existing_type_id);

    void createArray(HDF5Id base_type_id, u32 rank, const hsize_t dims[]);

    void enumInsert(const char *name, void *value) const;
    void compoundInsert(const String &member_name, size_t offset, 
                        HDF5Id member_type_id) const;

    bool open(HDF5Id location_id, const String &name);

    void getFromAttribute(HDF5Id attr_id);
    void getFromDataSet(HDF5Id dataset_id);

    void commit(HDF5Id location_id, const String &name) const;

    void getOpaqueTag(String &tag) const;
    void setOpaqueTag(String &tag) const;

};

END_NKHIVE_NS

#endif // __NKHIVE_IO_HDF5_HDF5DATATYPE_H__

