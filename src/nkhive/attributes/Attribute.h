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
// Attribute.h
//------------------------------------------------------------------------------

#ifndef __NKHIVE_ATTRIBUTES_ATTRIBUTE_H__
#define __NKHIVE_ATTRIBUTES_ATTRIBUTE_H__

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------

#include <nkhive/Defs.h>
#include <nkbase/String.h>
#include <boost/shared_ptr.hpp>

#include <nkhive/io/hdf5/HDF5Attribute.h>

//------------------------------------------------------------------------------
// class definition
//------------------------------------------------------------------------------

BEGIN_NKHIVE_NS

/** 
 * The base attribute class.
 */
class Attribute 
{
public:

    typedef boost::shared_ptr<Attribute> shared_ptr;

    Attribute() {}
    virtual ~Attribute() {}

    /**
     * Get the attribute's dynamic type name. This must be registered for each
     * supported templated type.
     */
    virtual String typeName() const = 0;

    /**
     * Make a copy of the attribute and return a pointer to it. 
     */
    virtual shared_ptr copy() const = 0;
    virtual void copy(const Attribute &other) = 0;

    /**
     * Read the attribute value in from an opened HDF5Attribute. Also given
     * the opaque data type id of the attribute
     */
    virtual void read(HDF5Attribute &attr, HDF5Id data_type_id) = 0;

    /**
     * Read the attribute value from an input stream. Also given the number of
     * bytes to read.
     */
    virtual void readValue(std::istream &is, int32_t size) = 0;

    /**
     * Write the attribute out to file under the given parent group
     */
    virtual void write(const String &key, HDF5Id parent_id) const = 0;

    /**
     * Write the attribute value to an output stream. 
     */
    virtual void writeValue(std::ostream &os) const = 0;

    /** 
     * Comparison operators.
     */
    virtual bool operator==(const Attribute &other) const = 0;
    virtual bool operator!=(const Attribute &other) const = 0;

    /** 
     * Factory pattern.
     */
    static shared_ptr newAttribute(const String &typeName);

    /** 
     * Test if the given item has already been registered. 
     */
    static bool knownType(const String &typeName);

    /**
     * Clear out the attribute registry.
     */
    static void clearAttributeRegistry();

protected:

    /**
     * Register the attribute type so that the newAttribute() factory method
     * knows how to create attribute objects of the supported type.
     */
    static void registerAttributeType(const String &typeName, 
                                      shared_ptr (*newAttribute)()); 

    /**
     * Un-register the given attribute type so that the newAttribute() factory
     * method no longer knows how to create this type. 
     */
    static void unregisterAttributeType(const String &typeName);
};

//------------------------------------------------------------------------------
// TypedAttribute<T>
//------------------------------------------------------------------------------

/**
 * Class template for attributes of a specified type.
 */
template <typename T>
class TypedAttribute : public Attribute
{

public:

    typedef boost::shared_ptr<TypedAttribute<T> > shared_ptr;

    TypedAttribute(); 
    TypedAttribute(const T &value);
    TypedAttribute(const Attribute &other);
    TypedAttribute(const TypedAttribute<T> &other);
    virtual ~TypedAttribute();

    /**
     * Access the attribute's value.
     */
    T& value();
    const T& value() const;

    /**
     * Get the attribute's dynamic type name. This must be registered for each
     * supported templated type.
     */
    virtual String typeName() const;

    /**
     * Static version of typeName(). This function must be specialized for each.
     * type T.
     */
    static String staticTypeName();

    /**
     * Make a copy of the attribute and return a pointer to it. 
     */
    virtual Attribute::shared_ptr copy() const;

    /**
     * Copy from another attribute. 
     */
    virtual void copy(const Attribute &other);

    /**
     * Read the attribute value from an input stream. Also given the number of
     * bytes to read.
     */
    virtual void readValue(std::istream &is, int32_t size);

    /**
     * Write the attribute value to an output stream. 
     */
    virtual void writeValue(std::ostream &os) const;

    /**
     * Read the attribute value in from an opened HDF5Attribute. Also given
     * the opaque data type id of the attribute
     */
    virtual void read(HDF5Attribute &attr, HDF5Id data_type_id);
    
    /**
     * Write the attribute out to file under the given parent group
     */
    virtual void write(const String &key, HDF5Id parent_id) const;
    
    /**
     * Comparison operators.
     */
    virtual bool operator==(const Attribute &other) const;
    virtual bool operator!=(const Attribute &other) const;

    /**
     * The template specialized factory creation method.
     */
    static Attribute::shared_ptr newAttribute();

    /**
     * Registration methods for this template specialized type.
     */
    static void registerAttributeType();
    static void unregisterAttributeType();

    static bool knownType();

private:

    T m_value;
};

END_NKHIVE_NS

//------------------------------------------------------------------------------
// class implementation
//------------------------------------------------------------------------------

BEGIN_NKHIVE_NS

#include <nkhive/attributes/TypedAttribute.hpp>

END_NKHIVE_NS

//------------------------------------------------------------------------------

#endif // __NKHIVE_ATTRIBUTES_ATTRIBUTE_H__
