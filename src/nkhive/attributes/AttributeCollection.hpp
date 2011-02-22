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
// AttributeCollection.hpp
//------------------------------------------------------------------------------ 
 
//------------------------------------------------------------------------------
// Template definitions
//------------------------------------------------------------------------------

template <typename T>
inline T&
AttributeCollection::typedAttribute(const String &n)
{
    Attribute *attr = &(*this)[n];
    if (attr->typeName() != T::staticTypeName()) {
        THROW(Iex::TypeExc, "Unexpected attribute type.");
    }

    return *((T*)attr);
}

//------------------------------------------------------------------------------

template <typename T>
inline const T&
AttributeCollection::typedAttribute(const String &n) const
{
    Attribute *attr = &(*this)[n];
    if (attr->typeName() != T::staticTypeName()) {
        THROW(Iex::TypeExc, "Unexpected attribute type.");
    }

    return *((const T*)attr);
}

//------------------------------------------------------------------------------

template <typename T>
inline typename T::shared_ptr
AttributeCollection::findTypedAttribute(const String &n)
{
    iterator iter = m_map.find(n);
    if (iter == m_map.end()) {
        return typename T::shared_ptr();
    }

    if (iter->second->typeName() == T::staticTypeName()) {
        return boost::static_pointer_cast<T, Attribute>(iter->second);
    }
    // else
    return typename T::shared_ptr();
}

//------------------------------------------------------------------------------

template <typename T>
inline const typename T::shared_ptr
AttributeCollection::findTypedAttribute(const String &n) const
{
    const_iterator iter = m_map.find(n);
    if (iter == m_map.end()) {
        return typename T::shared_ptr();
    }

    if (iter->second->typeName() == T::staticTypeName()) {
        return boost::static_pointer_cast<const T, Attribute>(iter->second);
    }
    // else
    return typename T::shared_ptr();
}

//------------------------------------------------------------------------------

template <typename T>
inline typename TypedAttribute<T>::shared_ptr
AttributeCollection::getValidTypedAttribute(const String &n) const
{
    const_iterator iter = m_map.find(n);
    if (iter == m_map.end()) {
        THROW(Iex::ArgExc, "Cannot find attribute \"" << n << "\".");
    }

    typename TypedAttribute<T>::shared_ptr m;
    if (iter->second->typeName() == TypedAttribute<T>::staticTypeName()) {
        m = boost::static_pointer_cast<TypedAttribute<T>, Attribute>(
                iter->second);
    }

    if (m.get() == NULL) 
        THROW(Iex::TypeExc, "Unexpected attribute type.");
    return m;
}

//------------------------------------------------------------------------------

template <typename T>
inline T&
AttributeCollection::value(const String &n)
{
    typename TypedAttribute<T>::shared_ptr m = getValidTypedAttribute<T>(n);
    return m->value();
}

//------------------------------------------------------------------------------

template <typename T>
inline const T&
AttributeCollection::value(const String &n) const
{
    typename TypedAttribute<const T>::shared_ptr m =
        getValidTypedAttribute<T>(n);
    return m->value();
}
