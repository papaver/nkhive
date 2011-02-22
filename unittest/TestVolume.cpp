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
// TestVolume.cpp
//------------------------------------------------------------------------------

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <nkbase/Tolerance.h>

#include <nkhive/attributes/PrimitiveTypes.h>
#include <nkhive/attributes/StringAttribute.h>
#include <nkhive/volume/Volume.h>
#include <nkhive/io/VolumeFile.h>

//------------------------------------------------------------------------------
// definitions
//------------------------------------------------------------------------------

#define TOL_CHECK(v1, v2) \
    CPPUNIT_ASSERT(fabs(v1 - v2) < Tolerance<double>::zero());

//------------------------------------------------------------------------------
// class definition
//------------------------------------------------------------------------------

template <typename T>
class TestVolume : public CppUnit::TestFixture 
{
    CPPUNIT_TEST_SUITE(TestVolume);
    CPPUNIT_TEST(test);
    CPPUNIT_TEST(testUnset);
    CPPUNIT_TEST(testIsEmpty);
    CPPUNIT_TEST(testLocalXform);
    CPPUNIT_TEST(testAttributes);
    CPPUNIT_TEST(testIO);
    CPPUNIT_TEST(testIOHDF5);
    CPPUNIT_TEST(testOperatorComparison);
    CPPUNIT_TEST(testSetIterator);
    CPPUNIT_TEST(testComputeSetBounds);
    CPPUNIT_TEST_SUITE_END();
    
public:
    void setUp() {}
    void tearDown() {}
    
    void test();
    void testUnset();
    void testIsEmpty();
    void testLocalXform();
    void testAttributes();
    void testIO();
    void testIOHDF5();
    void testOperatorComparison();
    void testSetIterator();
    void testComputeSetBounds();
};

//-----------------------------------------------------------------------------
// test suite registration
//-----------------------------------------------------------------------------

CPPUNIT_TEST_SUITE_REGISTRATION(TestVolume<float>);
CPPUNIT_TEST_SUITE_REGISTRATION(TestVolume<double>);
CPPUNIT_TEST_SUITE_REGISTRATION(TestVolume<int32_t>);

//------------------------------------------------------------------------------
// tests
//------------------------------------------------------------------------------

template <typename T>
void
TestVolume<T>::test()
{
    USING_NK_NS
    USING_NKHIVE_NS
    
    Volume<T> volume(2, 2, T(1));

    CPPUNIT_ASSERT(volume.getDefault() == T(1));

    // Check default attributes
    CPPUNIT_ASSERT(volume.getAttributeCollection().count() == 2);
    AttributeCollection &c = volume.getAttributeCollection();
    CPPUNIT_ASSERT(c.value<String>(kVolumeNameAttr) == String("unknown"));
    CPPUNIT_ASSERT(c.value<String>(kVolumeDescAttr) == String(""));

    volume.set(16, 0, 0, T(2));
    CPPUNIT_ASSERT(volume.get(16, 0, 0) == T(2));
    CPPUNIT_ASSERT(volume.get(0, 0, 0) == T(1));

    volume.set(64, 123, 64, T(2));

    CPPUNIT_ASSERT(volume.get(16, 0, 0) == T(2));
    CPPUNIT_ASSERT(volume.get(64, 123, 64) == T(2));

    volume.set(-64, -23, -1, T(2));
    volume.set(-64, -23, 1, T(2));
    volume.set(-64, 23, 1, T(2));
    volume.set(64, 23, 1, T(2));
    volume.set(64, -23, 1, T(2));
    volume.set(64, 23, -1, T(2));
    volume.set(64, -23, -1, T(2));
    volume.set(signed_index_vec(-64, 23, -1), T(2));

    CPPUNIT_ASSERT(volume.get(16, 0, 0) == T(2));
    CPPUNIT_ASSERT(volume.get(64, 123, 64) == T(2));
    CPPUNIT_ASSERT(volume.get(64, 23, 1) == T(2));
    CPPUNIT_ASSERT(volume.get(64, 23, -1) == T(2));
    CPPUNIT_ASSERT(volume.get(64, -23, 1) == T(2));
    CPPUNIT_ASSERT(volume.get(-64, 23, 1) == T(2));
    CPPUNIT_ASSERT(volume.get(-64, -23, 1) == T(2));
    CPPUNIT_ASSERT(volume.get(-64, 23, -1) == T(2));
    CPPUNIT_ASSERT(volume.get(64, -23, -1) == T(2));
    CPPUNIT_ASSERT(volume.get(signed_index_vec(-64, -23, -1)) == T(2));

    volume.update(64, 23, -1, T(2), std::plus<T>());
    CPPUNIT_ASSERT(volume.get(64, 23, -1) == T(4));
}

//------------------------------------------------------------------------------

template <typename T>
void
TestVolume<T>::testUnset()
{
    USING_NKHIVE_NS

    Volume<T> v(2, 2, T(0));

    v.set(1, 1, 1, T(1));

    CPPUNIT_ASSERT(v.get(1, 1, 1) == T(1));

    v.unset(1, 1, 1);

    CPPUNIT_ASSERT(v.get(1, 1, 1) == T(0));
}

//------------------------------------------------------------------------------


template <typename T>
void
TestVolume<T>::testIsEmpty()
{
    USING_NK_NS
    USING_NKHIVE_NS
    
    // check empty volume
    {
        Volume<T> volume(2, 2, T(1));

        CPPUNIT_ASSERT(volume.isEmpty());
    }

    // check set then unset.
    {
        Volume<T> volume(2, 2, T(1));

        volume.set(-1, 2, 4, T(1));
        volume.unset(-1, 2, 4);
        CPPUNIT_ASSERT(volume.isEmpty());
    }

    // check set in each octant
    {
        Volume<T> volume(2, 2, T(1));

        volume.set(1, 2, 4, T(1));
        CPPUNIT_ASSERT(!volume.isEmpty());
    }

    {
        Volume<T> volume(2, 2, T(1));

        volume.set(-1, 2, 4, T(1));
        CPPUNIT_ASSERT(!volume.isEmpty());
    }

    {
        Volume<T> volume(2, 2, T(1));

        volume.set(-1, -2, 4, T(1));
        CPPUNIT_ASSERT(!volume.isEmpty());
    }

    {
        Volume<T> volume(2, 2, T(1));

        volume.set(-1, -2, -4, T(1));
        CPPUNIT_ASSERT(!volume.isEmpty());
    }

    {
        Volume<T> volume(2, 2, T(1));

        volume.set(-1, 2, -4, T(1));
        CPPUNIT_ASSERT(!volume.isEmpty());
    }

    {
        Volume<T> volume(2, 2, T(1));

        volume.set(1, 2, -4, T(1));
        CPPUNIT_ASSERT(!volume.isEmpty());
    }

    {
        Volume<T> volume(2, 2, T(1));

        volume.set(1, -2, 4, T(1));
        CPPUNIT_ASSERT(!volume.isEmpty());
    }

    {
        Volume<T> volume(2, 2, T(1));

        volume.set(1, -2, -4, T(1));
        CPPUNIT_ASSERT(!volume.isEmpty());
    }
}

//------------------------------------------------------------------------------

template <typename T>
void
TestVolume<T>::testLocalXform()
{
    USING_NK_NS
    USING_NKHIVE_NS

    Volume<T> v(2, 2, T(1), vec3d(0.1, 0.2, 0.5));

    CPPUNIT_ASSERT(v.localToVoxel(vec3d(0, 0, 0)) == vec3d(0, 0, 0));
    CPPUNIT_ASSERT(v.localToIndex(vec3d(0, 0, 0)) == vec3i(0, 0, 0));

    CPPUNIT_ASSERT(v.localToVoxel(vec3d(1, 1, 1)) == vec3d(0.1, 0.2, 0.5));
    CPPUNIT_ASSERT(v.localToIndex(vec3d(1, 1, 1)) == vec3i(0, 0, 0));

    CPPUNIT_ASSERT(v.localToVoxel(vec3d(10, 5, 2)) == vec3d(1, 1, 1));
    CPPUNIT_ASSERT(v.localToIndex(vec3d(10, 5, 2)) == vec3i(1, 1, 1));

    CPPUNIT_ASSERT(v.voxelToLocal(vec3d(10, 5, 2)) == vec3d(100, 25, 4));
    CPPUNIT_ASSERT(v.indexToLocal(vec3i(10, 5, 2)) == vec3d(100, 25, 4));

    CPPUNIT_ASSERT(v.voxelToLocal(vec3d(1, 1, 1)) == vec3d(10, 5, 2));
    CPPUNIT_ASSERT(v.indexToLocal(vec3i(1, 1, 1)) == vec3d(10, 5, 2));

    CPPUNIT_ASSERT(v.voxelToLocal(vec3d(1, 2, 3)) == vec3d(10, 10, 6));
    CPPUNIT_ASSERT(v.indexToLocal(vec3i(1, 2, 3)) == vec3d(10, 10, 6));

    v.setLocalXform(vec3d(1, 1, 1));

    CPPUNIT_ASSERT(v.localToVoxel(vec3d(0, 0, 0)) == vec3d(0, 0, 0));
    CPPUNIT_ASSERT(v.localToVoxel(vec3d(1, 1, 1)) == vec3d(1, 1, 1));

    CPPUNIT_ASSERT(v.voxelToLocal(vec3d(10, 5, 2)) == vec3d(10, 5, 2));
    CPPUNIT_ASSERT(v.voxelToLocal(vec3d(1, 1, 1)) == vec3d(1, 1, 1));
    CPPUNIT_ASSERT(v.voxelToLocal(vec3d(0, 0, 0)) == vec3d(0, 0, 0));
}

//------------------------------------------------------------------------------

template <typename T>
void
TestVolume<T>::testAttributes()
{
    USING_NKHIVE_NS
    
    Volume<T> v(2, 2, T(1));

    v.getAttributeCollection().insert("year", TypedAttribute<int32_t>(2010));

    const AttributeCollection &a = v.getAttributeCollection();
    CPPUNIT_ASSERT(a.value<const int32_t>("year") == 2010);
}

//------------------------------------------------------------------------------

template <typename T>
void 
TestVolume<T>::testOperatorComparison()
{
    USING_NK_NS
    USING_NKHIVE_NS

    Volume<T> v(2, 2,  T(1));
    Volume<T> v2(2, 3, T(1));
    Volume<T> v3(2, 2, T(5));
    Volume<T> v4(3, 2, T(1));
    Volume<T> v5(2, 2, T(1), vec3d(0.1, 0.2, 0.3));
    Volume<T> v6(2, 2, T(1));

    CPPUNIT_ASSERT(v != v2);
    CPPUNIT_ASSERT(v != v3);
    CPPUNIT_ASSERT(v != v4);
    CPPUNIT_ASSERT(v != v5);
    CPPUNIT_ASSERT(v == v6);

    v.set(-3, -2, 2, T(5));
    v2.set(-3, -2, 2, T(5));
    CPPUNIT_ASSERT(v != v2);
    CPPUNIT_ASSERT(v != v5);
    CPPUNIT_ASSERT(v != v6);

    v5.set(-3, -2, 2, T(5));
    v6.set(-3, -2, 2, T(5));
    CPPUNIT_ASSERT(v != v5);
    CPPUNIT_ASSERT(v == v6);

    v.set(1, 16, -64, T(2));
    v5.set(1, 16, -64, T(4));
    v6.set(1, 16, -64, T(4));
    CPPUNIT_ASSERT(v != v5);
    CPPUNIT_ASSERT(v != v6);

    v.set(1, 16, -64, T(4));
    CPPUNIT_ASSERT(v != v5);
    CPPUNIT_ASSERT(v == v6);

    // Add an attribute
    v.getAttributeCollection().insert("something", Int32Attribute(5));
    CPPUNIT_ASSERT(v != v6);

    v6.getAttributeCollection().insert("something", Int32Attribute(3));
    CPPUNIT_ASSERT(v != v6);
    v6.getAttributeCollection().template value<int32_t>("something") = 5;
    CPPUNIT_ASSERT(v == v6);
}

//------------------------------------------------------------------------------

template <typename T>
void 
TestVolume<T>::testIO()
{
    USING_NK_NS
    USING_NKHIVE_NS

    #define IO_TEST(v) {                                            \
        std::ostringstream ostr(std::ios_base::binary);             \
        v.write(ostr);                                              \
        std::istringstream istr(ostr.str(), std::ios_base::binary); \
        Volume<T> v2(3, 3, T(5));                                   \
        v2.set(0, 0, 0, T(10));                                     \
        v2.set(-10, 0, 0, T(6));                                    \
        v2.read(istr);                                              \
        CPPUNIT_ASSERT(v == v2);                                    \
    } 

    // register the string attribute.
    Attribute::clearAttributeRegistry();
    StringAttribute::registerAttributeType();

    Volume<T> v(2, 2, T(1));
    IO_TEST(v);

    v.set(0, 0, 0, T(2));
    IO_TEST(v);

    v.set(-1, -2, -4, T(2));
    IO_TEST(v);

    v.set(64, -64, 32, T(2));
    IO_TEST(v);

    v.set(34, -23, -34, T(2));
    IO_TEST(v);

    v.getAttributeCollection().insert("something", StringAttribute("test"));
    IO_TEST(v);

    // clear the registry
    Attribute::clearAttributeRegistry();
    
    #undef IO_TEST
}

//------------------------------------------------------------------------------

template <typename T>
void 
TestVolume<T>::testIOHDF5()
{
    USING_NK_NS
    USING_NKHIVE_NS

    #define IO_TEST(v) {                                            \
        remove("testingHDF5.hv");                                   \
        VolumeFile file("testingHDF5.hv", VoidFile::WRITE_TRUNC);   \
        v.write(file.m_id);                                         \
        file.close();                                               \
        Volume<T> v2(3, 3, T(5));                                   \
        v2.set(0, 0, 0, T(10));                                     \
        v2.set(-10, 0, 0, T(6));                                    \
        VolumeFile file2("testingHDF5.hv", VoidFile::READ_ONLY);    \
        v2.read(file2.m_id);                                        \
        CPPUNIT_ASSERT(v == v2);                                    \
        file2.close();                                              \
        remove("testingHDF5.hv");                                   \
    } 
        
    // register the string attribute.
    Attribute::clearAttributeRegistry();
    StringAttribute::registerAttributeType();

    Volume<T> v(2, 2, T(1));
    IO_TEST(v);

    v.set(0, 0, 0, T(2));
    IO_TEST(v);

    v.set(-1, -2, -4, T(2));
    IO_TEST(v);

    v.set(64, -64, 32, T(2));
    IO_TEST(v);

    v.set(34, -23, -34, T(2));
    IO_TEST(v);

    v.getAttributeCollection().insert("something", StringAttribute("test"));
    IO_TEST(v);

    // clear the registry
    Attribute::clearAttributeRegistry();

    #undef IO_TEST
}

//------------------------------------------------------------------------------


template <typename T>
void
TestVolume<T>::testSetIterator()
{
    USING_NK_NS
    USING_NKHIVE_NS

    Volume<T> v(2, 2, T(1));

    // store coordinate values for getCoordinate tests
    i32 test_coords[4][3];
    test_coords[0][0] =  1; test_coords[0][1] =  1; test_coords[0][2] =  1;
    test_coords[1][0] =  0; test_coords[1][1] = -5; test_coords[1][2] = -6;
    test_coords[2][0] = -1; test_coords[2][1] = -1; test_coords[2][2] =  0;
    test_coords[3][0] = -1; test_coords[3][1] = -4; test_coords[3][2] = -2;

    v.set(test_coords[0][0], test_coords[0][1], test_coords[0][2], T(5));
    v.set(test_coords[1][0], test_coords[1][1], test_coords[1][2], T(5));
    v.set(test_coords[2][0], test_coords[2][1], test_coords[2][2], T(5));
    v.set(test_coords[3][0], test_coords[3][1], test_coords[3][2], T(5));

    int iter_count = 0;
    int i, j, k;
    typename Volume<T>::set_iterator sit = v.setIterator();
    for ( ; sit(); ++sit) {
        CPPUNIT_ASSERT(*sit == T(5));
            
        sit.getCoordinates(i, j, k);
        CPPUNIT_ASSERT(test_coords[iter_count][0] == i);
        CPPUNIT_ASSERT(test_coords[iter_count][1] == j);
        CPPUNIT_ASSERT(test_coords[iter_count][2] == k);
        
        ++iter_count;
    }

    CPPUNIT_ASSERT(iter_count == 4);
}

//------------------------------------------------------------------------------

template <typename T>
void
TestVolume<T>::testComputeSetBounds()
{
    USING_NK_NS
    USING_NKHIVE_NS

    Volume<T> v(2, 2, T(1));

    v.set(-1, -3, 4, T(5));
    v.set(3, 5, -5, T(5));

    signed_index_bounds bounds;
    CPPUNIT_ASSERT(v.computeSetBounds(bounds));

    CPPUNIT_ASSERT(bounds.min() == vec3i(-1, -3, -5));
    CPPUNIT_ASSERT(bounds.max() == vec3i(4, 6, 5));

    Bounds3D<double> local_bounds;
    CPPUNIT_ASSERT(v.computeSetBounds(local_bounds));

    TOL_CHECK(local_bounds.min()[0], -1.0);
    TOL_CHECK(local_bounds.min()[1], -3.0);
    TOL_CHECK(local_bounds.min()[2], -5.0);

    TOL_CHECK(local_bounds.max()[0], 4.0);
    TOL_CHECK(local_bounds.max()[1], 6.0);
    TOL_CHECK(local_bounds.max()[2], 5.0);

    // change the local xform
    v.setLocalXform(vec3d(2.0, 2.0, 2.0));
    CPPUNIT_ASSERT(v.computeSetBounds(local_bounds));

    TOL_CHECK(local_bounds.min()[0], -0.5);
    TOL_CHECK(local_bounds.min()[1], -1.5);
    TOL_CHECK(local_bounds.min()[2], -2.5);

    TOL_CHECK(local_bounds.max()[0], 2.0);
    TOL_CHECK(local_bounds.max()[1], 3.0);
    TOL_CHECK(local_bounds.max()[2], 2.5);
}

//------------------------------------------------------------------------------
