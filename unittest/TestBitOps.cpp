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
// TestBitOps.cpp
//------------------------------------------------------------------------------

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <nkhive/bitfields/BitOps.h>

class TestBitFieldOps : public CppUnit::TestFixture {

    CPPUNIT_TEST_SUITE(TestBitFieldOps);
    CPPUNIT_TEST(testBitsOf);
    CPPUNIT_TEST(testGetBitMask);
    CPPUNIT_TEST(testGetBitMaskRange);
    CPPUNIT_TEST(testGetBitMaskFilled);
    CPPUNIT_TEST(testNumBits3D);
    CPPUNIT_TEST(testGetIndex);
    CPPUNIT_TEST(testModulo);
    CPPUNIT_TEST(testGetBitCount);
    CPPUNIT_TEST(testIsPow2);
    CPPUNIT_TEST(testRoundPow2);
    CPPUNIT_TEST(testIsNegative);
    CPPUNIT_TEST(testGetQuadrant);
    CPPUNIT_TEST(testGetQuadrantCoordinates);
    CPPUNIT_TEST(testAbs);
    CPPUNIT_TEST(testGetFirstSetBitIndex);
    CPPUNIT_TEST(testGetLastSetBitIndex);
    CPPUNIT_TEST_SUITE_END();
    
public:
    void setUp() {}
    void tearDown() {}
    
    void testBitsOf();
    void testGetBitMask();
    void testGetBitMaskRange();
    void testGetBitMaskFilled();
    void testNumBits3D();
    void testGetIndex();
    void testModulo();
    void testGetBitCount();
    void testIsPow2();
    void testRoundPow2();
    void testIsNegative();
    void testGetQuadrant();
    void testGetQuadrantCoordinates();
    void testAbs();
    void testGetFirstSetBitIndex();
    void testGetLastSetBitIndex();
};

//------------------------------------------------------------------------------

CPPUNIT_TEST_SUITE_REGISTRATION(TestBitFieldOps);

//------------------------------------------------------------------------------
    
void 
TestBitFieldOps::testBitsOf() 
{
    USING_NKHIVE_NS;

    CPPUNIT_ASSERT(bitsof(int8_t)   == 8);
    CPPUNIT_ASSERT(bitsof(uint8_t)  == 8);
    CPPUNIT_ASSERT(bitsof(int16_t)  == 16);
    CPPUNIT_ASSERT(bitsof(uint16_t) == 16);
    CPPUNIT_ASSERT(bitsof(int32_t)  == 32);
    CPPUNIT_ASSERT(bitsof(uint32_t) == 32);
    CPPUNIT_ASSERT(bitsof(int64_t)  == 64);
    CPPUNIT_ASSERT(bitsof(uint64_t) == 64);
}

//------------------------------------------------------------------------------
    
void 
TestBitFieldOps::testGetBitMask() 
{
    USING_NKHIVE_NS;

    CPPUNIT_ASSERT(getBitMask(int8_t,    4) == 0x08);
    CPPUNIT_ASSERT(getBitMask(int8_t,    1) == 0x40);
    CPPUNIT_ASSERT(getBitMask(uint8_t,   3) == 0x10);
    CPPUNIT_ASSERT(getBitMask(uint8_t,   2) == 0x20);
    CPPUNIT_ASSERT(getBitMask(int16_t,   0) == 0x8000);
    CPPUNIT_ASSERT(getBitMask(int16_t,  10) == 0x0020);
    CPPUNIT_ASSERT(getBitMask(uint16_t, 15) == 0x0001);
    CPPUNIT_ASSERT(getBitMask(uint16_t,  9) == 0x0040);
    CPPUNIT_ASSERT(getBitMask(int32_t,  31) == 0x00000001);
    CPPUNIT_ASSERT(getBitMask(int32_t,  20) == 0x00000800);
    CPPUNIT_ASSERT(getBitMask(uint32_t, 12) == 0x00080000);
    CPPUNIT_ASSERT(getBitMask(uint32_t, 24) == 0x00000080);
    CPPUNIT_ASSERT(getBitMask(int64_t,  61) == 0x0000000000000004);
    CPPUNIT_ASSERT(getBitMask(int64_t,  15) == 0x0001000000000000ll);
    CPPUNIT_ASSERT(getBitMask(uint64_t,  2) == 0x2000000000000000llu);
    CPPUNIT_ASSERT(getBitMask(uint64_t,  7) == 0x0100000000000000llu);
}

//------------------------------------------------------------------------------
    
void 
TestBitFieldOps::testGetBitMaskRange() 
{
    USING_NKHIVE_NS;

    CPPUNIT_ASSERT(getBitMaskRange(uint8_t,   4) == 0xF0);
    CPPUNIT_ASSERT(getBitMaskRange(uint8_t,   1) == 0x80);
    CPPUNIT_ASSERT(getBitMaskRange(uint8_t,   3) == 0xE0);
    CPPUNIT_ASSERT(getBitMaskRange(uint8_t,   2) == 0xC0);
    CPPUNIT_ASSERT(getBitMaskRange(uint16_t,  0) == 0x0000);
    CPPUNIT_ASSERT(getBitMaskRange(uint16_t, 10) == 0xFFC0);
    CPPUNIT_ASSERT(getBitMaskRange(uint16_t, 15) == 0xFFFE);
    CPPUNIT_ASSERT(getBitMaskRange(uint16_t,  9) == 0xFF80);
    CPPUNIT_ASSERT(getBitMaskRange(uint32_t, 31) == 0xFFFFFFFE);
    CPPUNIT_ASSERT(getBitMaskRange(uint32_t, 20) == 0xFFFFF000);
    CPPUNIT_ASSERT(getBitMaskRange(uint32_t, 12) == 0xFFF00000);
    CPPUNIT_ASSERT(getBitMaskRange(uint32_t, 24) == 0xFFFFFF00);
    CPPUNIT_ASSERT(getBitMaskRange(uint64_t, 61) == 0xFFFFFFFFFFFFFFF8llu);
    CPPUNIT_ASSERT(getBitMaskRange(uint64_t, 15) == 0xFFFE000000000000llu);
    CPPUNIT_ASSERT(getBitMaskRange(uint64_t,  2) == 0xC000000000000000llu);
    CPPUNIT_ASSERT(getBitMaskRange(uint64_t,  7) == 0xFE00000000000000llu);
}

//------------------------------------------------------------------------------
    
void 
TestBitFieldOps::testGetBitMaskFilled() 
{
    USING_NKHIVE_NS;

    CPPUNIT_ASSERT(getBitMaskFilled(int8_t)   == (int8_t)0xFF);
    CPPUNIT_ASSERT(getBitMaskFilled(uint8_t)  == (uint8_t)0xFF);
    CPPUNIT_ASSERT(getBitMaskFilled(int16_t)  == (int16_t)0xFFFF);
    CPPUNIT_ASSERT(getBitMaskFilled(uint16_t) == (uint16_t)0xFFFF);
    CPPUNIT_ASSERT(getBitMaskFilled(int32_t)  == (int32_t)0xFFFFFFFF);
    CPPUNIT_ASSERT(getBitMaskFilled(uint32_t) == (uint32_t)0xFFFFFFFF);
    CPPUNIT_ASSERT(getBitMaskFilled(int64_t)  == (int64_t)0xFFFFFFFFFFFFFFFFll);
    CPPUNIT_ASSERT(getBitMaskFilled(uint64_t) == 
                                    (uint64_t)0xFFFFFFFFFFFFFFFFllu);
}

//------------------------------------------------------------------------------

void 
TestBitFieldOps::testNumBits3D()
{
    USING_NKHIVE_NS;

    index_type size = numBits3D(1);
    CPPUNIT_ASSERT(size == 8);

    size = numBits3D(2);
    CPPUNIT_ASSERT(size == 64);

    size = numBits3D(3); 
    CPPUNIT_ASSERT(size == 512);
}

//------------------------------------------------------------------------------

void 
TestBitFieldOps::testGetIndex()
{
    USING_NKHIVE_NS

    index_type i = getIndex(0, 0, 0, 1);
    CPPUNIT_ASSERT(i == 0);

    i = getIndex(0, 0, 1, 2); 
    CPPUNIT_ASSERT(i == 16);

    i = getIndex(3, 3, 3, 2);
    CPPUNIT_ASSERT(i == 63);

    i = getIndex(2, 1, 0, 2);
    CPPUNIT_ASSERT(i == 6);

    i = getIndex(2, 1, 3, 2);
    CPPUNIT_ASSERT(i == 54);
}

//------------------------------------------------------------------------------

void 
TestBitFieldOps::testModulo()
{
    USING_NKHIVE_NS
    
    CPPUNIT_ASSERT(1 == moduloLg(1, 1));
    CPPUNIT_ASSERT(0 == moduloLg(2, 1));
    
    for (index_type i = 0; i < 64; ++i) {
        CPPUNIT_ASSERT(i == moduloLg(i, 6));
    }
        
    CPPUNIT_ASSERT(0 == moduloLg(64, 6));
    
    for (index_type i = 65; i < 128; ++i) {
        CPPUNIT_ASSERT((i - 64) == moduloLg(i, 6));
    }
        
    CPPUNIT_ASSERT(0 == moduloLg(128, 6));
    
    CPPUNIT_ASSERT(2 == moduloLg(130, 6));
}

//------------------------------------------------------------------------------

void
TestBitFieldOps::testGetBitCount()
{
    USING_NKHIVE_NS

    index_type c;
   
    c = getBitCount<index_type>(0x00000000); 
    CPPUNIT_ASSERT(c == 0);

    c = getBitCount<index_type>(0xF0000000); 
    CPPUNIT_ASSERT(c == 4);

    c = getBitCount<index_type>(0x0000000F); 
    CPPUNIT_ASSERT(c == 4);

    c = getBitCount<index_type>(0x000FF000); 
    CPPUNIT_ASSERT(c == 8);

    c = getBitCount<index_type>(0xF00FF00F); 
    CPPUNIT_ASSERT(c == 16);

    c = getBitCount<index_type>(0x01234567); 
    CPPUNIT_ASSERT(c == 12);

    c = getBitCount<index_type>(0x89ABCDEF); 
    CPPUNIT_ASSERT(c == 20);
}

//------------------------------------------------------------------------------

void 
TestBitFieldOps::testIsPow2()
{
    USING_NKHIVE_NS

    bool is_pow2;

    is_pow2 = isPow2(0);
    CPPUNIT_ASSERT(is_pow2 == false);

    is_pow2 = isPow2(3); 
    CPPUNIT_ASSERT(is_pow2 == false);

    is_pow2 = isPow2(5);
    CPPUNIT_ASSERT(is_pow2 == false);

    is_pow2 = isPow2(7);
    CPPUNIT_ASSERT(is_pow2 == false);

    is_pow2 = isPow2(15);
    CPPUNIT_ASSERT(is_pow2 == false);

    is_pow2 = isPow2(127);
    CPPUNIT_ASSERT(is_pow2 == false);

    is_pow2 = isPow2(1);
    CPPUNIT_ASSERT(is_pow2 == true);

    is_pow2 = isPow2(2); 
    CPPUNIT_ASSERT(is_pow2 == true);

    is_pow2 = isPow2(16);
    CPPUNIT_ASSERT(is_pow2 == true);

    is_pow2 = isPow2(32);
    CPPUNIT_ASSERT(is_pow2 == true);

    is_pow2 = isPow2(64);
    CPPUNIT_ASSERT(is_pow2 == true);

    is_pow2 = isPow2(128);
    CPPUNIT_ASSERT(is_pow2 == true);
}

//------------------------------------------------------------------------------

void 
TestBitFieldOps::testRoundPow2()
{
    USING_NKHIVE_NS

    index_type pow2;

    pow2 = roundPow2(0, 2);
    CPPUNIT_ASSERT(pow2 == 0);

    pow2 = roundPow2(3, 4); 
    CPPUNIT_ASSERT(pow2 == 4);

    pow2 = roundPow2(7, 8);
    CPPUNIT_ASSERT(pow2 == 8);

    pow2 = roundPow2(8, 32);
    CPPUNIT_ASSERT(pow2 == 32);

    pow2 = roundPow2(16, 32);
    CPPUNIT_ASSERT(pow2 == 32);

    pow2 = roundPow2(16, 32);
    CPPUNIT_ASSERT(pow2 == 32);

    pow2 = roundPow2(32, 32);
    CPPUNIT_ASSERT(pow2 == 32);

    pow2 = roundPow2(33, 32); 
    CPPUNIT_ASSERT(pow2 == 64);

    pow2 = roundPow2(63, 64);
    CPPUNIT_ASSERT(pow2 == 64);

    pow2 = roundPow2(100, 64);
    CPPUNIT_ASSERT(pow2 == 128);

    pow2 = roundPow2(128, 64);
    CPPUNIT_ASSERT(pow2 == 128);

    pow2 = roundPow2(129, 128);
    CPPUNIT_ASSERT(pow2 == 256);
}

//------------------------------------------------------------------------------

void
TestBitFieldOps::testIsNegative()
{
    USING_NKHIVE_NS

    int32_t i = 0;
    CPPUNIT_ASSERT(isNegative(i) == 0);

    i = -1;
    CPPUNIT_ASSERT(isNegative(i) == 1);

    i = 1;
    CPPUNIT_ASSERT(isNegative(i) == 0);

    i = 123156;
    CPPUNIT_ASSERT(isNegative(i) == 0);

    i = -123556;
    CPPUNIT_ASSERT(isNegative(i) == 1);

    i = 1 << 31;
    CPPUNIT_ASSERT(isNegative(i) == 1);

    int64_t li = 0;
    CPPUNIT_ASSERT(isNegative(li) == 0);

    li = -1;
    CPPUNIT_ASSERT(isNegative(li) == 1);

    li = 1;
    CPPUNIT_ASSERT(isNegative(li) == 0);

    li = 123156;
    CPPUNIT_ASSERT(isNegative(li) == 0);

    li = -123556;
    CPPUNIT_ASSERT(isNegative(li) == 1);

    li = 1ll << 63;
    CPPUNIT_ASSERT(isNegative(li) == 1);
}

//------------------------------------------------------------------------------

void
TestBitFieldOps::testGetQuadrant()
{
    USING_NKHIVE_NS

    #define TEST_QUADRANT(inttype)                      \
    {                                                   \
        inttype i, j, k, ii, jj, kk;                    \
        i = 1, j = 2, k = 3;                            \
        CPPUNIT_ASSERT(getQuadrant(i, j, k) == 0);      \
        getQuadrantOffsets(ii, jj, kk, 0);                     \
        CPPUNIT_ASSERT(ii == 0 && jj == 0 && kk == 0);  \
        i = 1; j = 2; k = -3;                           \
        CPPUNIT_ASSERT(getQuadrant(i, j, k) == 1);      \
        getQuadrantOffsets(ii, jj, kk, 1);                     \
        CPPUNIT_ASSERT(ii == 0 && jj == 0 && kk == 1);  \
        i = 1; j = -2; k = 3;                           \
        CPPUNIT_ASSERT(getQuadrant(i, j, k) == 2);      \
        getQuadrantOffsets(ii, jj, kk, 2);                     \
        CPPUNIT_ASSERT(ii == 0 && jj == 1 && kk == 0);  \
        i = -1; j = 2; k = 3;                           \
        CPPUNIT_ASSERT(getQuadrant(i, j, k) == 4);      \
        getQuadrantOffsets(ii, jj, kk, 4);                     \
        CPPUNIT_ASSERT(ii == 1 && jj == 0 && kk == 0);  \
        i = 1; j = -2; k = -3;                          \
        CPPUNIT_ASSERT(getQuadrant(i, j, k) == 3);      \
        getQuadrantOffsets(ii, jj, kk, 3);                     \
        CPPUNIT_ASSERT(ii == 0 && jj == 1 && kk == 1);  \
        i = -1; j = -2; k = 3;                          \
        CPPUNIT_ASSERT(getQuadrant(i, j, k) == 6);      \
        getQuadrantOffsets(ii, jj, kk, 6);                     \
        CPPUNIT_ASSERT(ii == 1 && jj == 1 && kk == 0);  \
        i = -1; j = 2; k = -3;                          \
        CPPUNIT_ASSERT(getQuadrant(i, j, k) == 5);      \
        getQuadrantOffsets(ii, jj, kk, 5);                     \
        CPPUNIT_ASSERT(ii == 1 && jj == 0 && kk == 1);  \
        i = -1; j = -2; k = -3;                         \
        CPPUNIT_ASSERT(getQuadrant(i, j, k) == 7);      \
        getQuadrantOffsets(ii, jj, kk, 7);                     \
        CPPUNIT_ASSERT(ii == 1 && jj == 1 && kk == 1);  \
    }                                                   \

    TEST_QUADRANT(int32_t);
    TEST_QUADRANT(int64_t);
}

//------------------------------------------------------------------------------

void
TestBitFieldOps::testGetQuadrantCoordinates()
{
    USING_NKHIVE_NS

    #define TEST_QUADRANT_COORDS(inttype)                       \
    {                                                           \
        {                                                       \
            inttype i = 1, j = 2, k = 3;                        \
            getQuadrantCoordinates(i, j, k, 0);                 \
            CPPUNIT_ASSERT(i == 1);                             \
            CPPUNIT_ASSERT(j == 2);                             \
            CPPUNIT_ASSERT(k == 3);                             \
        }                                                       \
                                                                \
        {                                                       \
            inttype i = 1, j = 2, k = 3;                        \
            getQuadrantCoordinates(i, j, k, 1);                 \
            CPPUNIT_ASSERT(i == 1);                             \
            CPPUNIT_ASSERT(j == 2);                             \
            CPPUNIT_ASSERT(k == -3);                            \
        }                                                       \
                                                                \
        {                                                       \
            inttype i = 1, j = 2, k = 3;                        \
            getQuadrantCoordinates(i, j, k, 2);                 \
            CPPUNIT_ASSERT(i == 1);                             \
            CPPUNIT_ASSERT(j == -2);                            \
            CPPUNIT_ASSERT(k == 3);                             \
        }                                                       \
                                                                \
        {                                                       \
            inttype i = 1, j = 2, k = 3;                        \
            getQuadrantCoordinates(i, j, k, 3);                 \
            CPPUNIT_ASSERT(i == 1);                             \
            CPPUNIT_ASSERT(j == -2);                            \
            CPPUNIT_ASSERT(k == -3);                            \
        }                                                       \
                                                                \
        {                                                       \
            inttype i = 1, j = 2, k = 3;                        \
            getQuadrantCoordinates(i, j, k, 4);                 \
            CPPUNIT_ASSERT(i == -1);                            \
            CPPUNIT_ASSERT(j == 2);                             \
            CPPUNIT_ASSERT(k == 3);                             \
        }                                                       \
                                                                \
        {                                                       \
            inttype i = 1, j = 2, k = 3;                        \
            getQuadrantCoordinates(i, j, k, 5);                 \
            CPPUNIT_ASSERT(i == -1);                            \
            CPPUNIT_ASSERT(j == 2);                             \
            CPPUNIT_ASSERT(k == -3);                            \
        }                                                       \
                                                                \
        {                                                       \
            inttype i = 1, j = 2, k = 3;                        \
            getQuadrantCoordinates(i, j, k, 6);                 \
            CPPUNIT_ASSERT(i == -1);                            \
            CPPUNIT_ASSERT(j == -2);                            \
            CPPUNIT_ASSERT(k == 3);                             \
        }                                                       \
                                                                \
        {                                                       \
            inttype i = 1, j = 2, k = 3;                        \
            getQuadrantCoordinates(i, j, k, 7);                 \
            CPPUNIT_ASSERT(i == -1);                            \
            CPPUNIT_ASSERT(j == -2);                            \
            CPPUNIT_ASSERT(k == -3);                            \
        }                                                       \
    }                                                           

    TEST_QUADRANT_COORDS(int8_t);
    TEST_QUADRANT_COORDS(int16_t);
    TEST_QUADRANT_COORDS(int32_t);
    TEST_QUADRANT_COORDS(int64_t);
}

//------------------------------------------------------------------------------

void
TestBitFieldOps::testAbs()
{
    USING_NKHIVE_NS

    #define TEST_ABS(inttype, uinttype) \
    {                                                           \
        inttype i = 0; uinttype u = 0;                          \
        u = abs<inttype, uinttype>(i);                          \
        CPPUNIT_ASSERT(u == 0);                                 \
        i = -5; u = abs<inttype, uinttype>(i);                  \
        CPPUNIT_ASSERT(u == 5);                                 \
        i = 5; u = abs<inttype, uinttype>(i);                   \
        CPPUNIT_ASSERT(u == 5);                                 \
        i = 1; u = abs<inttype, uinttype>(i);                   \
        CPPUNIT_ASSERT(u == 1);                                 \
        i = -1; u = abs<inttype, uinttype>(i);                  \
        CPPUNIT_ASSERT(u == 1);                                 \
    }                                                           \

    TEST_ABS(int8_t, uint8_t);
    TEST_ABS(int16_t, uint16_t);
    TEST_ABS(int32_t, uint32_t);
    TEST_ABS(int64_t, uint64_t);

    // signed_index_vec / index_vec
    index_vec u;
    u = abs<signed_index_vec, index_vec>(signed_index_vec(0));             
    CPPUNIT_ASSERT(u == index_vec(0));                    
    u = abs<signed_index_vec, index_vec>(signed_index_vec(1, 2, 1));
    CPPUNIT_ASSERT(u == index_vec(signed_index_vec(1, 2, 1)));                    
    u = abs<signed_index_vec, index_vec>(signed_index_vec(-5));      
    CPPUNIT_ASSERT(u == index_vec(5));                    
    u = abs<signed_index_vec, index_vec>(signed_index_vec(-5, -4, -5));      
    CPPUNIT_ASSERT(u == index_vec(5, 4, 5));                    
}

//------------------------------------------------------------------------------

void
TestBitFieldOps::testGetFirstSetBitIndex()
{
    USING_NKHIVE_NS

    // Test uint8
    {
        uint8_t n = 0;
        CPPUNIT_ASSERT(getFirstSetBitIndex<uint8_t>(n) == 0);

        n = 1;
        CPPUNIT_ASSERT(getFirstSetBitIndex<uint8_t>(n) == 1);

        n = 0x80;
        CPPUNIT_ASSERT(getFirstSetBitIndex<uint8_t>(n) == 8);

        n = 8;
        CPPUNIT_ASSERT(getFirstSetBitIndex<uint8_t>(n) == 4);

        n = 9;
        CPPUNIT_ASSERT(getFirstSetBitIndex<uint8_t>(n) == 1);
    }

    // Test uint16
    {
        uint16_t n = 0;
        CPPUNIT_ASSERT(getFirstSetBitIndex<uint16_t>(n) == 0);

        n = 1;
        CPPUNIT_ASSERT(getFirstSetBitIndex<uint16_t>(n) == 1);

        n = 0x0080;
        CPPUNIT_ASSERT(getFirstSetBitIndex<uint16_t>(n) == 8);

        n = 0x8000;
        CPPUNIT_ASSERT(getFirstSetBitIndex<uint16_t>(n) == 16);

        n = 9;
        CPPUNIT_ASSERT(getFirstSetBitIndex<uint16_t>(n) == 1);

        n = 0x0900;
        CPPUNIT_ASSERT(getFirstSetBitIndex<uint16_t>(n) == 9);
    }

    // Test uint32
    {
        uint32_t n = 0;
        CPPUNIT_ASSERT(getFirstSetBitIndex<uint32_t>(n) == 0);

        n = 1;
        CPPUNIT_ASSERT(getFirstSetBitIndex<uint32_t>(n) == 1);

        n = 0x00000080;
        CPPUNIT_ASSERT(getFirstSetBitIndex<uint32_t>(n) == 8);

        n = 0x00008000;
        CPPUNIT_ASSERT(getFirstSetBitIndex<uint32_t>(n) == 16);

        n = 0x80000000;
        CPPUNIT_ASSERT(getFirstSetBitIndex<uint32_t>(n) == 32);

        n = 9;
        CPPUNIT_ASSERT(getFirstSetBitIndex<uint32_t>(n) == 1);

        n = 0x00000900;
        CPPUNIT_ASSERT(getFirstSetBitIndex<uint32_t>(n) == 9);

        n = 0x00090000;
        CPPUNIT_ASSERT(getFirstSetBitIndex<uint32_t>(n) == 17);
    }

    // Test uint64
    {
        uint64_t n = 0;
        CPPUNIT_ASSERT(getFirstSetBitIndex<uint64_t>(n) == 0);

        n = 1;
        CPPUNIT_ASSERT(getFirstSetBitIndex<uint64_t>(n) == 1);

        n = 0x0000000000000080;
        CPPUNIT_ASSERT(getFirstSetBitIndex<uint64_t>(n) == 8);

        n = 0x0000000000008000;
        CPPUNIT_ASSERT(getFirstSetBitIndex<uint64_t>(n) == 16);

        n = 0x0000000080000000;
        CPPUNIT_ASSERT(getFirstSetBitIndex<uint64_t>(n) == 32);

        n = 0x8000000000000000;
        CPPUNIT_ASSERT(getFirstSetBitIndex<uint64_t>(n) == 64);

        n = 9;
        CPPUNIT_ASSERT(getFirstSetBitIndex<uint64_t>(n) == 1);

        n = 0x0000000000000900;
        CPPUNIT_ASSERT(getFirstSetBitIndex<uint64_t>(n) == 9);

        n = 0x0000900000000000;
        CPPUNIT_ASSERT(getFirstSetBitIndex<uint64_t>(n) == 45);
    }
}

//------------------------------------------------------------------------------

void
TestBitFieldOps::testGetLastSetBitIndex()
{
    USING_NKHIVE_NS

    // Test uint8
    {
        uint8_t n = 0;
        CPPUNIT_ASSERT(getLastSetBitIndex<uint8_t>(n) == 0);

        n = 1;
        CPPUNIT_ASSERT(getLastSetBitIndex<uint8_t>(n) == 1);

        n = 0x80;
        CPPUNIT_ASSERT(getLastSetBitIndex<uint8_t>(n) == 8);

        n = 8;
        CPPUNIT_ASSERT(getLastSetBitIndex<uint8_t>(n) == 4);

        n = 9;
        CPPUNIT_ASSERT(getLastSetBitIndex<uint8_t>(n) == 4);
    }

    // Test uint16
    {
        uint16_t n = 0;
        CPPUNIT_ASSERT(getLastSetBitIndex<uint16_t>(n) == 0);

        n = 1;
        CPPUNIT_ASSERT(getLastSetBitIndex<uint16_t>(n) == 1);

        n = 0x0080;
        CPPUNIT_ASSERT(getLastSetBitIndex<uint16_t>(n) == 8);

        n = 0x8000;
        CPPUNIT_ASSERT(getLastSetBitIndex<uint16_t>(n) == 16);

        n = 9;
        CPPUNIT_ASSERT(getLastSetBitIndex<uint16_t>(n) == 4);

        n = 0x0900;
        CPPUNIT_ASSERT(getLastSetBitIndex<uint16_t>(n) == 12);
    }

    // Test uint32
    {
        uint32_t n = 0;
        CPPUNIT_ASSERT(getLastSetBitIndex<uint32_t>(n) == 0);

        n = 1;
        CPPUNIT_ASSERT(getLastSetBitIndex<uint32_t>(n) == 1);

        n = 0x00000080;
        CPPUNIT_ASSERT(getLastSetBitIndex<uint32_t>(n) == 8);

        n = 0x00008000;
        CPPUNIT_ASSERT(getLastSetBitIndex<uint32_t>(n) == 16);

        n = 0x80000000;
        CPPUNIT_ASSERT(getLastSetBitIndex<uint32_t>(n) == 32);

        n = 9;
        CPPUNIT_ASSERT(getLastSetBitIndex<uint32_t>(n) == 4);

        n = 0x00000900;
        CPPUNIT_ASSERT(getLastSetBitIndex<uint32_t>(n) == 12);

        n = 0x00090F00;
        CPPUNIT_ASSERT(getLastSetBitIndex<uint32_t>(n) == 20);
    }

    // Test uint64
    {
        uint64_t n = 0;
        CPPUNIT_ASSERT(getLastSetBitIndex<uint64_t>(n) == 0);

        n = 1;
        CPPUNIT_ASSERT(getLastSetBitIndex<uint64_t>(n) == 1);

        n = 0x0000000000000080;
        CPPUNIT_ASSERT(getLastSetBitIndex<uint64_t>(n) == 8);

        n = 0x0000000000008000;
        CPPUNIT_ASSERT(getLastSetBitIndex<uint64_t>(n) == 16);

        n = 0x0000000080000000;
        CPPUNIT_ASSERT(getLastSetBitIndex<uint64_t>(n) == 32);

        n = 0x8000000000000000;
        CPPUNIT_ASSERT(getLastSetBitIndex<uint64_t>(n) == 64);

        n = 9;
        CPPUNIT_ASSERT(getLastSetBitIndex<uint64_t>(n) == 4);

        n = 0x0000000000000900;
        CPPUNIT_ASSERT(getLastSetBitIndex<uint64_t>(n) == 12);

        n = 0x0000900FFF000000;
        CPPUNIT_ASSERT(getLastSetBitIndex<uint64_t>(n) == 48);
    }
}
