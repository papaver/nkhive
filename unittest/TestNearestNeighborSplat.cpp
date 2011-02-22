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
// TestNearestNeighborSplat.cpp
//------------------------------------------------------------------------------

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <nkbase/Tolerance.h>

#include <nkhive/interpolation/NearestNeighborSplat.h>
#include <nkhive/Defs.h>

//------------------------------------------------------------------------------
// definitions
//------------------------------------------------------------------------------

#define CHECK_RESULT(v) \
    CPPUNIT_ASSERT(fabs(T(result) - T(v)) < NK_NS::Tolerance<T>::zero());

#define TOL_CHECK(v1, v2) \
    CPPUNIT_ASSERT(fabs(T(v1) - T(v2)) < NK_NS::Tolerance<T>::zero());
        
//------------------------------------------------------------------------------
// class definition
//------------------------------------------------------------------------------

template <typename T>
class TestNearestNeighborSplat : public CppUnit::TestFixture {

    CPPUNIT_TEST_SUITE(TestNearestNeighborSplat);
    CPPUNIT_TEST(test);
    CPPUNIT_TEST(testPolicies);
    CPPUNIT_TEST_SUITE_END();
    
public:
    void setUp() {}
    void tearDown() {}
    
    void test();
    void testPolicies();
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestNearestNeighborSplat<float>);
CPPUNIT_TEST_SUITE_REGISTRATION(TestNearestNeighborSplat<double>);

//------------------------------------------------------------------------------

template <typename T>
void
TestNearestNeighborSplat<T>::test()
{
    USING_NKHIVE_NS

    // construct volume
    T default_val(0); 
    T splat_val(1);

    // splat value at the origin.
    {
        typename Volume<T>::shared_ptr volume(new Volume<T>(2, 1, default_val));
        NearestNeighborSplat<T> splatter(volume);
        splatter.splat(0, 0, 0, splat_val);

        TOL_CHECK(volume->get(0, 0, 0), 1);
        TOL_CHECK(volume->get(0, 0, -1), 0);
        TOL_CHECK(volume->get(0, -1, 0), 0);
        TOL_CHECK(volume->get(0, -1, -1), 0);
        TOL_CHECK(volume->get(-1, 0, 0), 0);
        TOL_CHECK(volume->get(-1, -1, 0), 0);
        TOL_CHECK(volume->get(-1, 0, -1), 0);
        TOL_CHECK(volume->get(-1, -1, -1), 0);
    }

    // splat value at center
    {
        typename Volume<T>::shared_ptr volume(new Volume<T>(2, 1, default_val));
        NearestNeighborSplat<T> splatter(volume);
        splatter.splat(0.5, 0.5, 0.5, splat_val);

        TOL_CHECK(volume->get(0, 0, 0), 1);
        TOL_CHECK(volume->get(0, 0, -1), 0);
        TOL_CHECK(volume->get(0, -1, 0), 0);
        TOL_CHECK(volume->get(0, -1, -1), 0);
        TOL_CHECK(volume->get(-1, 0, 0), 0);
        TOL_CHECK(volume->get(-1, -1, 0), 0);
        TOL_CHECK(volume->get(-1, 0, -1), 0);
        TOL_CHECK(volume->get(-1, -1, -1), 0);
    }

    // splat value off center
    {
        typename Volume<T>::shared_ptr volume(new Volume<T>(2, 1, default_val));
        NearestNeighborSplat<T> splatter(volume);
        splatter.splat(0.1, 0.5, 0.8, splat_val);

        TOL_CHECK(volume->get(0, 0, 0), 1);
        TOL_CHECK(volume->get(0, 0, -1), 0);
        TOL_CHECK(volume->get(0, -1, 0), 0);
        TOL_CHECK(volume->get(0, -1, -1), 0);
        TOL_CHECK(volume->get(-1, 0, 0), 0);
        TOL_CHECK(volume->get(-1, -1, 0), 0);
        TOL_CHECK(volume->get(-1, 0, -1), 0);
        TOL_CHECK(volume->get(-1, -1, -1), 0);
    }

    // splat negative values.
    {
        typename Volume<T>::shared_ptr volume(new Volume<T>(2, 1, default_val));
        NearestNeighborSplat<T> splatter(volume);
        splatter.splat(-0.2, -1.2, -0.8, splat_val);

        TOL_CHECK(volume->get(-1, -2, -1), 1);
        TOL_CHECK(volume->get(0, 0, -2), 0);
        TOL_CHECK(volume->get(0, -2, 0), 0);
        TOL_CHECK(volume->get(0, -2, -2), 0);
        TOL_CHECK(volume->get(-2, 0, 0), 0);
        TOL_CHECK(volume->get(-2, -2, 0), 0);
        TOL_CHECK(volume->get(-2, 0, -2), 0);
        TOL_CHECK(volume->get(-2, -2, -2), 0);
        TOL_CHECK(volume->get(0, 0, 0), 0);
        TOL_CHECK(volume->get(1, 0, 0), 0);
        TOL_CHECK(volume->get(0, 1, 0), 0);
        TOL_CHECK(volume->get(0, 0, 1), 0);
        TOL_CHECK(volume->get(1, 0, 1), 0);
        TOL_CHECK(volume->get(1, 1, 0), 0);
        TOL_CHECK(volume->get(1, 1, 1), 0);
    }
}

//------------------------------------------------------------------------------

template <typename T>
void 
TestNearestNeighborSplat<T>::testPolicies()
{
    USING_NK_NS
    USING_NKHIVE_NS
    
    // construct volume
    T default_val(0); 
    T splat_val(1);
    
    // Test plus policy
    {
        typename Volume<T>::shared_ptr volume(new Volume<T>(2, 1, default_val));
        NearestNeighborSplat<T, std::plus> splatter(volume);
        splatter.splat(-0.2, -1.2, -0.8, splat_val);
        splatter.splat(-0.2, -1.2, -0.8, splat_val);

        TOL_CHECK(volume->get(-1, -2, -1), 2);
    }

    // Test set policy
    {
        typename Volume<T>::shared_ptr volume(new Volume<T>(2, 1, default_val));
        NearestNeighborSplat<T, set_op> splatter(volume);
        splatter.splat(-0.2, -1.2, -0.8, splat_val);
        splatter.splat(-0.2, -1.2, -0.8, splat_val);

        TOL_CHECK(volume->get(-1, -2, -1), 1);
    }
}

//------------------------------------------------------------------------------
