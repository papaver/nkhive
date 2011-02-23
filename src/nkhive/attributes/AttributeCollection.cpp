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
// AttributeCollection.cpp
//------------------------------------------------------------------------------

#include <nkhive/attributes/AttributeCollection.h>
#include <nkhive/Types.h>
#include <nkhive/io/hdf5/HDF5DataType.h>
#include <nkhive/io/hdf5/HDF5Group.h>
#include <nkhive/io/hdf5/HDF5Util.h>

BEGIN_NKHIVE_NS

//------------------------------------------------------------------------------

AttributeCollection::AttributeCollection()
{
}

//------------------------------------------------------------------------------

AttributeCollection::~AttributeCollection()
{
}

//------------------------------------------------------------------------------

AttributeCollection::AttributeCollection(const AttributeCollection &other)
{
    const_iterator iter = other.begin();
    for ( ; iter != other.end(); ++iter) {
        this->insert(iter->first, *(iter->second));
    }
}

//------------------------------------------------------------------------------

void
AttributeCollection::insert(const String &n, const Attribute &attr)
{
    if (n.size() == 0)
        THROW(Iex::ArgExc, "Attribute cannot have empty name.");

    // See if the value already exists. 
    iterator iter = m_map.find(n);

    if (iter == m_map.end()) {
        // Create a copy of the attribute and store.
        attribute_ptr tmp = attr.copy();
        m_map[n] = tmp;
    } else {
        if (iter->second->typeName() != attr.typeName()) {
            THROW(Iex::TypeExc, "Cannot assign value of type "
                  << attr.typeName() << " to attribute " << n 
                  << " of type " << iter->second->typeName());
        }
        // else
        attribute_ptr tmp = attr.copy();
        iter->second = tmp;
    }
}

//------------------------------------------------------------------------------

void
AttributeCollection::remove(const String &n)
{
    iterator iter = m_map.find(n);

    if (iter == m_map.end())
        return;
    // else
    m_map.erase(iter);
}

//------------------------------------------------------------------------------

Attribute&
AttributeCollection::operator[](const String &n)
{
    iterator iter = m_map.find(n);
    if (iter == m_map.end()) {
        THROW(Iex::ArgExc, "Cannot find attribute \"" << n << "\".");
    }

    return *(iter->second);
}

//------------------------------------------------------------------------------

const Attribute&
AttributeCollection::operator[](const String &n) const
{
    const_iterator iter = m_map.find(n);
    if (iter == m_map.end()) {
        THROW(Iex::ArgExc, "Cannot find attribute \"" << n << "\".");
    }

    return *(iter->second);
}

//------------------------------------------------------------------------------

bool
AttributeCollection::operator==(const AttributeCollection &other) const
{
    if (count() != other.count()) return false;

    const_iterator iter = m_map.begin();
    const_iterator oiter = other.m_map.begin();
    for ( ; iter != m_map.end(); ++iter, ++oiter) {
        // Compare the key.
        if (iter->first != oiter->first) return false;

        // Compare the attribute.
        if (iter->second->operator!=(*(oiter->second))) return false;
    }

    return true;
}

//------------------------------------------------------------------------------

bool
AttributeCollection::operator!=(const AttributeCollection &other) const
{
    return !(operator==(other));
}

//------------------------------------------------------------------------------

AttributeCollection::iterator
AttributeCollection::begin()
{
    return m_map.begin();
}

//------------------------------------------------------------------------------

AttributeCollection::iterator
AttributeCollection::end()
{
    return m_map.end();
}

//------------------------------------------------------------------------------

AttributeCollection::const_iterator
AttributeCollection::begin() const
{
    return m_map.begin();
}

//------------------------------------------------------------------------------

AttributeCollection::const_iterator
AttributeCollection::end() const
{
    return m_map.end();
}

//------------------------------------------------------------------------------

void
AttributeCollection::clear()
{
    return m_map.clear();
}

//------------------------------------------------------------------------------

size_t
AttributeCollection::count() const
{
    return m_map.size();
}

//------------------------------------------------------------------------------

bool
AttributeCollection::empty() const
{
    return m_map.empty();
}

//------------------------------------------------------------------------------

void 
AttributeCollection::read(std::istream &is) 
{
    // Clear out all current attributes.
    clear();

    // Read in the number of attributes.
    index_type count = 0;
    is.read((char*)&count, sizeof(index_type));

    // Read in each attribute.
    for (index_type i = 0; i < count; ++i) {
        // Read in the key. 
        String name; 
        name.read(is);

        // Read in the attribute's typename.
        String typeName;
        typeName.read(is);

        // Create the attribute from the type name. Make sure that the type is
        // registered. 
        if (!Attribute::knownType(typeName)) {
            THROW(Iex::TypeExc, "Cannot read attribute type " << typeName
                    << ". Type is not registered in attribute registry.");
        }
        // else
        Attribute::shared_ptr attribute = Attribute::newAttribute(typeName);

        // Read the value from the stream.
        attribute->readValue(is, 0);

        // Add the name and the attribute to the map.
        insert(name, *attribute);
    }
}

//------------------------------------------------------------------------------

HDF5Err
AttributeCollection::readAttribute(HDF5Id volume_group_id, 
                                   const char *attr_name, 
                                   const H5A_info_t *NK_UNUSED_PARAM(attr_info),
                                   void *op_data)
{
    // open the attribute 
    HDF5Attribute attr;
    String attr_name_str(attr_name);
    attr.open(volume_group_id, attr_name_str);

    // read in the typename from the attributes opaque datatype
    HDF5DataType data_type;
    data_type.getFromAttribute(attr.id());
    String type_name;
    data_type.getOpaqueTag(type_name);
  
    // Create the attribute from the type name. Make sure that the type is
    // registered. 
    if (!Attribute::knownType(type_name)) {
        THROW(Iex::TypeExc, "Cannot read attribute type " << type_name
                << ". Type is not registered in attribute registry.");
    }

    // read the value
    Attribute::shared_ptr attr_ptr = Attribute::newAttribute(type_name);
    attr_ptr->read(attr, data_type.id());

    // get the pointer to the attribute collection from user data
    AttributeCollection *attr_coll = 
        reinterpret_cast<AttributeCollection *>(op_data);

    // add the name and attribute to the map
    attr_coll->insert(attr_name_str, *attr_ptr);

    return 0;
}

//------------------------------------------------------------------------------

void
AttributeCollection::read(HDF5Id volume_group_id)
{
    // clear out all current attributes
    clear();

    // open user attribute group
    HDF5Group user_attr_group;
    user_attr_group.open(volume_group_id, kUserAttrGroup);
   
    if (!user_attr_group.isValid()) {
        THROW(Iex::IoExc, "No User Attributes group exists");
    }

    // iterate over all attributes
    HDF5Size attr_index = 0;
    H5Aiterate(user_attr_group.id(), 
               H5_INDEX_NAME, 
               H5_ITER_NATIVE, 
               &attr_index,
               AttributeCollection::readAttribute,
               reinterpret_cast<void *>(this));

}

//------------------------------------------------------------------------------

void 
AttributeCollection::write(std::ostream &os) const
{
    // Write out the number of attributes we have. 
    index_type c = count();
    os.write((char*)&c, sizeof(index_type));

    // Write out each attribute.
    const_iterator iter = begin();
    for ( ; iter != end(); ++iter) {
        // Write the key. 
        iter->first.write(os);

        // Write the type name of the attribute.
        String typeName = iter->second->typeName();
        typeName.write(os);
        
        // Write out the attribute value.
        iter->second->writeValue(os);
    }
}

//------------------------------------------------------------------------------

void
AttributeCollection::write(HDF5Id volume_group_id) const
{
    // write user attributes under a special group
    HDF5Group user_attr_group;
    user_attr_group.open(volume_group_id, kUserAttrGroup);

    // doesn't exist. Create it.
    if (!user_attr_group.isValid()) {
        user_attr_group.create(volume_group_id, kUserAttrGroup);
    }

    // write out each attribute
    const_iterator iter = begin();
    for ( ; iter != end(); ++iter) {
        iter->second->write(iter->first, user_attr_group.id());
    }
}    

//------------------------------------------------------------------------------

AttributeCollection&
AttributeCollection::operator=(const AttributeCollection &that)
{
    // Clear the current entries.
    clear();

    const_iterator iter = that.begin();
    for ( ; iter != that.end(); ++iter) {
        this->insert(iter->first, *(iter->second));
    }

    return *this;
}

//------------------------------------------------------------------------------

END_NKHIVE_NS
