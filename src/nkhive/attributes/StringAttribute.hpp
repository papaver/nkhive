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
// StringAttribute.hpp
//------------------------------------------------------------------------------

// no includes allowed

//------------------------------------------------------------------------------

template <>
inline void
StringAttribute::readValue(std::istream &is, int32_t)
{
    m_value.read(is);
}

//------------------------------------------------------------------------------

template <>
inline void
StringAttribute::read(HDF5Attribute &attr, HDF5Id data_type_id)
{
    size_t buf_size = H5Tget_size(data_type_id);
    char buf[buf_size + 1];

    attr.read(data_type_id, reinterpret_cast<void *>(buf));
    buf[buf_size] = '\0';
    m_value = buf;
}

//------------------------------------------------------------------------------

template<>
inline void
StringAttribute::writeValue(std::ostream &os) const
{
    m_value.write(os);
}

//------------------------------------------------------------------------------

template<>
inline void
StringAttribute::write(const String &key, HDF5Id parent_id) const
{
    // check for existence of the attribute
    HDF5Tri attr_exists = H5Aexists(parent_id, key.c_str());

    if (attr_exists > 0) {
        // for strings we store a fixed length buffer
        // so if we're writing to an existing string attribute
        // we must destroy and recreate it with proper size type
        // NOTE: if this every gets threaded make sure we're not
        // destroying attributes while others are open as that will
        // screw up indices
        H5Adelete(parent_id, key.c_str());
    }

    HDF5DataType data_type;
    
    // create the opaque data type
    size_t write_size = m_value.size();
    if (m_value.size() == 0) {
        write_size = 1;
    }
    
    data_type.create(H5T_OPAQUE, write_size);
    
    // add the typename as a tag on the opaque type
    String type_name = typeName();
    data_type.setOpaqueTag(type_name);
    
    // create the data space for the attribute
    HDF5DataSpace attr_ds;
    attr_ds.createScalar();

    // create the attribute
    HDF5Attribute attr;
    attr.create(parent_id, key, data_type.id(), attr_ds.id()); 

    // write the attribute to the file
    String data = m_value;
    if (m_value.size() == 0) {
        data = String("\0"); 
    }
    
    attr.write(data_type.id(), data.c_str());
}

//------------------------------------------------------------------------------

