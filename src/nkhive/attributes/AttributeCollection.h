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
// AttributeCollection.h
//------------------------------------------------------------------------------

#ifndef __NKHIVE_ATTRIBUTES_ATTRIBUTECOLLECTION_H__
#define __NKHIVE_ATTRIBUTES_ATTRIBUTECOLLECTION_H__

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------

#include <map>
#include <hdf5.h>

#include <nkhive/Defs.h>
#include <nkhive/attributes/Attribute.h>
#include <nkhive/attributes/StringAttribute.h>
#include <nkbase/Exceptions.h>

//------------------------------------------------------------------------------
// class definition
//------------------------------------------------------------------------------

BEGIN_NKHIVE_NS

/** 
 * Provides a collection of attributes stored (key,value) pairs, where key is a
 * string. 
 */

class AttributeCollection 
{

public:

    typedef Attribute::shared_ptr                   attribute_ptr;
    typedef std::map<String, attribute_ptr>         attribute_map;
    typedef attribute_map::iterator                 iterator;
    typedef attribute_map::const_iterator           const_iterator;
    typedef boost::shared_ptr<AttributeCollection>  shared_ptr;

    AttributeCollection();
    ~AttributeCollection();

    AttributeCollection(const AttributeCollection &other);

    /**
     * Inserts a new attribute in the collection. If no attribute with the name
     * exists a new attribute with the name, and the same type as attr is added
     * and teh value of attr is copied into the new attribute. 
     *
     * If an attribute with the name n exists, and its type is the same as
     * attr, the value of attr is copied into this attribute. 
     *
     * If an attribute with name n exists, and its type is different from attr,
     * an exception is thrown. 
     */
    void insert(const String &n, const Attribute &attr);

    /**
     * Remove the attribute with the given name n if it exists. If there is no
     * such attribute with that name, then do nothing. 
     */
    void remove(const String &n);

    /** 
     * Attribute access
     */
    Attribute& operator[](const String &n);
    const Attribute& operator[](const String &n) const;

    /**
     * Returns a reference the attribute with the given name and the templated
     * type T. If no attribute with the name then an Iex::ArgExc is thrown, if
     * an attribute with name n exists,  but its type T is not compatible then
     * a Iex::TypeExc is thrown.
     */
    template <typename T> T& typedAttribute(const String &n);
    template <typename T> const T& typedAttribute(const String &n) const;

    /**
     * Returns a pointer to the attribute with the name n and Type T or invalid
     * Ptr if no attribute with name n and type T exists. 
     */
    template <typename T> typename T::shared_ptr
    findTypedAttribute(const String &n);

    template <typename T> const typename T::shared_ptr
    findTypedAttribute(const String &n) const;

    /**
     * Return the value in the attribute of the given name n. If the attribute
     * with the name doesn't exist, then a Iex::ArgExc is thrown. If the
     * attribute exists, but has incompatible type with the one given, then
     * throws Iex::TypeExc.
     */
    template <typename T> T& value(const String &n);
    template <typename T> const T& value(const String &n) const;

    /**
     * Functions for iterating over the attributes in the collection. 
     */
    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;

    /** 
     * Remove all attributes.
     */
    void clear();

    /** 
     * Number of attributes in the collection.
     */
    size_t count() const;

    /** 
     * True if collection contains no attributes. 
     */
    bool empty() const;

    /**
     * I/O operations.
     */
    void read(HDF5Id volume_group_id);
    void write(HDF5Id volume_group_id) const;

    void read(std::istream &is);
    void write(std::ostream &os) const;

    /** 
     * Copy operator.
     */
    AttributeCollection& operator=(const AttributeCollection& that);

    /** 
     * Comparison operators.
     */
    bool operator==(const AttributeCollection& that) const;
    bool operator!=(const AttributeCollection& that) const;

private:

    template <typename T> typename TypedAttribute<T>::shared_ptr
    getValidTypedAttribute(const String &n) const;

    static HDF5Err readAttribute(HDF5Id volume_group_id, 
                                 const char *attr_name, 
                                 const H5A_info_t *attr_info,
                                 void *op_data);

private:

    attribute_map m_map;

};

END_NKHIVE_NS

//-----------------------------------------------------------------------------
// class implementation
//-----------------------------------------------------------------------------

BEGIN_NKHIVE_NS

#include <nkhive/attributes/AttributeCollection.hpp>

END_NKHIVE_NS

//-----------------------------------------------------------------------------

#endif
