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
// TestPrimitiveAttributes.cpp
//------------------------------------------------------------------------------

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <nkhive/Defs.h>
#include <nkhive/attributes/Attribute.h>
#include <nkhive/attributes/PrimitiveTypes.h>
#include <nkhive/io/VolumeFile.h>

template <typename T>
class TestPrimitiveAttributes : public CppUnit::TestFixture {

    CPPUNIT_TEST_SUITE(TestPrimitiveAttributes);
    CPPUNIT_TEST(test);
    CPPUNIT_TEST(testIO);
    CPPUNIT_TEST(testIOHDF5);
    CPPUNIT_TEST(testMultipleIO);
    CPPUNIT_TEST(testMultipleIOHDF5);
    CPPUNIT_TEST(testComparisonOperators);
    CPPUNIT_TEST_SUITE_END();
    
public:
    void setUp() { remove("testingHDF5.hv"); }
    void tearDown() { remove("testingHDF5.hv"); }
    
    void test();
    void testIO();
    void testIOHDF5();
    void testMultipleIO();
    void testMultipleIOHDF5();
    void testComparisonOperators();
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestPrimitiveAttributes<float>);
CPPUNIT_TEST_SUITE_REGISTRATION(TestPrimitiveAttributes<double>);
CPPUNIT_TEST_SUITE_REGISTRATION(TestPrimitiveAttributes<half>);
CPPUNIT_TEST_SUITE_REGISTRATION(TestPrimitiveAttributes<int32_t>);
CPPUNIT_TEST_SUITE_REGISTRATION(TestPrimitiveAttributes<int64_t>);

//------------------------------------------------------------------------------

template <typename T>
void
TestPrimitiveAttributes<T>::test()
{
    USING_NKHIVE_NS

    Attribute::shared_ptr m(new TypedAttribute<T>(T(1)));
    Attribute::shared_ptr m2 = m->copy();

    CPPUNIT_ASSERT(dynamic_cast<TypedAttribute<T>*>(m.get()) != 0);
    CPPUNIT_ASSERT(dynamic_cast<TypedAttribute<T>*>(m2.get()) != 0);

    CPPUNIT_ASSERT(m->typeName().compare(typeid(T).name()) == 0);
    CPPUNIT_ASSERT(m2->typeName().compare(m->typeName()) == 0);

    TypedAttribute<T> *s = dynamic_cast<TypedAttribute<T>*>(m.get());
    CPPUNIT_ASSERT(s->value() == T(1));
    s->value() = T(2);
    CPPUNIT_ASSERT(s->value() == T(2));

    m2->copy(*s);

    s = dynamic_cast<TypedAttribute<T>*>(m2.get());
    CPPUNIT_ASSERT(s->value() == T(2));
}

//------------------------------------------------------------------------------

template <typename T>
void
TestPrimitiveAttributes<T>::testIO()
{
    USING_NKHIVE_NS

    Attribute::shared_ptr m(new TypedAttribute<T>(1));

    std::ostringstream ostr(std::ios_base::binary);

    m->writeValue(ostr);

    std::istringstream istr(ostr.str(), std::ios_base::binary);

    TypedAttribute<T> tm;
    tm.readValue(istr, 0);

    CPPUNIT_ASSERT(tm.value() == T(1));
}

//------------------------------------------------------------------------------

template <typename T>
void
TestPrimitiveAttributes<T>::testIOHDF5()
{
    USING_NK_NS
    USING_NKHIVE_NS

    Attribute::shared_ptr m(new TypedAttribute<T>(1));

    // open a dummy volume file
    VolumeFile file("testingHDF5.hv", VoidFile::WRITE_TRUNC);       
    HDF5Group root_group;                                           
    root_group.open(file.m_id, NK_NS::String("/"));                    
    String attr_key("attribute1"); 
    m->write(attr_key, root_group.id());
    file.close();
    
    // open the attribute from file
    VolumeFile file2("testingHDF5.hv", VoidFile::READ_ONLY);                             
    HDF5Group root_group2;                                          
    root_group2.open(file2.m_id, NK_NS::String("/"));                  
    
    HDF5Attribute attr;
    attr.open(root_group2.id(), attr_key);

    // read in the typename from the attributes opaque datatype
    HDF5DataType data_type;
    data_type.getFromAttribute(attr.id());
    String type_name;
    data_type.getOpaqueTag(type_name);

    // read the value
    TypedAttribute<T> tm;
    tm.read(attr, data_type.id());

    file2.close();

    CPPUNIT_ASSERT(tm.value() == T(1));
}

//------------------------------------------------------------------------------

template <typename T>
void
TestPrimitiveAttributes<T>::testMultipleIO()
{
    USING_NKHIVE_NS

    Attribute::shared_ptr m(new TypedAttribute<T>(1));
    Attribute::shared_ptr m2(new TypedAttribute<T>(2));

    std::ostringstream ostr(std::ios_base::binary);

    m->writeValue(ostr);
    m2->writeValue(ostr);

    std::istringstream istr(ostr.str(), std::ios_base::binary);

    TypedAttribute<T> tm, tm2;
    tm.readValue(istr, 0);
    tm2.readValue(istr, 0);

    CPPUNIT_ASSERT(tm.value() == T(1));
    CPPUNIT_ASSERT(tm2.value() == T(2));
}

//------------------------------------------------------------------------------

template <typename T>
void
TestPrimitiveAttributes<T>::testMultipleIOHDF5()
{
    USING_NK_NS
    USING_NKHIVE_NS

    Attribute::shared_ptr m(new TypedAttribute<T>(1));
    Attribute::shared_ptr m2(new TypedAttribute<T>(2));

    // open a dummy volume file
    VolumeFile file("testingHDF5.hv", VoidFile::WRITE_TRUNC);       
    HDF5Group root_group;                                           
    root_group.open(file.m_id, NK_NS::String("/"));                    
    String attr_key("attribute1"); 
    String attr_key2("attribute2"); 
    m->write(attr_key, root_group.id());
    m2->write(attr_key2, root_group.id());
    file.close();

    // open the attribute from file
    VolumeFile file2("testingHDF5.hv", VoidFile::READ_ONLY);                             
    HDF5Group root_group2;                                          
    root_group2.open(file2.m_id, NK_NS::String("/"));                  

    // read attribute 1    
    HDF5Attribute attr;
    attr.open(root_group2.id(), attr_key);

    // read in the typename from the attributes opaque datatype
    HDF5DataType data_type;
    data_type.getFromAttribute(attr.id());
    String type_name;
    data_type.getOpaqueTag(type_name);

    // read the value
    TypedAttribute<T> tm;
    tm.read(attr, data_type.id());

    // read attribute 2
    HDF5Attribute attr2;
    attr2.open(root_group2.id(), attr_key2);

    // read in the typename from the attributes opaque datatype
    HDF5DataType data_type2;
    data_type2.getFromAttribute(attr2.id());
    String type_name2;
    data_type2.getOpaqueTag(type_name2);

    // read the value
    TypedAttribute<T> tm2;
    tm2.read(attr2, data_type2.id());

    file2.close();

    CPPUNIT_ASSERT(tm.value() == T(1));
    CPPUNIT_ASSERT(tm2.value() == T(2));
}

//------------------------------------------------------------------------------

template <typename T>
void
TestPrimitiveAttributes<T>::testComparisonOperators()
{
    USING_NKHIVE_NS

    Attribute::shared_ptr m(new TypedAttribute<T>(1));
    Attribute::shared_ptr m2(new TypedAttribute<T>(2));

    CPPUNIT_ASSERT(m->operator!=(*m2));

    (boost::static_pointer_cast<TypedAttribute<T>, Attribute>(m2))->value() = 1;

    CPPUNIT_ASSERT(m->operator==(*m2));
}

//------------------------------------------------------------------------------
