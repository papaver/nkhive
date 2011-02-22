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
// TestBounds3D.cpp
//------------------------------------------------------------------------------

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <nkhive/Defs.h>
#include <nkhive/Types.h>
#include <nkhive/util/Bounds3D.h>

template <typename T>
class TestBounds3D : public CppUnit::TestFixture 
{

    CPPUNIT_TEST_SUITE(TestBounds3D);
    CPPUNIT_TEST(test);
    CPPUNIT_TEST(testSetExtrema);
    CPPUNIT_TEST(testUpdateExtrema);
    CPPUNIT_TEST(testUpdateExtremaWithBounds);
    CPPUNIT_TEST(testInRange);
    CPPUNIT_TEST(testGetAccessors);
    CPPUNIT_TEST(testSetAccessors);
    CPPUNIT_TEST(testClamp);
    CPPUNIT_TEST(testIntersects);
    CPPUNIT_TEST(testContains);
    CPPUNIT_TEST(testIntersection);
    CPPUNIT_TEST(testSwap);
    CPPUNIT_TEST(testToString);
    CPPUNIT_TEST_SUITE_END();
    
public:
    void setUp() {}
    void tearDown() {}
    
    void test();
    void testSetExtrema();
    void testUpdateExtrema();
    void testUpdateExtremaWithBounds();
    void testInRange();
    void testGetAccessors();
    void testSetAccessors();
    void testClamp();
    void testIntersects();
    void testContains();
    void testIntersection();
    void testSwap();
    void testToString();
};

//-----------------------------------------------------------------------------
// test suite registration
//-----------------------------------------------------------------------------

CPPUNIT_TEST_SUITE_REGISTRATION(TestBounds3D<NK_NS::f32>);
CPPUNIT_TEST_SUITE_REGISTRATION(TestBounds3D<NKHIVE_NS::index_type>);
CPPUNIT_TEST_SUITE_REGISTRATION(TestBounds3D<NKHIVE_NS::signed_index_type>);

//------------------------------------------------------------------------------

template <typename T>
void 
TestBounds3D<T>::test()
{
    USING_NKHIVE_NS

    typedef typename Bounds3D<T>::vector_type   vector_type;

    Bounds3D<T> bounds(vector_type(0), vector_type(2));

    CPPUNIT_ASSERT(bounds.min() == vector_type(0));
    CPPUNIT_ASSERT(bounds.max() == vector_type(2));
}

//------------------------------------------------------------------------------

template <typename T>
void 
TestBounds3D<T>::testSetExtrema()
{

    USING_NKHIVE_NS

    typedef typename Bounds3D<T>::vector_type   vector_type;

    Bounds3D<T> bounds;
    
    // near bottom left / far top right
    bounds.setExtrema(vector_type(-1, -1, -1), vector_type( 1,  1,  1));
    CPPUNIT_ASSERT(bounds.min() == vector_type(-1));
    CPPUNIT_ASSERT(bounds.max() == vector_type( 1));
    bounds.setExtrema(vector_type( 1,  1,  1), vector_type(-1, -1, -1));
    CPPUNIT_ASSERT(bounds.min() == vector_type(-1));
    CPPUNIT_ASSERT(bounds.max() == vector_type( 1));

    // near bottom right / far top left
    bounds.setExtrema(vector_type( 1, -1, -1), vector_type(-1,  1,  1));
    CPPUNIT_ASSERT(bounds.min() == vector_type(-1));
    CPPUNIT_ASSERT(bounds.max() == vector_type( 1));
    bounds.setExtrema(vector_type(-1,  1,  1), vector_type( 1, -1, -1));
    CPPUNIT_ASSERT(bounds.min() == vector_type(-1));
    CPPUNIT_ASSERT(bounds.max() == vector_type( 1));

    // near top left / far bottom right
    bounds.setExtrema(vector_type(-1,  1, -1), vector_type( 1, -1,  1));
    CPPUNIT_ASSERT(bounds.min() == vector_type(-1));
    CPPUNIT_ASSERT(bounds.max() == vector_type( 1));
    bounds.setExtrema(vector_type( 1, -1,  1), vector_type(-1,  1, -1));
    CPPUNIT_ASSERT(bounds.min() == vector_type(-1));
    CPPUNIT_ASSERT(bounds.max() == vector_type( 1));

    // near top right / far bottom left
    bounds.setExtrema(vector_type( 1,  1, -1), vector_type(-1, -1,  1));
    CPPUNIT_ASSERT(bounds.min() == vector_type(-1));
    CPPUNIT_ASSERT(bounds.max() == vector_type( 1));
    bounds.setExtrema(vector_type(-1, -1,  1), vector_type( 1,  1, -1));
    CPPUNIT_ASSERT(bounds.min() == vector_type(-1));
    CPPUNIT_ASSERT(bounds.max() == vector_type( 1));
}

//------------------------------------------------------------------------------

template <>
void 
TestBounds3D<NKHIVE_NS::index_type>::testSetExtrema()
{

    USING_NKHIVE_NS

    typedef Bounds3D<NKHIVE_NS::index_type>::vector_type vector_type;

    Bounds3D<NKHIVE_NS::index_type> bounds;
    
    // near bottom left / far top right
    bounds.setExtrema(vector_type(0, 0, 0), vector_type(1, 1, 1));
    CPPUNIT_ASSERT(bounds.min() == vector_type(0));
    CPPUNIT_ASSERT(bounds.max() == vector_type(1));
    bounds.setExtrema(vector_type(1, 1, 1), vector_type(0, 0, 0));
    CPPUNIT_ASSERT(bounds.min() == vector_type(0));
    CPPUNIT_ASSERT(bounds.max() == vector_type(1));

    // near bottom right / far top left
    bounds.setExtrema(vector_type(1, 0, 0), vector_type(0, 1, 1));
    CPPUNIT_ASSERT(bounds.min() == vector_type(0));
    CPPUNIT_ASSERT(bounds.max() == vector_type(1));
    bounds.setExtrema(vector_type(0, 1, 1), vector_type(1, 0, 0));
    CPPUNIT_ASSERT(bounds.min() == vector_type(0));
    CPPUNIT_ASSERT(bounds.max() == vector_type(1));

    // near top left / far bottom right
    bounds.setExtrema(vector_type(0, 1, 0), vector_type(1, 0, 1));
    CPPUNIT_ASSERT(bounds.min() == vector_type(0));
    CPPUNIT_ASSERT(bounds.max() == vector_type(1));
    bounds.setExtrema(vector_type(1, 0, 1), vector_type(0, 1, 0));
    CPPUNIT_ASSERT(bounds.min() == vector_type(0));
    CPPUNIT_ASSERT(bounds.max() == vector_type(1));

    // near top right / far bottom left
    bounds.setExtrema(vector_type(1, 1, 0), vector_type(0, 0, 1));
    CPPUNIT_ASSERT(bounds.min() == vector_type(0));
    CPPUNIT_ASSERT(bounds.max() == vector_type(1));
    bounds.setExtrema(vector_type(0, 0, 1), vector_type(1, 1, 0));
    CPPUNIT_ASSERT(bounds.min() == vector_type(0));
    CPPUNIT_ASSERT(bounds.max() == vector_type(1));
}

//------------------------------------------------------------------------------

template <typename T>
void 
TestBounds3D<T>::testUpdateExtrema()
{
    USING_NKHIVE_NS

    typedef typename Bounds3D<T>::vector_type   vector_type;

    // Test for min extrema
    {
        Bounds3D<T> bounds(vector_type(1), vector_type(2));

        bounds.updateExtrema(1, 1, 1);
        CPPUNIT_ASSERT(bounds.min() == vector_type(1));
        CPPUNIT_ASSERT(bounds.max() == vector_type(2));

        bounds.updateExtrema(0, 1, 0);
        CPPUNIT_ASSERT(bounds.min() == vector_type(0, 1, 0));
        CPPUNIT_ASSERT(bounds.max() == vector_type(2));

        bounds.updateExtrema(0, 0, 0);
        CPPUNIT_ASSERT(bounds.min() == vector_type(0));
        CPPUNIT_ASSERT(bounds.max() == vector_type(2));
    }

    // Test for max
    {
        Bounds3D<T> bounds(vector_type(1), vector_type(2));

        bounds.updateExtrema(2, 2, 2);
        CPPUNIT_ASSERT(bounds.min() == vector_type(1));
        CPPUNIT_ASSERT(bounds.max() == vector_type(2));


        bounds.updateExtrema(2, 3, 2);
        CPPUNIT_ASSERT(bounds.min() == vector_type(1));
        CPPUNIT_ASSERT(bounds.max() == vector_type(2, 3, 2));

        bounds.updateExtrema(3, 3, 3);
        CPPUNIT_ASSERT(bounds.min() == vector_type(1));
        CPPUNIT_ASSERT(bounds.max() == vector_type(3, 3, 3));
    }

    // Test for min and max
    {
        Bounds3D<T> bounds(vector_type(1), vector_type(2));

        bounds.updateExtrema(0, 3, 1);
        CPPUNIT_ASSERT(bounds.min() == vector_type(0, 1, 1));
        CPPUNIT_ASSERT(bounds.max() == vector_type(2, 3, 2));
    }
}

//------------------------------------------------------------------------------

template <typename T>
void 
TestBounds3D<T>::testUpdateExtremaWithBounds()
{
    USING_NKHIVE_NS

    typedef typename Bounds3D<T>::vector_type   vector_type;

    // test if two bounds do not extremize the current bonds.
    {
        Bounds3D<T> bounds(vector_type(0), vector_type(3));
        Bounds3D<T> bounds2(vector_type(1), vector_type(2));

        bounds.updateExtrema(bounds2);
        CPPUNIT_ASSERT(bounds.min() == vector_type(0));
        CPPUNIT_ASSERT(bounds.max() == vector_type(3));
    }

    // test if bounds are equal
    {
        Bounds3D<T> bounds(vector_type(1), vector_type(2));
        Bounds3D<T> bounds2(vector_type(1), vector_type(2));

        bounds.updateExtrema(bounds2);
        CPPUNIT_ASSERT(bounds.min() == vector_type(1));
        CPPUNIT_ASSERT(bounds.max() == vector_type(2));
    }

    // test extremizing min
    {
        Bounds3D<T> bounds(vector_type(1), vector_type(2));
        Bounds3D<T> bounds2(vector_type(0), vector_type(2));

        bounds.updateExtrema(bounds2);
        CPPUNIT_ASSERT(bounds.min() == vector_type(0));
        CPPUNIT_ASSERT(bounds.max() == vector_type(2));
    }

    // test extremizing max
    {
        Bounds3D<T> bounds(vector_type(1), vector_type(2));
        Bounds3D<T> bounds2(vector_type(1), vector_type(3));

        bounds.updateExtrema(bounds2);
        CPPUNIT_ASSERT(bounds.min() == vector_type(1));
        CPPUNIT_ASSERT(bounds.max() == vector_type(3));
    }

    // test extremizing both.
    {
        Bounds3D<T> bounds(vector_type(1), vector_type(2));
        Bounds3D<T> bounds2(vector_type(0), vector_type(3));

        bounds.updateExtrema(bounds2);
        CPPUNIT_ASSERT(bounds.min() == vector_type(0));
        CPPUNIT_ASSERT(bounds.max() == vector_type(3));
    }
}

//------------------------------------------------------------------------------

template <typename T>
void
TestBounds3D<T>::testInRange()
{
    USING_NKHIVE_NS

    typedef typename Bounds3D<T>::vector_type vector_type;

    Bounds3D<T> bounds(vector_type(0), vector_type(3));
    CPPUNIT_ASSERT(bounds.inRange(vector_type(0)) == true);
    CPPUNIT_ASSERT(bounds.inRange(vector_type(1)) == true);
    CPPUNIT_ASSERT(bounds.inRange(vector_type(2)) == true);
    CPPUNIT_ASSERT(bounds.inRange(vector_type(3)) == false);
    CPPUNIT_ASSERT(bounds.inRange(vector_type(0, 1, 2)) == true);
    CPPUNIT_ASSERT(bounds.inRange(vector_type(0, 2, 1)) == true);
    CPPUNIT_ASSERT(bounds.inRange(vector_type(1, 0, 2)) == true);
    CPPUNIT_ASSERT(bounds.inRange(vector_type(1, 2, 0)) == true);
    CPPUNIT_ASSERT(bounds.inRange(vector_type(2, 0, 1)) == true);
    CPPUNIT_ASSERT(bounds.inRange(vector_type(2, 1, 0)) == true);
    CPPUNIT_ASSERT(bounds.inRange(vector_type(0, 0, 3)) == false);
    CPPUNIT_ASSERT(bounds.inRange(vector_type(0, 3, 0)) == false);
    CPPUNIT_ASSERT(bounds.inRange(vector_type(3, 0, 0)) == false);
}

//------------------------------------------------------------------------------

template <typename T>
void
TestBounds3D<T>::testGetAccessors()
{
    USING_NKHIVE_NS

    typedef typename Bounds3D<T>::vector_type vector_type;

    vector_type min(-1, -1, -1);
    vector_type max( 1,  1,  1);
    Bounds3D<T> bounds(min, max);
    CPPUNIT_ASSERT(bounds.nbl() == vector_type(-1, -1, -1));
    CPPUNIT_ASSERT(bounds.nbr() == vector_type( 1, -1, -1));
    CPPUNIT_ASSERT(bounds.ntr() == vector_type( 1,  1, -1));
    CPPUNIT_ASSERT(bounds.ntl() == vector_type(-1,  1, -1));
    CPPUNIT_ASSERT(bounds.ftl() == vector_type(-1,  1,  1));
    CPPUNIT_ASSERT(bounds.ftr() == vector_type( 1,  1,  1));
    CPPUNIT_ASSERT(bounds.fbr() == vector_type( 1, -1,  1));
    CPPUNIT_ASSERT(bounds.fbl() == vector_type(-1, -1,  1));

    CPPUNIT_ASSERT(bounds.get(Bounds3D<T>::NBL) == vector_type(-1, -1, -1));
    CPPUNIT_ASSERT(bounds.get(Bounds3D<T>::NBR) == vector_type( 1, -1, -1));
    CPPUNIT_ASSERT(bounds.get(Bounds3D<T>::NTR) == vector_type( 1,  1, -1));
    CPPUNIT_ASSERT(bounds.get(Bounds3D<T>::NTL) == vector_type(-1,  1, -1));
    CPPUNIT_ASSERT(bounds.get(Bounds3D<T>::FTL) == vector_type(-1,  1,  1));
    CPPUNIT_ASSERT(bounds.get(Bounds3D<T>::FTR) == vector_type( 1,  1,  1));
    CPPUNIT_ASSERT(bounds.get(Bounds3D<T>::FBR) == vector_type( 1, -1,  1));
    CPPUNIT_ASSERT(bounds.get(Bounds3D<T>::FBL) == vector_type(-1, -1,  1));
}

//------------------------------------------------------------------------------

template <typename T>
void
TestBounds3D<T>::testSetAccessors()
{
    USING_NKHIVE_NS

    typedef typename Bounds3D<T>::vector_type vector_type;

    vector_type min(-5, -5, -5);
    vector_type max( 5,  5,  5);
    Bounds3D<T> bounds;

    // new bottom left
    bounds.max() = max; bounds.min() = min;
    bounds.setNBL(vector_type(-4, -4, -4));
    CPPUNIT_ASSERT(bounds.min() == vector_type(-4, -4, -4));
    CPPUNIT_ASSERT(bounds.max() == vector_type( 5,  5,  5));
    bounds.max() = max; bounds.min() = min;
    bounds.set(vector_type(-4, -4, -4), Bounds3D<T>::NBL);
    CPPUNIT_ASSERT(bounds.min() == vector_type(-4, -4, -4));
    CPPUNIT_ASSERT(bounds.max() == vector_type( 5,  5,  5));

    // near bottom right
    bounds.max() = max; bounds.min() = min;
    bounds.setNBR(vector_type( 4, -4, -4));
    CPPUNIT_ASSERT(bounds.min() == vector_type(-5, -4, -4));
    CPPUNIT_ASSERT(bounds.max() == vector_type( 4,  5,  5));
    bounds.max() = max; bounds.min() = min;
    bounds.set(vector_type( 4, -4, -4), Bounds3D<T>::NBR);
    CPPUNIT_ASSERT(bounds.min() == vector_type(-5, -4, -4));
    CPPUNIT_ASSERT(bounds.max() == vector_type( 4,  5,  5));

    // near top right
    bounds.max() = max; bounds.min() = min;
    bounds.setNTR(vector_type( 4,  4, -4));
    CPPUNIT_ASSERT(bounds.min() == vector_type(-5, -5, -4));
    CPPUNIT_ASSERT(bounds.max() == vector_type( 4,  4,  5));
    bounds.max() = max; bounds.min() = min;
    bounds.set(vector_type( 4,  4, -4), Bounds3D<T>::NTR);
    CPPUNIT_ASSERT(bounds.min() == vector_type(-5, -5, -4));
    CPPUNIT_ASSERT(bounds.max() == vector_type( 4,  4,  5));

    // near top left
    bounds.max() = max; bounds.min() = min;
    bounds.setNTL(vector_type(-4,  4, -4));
    CPPUNIT_ASSERT(bounds.min() == vector_type(-4, -5, -4));
    CPPUNIT_ASSERT(bounds.max() == vector_type( 5,  4,  5));
    bounds.max() = max; bounds.min() = min;
    bounds.set(vector_type(-4,  4, -4), Bounds3D<T>::NTL);
    CPPUNIT_ASSERT(bounds.min() == vector_type(-4, -5, -4));
    CPPUNIT_ASSERT(bounds.max() == vector_type( 5,  4,  5));

    // far top left
    bounds.max() = max; bounds.min() = min;
    bounds.setFTL(vector_type(-4,  4,  4));
    CPPUNIT_ASSERT(bounds.min() == vector_type(-4, -5, -5));
    CPPUNIT_ASSERT(bounds.max() == vector_type( 5,  4,  4));
    bounds.max() = max; bounds.min() = min;
    bounds.set(vector_type(-4,  4,  4), Bounds3D<T>::FTL);
    CPPUNIT_ASSERT(bounds.min() == vector_type(-4, -5, -5));
    CPPUNIT_ASSERT(bounds.max() == vector_type( 5,  4,  4));

    // far top right
    bounds.max() = max; bounds.min() = min;
    bounds.setFTR(vector_type( 4,  4,  4));
    CPPUNIT_ASSERT(bounds.min() == vector_type(-5, -5, -5));
    CPPUNIT_ASSERT(bounds.max() == vector_type( 4,  4,  4));
    bounds.max() = max; bounds.min() = min;
    bounds.set(vector_type( 4,  4,  4), Bounds3D<T>::FTR);
    CPPUNIT_ASSERT(bounds.min() == vector_type(-5, -5, -5));
    CPPUNIT_ASSERT(bounds.max() == vector_type( 4,  4,  4));

    // far bottom right
    bounds.max() = max; bounds.min() = min;
    bounds.setFBR(vector_type( 4, -4,  4));
    CPPUNIT_ASSERT(bounds.min() == vector_type(-5, -4, -5));
    CPPUNIT_ASSERT(bounds.max() == vector_type( 4,  5,  4));
    bounds.max() = max; bounds.min() = min;
    bounds.set(vector_type( 4, -4,  4), Bounds3D<T>::FBR);
    CPPUNIT_ASSERT(bounds.min() == vector_type(-5, -4, -5));
    CPPUNIT_ASSERT(bounds.max() == vector_type( 4,  5,  4));

    // far bottom left
    bounds.max() = max; bounds.min() = min;
    bounds.setFBL(vector_type(-4, -4,  4));
    CPPUNIT_ASSERT(bounds.min() == vector_type(-4, -4, -5));
    CPPUNIT_ASSERT(bounds.max() == vector_type( 5,  5,  4));
    bounds.max() = max; bounds.min() = min;
    bounds.set(vector_type(-4, -4,  4), Bounds3D<T>::FBL);
    CPPUNIT_ASSERT(bounds.min() == vector_type(-4, -4, -5));
    CPPUNIT_ASSERT(bounds.max() == vector_type( 5,  5,  4));
}

//------------------------------------------------------------------------------

template <typename T>
void
TestBounds3D<T>::testClamp()
{
    USING_NKHIVE_NS

    typedef typename Bounds3D<T>::vector_type vector_type;

    vector_type min(-5, -5, -5);
    vector_type max( 5,  5,  5);
    Bounds3D<T> bounds;

    // near bottom left
    bounds.max() = max; bounds.min() = min;
    bounds.clampNBL(vector_type(-4, -4, -4));
    CPPUNIT_ASSERT(bounds.min() == vector_type(-4, -4, -4));
    CPPUNIT_ASSERT(bounds.max() == vector_type( 5,  5,  5));
    bounds.max() = max; bounds.min() = min;
    bounds.clampNBL(vector_type(-6, -6, -6));
    CPPUNIT_ASSERT(bounds.min() == vector_type(-5, -5, -5));
    CPPUNIT_ASSERT(bounds.max() == vector_type( 5,  5,  5));
    bounds.max() = max; bounds.min() = min;
    bounds.clamp(vector_type(-4, -4, -4), Bounds3D<T>::NBL);
    CPPUNIT_ASSERT(bounds.min() == vector_type(-4, -4, -4));
    CPPUNIT_ASSERT(bounds.max() == vector_type( 5,  5,  5));

    // near bottom right
    bounds.max() = max; bounds.min() = min;
    bounds.clampNBR(vector_type( 4, -4, -4));
    CPPUNIT_ASSERT(bounds.min() == vector_type(-5, -4, -4));
    CPPUNIT_ASSERT(bounds.max() == vector_type( 4,  5,  5));
    bounds.max() = max; bounds.min() = min;
    bounds.clampNBR(vector_type( 6, -6, -6));
    CPPUNIT_ASSERT(bounds.min() == vector_type(-5, -5, -5));
    CPPUNIT_ASSERT(bounds.max() == vector_type( 5,  5,  5));
    bounds.max() = max; bounds.min() = min;
    bounds.clamp(vector_type( 4, -4, -4), Bounds3D<T>::NBR);
    CPPUNIT_ASSERT(bounds.min() == vector_type(-5, -4, -4));
    CPPUNIT_ASSERT(bounds.max() == vector_type( 4,  5,  5));

    // near top right
    bounds.max() = max; bounds.min() = min;
    bounds.clampNTR(vector_type( 4,  4, -4));
    CPPUNIT_ASSERT(bounds.min() == vector_type(-5, -5, -4));
    CPPUNIT_ASSERT(bounds.max() == vector_type( 4,  4,  5));
    bounds.max() = max; bounds.min() = min;
    bounds.clampNTR(vector_type( 6,  6, -6));
    CPPUNIT_ASSERT(bounds.min() == vector_type(-5, -5, -5));
    CPPUNIT_ASSERT(bounds.max() == vector_type( 5,  5,  5));
    bounds.max() = max; bounds.min() = min;
    bounds.clamp(vector_type( 4,  4, -4), Bounds3D<T>::NTR);
    CPPUNIT_ASSERT(bounds.min() == vector_type(-5, -5, -4));
    CPPUNIT_ASSERT(bounds.max() == vector_type( 4,  4,  5));

    // near top left
    bounds.max() = max; bounds.min() = min;
    bounds.clampNTL(vector_type(-4,  4, -4));
    CPPUNIT_ASSERT(bounds.min() == vector_type(-4, -5, -4));
    CPPUNIT_ASSERT(bounds.max() == vector_type( 5,  4,  5));
    bounds.max() = max; bounds.min() = min;
    bounds.clampNTL(vector_type(-6,  6, -6));
    CPPUNIT_ASSERT(bounds.min() == vector_type(-5, -5, -5));
    CPPUNIT_ASSERT(bounds.max() == vector_type( 5,  5,  5));
    bounds.max() = max; bounds.min() = min;
    bounds.clamp(vector_type(-4,  4, -4), Bounds3D<T>::NTL);
    CPPUNIT_ASSERT(bounds.min() == vector_type(-4, -5, -4));
    CPPUNIT_ASSERT(bounds.max() == vector_type( 5,  4,  5));

    // far top left
    bounds.max() = max; bounds.min() = min;
    bounds.clampFTL(vector_type(-4,  4,  4));
    CPPUNIT_ASSERT(bounds.min() == vector_type(-4, -5, -5));
    CPPUNIT_ASSERT(bounds.max() == vector_type( 5,  4,  4));
    bounds.max() = max; bounds.min() = min;
    bounds.clampFTL(vector_type(-6,  6,  6));
    CPPUNIT_ASSERT(bounds.min() == vector_type(-5, -5, -5));
    CPPUNIT_ASSERT(bounds.max() == vector_type( 5,  5,  5));
    bounds.max() = max; bounds.min() = min;
    bounds.clamp(vector_type(-4,  4,  4), Bounds3D<T>::FTL);
    CPPUNIT_ASSERT(bounds.min() == vector_type(-4, -5, -5));
    CPPUNIT_ASSERT(bounds.max() == vector_type( 5,  4,  4));

    // far top right
    bounds.max() = max; bounds.min() = min;
    bounds.clampFTR(vector_type( 4,  4,  4));
    CPPUNIT_ASSERT(bounds.min() == vector_type(-5, -5, -5));
    CPPUNIT_ASSERT(bounds.max() == vector_type( 4,  4,  4));
    bounds.max() = max; bounds.min() = min;
    bounds.clampFTR(vector_type( 6,  6,  6));
    CPPUNIT_ASSERT(bounds.min() == vector_type(-5, -5, -5));
    CPPUNIT_ASSERT(bounds.max() == vector_type( 5,  5,  5));
    bounds.max() = max; bounds.min() = min;
    bounds.clamp(vector_type( 4,  4,  4), Bounds3D<T>::FTR);
    CPPUNIT_ASSERT(bounds.min() == vector_type(-5, -5, -5));
    CPPUNIT_ASSERT(bounds.max() == vector_type( 4,  4,  4));

    // far bottom right
    bounds.max() = max; bounds.min() = min;
    bounds.clampFBR(vector_type( 4, -4,  4));
    CPPUNIT_ASSERT(bounds.min() == vector_type(-5, -4, -5));
    CPPUNIT_ASSERT(bounds.max() == vector_type( 4,  5,  4));
    bounds.max() = max; bounds.min() = min;
    bounds.clampFBR(vector_type( 6, -6,  6));
    CPPUNIT_ASSERT(bounds.min() == vector_type(-5, -5, -5));
    CPPUNIT_ASSERT(bounds.max() == vector_type( 5,  5,  5));
    bounds.max() = max; bounds.min() = min;
    bounds.clamp(vector_type( 4, -4,  4), Bounds3D<T>::FBR);
    CPPUNIT_ASSERT(bounds.min() == vector_type(-5, -4, -5));
    CPPUNIT_ASSERT(bounds.max() == vector_type( 4,  5,  4));

    // far bottom left
    bounds.max() = max; bounds.min() = min;
    bounds.clampFBL(vector_type(-4, -4,  4));
    CPPUNIT_ASSERT(bounds.min() == vector_type(-4, -4, -5));
    CPPUNIT_ASSERT(bounds.max() == vector_type( 5,  5,  4));
    bounds.max() = max; bounds.min() = min;
    bounds.clampFBL(vector_type(-6, -6,  6));
    CPPUNIT_ASSERT(bounds.min() == vector_type(-5, -5, -5));
    CPPUNIT_ASSERT(bounds.max() == vector_type( 5,  5,  5));
    bounds.max() = max; bounds.min() = min;
    bounds.clamp(vector_type(-4, -4,  4), Bounds3D<T>::FBL);
    CPPUNIT_ASSERT(bounds.min() == vector_type(-4, -4, -5));
    CPPUNIT_ASSERT(bounds.max() == vector_type( 5,  5,  4));
}

//------------------------------------------------------------------------------

template <typename T>
void
TestBounds3D<T>::testIntersects()
{
    USING_NKHIVE_NS

    typedef typename Bounds3D<T>::vector_type vector_type;

    // no overlap
    {
        Bounds3D<T> a(vector_type(0, 0, 0), vector_type(5, 5, 5));
        Bounds3D<T> b(vector_type(5, 5, 5), vector_type(10, 10, 10));
        CPPUNIT_ASSERT(a.intersects(b) == false);
    }

    // unit over lap
    {
        Bounds3D<T> a(vector_type(0, 0, 0), vector_type(6, 6, 6));
        Bounds3D<T> b(vector_type(5, 5, 5), vector_type(10, 10, 10));
        CPPUNIT_ASSERT(a.intersects(b) == true);
    }

    // no overlap by one unit
    {
        Bounds3D<T> a(vector_type(0, 0, 0), vector_type(6, 6, 6));
        Bounds3D<T> b(vector_type(6, 5, 5), vector_type(10, 10, 10));
        CPPUNIT_ASSERT(a.intersects(b) == false);
    }

    // plane overlap
    {
        Bounds3D<T> a(vector_type(0, 0, 0), vector_type(6, 6, 6));
        Bounds3D<T> b(vector_type(5, 0, 0), vector_type(10, 6, 6));
        CPPUNIT_ASSERT(a.intersects(b) == true);
    }

    // inside bounds
    {
        Bounds3D<T> a(vector_type(0, 0, 0), vector_type(6, 6, 6));
        Bounds3D<T> b(vector_type(2, 2, 2), vector_type(4, 4, 4));
        CPPUNIT_ASSERT(a.intersects(b) == true);
    }

    // same
    {
        Bounds3D<T> a(vector_type(0, 0, 0), vector_type(6, 6, 6));
        Bounds3D<T> b(vector_type(0, 0, 0), vector_type(6, 6, 6));
        CPPUNIT_ASSERT(a.intersects(b) == true);
    }
}

//------------------------------------------------------------------------------

template <typename T>
void
TestBounds3D<T>::testIntersection()
{
    USING_NKHIVE_NS

    typedef typename Bounds3D<T>::vector_type vector_type;

    // no overlap
    {
        Bounds3D<T> a(vector_type(0, 0, 0), vector_type(5, 5, 5));
        Bounds3D<T> b(vector_type(5, 5, 5), vector_type(10, 10, 10));
        Bounds3D<T> i = a.intersection(b);
        CPPUNIT_ASSERT(i.min() == vector_type(0, 0, 0));
        CPPUNIT_ASSERT(i.max() == vector_type(0, 0, 0));
    }

    // unit overlap
    {
        Bounds3D<T> a(vector_type(0, 0, 0), vector_type(6, 6, 6));
        Bounds3D<T> b(vector_type(5, 5, 5), vector_type(10, 10, 10));
        Bounds3D<T> i = a.intersection(b);
        CPPUNIT_ASSERT(i.min() == vector_type(5, 5, 5));
        CPPUNIT_ASSERT(i.max() == vector_type(6, 6, 6));
        i = b.intersection(a);
        CPPUNIT_ASSERT(i.min() == vector_type(5, 5, 5));
        CPPUNIT_ASSERT(i.max() == vector_type(6, 6, 6));
    }

    // plane overlap
    {
        Bounds3D<T> a(vector_type(0, 0, 0), vector_type(6, 6, 6));
        Bounds3D<T> b(vector_type(5, 0, 0), vector_type(10, 6, 6));
        Bounds3D<T> i = a.intersection(b);
        CPPUNIT_ASSERT(i.min() == vector_type(5, 0, 0));
        CPPUNIT_ASSERT(i.max() == vector_type(6, 6, 6));
        i = b.intersection(a);
        CPPUNIT_ASSERT(i.min() == vector_type(5, 0, 0));
        CPPUNIT_ASSERT(i.max() == vector_type(6, 6, 6));
    }

    // inside bounds
    {
        Bounds3D<T> a(vector_type(0, 0, 0), vector_type(6, 6, 6));
        Bounds3D<T> b(vector_type(2, 2, 2), vector_type(4, 4, 4));
        Bounds3D<T> i = a.intersection(b);
        CPPUNIT_ASSERT(i.min() == vector_type(2, 2, 2));
        CPPUNIT_ASSERT(i.max() == vector_type(4, 4, 4));
        i = b.intersection(a);
        CPPUNIT_ASSERT(i.min() == vector_type(2, 2, 2));
        CPPUNIT_ASSERT(i.max() == vector_type(4, 4, 4));
    }

    // same bounds
    {
        Bounds3D<T> a(vector_type(0, 0, 0), vector_type(6, 6, 6));
        Bounds3D<T> b(vector_type(0, 0, 0), vector_type(6, 6, 6));
        Bounds3D<T> i = a.intersection(b);
        CPPUNIT_ASSERT(i.min() == vector_type(0, 0, 0));
        CPPUNIT_ASSERT(i.max() == vector_type(6, 6, 6));
        i = b.intersection(a);
        CPPUNIT_ASSERT(i.min() == vector_type(0, 0, 0));
        CPPUNIT_ASSERT(i.max() == vector_type(6, 6, 6));
    }
}


//------------------------------------------------------------------------------

template <typename T>
void
TestBounds3D<T>::testContains()
{
    USING_NKHIVE_NS

    typedef typename Bounds3D<T>::vector_type vector_type;

    // same bounds
    {
        Bounds3D<T> a(vector_type(0, 0, 0), vector_type(6, 6, 6));
        Bounds3D<T> b(vector_type(0, 0, 0), vector_type(6, 6, 6));
        CPPUNIT_ASSERT(a.contains(b) == true);
    }

    // inside
    {
        Bounds3D<T> a(vector_type(0, 0, 0), vector_type(6, 6, 6));
        Bounds3D<T> b(vector_type(2, 2, 2), vector_type(4, 4, 4));
        CPPUNIT_ASSERT(a.contains(b) == true);
    }

    // overlap
    {
        Bounds3D<T> a(vector_type(0, 0, 0), vector_type(6, 6, 6));
        Bounds3D<T> b(vector_type(4, 4, 4), vector_type(7, 7, 7));
        CPPUNIT_ASSERT(a.contains(b) == false);
    }
}

//------------------------------------------------------------------------------

template <typename T>
void
TestBounds3D<T>::testSwap()
{
    USING_NKHIVE_NS

    typedef typename Bounds3D<T>::vector_type vector_type;

    vector_type a_min = vector_type(0, 0, 0);
    vector_type a_max = vector_type(2, 2, 2);
    vector_type b_min = vector_type(3, 3, 3);
    vector_type b_max = vector_type(5, 5, 5);

    Bounds3D<T> a(a_min, a_max);
    Bounds3D<T> b(b_min, b_max);
    a.swap(b);

    CPPUNIT_ASSERT(a.min() == b_min);
    CPPUNIT_ASSERT(a.max() == b_max);
    CPPUNIT_ASSERT(b.min() == a_min);
    CPPUNIT_ASSERT(b.max() == a_max);
}

//------------------------------------------------------------------------------

template <typename T>
void
TestBounds3D<T>::testToString()
{
    USING_NKHIVE_NS

    typedef typename Bounds3D<T>::vector_type vector_type;

    Bounds3D<T> bounds;
    bounds.min() = vector_type(-1, -1, -1);
    bounds.max() = vector_type( 1,  1,  1);
    CPPUNIT_ASSERT(bounds.toString() == "[-1, -1, -1] / [1, 1, 1]");
}

//------------------------------------------------------------------------------

template <>
void
TestBounds3D<NKHIVE_NS::index_type>::testToString()
{
    USING_NKHIVE_NS

    typedef Bounds3D<index_type>::vector_type vector_type;

    Bounds3D<index_type> bounds;
    bounds.min() = vector_type(0, 0, 0);
    bounds.max() = vector_type(1, 1, 1);
    CPPUNIT_ASSERT(bounds.toString() == "[0, 0, 0] / [1, 1, 1]");
}

