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
// TypedAttribute.hpp
//------------------------------------------------------------------------------

template <typename T>
inline
TypedAttribute<T>::TypedAttribute() : 
    Attribute(),
    m_value(T())
{
}

//------------------------------------------------------------------------------

template <typename T>
inline
TypedAttribute<T>::TypedAttribute(const T &value) : 
    Attribute(),
    m_value(value)
{
}

//------------------------------------------------------------------------------

template <typename T>
inline 
TypedAttribute<T>::TypedAttribute(const Attribute &other) :
    Attribute()
{
    copy(other);
}

//------------------------------------------------------------------------------

template <typename T>
inline
TypedAttribute<T>::TypedAttribute(const TypedAttribute<T> &o) :
    Attribute(),
    m_value(o.m_value)
{
}

//------------------------------------------------------------------------------

template <typename T>
inline
TypedAttribute<T>::~TypedAttribute()
{
}

//------------------------------------------------------------------------------

template <typename T>
inline T&
TypedAttribute<T>::value()
{
    return m_value;
}

//------------------------------------------------------------------------------

template <typename T>
inline const T&
TypedAttribute<T>::value() const
{
    return m_value;
}

//------------------------------------------------------------------------------

template <typename T>
inline String
TypedAttribute<T>::typeName() const
{
    return staticTypeName();
}

//------------------------------------------------------------------------------

template <typename T>
inline String
TypedAttribute<T>::staticTypeName()
{
    return typeid(T).name();
}

//------------------------------------------------------------------------------

template <typename T>
inline Attribute::shared_ptr
TypedAttribute<T>::copy() const
{
    Attribute::shared_ptr attribute(new TypedAttribute<T>());
    attribute->copy(*this);
    return attribute;
}

//------------------------------------------------------------------------------

template <typename T>
inline void
TypedAttribute<T>::copy(const Attribute &other)
{
    /**
     * We compare against the typename here instead of doing a direct
     * dynamic_cast since rtti information does not live across library
     * boundaries. This could happen when attribute data is transferred between
     * different libraries, e.g., houdini plugins.
     */
    if (typeName() == other.typeName()) {
        m_value = ((const TypedAttribute<T>*)(&other))->m_value;
    } else {
        throw std::bad_cast();
    }
    //const TypedAttribute<T> *t = 
    //    dynamic_cast<const TypedAttribute<T>* >(&other);
    //if (t == 0) {
    //    throw std::bad_cast();
    //} 
    //m_value = t->m_value;
}

//------------------------------------------------------------------------------

template <typename T>
inline bool
TypedAttribute<T>::operator==(const Attribute &other) const
{
    if (typeName() != other.typeName()) return false;

    // Know we have the same type, convert and compare values.
    return (m_value == ((const TypedAttribute<T>*)(&other))->m_value);
}

//------------------------------------------------------------------------------

template <typename T>
inline bool
TypedAttribute<T>::operator!=(const Attribute &other) const
{
    return !(operator==(other));
}

//------------------------------------------------------------------------------

template <typename T>
inline Attribute::shared_ptr
TypedAttribute<T>::newAttribute()
{
    Attribute::shared_ptr ret(new TypedAttribute<T>());
    return ret;
}

//------------------------------------------------------------------------------

template <typename T>
inline void
TypedAttribute<T>::registerAttributeType()
{
    Attribute::registerAttributeType(
        TypedAttribute<T>::staticTypeName(), 
        TypedAttribute<T>::newAttribute);
}

//------------------------------------------------------------------------------

template <typename T>
inline void
TypedAttribute<T>::unregisterAttributeType()
{
    Attribute::unregisterAttributeType(
        TypedAttribute<T>::staticTypeName());
}

//------------------------------------------------------------------------------

template <typename T>
inline bool
TypedAttribute<T>::knownType()
{
    return Attribute::knownType(TypedAttribute<T>::staticTypeName());
}

//------------------------------------------------------------------------------
