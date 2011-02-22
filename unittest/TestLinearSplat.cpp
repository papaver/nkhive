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
// TestLinearSplat.cpp
//------------------------------------------------------------------------------

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <nkbase/Tolerance.h>

#include <nkhive/interpolation/LinearSplat.h>

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
class TestLinearSplat : public CppUnit::TestFixture {

    CPPUNIT_TEST_SUITE(TestLinearSplat);
    CPPUNIT_TEST(test);
    CPPUNIT_TEST(testPolicies);
    CPPUNIT_TEST_SUITE_END();
    
public:
    void setUp() {}
    void tearDown() {}
    
    void test();
    void testPolicies();
};

//------------------------------------------------------------------------------
// test suite registration
//------------------------------------------------------------------------------

CPPUNIT_TEST_SUITE_REGISTRATION(TestLinearSplat<float>);
CPPUNIT_TEST_SUITE_REGISTRATION(TestLinearSplat<double>);
CPPUNIT_TEST_SUITE_REGISTRATION(TestLinearSplat<half>);

//------------------------------------------------------------------------------
// class implementation
//------------------------------------------------------------------------------

template<typename T>
void
TestLinearSplat<T>::test()
{
    USING_NKHIVE_NS

    // construct volume
    T default_val(0); 
    T splat_val(1);

    // splat value at the origin.
    {
        typename Volume<T>::shared_ptr volume(new Volume<T>(2, 1, default_val));
        LinearSplat<T> splatter(volume);
        splatter.splat(0, 0, 0, splat_val);

        TOL_CHECK(volume->get(0, 0, 0), 0.125);
        TOL_CHECK(volume->get(0, 0, -1), 0.125);
        TOL_CHECK(volume->get(0, -1, 0), 0.125);
        TOL_CHECK(volume->get(0, -1, -1), 0.125);
        TOL_CHECK(volume->get(-1, 0, 0), 0.125);
        TOL_CHECK(volume->get(-1, -1, 0), 0.125);
        TOL_CHECK(volume->get(-1, 0, -1), 0.125);
        TOL_CHECK(volume->get(-1, -1, -1), 0.125);
    }

    // splat value at the center of a voxel.
    {
        typename Volume<T>::shared_ptr volume(new Volume<T>(2, 1, default_val));
        LinearSplat<T> splatter(volume);
        splatter.splat(0.5, 0.5, 0.5, splat_val);

        TOL_CHECK(volume->get(0, 0, 0), 1);
        TOL_CHECK(volume->get(0, 0, -1), 0);
        TOL_CHECK(volume->get(0, -1, 0), 0);
        TOL_CHECK(volume->get(0, -1, -1), 0);
        TOL_CHECK(volume->get(-1, 0, 0), 0);
        TOL_CHECK(volume->get(-1, -1, 0), 0);
        TOL_CHECK(volume->get(-1, 0, -1), 0);
        TOL_CHECK(volume->get(-1, -1, -1), 0);
        TOL_CHECK(volume->get(0, 0, 1), 0);
        TOL_CHECK(volume->get(0, 1, 0), 0);
        TOL_CHECK(volume->get(0, 1, 1), 0);
        TOL_CHECK(volume->get(1, 0, 0), 0);
        TOL_CHECK(volume->get(1, 1, 0), 0);
        TOL_CHECK(volume->get(1, 0, 1), 0);
        TOL_CHECK(volume->get(1, 1, 1), 0);
    }

    // splat value off center closer to origin
    {
        typename Volume<T>::shared_ptr volume(new Volume<T>(2, 1, default_val));
        LinearSplat<T> splatter(volume);
        splatter.splat(0.25, 0.25, 0.25, splat_val);

        TOL_CHECK(volume->get(0, 0, 0),    0.421875);
        TOL_CHECK(volume->get(0, 0, -1),   0.140625);
        TOL_CHECK(volume->get(0, -1, 0),   0.140625);
        TOL_CHECK(volume->get(0, -1, -1),  0.046875);
        TOL_CHECK(volume->get(-1, 0, 0),   0.140625);
        TOL_CHECK(volume->get(-1, -1, 0),  0.046875);
        TOL_CHECK(volume->get(-1, 0, -1),  0.046875);
        TOL_CHECK(volume->get(-1, -1, -1), 0.015625);
    }

    // splat value off center further from origin
    {
        typename Volume<T>::shared_ptr volume(new Volume<T>(2, 1, default_val));
        LinearSplat<T> splatter(volume);
        splatter.splat(0.75, 0.75, 0.75, splat_val);

        TOL_CHECK(volume->get(0, 0, 0), 0.421875);
        TOL_CHECK(volume->get(0, 0, 1), 0.140625);
        TOL_CHECK(volume->get(0, 1, 0), 0.140625);
        TOL_CHECK(volume->get(0, 1, 1), 0.046875);
        TOL_CHECK(volume->get(1, 0, 0), 0.140625);
        TOL_CHECK(volume->get(1, 1, 0), 0.046875);
        TOL_CHECK(volume->get(1, 0, 1), 0.046875);
        TOL_CHECK(volume->get(1, 1, 1), 0.015625);
    }

    // splat value at negative voxel
    {
        typename Volume<T>::shared_ptr volume(new Volume<T>(2, 1, default_val));
        LinearSplat<T> splatter(volume);
        splatter.splat(-0.75, -0.75, -0.75, splat_val);

        TOL_CHECK(volume->get(-1, -1, -1), 0.421875);
        TOL_CHECK(volume->get(-1, -1, -2), 0.140625);
        TOL_CHECK(volume->get(-1, -2, -1), 0.140625);
        TOL_CHECK(volume->get(-1, -2, -2), 0.046875);
        TOL_CHECK(volume->get(-2, -1, -1), 0.140625);
        TOL_CHECK(volume->get(-2, -2, -1), 0.046875);
        TOL_CHECK(volume->get(-2, -1, -2), 0.046875);
        TOL_CHECK(volume->get(-2, -2, -2), 0.015625);
    }

    // splat value at negative voxel closer to zero
    {
        typename Volume<T>::shared_ptr volume(new Volume<T>(2, 1, default_val));
        LinearSplat<T> splatter(volume);
        splatter.splat(-0.25, -0.25, -0.25, splat_val);

        TOL_CHECK(volume->get(-1, -1, -1), 0.421875);
        TOL_CHECK(volume->get(-1, -1,  0), 0.140625);
        TOL_CHECK(volume->get(-1,  0, -1), 0.140625);
        TOL_CHECK(volume->get(-1,  0,  0), 0.046875);
        TOL_CHECK(volume->get( 0, -1, -1), 0.140625);
        TOL_CHECK(volume->get( 0,  0, -1), 0.046875);
        TOL_CHECK(volume->get( 0, -1,  0), 0.046875);
        TOL_CHECK(volume->get( 0,  0,  0), 0.015625);
    }
}

//------------------------------------------------------------------------------

template <typename T>
void
TestLinearSplat<T>::testPolicies()
{
    USING_NK_NS
    USING_NKHIVE_NS

    // construct volume
    T default_val(0); 
    T splat_val(1);

    // test std::plus
    {
        typename Volume<T>::shared_ptr volume(new Volume<T>(2, 1, default_val));
        LinearSplat<T, std::plus> splatter(volume);
        splatter.splat(0, 0, 0, splat_val);
        splatter.splat(0, 0, 0, splat_val);

        TOL_CHECK(volume->get(0, 0, 0), 0.25);
        TOL_CHECK(volume->get(0, 0, -1), 0.25);
        TOL_CHECK(volume->get(0, -1, 0), 0.25);
        TOL_CHECK(volume->get(0, -1, -1), 0.25);
        TOL_CHECK(volume->get(-1, 0, 0), 0.25);
        TOL_CHECK(volume->get(-1, -1, 0), 0.25);
        TOL_CHECK(volume->get(-1, 0, -1), 0.25);
        TOL_CHECK(volume->get(-1, -1, -1), 0.25);
    }

    // test set
    {
        typename Volume<T>::shared_ptr volume(new Volume<T>(2, 1, default_val));
        LinearSplat<T, set_op> splatter(volume);
        splatter.splat(0, 0, 0, splat_val);
        splatter.splat(0, 0, 0, splat_val);

        TOL_CHECK(volume->get(0, 0, 0), 0.125);
        TOL_CHECK(volume->get(0, 0, -1), 0.125);
        TOL_CHECK(volume->get(0, -1, 0), 0.125);
        TOL_CHECK(volume->get(0, -1, -1), 0.125);
        TOL_CHECK(volume->get(-1, 0, 0), 0.125);
        TOL_CHECK(volume->get(-1, -1, 0), 0.125);
        TOL_CHECK(volume->get(-1, 0, -1), 0.125);
        TOL_CHECK(volume->get(-1, -1, -1), 0.125);
    }
}

//------------------------------------------------------------------------------
