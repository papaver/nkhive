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
// PrimitiveTypes.cpp
//------------------------------------------------------------------------------

#include <nkbase/Exceptions.h>
#include <nkhive/attributes/PrimitiveTypes.h>
#include <nkhive/io/hdf5/HDF5DataType.h>
#include <nkhive/io/hdf5/HDF5DataSpace.h>

BEGIN_NKHIVE_NS

//------------------------------------------------------------------------------

template <typename T>
void
TypedAttribute<T>::readValue(std::istream &is, int32_t)
{
    is.read((char*)&m_value, sizeof(T));
}

//------------------------------------------------------------------------------

template <typename T>
void
TypedAttribute<T>::read(HDF5Attribute &attr, HDF5Id data_type_id)
{
    attr.read(data_type_id, reinterpret_cast<void *>(&m_value));
}

//------------------------------------------------------------------------------

template <typename T>
void
TypedAttribute<T>::writeValue(std::ostream &os) const
{
    os.write((char*)&m_value, sizeof(T));
}

//------------------------------------------------------------------------------

template <typename T>
void
TypedAttribute<T>::write(const String &key, HDF5Id parent_id) const
{
    // check for existence of the attribute
    HDF5Tri attr_exists = H5Aexists(parent_id, key.c_str());

    // create the opaque data type
    HDF5DataType data_type;
    data_type.create(H5T_OPAQUE, sizeof(T));

    // add the typename as a tag on the opaque type
    String type_name = typeName();
    data_type.setOpaqueTag(type_name);
   
    // if the attribute exists delete it so we can write the new value 
    // warning if this is threaded then we must make sure no attributes
    // are open under this group or the indices will be messed up
    if (attr_exists > 0) {
        HDF5Err ret = H5Adelete(parent_id, key.c_str()); 
        
        if (ret < 0) {
            THROW(Iex::IoExc, "Could not delete attribute");
        }
    } 

    HDF5Attribute attr;
    
    // first create the data space for the attribute
    HDF5DataSpace attr_ds;
    attr_ds.createScalar();

    // create the attribute
    attr.create(parent_id, key, data_type.id(), attr_ds.id()); 

    // write the attribute to the file
    attr.write(data_type.id(), &m_value);
}

//------------------------------------------------------------------------------
// Template instantiations
//------------------------------------------------------------------------------

template void TypedAttribute<float>::readValue(std::istream&, int32_t);
template void TypedAttribute<double>::readValue(std::istream&, int32_t);
template void TypedAttribute<half>::readValue(std::istream&, int32_t);
template void TypedAttribute<int32_t>::readValue(std::istream&, int32_t);
template void TypedAttribute<int64_t>::readValue(std::istream&, int32_t);

template void TypedAttribute<float>::writeValue(std::ostream&) const;
template void TypedAttribute<double>::writeValue(std::ostream&) const;
template void TypedAttribute<half>::writeValue(std::ostream&) const;
template void TypedAttribute<int32_t>::writeValue(std::ostream&) const;
template void TypedAttribute<int64_t>::writeValue(std::ostream&) const;

template
void 
TypedAttribute<f32>::read(HDF5Attribute &attr, HDF5Id data_type_id);

template
void 
TypedAttribute<f64>::read(HDF5Attribute &attr, HDF5Id data_type_id);

template
void 
TypedAttribute<f16>::read(HDF5Attribute &attr, HDF5Id data_type_id);

template
void 
TypedAttribute<i32>::read(HDF5Attribute &attr, HDF5Id data_type_id);

template
void 
TypedAttribute<i64>::read(HDF5Attribute &attr, HDF5Id data_type_id);

template
void 
TypedAttribute<f32>::write(const String &key, HDF5Id parent_id) const;

template
void 
TypedAttribute<f64>::write(const String &key, HDF5Id parent_id) const;

template
void 
TypedAttribute<half>::write(const String &key, HDF5Id parent_id) const;

template
void 
TypedAttribute<int32_t>::write(const String &key, HDF5Id parent_id) const;

template
void 
TypedAttribute<int64_t>::write(const String &key, HDF5Id parent_id) const;

END_NKHIVE_NS
