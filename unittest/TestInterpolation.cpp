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
// TestInterpolation.cpp
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------

#include <nkbase/Tolerance.h>

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <nkhive/interpolation/LinearInterpolation.h>
#include <nkhive/interpolation/CubicInterpolation.h>
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

template<typename T>
class TestInterpolation : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TestInterpolation);
    CPPUNIT_TEST(testLinear);
    CPPUNIT_TEST(testCubic1d);
    CPPUNIT_TEST(testCubic2d);
    CPPUNIT_TEST(testCubic3d);
    CPPUNIT_TEST(testGetIndexBounds);
    CPPUNIT_TEST(testCollectInterpolants);
    CPPUNIT_TEST(testInterp);
    CPPUNIT_TEST(testInterpNegative);
    CPPUNIT_TEST_SUITE_END();

public:

    void setUp();
    void tearDown();
    void testLinear();
    void testCubic1d();
    void testCubic2d();
    void testCubic3d();
    void testGetIndexBounds();
    void testCollectInterpolants();
    void testInterp();
    void testInterpNegative();
};

//------------------------------------------------------------------------------
// test suite registration
//------------------------------------------------------------------------------

CPPUNIT_TEST_SUITE_REGISTRATION(TestInterpolation<float>);
CPPUNIT_TEST_SUITE_REGISTRATION(TestInterpolation<double>);
CPPUNIT_TEST_SUITE_REGISTRATION(TestInterpolation<half>);

//------------------------------------------------------------------------------
// class implementation
//------------------------------------------------------------------------------

template<typename T>
void
TestInterpolation<T>::setUp()
{
}

//------------------------------------------------------------------------------

template<typename T>
void
TestInterpolation<T>::tearDown()
{
}

//------------------------------------------------------------------------------

template<typename T>
void
TestInterpolation<T>::testLinear()
{
    USING_NK_NS
    USING_NKHIVE_NS
  
    // construct volume 
    T default_val(1); 
    vec3d res(1.0);
    vec3d kernel_offset(0.5);
    typename Volume<T>::shared_ptr volume(
                        new Volume<T>(2, 1, default_val, res, kernel_offset));

    // set some cell values
    volume->set(0, 1, 0, T(2));
    volume->set(0, 1, 1, T(2));
    volume->set(1, 1, 0, T(2));
    volume->set(1, 1, 1, T(2));

    // create interpolation object
    LinearInterpolation<T> linear_interp(volume);
    
    // test interior point
    T result(0);
    linear_interp.interp(1.0, 1.0, 1.0, result);
    CHECK_RESULT(1.5);

    // test boundary point
    linear_interp.interp(1.0, 0.0, 1.0, result);
    CHECK_RESULT(1.0);

    // test boundary point
    linear_interp.interp(1.0, 2.0, 1.0, result);
    CHECK_RESULT(1.5);

    // test interior point
    linear_interp.interp(1.0, 0.75, 1.0, result);
    CHECK_RESULT(1.25);
  
    // test interior point
    linear_interp.interp(1.0, 1.25, 1.0, result);
    CHECK_RESULT(1.75);
    
    // test y axis 
    // destroy and create new one
    // clear would be nice here
    volume = typename Volume<T>::shared_ptr(
                        new Volume<T>(2, 1, default_val, res, kernel_offset));
   
    // set some cell values
    volume->set(1, 0, 0, T(2));
    volume->set(1, 0, 1, T(2));
    volume->set(1, 1, 0, T(2));
    volume->set(1, 1, 1, T(2));
    
    // create interpolation object
    LinearInterpolation<T> linear_interp2(volume);
    
    // test interior point
    linear_interp2.interp(1.0, 1.0, 1.0, result);
    CHECK_RESULT(1.5);

    // test boundary point
    linear_interp2.interp(0.0, 0.75, 1.0, result);
    CHECK_RESULT(1.0);

    // test boundary point
    linear_interp2.interp(2.0, 1.0, 1.0, result);
    CHECK_RESULT(1.5);
   
    // test z axis 
    // destroy and create new one
    // clear would be nice here
    volume = typename Volume<T>::shared_ptr(
                        new Volume<T>(2, 1, default_val, res, kernel_offset)); 
   
    // set some cell values
    volume->set(0, 0, 1, T(2));
    volume->set(1, 0, 1, T(2));
    volume->set(0, 1, 1, T(2));
    volume->set(1, 1, 1, T(2));
    
    // create interpolation object
    LinearInterpolation<T> linear_interp3(volume);
    
    // test interior point
    linear_interp3.interp(1.0, 1.0, 1.0, result);
    CHECK_RESULT(1.5);

    // test boundary point
    linear_interp3.interp(1.0, 1.9, 0.0, result);
    CHECK_RESULT(1.0);

    // test boundary point
    linear_interp3.interp(1.0, 1.0, 2.0, result);
    CHECK_RESULT(1.5);

    // test some more interior points
    volume = typename Volume<T>::shared_ptr(
                        new Volume<T>(2, 1, default_val, res, kernel_offset));

    // set some cell values
    volume->set(0,0,0, T(1));
    volume->set(0,0,1, T(2));
    volume->set(0,1,0, T(3));
    volume->set(0,1,1, T(4));
    volume->set(1,0,0, T(5));
    volume->set(1,0,1, T(6));
    volume->set(1,1,0, T(7));
    volume->set(1,1,1, T(8));

    LinearInterpolation<T> linear_interp4(volume);
    linear_interp4.interp(0.75, 1.15, 0.9, result);
    CHECK_RESULT(3.7);
}

//------------------------------------------------------------------------------

template<typename T>
void
TestInterpolation<T>::testCubic1d()
{
    USING_NKHIVE_NS
  
    // create interpolator
    CubicInterpolation<T> cubic_interp;

    typename CubicInterpolation<T>::calc_type interpolants[4];
    interpolants[0] = T(0);
    interpolants[1] = T(1);
    interpolants[2] = T(0);
    interpolants[3] = T(1);

    typename CubicInterpolation<T>::calc_type result;
    cubic_interp.interpolate1d(interpolants, 0.25, result);
    CHECK_RESULT(0.84375); 

    cubic_interp.interpolate1d(interpolants, 0.3, result);
    CHECK_RESULT(0.784); 

    cubic_interp.interpolate1d(interpolants, 0.0, result);
    CHECK_RESULT(1.0); 

    cubic_interp.interpolate1d(interpolants, 1.0, result);
    CHECK_RESULT(0.0); 

    cubic_interp.interpolate1d(interpolants, 0.675, result);
    CHECK_RESULT(0.24821875); 
    
}

//------------------------------------------------------------------------------

template<typename T>
void
TestInterpolation<T>::testCubic2d()
{
    USING_NK_NS
    USING_NKHIVE_NS
  
    // create interpolator
    CubicInterpolation<T> cubic_interp;

    typename CubicInterpolation<T>::calc_type interpolants[16];
   
    // fill a 2d grid with some data 
    i32 value=0; 
    for (i32 y=0; y<4; ++y) {
        for (i32 x=0; x<4; ++x) {
            interpolants[GET_INDEX_2D(x,y)] = value++;
        }
    }

    // test internal point
    typename CubicInterpolation<T>::calc_type coords[2];
    coords[0] = 0.25;
    coords[1] = 0.5;

    typename CubicInterpolation<T>::calc_type result;
    cubic_interp.interpolate2d(interpolants, coords, result);
    CHECK_RESULT(7.25);

    // test boundary points
    coords[0] = 0.0;
    coords[1] = 0.0;
    cubic_interp.interpolate2d(interpolants, coords, result);
    CHECK_RESULT(5.0);

    coords[0] = 1.0;
    coords[1] = 0.0;
    cubic_interp.interpolate2d(interpolants, coords, result);
    CHECK_RESULT(6.0);

    coords[0] = 0.0;
    coords[1] = 1.0;
    cubic_interp.interpolate2d(interpolants, coords, result);
    CHECK_RESULT(9.0);

    coords[0] = 1.0;
    coords[1] = 1.0;
    cubic_interp.interpolate2d(interpolants, coords, result);
    CHECK_RESULT(10.0);

    // try some non-linear values
    value=1; 
    for (i32 x=0; x<4; ++x) {
        for (i32 y=0; y<4; ++y) {
            value = (y==0) ? value : value ^ 0x1;
            interpolants[GET_INDEX_2D(x,y)] = value; 
        }
    }

    coords[0] = 0.75;
    coords[1] = 0.25;
    cubic_interp.interpolate2d(interpolants, coords, result);
    CHECK_RESULT(0.263671875);

    
}

//------------------------------------------------------------------------------

template<typename T>
void
TestInterpolation<T>::testCubic3d()
{
    USING_NK_NS
    USING_NKHIVE_NS
  
    // create interpolator
    CubicInterpolation<T> cubic_interp;

    typename CubicInterpolation<T>::calc_type interpolants[64];
    index_type lg_size = 2;

    // easy to track values    
    i32 value=0; 
    for (i32 z=0; z<4; ++z) {
        for (i32 y=0; y<4; ++y) {
            for (i32 x=0; x<4; ++x) {
                interpolants[getIndex(x,y,z,lg_size)] = value++;
            }
        }
    }

    // test interiot point
    typename CubicInterpolation<T>::calc_type coords[3];
    coords[0] = 0.25;
    coords[1] = 0.5;
    coords[2] = 0.75;

    typename CubicInterpolation<T>::calc_type result;
    cubic_interp.interpolate3d(interpolants, coords, result);
    CHECK_RESULT(35.25);

    // test boundary points
    coords[0] = 0.0;
    coords[1] = 0.0;
    coords[2] = 0.0;
    cubic_interp.interpolate3d(interpolants, coords, result);
    CHECK_RESULT(21.0);

    coords[0] = 0.0;
    coords[1] = 0.0;
    coords[2] = 1.0;
    cubic_interp.interpolate3d(interpolants, coords, result);
    CHECK_RESULT(37.0);

    coords[0] = 1.0;
    coords[1] = 0.0;
    coords[2] = 1.0;
    cubic_interp.interpolate3d(interpolants, coords, result);
    CHECK_RESULT(38.0);

    coords[0] = 1.0;
    coords[1] = 1.0;
    coords[2] = 1.0;
    cubic_interp.interpolate3d(interpolants, coords, result);
    CHECK_RESULT(42.0);

    // try some non-linear values
    typename CubicInterpolation<T>::calc_type interpolants2[64] = {  1, 0, 1, 0,
                                                                     0, 1, 0, 1,
                                                                     1, 0, 1, 0,
                                                                     0, 1, 0, 1,
                        
                                                                     0, 1, 0, 1,
                                                                     1, 0, 1, 0,
                                                                     0, 1, 0, 1,
                                                                     1, 0, 1, 0,
                                                        
                                                                     1, 0, 1, 0,
                                                                     0, 1, 0, 1,
                                                                     1, 0, 1, 0,
                                                                     0, 1, 0, 1,
                        
                                                                     0, 1, 0, 1,
                                                                     1, 0, 1, 0,
                                                                     0, 1, 0, 1,
                                                                     1, 0, 1, 0
                                                                  };
    
    coords[0] = 0.15;
    coords[1] = 0.75;
    coords[2] = 0.25;
    cubic_interp.interpolate3d(interpolants2, coords, result);
    CHECK_RESULT(0.7076142578125);

}

//------------------------------------------------------------------------------

template<typename T>
void
TestInterpolation<T>::testGetIndexBounds()
{
    USING_NK_NS
    USING_NKHIVE_NS
  
    // construct volume 
    T default_val(1); 
    vec3d res(1.0);
    vec3d kernel_offset(0.5);
    typename Volume<T>::shared_ptr volume(
                        new Volume<T>(1, 2, default_val, res, kernel_offset));

    // create interpolator
    CubicInterpolation<T> cubic_interp(volume);

    // test interior point on a cell boundary
    vec3d voxel_coords(2.0,2.0,2.0);
    vec3i voxel_indices(2,2,2);

    vec3i min_indices, max_indices;
    cubic_interp.getIndexBounds(voxel_coords, voxel_indices, 
                               min_indices, max_indices);

    CPPUNIT_ASSERT(min_indices == vec3i(0,0,0));
    CPPUNIT_ASSERT(max_indices == vec3i(3,3,3));

    // test interior point
    voxel_coords = vec3d(2.15,2.2,2.1);
    voxel_indices = vec3i(2,2,2);

    cubic_interp.getIndexBounds(voxel_coords, voxel_indices, 
                               min_indices, max_indices);

    CPPUNIT_ASSERT(min_indices == vec3i(0,0,0));
    CPPUNIT_ASSERT(max_indices == vec3i(3,3,3));
    
    // test interior point rounded up
    voxel_coords = vec3d(2.75,2.8,2.9);
    voxel_indices = vec3i(2,2,2);

    cubic_interp.getIndexBounds(voxel_coords, voxel_indices, 
                               min_indices, max_indices);

    CPPUNIT_ASSERT(min_indices == vec3i(1,1,1));
    CPPUNIT_ASSERT(max_indices == vec3i(4,4,4));
    
    // test an arbitrary point
    voxel_coords = vec3d(1.9,1.2,3.2);
    voxel_indices = vec3i(1,1,3);

    cubic_interp.getIndexBounds(voxel_coords, voxel_indices, 
                               min_indices, max_indices);

    CPPUNIT_ASSERT(min_indices == vec3i(0,-1,1));
    CPPUNIT_ASSERT(max_indices == vec3i(3,2,4));
}

//------------------------------------------------------------------------------

template<typename T>
void
TestInterpolation<T>::testCollectInterpolants()
{
    USING_NK_NS
    USING_NKHIVE_NS

    // construct volume 
    T default_val(1); 
    vec3d res(1.0);
    vec3d kernel_offset(0.5);
    typename Volume<T>::shared_ptr volume(
                        new Volume<T>(1, 2, default_val, res, kernel_offset));
    
    // easy to track values    
    i32 value=0; 
    for (i32 z=0; z<4; ++z) {
        for (i32 y=0; y<4; ++y) {
            for (i32 x=0; x<4; ++x) {
                volume->set(x,y,z,value++);
            }
        }
    }

    vec3i min_indices(0,0,0);
    vec3i max_indices(3,3,3);

    CubicInterpolation<T> cubic_interp(volume);

    // test interior
    typename CubicInterpolation<T>::calc_type interpolants[64];
    cubic_interp.collectInterpolants(min_indices, max_indices, interpolants);

    for (i32 i=0; i<64; ++i) {
        CPPUNIT_ASSERT(interpolants[i] == 
                                typename CubicInterpolation<T>::calc_type(i));
    }

    // test boundary
    min_indices = vec3i(0,0,1);
    max_indices = vec3i(3,3,4);
    cubic_interp.collectInterpolants(min_indices, max_indices, interpolants);

    for (i32 i=0; i<48; ++i) {
        CPPUNIT_ASSERT(interpolants[i] == 
                            typename CubicInterpolation<T>::calc_type(i + 16));
    }

    for (i32 i=0; i<16; ++i) {
        CPPUNIT_ASSERT(interpolants[i + 48] == 
                                 typename CubicInterpolation<T>::calc_type(1));
    }
}

//------------------------------------------------------------------------------

template<typename T>
void
TestInterpolation<T>::testInterp()
{
    USING_NK_NS
    USING_NKHIVE_NS

    // construct volume 
    T default_val(1); 
    vec3d res(1.0);
    vec3d kernel_offset(0.5);
    typename Volume<T>::shared_ptr volume(
                        new Volume<T>(1, 2, default_val, res, kernel_offset));

    // easy to track values    
    i32 value=0; 
    for (i32 z=0; z<4; ++z) {
        for (i32 y=0; y<4; ++y) {
            for (i32 x=0; x<4; ++x) {
                volume->set(x,y,z,value++);
            }
        }
    }

    // test an interior point
    typename CubicInterpolation<T>::calc_type coords[3];
    coords[0] = 1.75;
    coords[1] = 2.0;
    coords[2] = 2.25;

    CubicInterpolation<T> cubic_interp(volume);

    T result;
    cubic_interp.interp(coords[0], coords[1], coords[2], result);
    CHECK_RESULT(35.25);
    
    // test boundary points
    coords[0] = 1.5;
    coords[1] = 1.5;
    coords[2] = 1.5;
    cubic_interp.interp(coords[0], coords[1], coords[2], result);
    CHECK_RESULT(21.0);

    coords[0] = 1.5;
    coords[1] = 1.5;
    coords[2] = 2.5;
    cubic_interp.interp(coords[0], coords[1], coords[2], result);
    CHECK_RESULT(37.0);

    coords[0] = 2.5;
    coords[1] = 1.5;
    coords[2] = 2.5;
    cubic_interp.interp(coords[0], coords[1], coords[2], result);
    CHECK_RESULT(38.0);

    coords[0] = 2.5;
    coords[1] = 2.5;
    coords[2] = 2.5;
    cubic_interp.interp(coords[0], coords[1], coords[2], result);
    CHECK_RESULT(42.0);

}

//------------------------------------------------------------------------------

template<typename T>
void
TestInterpolation<T>::testInterpNegative()
{
    USING_NK_NS
    USING_NKHIVE_NS
  
    // construct volume 
    T default_val(1); 
    vec3d res(1.0);
    vec3d kernel_offset(0.5);
    typename Volume<T>::shared_ptr volume(
                        new Volume<T>(2, 1, default_val, res, kernel_offset));

    // set some cell values
    volume->set(-1, -2, -1, T(2));
    volume->set(-1, -2, -2, T(2));
    volume->set(-2, -2, -1, T(2));
    volume->set(-2, -2, -2, T(2));

    // create interpolation object
    LinearInterpolation<T> linear_interp(volume);
    
    // test interior point
    T result(0);
    linear_interp.interp(-1.0, -1.0, -1.0, result);
    CHECK_RESULT(1.5);

    // test boundary point
    linear_interp.interp(-1.0, 0.0, -1.0, result);
    CHECK_RESULT(1.0);

    // test boundary point
    linear_interp.interp(-1.0, -2.0, -1.0, result);
    CHECK_RESULT(1.5);

    // test interior point
    linear_interp.interp(-1.0, -0.75, -1.0, result);
    CHECK_RESULT(1.25);
  
    // test interior point
    linear_interp.interp(-1.0, -1.25, -1.0, result);
    CHECK_RESULT(1.75);
}

//------------------------------------------------------------------------------

