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
// TestVolumeFile.cpp
//------------------------------------------------------------------------------

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <nkbase/Tolerance.h>
#include <nkhive/attributes/PrimitiveTypes.h>
#include <nkhive/attributes/StringAttribute.h>
#include <nkhive/volume/Volume.h>
#include <nkhive/io/VolumeFile.h>

class TestVolumeFile : public CppUnit::TestFixture 
{
    CPPUNIT_TEST_SUITE(TestVolumeFile);
    CPPUNIT_TEST(testSingleVolume);
    CPPUNIT_TEST(testReadWriteSingleVolumeOverwriteEmpty);
    CPPUNIT_TEST(testReadWriteSingleVolume);
    CPPUNIT_TEST(testMultipleVolume);
    CPPUNIT_TEST(testReadWrite);
    CPPUNIT_TEST(testSingleVolumeStream);
    CPPUNIT_TEST_SUITE_END();
    
public:
    void setUp() { remove("testingHDF5.hv"); 
                   remove("empty.hv");
                   remove("filled.hv");
                   remove("deleted.hv");
    }
    void tearDown() { remove("testingHDF5.hv"); 
                      remove("empty.hv");
                      remove("filled.hv");
                      remove("deleted.hv");
    }
    
    void testSingleVolume();
    void testMultipleVolume();
    void testReadWrite();
    void testReadWriteSingleVolume();
    void testReadWriteSingleVolumeOverwriteEmpty();
    void testSingleVolumeStream();
};

//-----------------------------------------------------------------------------
// test suite registration
//-----------------------------------------------------------------------------

CPPUNIT_TEST_SUITE_REGISTRATION(TestVolumeFile);

//------------------------------------------------------------------------------
// tests
//------------------------------------------------------------------------------

void
TestVolumeFile::testSingleVolume()
{
    USING_NK_NS
    USING_NKHIVE_NS
   
    Volume<float> volume(2, 2, 1.0f);

    // register the string attributes for io
    StringAttribute::registerAttributeType();

    // Check default attributes
    CPPUNIT_ASSERT(volume.getAttributeCollection().count() == 2);
    AttributeCollection &c = volume.getAttributeCollection();
    CPPUNIT_ASSERT(c.value<String>(kVolumeNameAttr) == String("unknown"));
    CPPUNIT_ASSERT(c.value<String>(kVolumeDescAttr) == String(""));

    volume.set(16, 0, 0, 2.0f);
    volume.set(64, 123, 64, 2.0f);
    volume.set(-64, -23, -1, 2.0f);
    volume.set(-64, -23, 1, 2.0f);
    volume.set(-64, 23, 1, 2.0f);
    volume.set(64, 23, 1, 2.0f);
    volume.set(64, -23, 1, 2.0f);
    volume.set(64, 23, -1, 2.0f);
    volume.set(64, -23, -1, 2.0f);
    volume.set(-64, 23, -1, 2.0f);

    // write the volume out
    VolumeFile file("testingHDF5.hv", VoidFile::WRITE_TRUNC);
    file.setVersion(1, 2, 3);
    file.write(volume);
    file.close();

    // open the file on disk now
    VolumeFile file2("testingHDF5.hv", VoidFile::WRITE_TRUNC);
    file2.setVersion(1, 2, 3);

    // write over an existing file
    file2.write(volume);
    file2.close();

    // open the file again
    VolumeFile file3("testingHDF5.hv", VoidFile::READ_ONLY);
    Volume<float>::shared_ptr v = file3.read<Volume<float> >();
    
    // test the version
    VoidFile::Version version = file3.getVersion();
    CPPUNIT_ASSERT(version.major    == 1);
    CPPUNIT_ASSERT(version.minor    == 2);
    CPPUNIT_ASSERT(version.revision == 3);
    
    file3.close();

    // check the rest of the volume
    CPPUNIT_ASSERT(v->operator==(volume));

    Attribute::clearAttributeRegistry();
}

//------------------------------------------------------------------------------

void
TestVolumeFile::testReadWriteSingleVolume()
{
    USING_NK_NS
    USING_NKHIVE_NS
   
    Volume<float> volume(2, 2, 1.0f);

    // register the string attributes for io
    StringAttribute::registerAttributeType();
    
    String name("volume1");
    volume.setName(name);
    
    // add some data
    volume.set(16, 0, 0, 2.0f);
    volume.set(64, 123, 64, 2.0f);
    volume.set(-64, -23, -1, 2.0f);
    volume.set(-64, -23, 1, 2.0f);
    volume.set(-64, 23, 1, 2.0f);
    volume.set(64, 23, 1, 2.0f);
    volume.set(64, -23, 1, 2.0f);
    volume.set(64, 23, -1, 2.0f);
    volume.set(64, -23, -1, 2.0f);
    volume.set(-64, 23, -1, 2.0f);
    
    // write new file with READ_WRITE
    VolumeFile file("testingHDF5.hv", VoidFile::READ_WRITE);
    file.write(volume);
    file.close();

    // read with READ_WRITE
    VolumeFile file2("testingHDF5.hv", VoidFile::READ_WRITE);
    Volume<float>::shared_ptr v_read = file2.read<Volume<float> >(0);

    // check equality
    CPPUNIT_ASSERT(v_read->operator==(volume));

    // modify volume read in
    v_read->set(16, 16, 0, 2.0f);
    v_read->set(64, -123, 64, 2.0f);

    // overwrite volume in file
    file2.write(*v_read);
    file2.close();
    
    // open same file with read write again
    VolumeFile file3("testingHDF5.hv", VoidFile::READ_WRITE);
    Volume<float>::shared_ptr v_read2 = file3.read<Volume<float> >(0);
    file3.close();

    // check equality
    CPPUNIT_ASSERT(v_read2->operator==(*v_read));

    // open file again
    VolumeFile file4("testingHDF5.hv", VoidFile::READ_WRITE);
    Volume<float>::shared_ptr v_read3 = file4.read<Volume<float> >(0);

    // create a new volume
    Volume<float> volume2(2, 2, 1.0f);
    String name2("volume2");
    volume2.setName(name2);

    // add some data
    volume2.set(-16, 0, 0, 2.0f);
    volume2.set(-64, 123, -64, 2.0f);
    volume2.set(-64, -23, 1, 2.0f);
    volume2.set(-64, 22, 1, 2.0f);
    volume2.set(-64, 21, 1, 2.0f);

    // write this volume to open file. Should go in slot 1.
    file4.write(volume2);
    file4.close();
    
    // open the file again
    VolumeFile file5("testingHDF5.hv", VoidFile::READ_WRITE);
    Volume<float>::shared_ptr v_read4 = file5.read<Volume<float> >(0);
    Volume<float>::shared_ptr v_read5 = file5.read<Volume<float> >(1);
    file5.close();

    // test both for equality
    CPPUNIT_ASSERT(v_read4->operator==(*v_read));
    CPPUNIT_ASSERT(v_read5->operator==(volume2));
    
    Attribute::clearAttributeRegistry();
}

//------------------------------------------------------------------------------

void
TestVolumeFile::testReadWriteSingleVolumeOverwriteEmpty()
{
    USING_NK_NS
    USING_NKHIVE_NS
   
    Volume<float> volume(2, 2, 1.0f);

    // register the string attributes for io
    StringAttribute::registerAttributeType();

    // save out an empty volume
    VolumeFile file("empty.hv", VoidFile::WRITE_TRUNC);
    file.write(volume);
    file.close();

    // add some data
    volume.set(16, 0, 0, 2.0f);
    volume.set(64, 123, 64, 2.0f);
    volume.set(-64, -23, -1, 2.0f);
    volume.set(-64, -23, 1, 2.0f);
    volume.set(-64, 23, 1, 2.0f);
    volume.set(64, 23, 1, 2.0f);
    volume.set(64, -23, 1, 2.0f);
    volume.set(64, 23, -1, 2.0f);
    volume.set(64, -23, -1, 2.0f);
    volume.set(-64, 23, -1, 2.0f);

    // write out volume with data
    VolumeFile file2("filled.hv", VoidFile::WRITE_TRUNC);
    file2.write(volume);
    file2.close();

    // write out another filled
    VolumeFile file3("deleted.hv", VoidFile::WRITE_TRUNC);
    file3.write(volume);
    file3.close();

    // read in to delete
    VolumeFile file4("deleted.hv", VoidFile::READ_WRITE);
    Volume<float>::shared_ptr v_read = file4.read<Volume<float> >(0);

    // check equality
    CPPUNIT_ASSERT(v_read->operator==(volume));

    // create an empty volume
    Volume<float> empty_volume(2, 2, 1.0f);

    // write it out over top
    // should overwrite since they both have default name
    file4.write(empty_volume);
    file4.close();

    // read in the freshly written empty volume file
    VolumeFile file5("deleted.hv", VoidFile::READ_ONLY);
    Volume<float>::shared_ptr v_read2 = file5.read<Volume<float> >(0);
    file5.close();

    // check equality
    CPPUNIT_ASSERT(v_read2->operator==(empty_volume));

    Attribute::clearAttributeRegistry();
}

//------------------------------------------------------------------------------
 
void
TestVolumeFile::testMultipleVolume()
{
    USING_NK_NS
    USING_NKHIVE_NS
    
    // register the string attributes for io
    StringAttribute::registerAttributeType();
    
    Volume<float> volume(2, 2, 1.0f);
    Volume<float> volume2(3, 3, 1.0f);

    String volume_name("volume1");
    String volume2_name("volume2");

    volume.setName(volume_name);
    volume2.setName(volume2_name);

    volume.set(16, 0, 0, 2.0f);
    volume.set(64, 123, 64, 2.0f);
    volume.set(-64, -23, -1, 2.0f);
    volume.set(-64, -23, 1, 2.0f);
    volume.set(-64, 23, 1, 2.0f);
    volume.set(64, 23, 1, 2.0f);
    volume2.set(64, -23, 1, 2.0f);
    volume2.set(64, 23, -1, 2.0f);
    volume2.set(64, -23, -1, 2.0f);
    volume2.set(-64, 23, -1, 2.0f);

    // write both volumes out to same file
    VolumeFile file("testingHDF5.hv", VoidFile::WRITE_TRUNC);
    file.write(volume);
    file.write(volume2);
    file.close();
    
    VolumeFile file2("testingHDF5.hv", VoidFile::READ_ONLY);
    
    // test random access read
    Volume<float>::shared_ptr v = file2.read<Volume<float> >(0);
    CPPUNIT_ASSERT(v->operator==(volume));

    v = file2.read<Volume<float> >(1);
    CPPUNIT_ASSERT(v->operator==(volume2));
   
    // test read by name
    v = file2.read<Volume<float> >(volume_name); 
    CPPUNIT_ASSERT(v->operator==(volume));
    
    v = file2.read<Volume<float> >(volume2_name); 
    CPPUNIT_ASSERT(v->operator==(volume2));

    file2.close();

    Attribute::clearAttributeRegistry();
}

//------------------------------------------------------------------------------

void
TestVolumeFile::testReadWrite()
{
    USING_NK_NS
    USING_NKHIVE_NS

    // register the string attributes for io
    StringAttribute::registerAttributeType();
    
    Volume<float> volume(2, 2, 1.0f);
    Volume<float> volume2(3, 3, 1.0f);

    String volume_name("volume1");
    String volume2_name("volume2");

    volume.setName(volume_name);
    volume2.setName(volume2_name);

    volume.set(16, 0, 0, 2.0f);
    volume.set(64, 123, 64, 2.0f);
    volume.set(-64, -23, -1, 2.0f);
    volume.set(-64, -23, 1, 2.0f);
    volume.set(-64, 23, 1, 2.0f);
    volume.set(64, 23, 1, 2.0f);
    volume2.set(64, -23, 1, 2.0f);
    volume2.set(64, 23, -1, 2.0f);
    volume2.set(64, -23, -1, 2.0f);
    volume2.set(-64, 23, -1, 2.0f);

    // write both volumes out to same file
    VolumeFile file("testingHDF5.hv", VoidFile::WRITE_TRUNC);
    file.write(volume);
    file.write(volume2);

    // now modify volume2
    volume2.set(64, 23, 1, 2.0f);

    // write over open file
    file.write(volume2);

    file.close(); 
   
    // reopen 
    VolumeFile file2("testingHDF5.hv", VoidFile::READ_ONLY);

    // read volume2 and test for equality
    Volume<float>::shared_ptr v = file2.read<Volume<float> >(volume2_name); 
    CPPUNIT_ASSERT(v->operator==(volume2));

    file2.close();

    Attribute::clearAttributeRegistry();
}

//------------------------------------------------------------------------------

void
TestVolumeFile::testSingleVolumeStream()
{
    USING_NK_NS
    USING_NKHIVE_NS
   
    Volume<float> volume(2, 2, 1.0f);

    // register the string attributes for io
    StringAttribute::registerAttributeType();

    // Check default attributes
    CPPUNIT_ASSERT(volume.getAttributeCollection().count() == 2);
    AttributeCollection &c = volume.getAttributeCollection();
    CPPUNIT_ASSERT(c.value<String>(kVolumeNameAttr) == String("unknown"));
    CPPUNIT_ASSERT(c.value<String>(kVolumeDescAttr) == String(""));

    volume.set(16, 0, 0, 2.0f);
    volume.set(64, 123, 64, 2.0f);
    volume.set(-64, -23, -1, 2.0f);
    volume.set(-64, -23, 1, 2.0f);
    volume.set(-64, 23, 1, 2.0f);
    volume.set(64, 23, 1, 2.0f);
    volume.set(64, -23, 1, 2.0f);
    volume.set(64, 23, -1, 2.0f);
    volume.set(64, -23, -1, 2.0f);
    volume.set(-64, 23, -1, 2.0f);

    // write the volume out to stream
    VolumeFile file;
    file.setVersion(1, 2, 3);
    std::ostringstream ostr(std::ios_base::binary);             
    file.write(ostr, volume);
    file.close();

    // read in file from stream
    VolumeFile file2;
    std::istringstream istr(ostr.str(), std::ios_base::binary);
    Volume<float>::shared_ptr v = file2.read< Volume<float> >(istr);

    // test the version
    VoidFile::Version version = file2.getVersion();
    CPPUNIT_ASSERT(version.major    == 1);
    CPPUNIT_ASSERT(version.minor    == 2);
    CPPUNIT_ASSERT(version.revision == 3);
    
    // check the rest of the volume
    CPPUNIT_ASSERT(v->operator==(volume));

    Attribute::clearAttributeRegistry();
}

//------------------------------------------------------------------------------
