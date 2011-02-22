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
// TestBoundingBox.cpp
//------------------------------------------------------------------------------

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <nkhive/Defs.h>
#include <nkhive/Types.h>
#include <nkhive/util/BoundingBox.h>

//-----------------------------------------------------------------------------
// class definition
//-----------------------------------------------------------------------------

template <typename T>
class TestBoundingBox : public CppUnit::TestFixture 
{

    CPPUNIT_TEST_SUITE(TestBoundingBox);
    CPPUNIT_TEST(test);
    CPPUNIT_TEST(testInRange);
    CPPUNIT_TEST(testCenter);
    CPPUNIT_TEST(testIsValid);
    CPPUNIT_TEST_SUITE_END();
    
public:
    void setUp() {}
    void tearDown() {}
    
    void test();
    void testInRange();
    void testCenter();
    void testIsValid();
};

//-----------------------------------------------------------------------------
// test suite registration
//-----------------------------------------------------------------------------

CPPUNIT_TEST_SUITE_REGISTRATION(TestBoundingBox<NK_NS::f16>);
CPPUNIT_TEST_SUITE_REGISTRATION(TestBoundingBox<NK_NS::f32>);
CPPUNIT_TEST_SUITE_REGISTRATION(TestBoundingBox<NK_NS::f64>);

//------------------------------------------------------------------------------

template <typename T>
void 
TestBoundingBox<T>::test()
{
    USING_NKHIVE_NS

    typedef typename BoundingBox<T>::vector_type vector_type;

    BoundingBox<T> bounds(vector_type(0.0f), vector_type(2.0f));

    CPPUNIT_ASSERT(bounds.min() == vector_type(0.0f));
    CPPUNIT_ASSERT(bounds.max() == vector_type(2.0f));
}

//------------------------------------------------------------------------------

template <typename T>
void 
TestBoundingBox<T>::testInRange()
{
    USING_NKHIVE_NS

    typedef typename BoundingBox<T>::vector_type vector_type;

    BoundingBox<T> bounds(vector_type(0), vector_type(3));
    CPPUNIT_ASSERT(bounds.inRange(vector_type(0)) == true);
    CPPUNIT_ASSERT(bounds.inRange(vector_type(1)) == true);
    CPPUNIT_ASSERT(bounds.inRange(vector_type(2)) == true);
    CPPUNIT_ASSERT(bounds.inRange(vector_type(3)) == true);
    CPPUNIT_ASSERT(bounds.inRange(vector_type(0, 1, 2)) == true);
    CPPUNIT_ASSERT(bounds.inRange(vector_type(0, 2, 1)) == true);
    CPPUNIT_ASSERT(bounds.inRange(vector_type(1, 0, 2)) == true);
    CPPUNIT_ASSERT(bounds.inRange(vector_type(1, 2, 0)) == true);
    CPPUNIT_ASSERT(bounds.inRange(vector_type(2, 0, 1)) == true);
    CPPUNIT_ASSERT(bounds.inRange(vector_type(2, 1, 0)) == true);
    CPPUNIT_ASSERT(bounds.inRange(vector_type(0, 0, 3)) == true);
    CPPUNIT_ASSERT(bounds.inRange(vector_type(0, 3, 0)) == true);
    CPPUNIT_ASSERT(bounds.inRange(vector_type(3, 0, 0)) == true);
    CPPUNIT_ASSERT(bounds.inRange(vector_type(0, 0, 4)) == false);
    CPPUNIT_ASSERT(bounds.inRange(vector_type(0, 4, 0)) == false);
    CPPUNIT_ASSERT(bounds.inRange(vector_type(4, 0, 0)) == false);
}

//------------------------------------------------------------------------------

template <typename T>
void 
TestBoundingBox<T>::testCenter()
{
    USING_NKHIVE_NS

    typedef typename BoundingBox<T>::vector_type vector_type;

    {
        BoundingBox<T> bounds(vector_type(0), vector_type(3));
        CPPUNIT_ASSERT(bounds.center() == vector_type(1.5f));
    }

    {
        BoundingBox<T> bounds(vector_type(-10), vector_type(-1));
        CPPUNIT_ASSERT(bounds.center() == vector_type(-5.5));
    }

    {
        BoundingBox<T> bounds(vector_type(2, 4, 6), vector_type(4, 6, 8));
        CPPUNIT_ASSERT(bounds.center() == vector_type(3, 5, 7));
    }

    {
        BoundingBox<T> bounds(vector_type(-4, -6, -8), vector_type(-2, -4, -6));
        CPPUNIT_ASSERT(bounds.center() == vector_type(-3, -5, -7));
    }
}

//------------------------------------------------------------------------------

template <typename T>
void
TestBoundingBox<T>::testIsValid()
{
    USING_NKHIVE_NS

    typedef typename BoundingBox<T>::vector_type vector_type;

    #define TEST_UNITS(min, max, value) {                          \
        BoundingBox<T> bounds(vector_type(min), vector_type(max)); \
        CPPUNIT_ASSERT(bounds.isValid() == value);                 \
    }                                                              \

    TEST_UNITS( 1,  0, false);
    TEST_UNITS( 1, -1, false);
    TEST_UNITS(10, -1, false);
    TEST_UNITS( 0,  0, true);
    TEST_UNITS( 1,  1, true);
    TEST_UNITS(-1, -1, true);
    TEST_UNITS( 0,  3, true);
    TEST_UNITS( 2,  3, true);
    TEST_UNITS(-2,  0, true);
    TEST_UNITS(-2, -1, true);

    #define TEST_MULTS(min, max, value) {                          \
        BoundingBox<T> bounds(vector_type(min), vector_type(max)); \
        CPPUNIT_ASSERT(bounds.isValid() == value);                 \
    }                                                              \

    TEST_MULTS(vector_type( 1,  2,  1), vector_type( 0, -1,  0), false);
    TEST_MULTS(vector_type( 1, -2,  1), vector_type( 0, -1,  0), false);
    TEST_MULTS(vector_type( 1,  2,  1), vector_type( 1,  2,  1), true);
    TEST_MULTS(vector_type(-1, -2, -1), vector_type(-1, -2, -1), true);
    TEST_MULTS(vector_type( 0, -2,  0), vector_type( 1, -1,  1), true);
    TEST_MULTS(vector_type( 1,  2,  1), vector_type( 2,  3,  3), true);
    TEST_MULTS(vector_type( 1, -2,  1), vector_type( 2,  0,  2), true);
    TEST_MULTS(vector_type(-4, -2, -4), vector_type(-2, -1, -2), true);
}

//-----------------------------------------------------------------------------
