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
// TestAttributeCollection.cpp
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <nkhive/attributes/AttributeCollection.h>
#include <nkhive/attributes/StringAttribute.h>
#include <nkhive/attributes/PrimitiveTypes.h>
#include <nkhive/io/VolumeFile.h>

//------------------------------------------------------------------------------
// class definition
//------------------------------------------------------------------------------

class TestAttributeCollection : public CppUnit::TestFixture 
{

    CPPUNIT_TEST_SUITE(TestAttributeCollection);
    CPPUNIT_TEST(testInsert);
    CPPUNIT_TEST(testRemove);
    CPPUNIT_TEST(testAttributeAccess);
    CPPUNIT_TEST(testCopyConstructor);
    CPPUNIT_TEST(testEmptyCopyConstructor);
    CPPUNIT_TEST(testCopyOperator);
    CPPUNIT_TEST(testComparisonOperators);
    CPPUNIT_TEST(testEmptyIO);
    CPPUNIT_TEST(testEmptyIOHDF5);
    CPPUNIT_TEST(testIO);
    CPPUNIT_TEST(testIOHDF5);
    CPPUNIT_TEST_SUITE_END();
    
public:
    void setUp() { remove("testingHDF5.hv"); }
    void tearDown() { remove("testingHDF5.hv"); }
    
    void testInsert();
    void testRemove();
    void testAttributeAccess();
    void testCopyConstructor();
    void testEmptyCopyConstructor();
    void testCopyOperator();
    void testComparisonOperators();
    void testEmptyIO();
    void testEmptyIOHDF5();
    void testIO();
    void testIOHDF5();
};

//------------------------------------------------------------------------------
// test suite registration
//------------------------------------------------------------------------------

CPPUNIT_TEST_SUITE_REGISTRATION(TestAttributeCollection);

//-----------------------------------------------------------------------------
// test suite container
//-----------------------------------------------------------------------------

void
TestAttributeCollection::testInsert()
{
    USING_NK_NS
    USING_NKHIVE_NS

    AttributeCollection collection;
    collection.insert("meta1", StringAttribute("testing"));
    collection.insert("meta2", Int32Attribute(20));
    collection.insert("meta3", FloatAttribute(2.0));

    AttributeCollection::iterator iter = collection.begin();
    int i = 1;
    for ( ; iter != collection.end(); ++iter, ++i) {
        if (i == 1) {
            CPPUNIT_ASSERT(iter->first.compare("meta1") == 0);
            String val = collection.value<String>("meta1");
            CPPUNIT_ASSERT(val == String("testing"));
        } else if (i == 2) {
            CPPUNIT_ASSERT(iter->first.compare("meta2") == 0);
            int32_t val = collection.value<int32_t>("meta2");
            CPPUNIT_ASSERT(val == 20);
        } else if (i == 3) {
            CPPUNIT_ASSERT(iter->first.compare("meta3") == 0);
            float val = collection.value<float>("meta3");
            CPPUNIT_ASSERT(val == 2.0);
        }
    }
}

//------------------------------------------------------------------------------

void
TestAttributeCollection::testRemove()
{
    USING_NK_NS
    USING_NKHIVE_NS

    AttributeCollection collection;
    collection.insert("meta1", StringAttribute("testing"));
    collection.insert("meta2", Int32Attribute(20));
    collection.insert("meta3", FloatAttribute(2.0));
        
    collection.remove("meta2");

    AttributeCollection::iterator iter = collection.begin();
    int i = 1;
    for ( ; iter != collection.end(); ++iter, ++i) {
        if (i == 1) {
            CPPUNIT_ASSERT(iter->first.compare("meta1") == 0);
            String val = collection.value<String>("meta1");
            CPPUNIT_ASSERT(val == String("testing"));
        } else if (i == 2) {
            CPPUNIT_ASSERT(iter->first.compare("meta3") == 0);
            float val = collection.value<float>("meta3");
            CPPUNIT_ASSERT(val == 2.0);
        }        
    }            
                 
    collection.remove("meta1");
    
    iter = collection.begin();
    for ( ; iter != collection.end(); ++iter, ++i) {
        CPPUNIT_ASSERT(iter->first.compare("meta3") == 0);
        float val = collection.value<float>("meta3");
        CPPUNIT_ASSERT(val == 2.0);
    }

    collection.remove("meta3");

    CPPUNIT_ASSERT(collection.empty());
}

//------------------------------------------------------------------------------

void
TestAttributeCollection::testAttributeAccess()
{
    USING_NK_NS
    USING_NKHIVE_NS

    AttributeCollection collection;
    collection.insert("meta1", StringAttribute("testing"));
    collection.insert("meta2", Int32Attribute(20));
    collection.insert("meta3", FloatAttribute(2.0));

    Attribute &attr = collection["meta2"];
    CPPUNIT_ASSERT(attr.typeName() == Int32Attribute::staticTypeName());

    FloatAttribute::shared_ptr fa = 
        collection.findTypedAttribute<FloatAttribute>("meta3");
    CPPUNIT_ASSERT(fa->value() == 2.0);

    const FloatAttribute::shared_ptr cfa = 
        collection.findTypedAttribute<FloatAttribute>("meta3");
    CPPUNIT_ASSERT(cfa->value() == 2.0);

    CPPUNIT_ASSERT(collection.findTypedAttribute<StringAttribute>("meta2") ==
            NULL);

    CPPUNIT_ASSERT_THROW(collection.typedAttribute<Int32Attribute>("meta3"), 
                         Iex::TypeExc);

    CPPUNIT_ASSERT_THROW(collection.typedAttribute<Int32Attribute>("meta5"), 
                         Iex::ArgExc);

    CPPUNIT_ASSERT_THROW(collection.value<int32_t>("meta3"), 
                         Iex::TypeExc);

    CPPUNIT_ASSERT_THROW(collection.value<int32_t>("meta5"), 
                         Iex::ArgExc);
}

//------------------------------------------------------------------------------

void
TestAttributeCollection::testCopyConstructor()
{
    USING_NK_NS
    USING_NKHIVE_NS

    AttributeCollection collection;
    collection.insert("meta1", StringAttribute("testing"));
    collection.insert("meta2", Int32Attribute(20));
    collection.insert("meta3", FloatAttribute(2.0));

    // copy constructor
    AttributeCollection collection2(collection);

    CPPUNIT_ASSERT(collection.count() == collection2.count());

    std::string str = collection.value<String>("meta1");
    std::string str2 = collection2.value<String>("meta1");
    CPPUNIT_ASSERT(str == str2);

    CPPUNIT_ASSERT(collection.value<int32_t>("meta2") ==
            collection2.value<int32_t>("meta2"));

    CPPUNIT_ASSERT(collection.value<float>("meta3") ==
            collection2.value<float>("meta3"));
}

//------------------------------------------------------------------------------

void
TestAttributeCollection::testEmptyCopyConstructor()
{
    USING_NK_NS
    USING_NKHIVE_NS

    AttributeCollection collection;

    // copy constructor
    AttributeCollection collection2(collection);

    CPPUNIT_ASSERT(collection.count() == 0);
    CPPUNIT_ASSERT(collection.count() == collection2.count());
}

//------------------------------------------------------------------------------

void
TestAttributeCollection::testCopyOperator()
{
    USING_NK_NS
    USING_NKHIVE_NS

    AttributeCollection collection;
    collection.insert("meta1", StringAttribute("testing"));
    collection.insert("meta2", Int32Attribute(20));
    collection.insert("meta3", FloatAttribute(2.0));

    AttributeCollection collection2;

    collection2 = collection;

    CPPUNIT_ASSERT(collection.count() == 3);
    CPPUNIT_ASSERT(collection2.count() == collection.count());

    AttributeCollection collection3;
    collection3.insert("meta5", StringAttribute("something"));
    collection3.insert("meta6", StringAttribute("something"));
    collection3.insert("meta7", StringAttribute("something"));
    collection3.insert("meta8", StringAttribute("something"));
    collection3.insert("meta9", StringAttribute("something"));

    CPPUNIT_ASSERT(collection3.count() == 5);

    collection3 = collection;

    CPPUNIT_ASSERT(collection.count() == 3);
    CPPUNIT_ASSERT(collection3.count() == collection.count());

    CPPUNIT_ASSERT_THROW(collection3.typedAttribute<StringAttribute>("meta5"), 
            Iex::ArgExc);
    CPPUNIT_ASSERT_THROW(collection3.typedAttribute<StringAttribute>("meta6"), 
            Iex::ArgExc);
    CPPUNIT_ASSERT_THROW(collection3.typedAttribute<StringAttribute>("meta7"), 
            Iex::ArgExc);
    CPPUNIT_ASSERT_THROW(collection3.typedAttribute<StringAttribute>("meta8"), 
            Iex::ArgExc);
    CPPUNIT_ASSERT_THROW(collection3.typedAttribute<StringAttribute>("meta9"), 
            Iex::ArgExc);

    CPPUNIT_ASSERT(collection3.value<String>("meta1") == 
                   collection.value<String>("meta1"));
    CPPUNIT_ASSERT(collection3.value<int32_t>("meta2") == 
                   collection.value<int32_t>("meta2"));
    CPPUNIT_ASSERT(collection3.value<float>("meta3") == 
                   collection.value<float>("meta3"));
}

//------------------------------------------------------------------------------

void
TestAttributeCollection::testComparisonOperators()
{
    USING_NK_NS
    USING_NKHIVE_NS

    AttributeCollection c;
    c.insert("meta1", StringAttribute("testing"));
    c.insert("meta2", Int32Attribute(20));
    c.insert("meta3", FloatAttribute(2.0));
    c.insert("meta4", DoubleAttribute(4.0));

    {
        AttributeCollection c2;
        CPPUNIT_ASSERT(c != c2);
        CPPUNIT_ASSERT(c2 != c);

        AttributeCollection c3;
        CPPUNIT_ASSERT(c2 == c3);
        CPPUNIT_ASSERT(c3 == c2);
    }

    {
        AttributeCollection c2;
        c2.insert("meta1", StringAttribute("testing"));
        c2.insert("meta2", Int32Attribute(20));
        c2.insert("meta3", FloatAttribute(2.0));

        CPPUNIT_ASSERT(c != c2);
        CPPUNIT_ASSERT(c2 != c);

        c2.insert("meta4", DoubleAttribute(4.0));
        CPPUNIT_ASSERT(c == c2);
        CPPUNIT_ASSERT(c2 == c);

        c2.insert("meta5", FloatAttribute(3.0));
        CPPUNIT_ASSERT(c != c2);
        CPPUNIT_ASSERT(c2 != c);
    }

    {
        AttributeCollection c2;
        c2.insert("meta2", StringAttribute("testing"));
        c2.insert("meta1", Int32Attribute(20));
        c2.insert("meta3", FloatAttribute(2.0));
        c2.insert("meta4", DoubleAttribute(4.0));
        CPPUNIT_ASSERT(c != c2);
        CPPUNIT_ASSERT(c2 != c);
    }
}

//------------------------------------------------------------------------------

void
TestAttributeCollection::testEmptyIO()
{
    USING_NK_NS
    USING_NKHIVE_NS

    AttributeCollection c;

    // Write out empty collection
    std::ostringstream ostr(std::ios_base::binary);
    c.write(ostr);

    // read in the attribute.
    AttributeCollection c2;
    std::istringstream istr(ostr.str(), std::ios_base::binary);

    CPPUNIT_ASSERT_NO_THROW(c2.read(istr));

    CPPUNIT_ASSERT(c.count() == 0);
    CPPUNIT_ASSERT(c == c2);
}

//------------------------------------------------------------------------------

void
TestAttributeCollection::testEmptyIOHDF5()
{
    USING_NK_NS
    USING_NKHIVE_NS

    AttributeCollection c;

    // open a dummy volume file
    VolumeFile file("testingHDF5.hv", VoidFile::WRITE_TRUNC);       
    HDF5Group root_group;                                           
    root_group.open(file.m_id, String("/"));                    
    HDF5Id root_group_id = root_group.id();                          
    HDF5Group volume_group;                                         
    volume_group.create(root_group_id, "Volume1");                  

    // write out empty collection
    c.write(volume_group.id());
    file.close();                                                   
    
    VolumeFile file2("testingHDF5.hv", VoidFile::READ_ONLY);                             
    HDF5Group root_group2;                                          
    root_group2.open(file2.m_id, String("/"));                  
    HDF5Id root_group_id2 = root_group2.id();                        
    HDF5Group volume_group2;                                        
    volume_group2.open(root_group_id2, "Volume1");                  

    // read in the attribute collection
    AttributeCollection c2;
    CPPUNIT_ASSERT_NO_THROW(c2.read(volume_group2.id()));
    CPPUNIT_ASSERT(c.count() == 0);
    CPPUNIT_ASSERT(c == c2);
    
    file2.close();                                                  
}

//------------------------------------------------------------------------------

void
TestAttributeCollection::testIO()
{
    USING_NK_NS
    USING_NKHIVE_NS

    AttributeCollection c;
    c.insert("meta1", StringAttribute("testing"));
    c.insert("meta2", Int32Attribute(20));
    c.insert("meta3", DoubleAttribute(2.0));

    std::ostringstream ostr(std::ios_base::binary);

    c.write(ostr);

    AttributeCollection c2;

    std::istringstream istr(ostr.str(), std::ios_base::binary);

    CPPUNIT_ASSERT_THROW(c2.read(istr), Iex::TypeExc);

    // Register the three types.
    Attribute::clearAttributeRegistry();
    StringAttribute::registerAttributeType();
    Int32Attribute::registerAttributeType();
    DoubleAttribute::registerAttributeType();

    // Now seek to beginning and read again.
    istr.seekg(0, std::ios_base::beg);

    CPPUNIT_ASSERT_NO_THROW(c2.read(istr));

    CPPUNIT_ASSERT(c == c2);

    // Clear the registry once the test is done.
    Attribute::clearAttributeRegistry();
}

//------------------------------------------------------------------------------

void
TestAttributeCollection::testIOHDF5()
{
    USING_NK_NS
    USING_NKHIVE_NS

    AttributeCollection c;
    c.insert("meta1", StringAttribute("testing"));
    c.insert("meta2", Int32Attribute(20));
    c.insert("meta3", DoubleAttribute(2.0));

    // open a dummy volume file
    VolumeFile file("testingHDF5.hv", VoidFile::WRITE_TRUNC);       
    HDF5Group root_group;                                           
    root_group.open(file.m_id, String("/"));                    
    HDF5Id root_group_id = root_group.id();                          
    HDF5Group volume_group;                                         
    volume_group.create(root_group_id, "Volume1");                  

    // write out attribute collection
    c.write(volume_group.id());
    file.close();                                                   
    
    VolumeFile file2("testingHDF5.hv", VoidFile::READ_ONLY);                             
    HDF5Group root_group2;                                          
    root_group2.open(file2.m_id, String("/"));                  
    HDF5Id root_group_id2 = root_group2.id();                        
    HDF5Group volume_group2;                                        
    volume_group2.open(root_group_id2, "Volume1");                  

    // read in the attribute collection
    AttributeCollection c2;
    CPPUNIT_ASSERT_THROW(c2.read(volume_group2.id()), Iex::TypeExc);
    
    // Register the three types.
    Attribute::clearAttributeRegistry();
    StringAttribute::registerAttributeType();
    Int32Attribute::registerAttributeType();
    DoubleAttribute::registerAttributeType();
   
    // now read again 
    CPPUNIT_ASSERT_NO_THROW(c2.read(volume_group2.id()));
    CPPUNIT_ASSERT(c == c2);
    
    file2.close();                                                  

    // Clear the registry once the test is done.
    Attribute::clearAttributeRegistry();
}

//------------------------------------------------------------------------------
