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
// TestLocalXform.cpp
//------------------------------------------------------------------------------

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <nkbase/Tolerance.h>

#include <nkhive/io/VolumeFile.h>
#include <nkhive/xforms/LocalXform.h>

class TestLocalXform : public CppUnit::TestFixture {

    CPPUNIT_TEST_SUITE(TestLocalXform);
    CPPUNIT_TEST(test);
    CPPUNIT_TEST(testScaling);
    CPPUNIT_TEST(testComparisonOperators);
    CPPUNIT_TEST(testIO);
    CPPUNIT_TEST(testIOHDF5);
    CPPUNIT_TEST_SUITE_END();
    
public:
    void setUp() {}
    void tearDown() {}
    
    void test();
    void testScaling();
    void testComparisonOperators();
    void testIO();
    void testIOHDF5();
};

//-----------------------------------------------------------------------------
// test suite registration
//-----------------------------------------------------------------------------

CPPUNIT_TEST_SUITE_REGISTRATION(TestLocalXform);

//------------------------------------------------------------------------------
// tests
//------------------------------------------------------------------------------

void
TestLocalXform::test()
{
    USING_NK_NS
    USING_NKHIVE_NS
    
    LocalXform xform;

    CPPUNIT_ASSERT(xform.res() == vec3d(1, 1, 1));

    LocalXform xform2(vec3d(1, 2, 3));
    CPPUNIT_ASSERT(xform2.res() == vec3d(1,2,3));

    CPPUNIT_ASSERT(xform.voxelToIndex(vec3d(1.1, 2.5, 3.8)) == vec3i(1, 2, 3));
    CPPUNIT_ASSERT(xform.indexToVoxel(vec3i(1, 2, 3)) == vec3d(1, 2, 3));

    CPPUNIT_ASSERT(
            xform.voxelToIndex(vec3d(-0.2, -1.1, -0.8)) == vec3i(-1, -2, -1));
}

//------------------------------------------------------------------------------

void 
TestLocalXform::testScaling()
{
    USING_NK_NS
    USING_NKHIVE_NS

    LocalXform xform(vec3d(0.1, 0.2, 0.5));

    CPPUNIT_ASSERT(xform.localToVoxel(vec3d(0, 0, 0)) == vec3d(0, 0, 0));
    CPPUNIT_ASSERT(xform.localToVoxel(vec3d(1, 1, 1)) == vec3d(0.1, 0.2, 0.5));
    CPPUNIT_ASSERT(xform.localToVoxel(vec3d(10, 5, 2)) == vec3d(1, 1, 1));

    CPPUNIT_ASSERT(xform.voxelToLocal(vec3d(10, 5, 2)) == vec3d(100, 25, 4));
    CPPUNIT_ASSERT(xform.voxelToLocal(vec3d(1, 1, 1)) == vec3d(10, 5, 2));
    CPPUNIT_ASSERT(xform.voxelToLocal(vec3d(0.1, 0.2, 0.5)) == vec3d(1, 1, 1));
    CPPUNIT_ASSERT(xform.voxelToLocal(vec3d(0, 0, 0)) == vec3d(0, 0, 0));
}

//------------------------------------------------------------------------------

void 
TestLocalXform::testComparisonOperators()
{
    USING_NK_NS
    USING_NKHIVE_NS

    LocalXform xform(vec3d(0.1, 0.2, 0.3));
    LocalXform xform2(vec3d(0.1, 0.2, 0.3));
    LocalXform xform3(vec3d(0.1, 0.1, 0.1));

    CPPUNIT_ASSERT(xform == xform2);
    CPPUNIT_ASSERT(xform != xform3);
    CPPUNIT_ASSERT(xform2 != xform3);
    CPPUNIT_ASSERT(xform3 != xform);
}

//------------------------------------------------------------------------------

void
TestLocalXform::testIO()
{
    USING_NK_NS
    USING_NKHIVE_NS

    LocalXform xform(vec3d(0.1f, 0.2f, 0.5f));

    LocalXform xform2;

    CPPUNIT_ASSERT(xform != xform2);

    std::ostringstream ostr(std::ios_base::binary);
    xform.write(ostr);
    std::istringstream istr(ostr.str(), std::ios_base::binary);
    xform2.read(istr);

    CPPUNIT_ASSERT(xform == xform2);
}

//------------------------------------------------------------------------------

void
TestLocalXform::testIOHDF5()
{
    USING_NK_NS
    USING_NKHIVE_NS

    LocalXform xform(vec3d(0.1, 0.2, 0.5));

    LocalXform xform2;

    CPPUNIT_ASSERT(xform != xform2);

    remove("testingHDF5.hv");                                   
    VolumeFile file("testingHDF5.hv", VoidFile::WRITE_TRUNC);  
    
    HDF5Group root_group;                                       
    root_group.open(file.m_id, NK_NS::String("/"));                
    HDF5Id root_group_id = root_group.id();                      
    
    xform.write(root_group_id);
    
    file.close();                                               
    
    VolumeFile file2("testingHDF5.hv", VoidFile::READ_ONLY);    
    HDF5Group root_group2;                                      
    root_group2.open(file2.m_id, NK_NS::String("/"));              
    HDF5Id root_group_id2 = root_group2.id();                    
    
    xform2.read(root_group_id2);
    CPPUNIT_ASSERT(xform == xform2);
    
    file2.close();                                              
    remove("testingHDF5.hv");
}

//------------------------------------------------------------------------------
