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
// TestAttributeRegistry.cpp
//------------------------------------------------------------------------------

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <nkhive/Defs.h>
#include <nkhive/attributes/Attribute.h>
#include <nkhive/attributes/PrimitiveTypes.h>
#include <nkhive/attributes/StringAttribute.h>

class TestAttributeRegistry : public CppUnit::TestFixture {

    CPPUNIT_TEST_SUITE(TestAttributeRegistry);
    CPPUNIT_TEST(test);
    CPPUNIT_TEST_SUITE_END();
    
public:
    void setUp() {}
    void tearDown() {}
    
    void test();
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestAttributeRegistry);

//------------------------------------------------------------------------------

void 
TestAttributeRegistry::test()
{
    USING_NKHIVE_NS
    
    Attribute::clearAttributeRegistry();

    Int32Attribute::registerAttributeType();

    StringAttribute strAttribute;

    CPPUNIT_ASSERT(!Attribute::knownType(strAttribute.typeName()));

    StringAttribute::registerAttributeType();

    CPPUNIT_ASSERT(Attribute::knownType(strAttribute.typeName()));
    CPPUNIT_ASSERT(Attribute::knownType(Int32Attribute::staticTypeName()));

    Attribute::shared_ptr stringAttribute = 
        Attribute::newAttribute(strAttribute.typeName());

    CPPUNIT_ASSERT(stringAttribute->typeName() == strAttribute.typeName());

    StringAttribute::unregisterAttributeType();

    CPPUNIT_ASSERT_THROW(Attribute::newAttribute(strAttribute.typeName()),
                         std::bad_typeid);
}

//------------------------------------------------------------------------------
