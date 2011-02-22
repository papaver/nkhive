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
// Attribute.cpp
//------------------------------------------------------------------------------

#include <iostream>
#include <nkhive/attributes/Attribute.h>
#include <boost/thread/mutex.hpp>

BEGIN_NKHIVE_NS

typedef boost::mutex Mutex;
typedef boost::mutex::scoped_lock Lock;

typedef Attribute::shared_ptr (*Constructor)();
typedef std::map<String, Constructor> AttributeTypeRegistry;

struct LockedAttributeTypeRegistry : public AttributeTypeRegistry {
    LockedAttributeTypeRegistry() : m_mutex() {}
    ~LockedAttributeTypeRegistry() {}
    Mutex m_mutex;
};

//------------------------------------------------------------------------------

LockedAttributeTypeRegistry*
getAttributeTypeRegistry()
{
    static Mutex mutex;

    Lock lock(mutex);

    static LockedAttributeTypeRegistry *registry = NULL;
    if (registry == NULL) {
        registry = new LockedAttributeTypeRegistry();
    }

    return registry;
}

//------------------------------------------------------------------------------

bool
Attribute::knownType(const String &typeName)
{
    LockedAttributeTypeRegistry *registry = getAttributeTypeRegistry();
    Lock lock(registry->m_mutex);

    return registry->find(typeName) != registry->end();
}

//------------------------------------------------------------------------------

void
Attribute::registerAttributeType(const String &typeName, 
                                 Attribute::shared_ptr (*newAttribute)())
{
    LockedAttributeTypeRegistry *registry = getAttributeTypeRegistry();
    Lock lock(registry->m_mutex);

    if (registry->find(typeName) != registry->end()) {
        throw std::bad_typeid();
    }

    registry->operator[](typeName) = newAttribute;
}

//------------------------------------------------------------------------------

void
Attribute::unregisterAttributeType(const String &typeName)
{
    LockedAttributeTypeRegistry *registry = getAttributeTypeRegistry();
    Lock lock(registry->m_mutex);

    registry->erase(typeName);
}

//------------------------------------------------------------------------------

Attribute::shared_ptr
Attribute::newAttribute(const String &typeName) 
{
    LockedAttributeTypeRegistry *registry = getAttributeTypeRegistry();
    Lock lock(registry->m_mutex);

    AttributeTypeRegistry::const_iterator iter = registry->find(typeName);

    if (iter == registry->end()) {
        throw std::bad_typeid();
    }

    return (iter->second)();
}

//------------------------------------------------------------------------------

void
Attribute::clearAttributeRegistry()
{
    LockedAttributeTypeRegistry *registry = getAttributeTypeRegistry();
    Lock lock(registry->m_mutex);

    registry->clear();
}

//------------------------------------------------------------------------------

END_NKHIVE_NS
