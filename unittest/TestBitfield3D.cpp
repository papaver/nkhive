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
// TestBitfield3D.cpp
//------------------------------------------------------------------------------

#include <cmath>
#include <string>
#include <cstring>
#include <vector>

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <nkhive/Defs.h>
#include <nkhive/bitfields/BitField3D.h>
#include <nkhive/bitfields/BitOps.h>
#include <nkhive/io/VolumeFile.h>

//-----------------------------------------------------------------------------
// class definition
//-----------------------------------------------------------------------------

template <typename T>
class TestBitField3D : public CppUnit::TestFixture {

    CPPUNIT_TEST_SUITE(TestBitField3D);
    CPPUNIT_TEST(testDefaultConstructor);
    CPPUNIT_TEST(testConstructor);
    CPPUNIT_TEST(testCopyConstructor);
    CPPUNIT_TEST(testCopyOperator);
    CPPUNIT_TEST(testResize);
    CPPUNIT_TEST(testSetBitIndex);
    CPPUNIT_TEST(testUnsetBitIndex);
    CPPUNIT_TEST(testSetBitCoordinate);
    CPPUNIT_TEST(testUnsetBitCoordinate);
    CPPUNIT_TEST(testFillBits);
    CPPUNIT_TEST(testClearBits);
    CPPUNIT_TEST(testInvertBits);
    CPPUNIT_TEST(testSetIterator);
    CPPUNIT_TEST(testUnsetIterator);
    CPPUNIT_TEST(testSetIteratorGetCoordinates);
    CPPUNIT_TEST(testIsSingleBitSet);
    CPPUNIT_TEST(testCount);
    CPPUNIT_TEST(testCountRange);
    CPPUNIT_TEST(testGetSetIndex);
    CPPUNIT_TEST(testIO);
    CPPUNIT_TEST(testIOHDF5);
    CPPUNIT_TEST(testOperatorComparison);
    CPPUNIT_TEST_SUITE_END();

public:

    void setUp() {}
    void tearDown() {}

    void testDefaultConstructor();
    void testConstructor();
    void testCopyConstructor();
    void testCopyOperator();
    void testResize();
    void testSetBitIndex();
    void testUnsetBitIndex();
    void testSetBitCoordinate();
    void testUnsetBitCoordinate();
    void testFillBits();
    void testClearBits();
    void testInvertBits();
    void testSetIterator();
    void testUnsetIterator();
    void testSetIteratorGetCoordinates();
    void testIsSingleBitSet();
    void testCount();
    void testCountRange();
    void testGetSetIndex();
    void testIO();
    void testIOHDF5();
    void testOperatorComparison();

};

//-----------------------------------------------------------------------------
// test suite registration
//-----------------------------------------------------------------------------

CPPUNIT_TEST_SUITE_REGISTRATION(TestBitField3D<uint8_t>);
CPPUNIT_TEST_SUITE_REGISTRATION(TestBitField3D<uint16_t>);
CPPUNIT_TEST_SUITE_REGISTRATION(TestBitField3D<uint32_t>);
CPPUNIT_TEST_SUITE_REGISTRATION(TestBitField3D<uint64_t>);

//-----------------------------------------------------------------------------
// macros
//-----------------------------------------------------------------------------

/**
 *  Typedefs/usings for tests.
 */
#define DEFINE_TYPEDEFS                                               \
    using namespace std;                                              \
                                                                      \
    typedef vector<T>                 vector;                         \
    typedef typename vector::iterator viterator;                      \
                                                                      \
    typedef NKHIVE_NS::BitField3D<T>                 BitField;        \
    typedef typename BitField::iterator              biterator;       \
    typedef typename BitField::const_iterator        bciterator;      \
    typedef typename                                                  \
        BitField::template set_iterator<viterator>   bset_iterator;   \
    typedef typename                                                  \
        BitField::template unset_iterator<viterator> bunset_iterator; \
    typedef NKHIVE_NS::VolumeFile                    VolumeFile;      \
    typedef NKHIVE_NS::VoidFile                      VoidFile;        \
    typedef NKHIVE_NS::HDF5Group                     HDF5Group;       \

//-----------------------------------------------------------------------------
 
/**
 *  Print out bitfield to stdout.
 */
#define PRINT_BITFIELD(bf) \
    std::cout << bf.toString() << std::endl;

//-----------------------------------------------------------------------------

/**
 *  Fill vector with specified range
 */
#define VECTOR_FILL_RANGE(v, min, max)     \
{                                          \
    v.clear();                             \
    for (int i = min; i < max; ++i) {      \
      v.push_back(i);                      \
    }                                      \
}

//-----------------------------------------------------------------------------

/**
 *  Print vector contents to stdout.
 */
#define PRINT_VECTOR(v)                                   \
{                                                         \
    for (viterator vi = v.begin(); vi != v.end(); ++vi) { \
      cout << *vi << " ";                                 \
    }                                                     \
    cout << endl;                                         \
}

//-----------------------------------------------------------------------------
// helper functions
//-----------------------------------------------------------------------------

/**
 *  Writes out vector using bitfield as status.
 */
template <typename T>
std::string
toStringSI(NKHIVE_NS::BitField3D<T>& bf, std::vector<T>& v) 
{
    DEFINE_TYPEDEFS;

    char buffer[10];
    string str       = "";
    bset_iterator si = bf.setIterator(v.begin());

    while (si()) {
      sprintf(buffer, "%d ", (int)*si++);
      str.append(buffer);
    } 

    if (str.length()) {
      str.erase(str.end() - 1);
    }

    return str;
}

//-----------------------------------------------------------------------------

/**
 *  Writes out vector using bitfield as status.
 */
template <typename T>
std::string
toStringUI(NKHIVE_NS::BitField3D<T>& bf, std::vector<T>& v) 
{
    DEFINE_TYPEDEFS;

    char buffer[10];
    string str        = "";
    bunset_iterator ui = bf.unsetIterator(v.begin());

    while (ui()) {
      sprintf(buffer, "%d ", (int)*ui++);
      str.append(buffer);
    } 

    if (str.length()) {
      str.erase(str.end() - 1);
    }

    return str;
}

/**
 *  Writes out indices of the set iterator.
 */
template <typename T>
std::string
SIIndexString(NKHIVE_NS::BitField3D<T>& bf, std::vector<T>& v) 
{
    USING_NKHIVE_NS
    DEFINE_TYPEDEFS;

    char buffer[10];
    string str       = "";
    bset_iterator si = bf.setIterator(v.begin());

    while (si()) {
      sprintf(buffer, "%d ", si.getIndex());
      str.append(buffer);

      index_type i, j, k;
      si.getCoordinates(i, j, k);
      sprintf(buffer, "(%d %d %d) ", i, j, k);
      str.append(buffer);

      si++;
    } 

    if (str.length()) {
      str.erase(str.end() - 1);
    }

    return str;
}

//-----------------------------------------------------------------------------
// test suite container
//-----------------------------------------------------------------------------

template <typename T>
void
TestBitField3D<T>::testDefaultConstructor()
{
    DEFINE_TYPEDEFS;

    BitField bf;
    CPPUNIT_ASSERT(bf.size() == 0);
    CPPUNIT_ASSERT(bf.capacity() == 0);
    CPPUNIT_ASSERT(bf.toString() == "");
}

//-----------------------------------------------------------------------------

template <typename T>
void
TestBitField3D<T>::testConstructor() 
{
    DEFINE_TYPEDEFS;
  
    #define CTOR_TEST(s) {                                     \
        BitField bf(s);                                        \
        uint32_t s2 = 1 << s;                                  \
        float s3    = (float)(s2 * s2 * s2);                   \
        float bpt   = (float)(sizeof(T) * 8);                  \
        CPPUNIT_ASSERT(bf.size() == s);                        \
        CPPUNIT_ASSERT(bf.capacity() == ceil(s3 / bpt) * bpt); \
    } 
  
    CTOR_TEST(0);
    CTOR_TEST(1);
    CTOR_TEST(2);
    CTOR_TEST(3);
    CTOR_TEST(4);
    CTOR_TEST(5);
    CTOR_TEST(6);
    CTOR_TEST(7);
    CTOR_TEST(8);
    CTOR_TEST(9);
    CTOR_TEST(10);
}

//-----------------------------------------------------------------------------

template <typename T>
void
TestBitField3D<T>::testCopyConstructor() 
{
    DEFINE_TYPEDEFS;

    #define COPY_CTOR_TEST(bf) {                          \
        BitField bf2(bf);                                 \
        CPPUNIT_ASSERT(bf.size() == bf2.size());          \
        CPPUNIT_ASSERT(bf.capacity() == bf2.capacity());  \
        CPPUNIT_ASSERT(bf.toString() == bf2.toString());  \
    }                   

    BitField bf(1);
    CPPUNIT_ASSERT(bf.toString() == "00000000");
    COPY_CTOR_TEST(bf);
  
    bf.setBit(0);
    CPPUNIT_ASSERT(bf.isSet(0) == true);
    CPPUNIT_ASSERT(bf.isSet(1) == false);
    CPPUNIT_ASSERT(bf.toString() == "10000000");
    COPY_CTOR_TEST(bf);

    bf.setBit(2);
    CPPUNIT_ASSERT(bf.isSet(2) == true);
    CPPUNIT_ASSERT(bf.toString() == "10100000");
    COPY_CTOR_TEST(bf);

    bf.setBit(4);
    CPPUNIT_ASSERT(bf.isSet(4) == true);
    CPPUNIT_ASSERT(bf.toString() == "10101000");
    COPY_CTOR_TEST(bf);

    bf.setBit(4);
    CPPUNIT_ASSERT(bf.isSet(4) == true);
    CPPUNIT_ASSERT(bf.toString() == "10101000");
    COPY_CTOR_TEST(bf);

    bf.setBit(6);
    CPPUNIT_ASSERT(bf.isSet(6) == true);
    CPPUNIT_ASSERT(bf.toString() == "10101010");
    CPPUNIT_ASSERT(bf.isFull()  == false);
    CPPUNIT_ASSERT(bf.isEmpty() == false);
    COPY_CTOR_TEST(bf);
}

//-----------------------------------------------------------------------------

template <typename T>
void
TestBitField3D<T>::testCopyOperator() 
{
    DEFINE_TYPEDEFS;

    #define COPY_OPERATOR_TEST(bf) {                      \
        BitField bf2(1);                                  \
        bf2 = bf;                                         \
        CPPUNIT_ASSERT(bf.size() == bf2.size());          \
        CPPUNIT_ASSERT(bf.capacity() == bf2.capacity());  \
        CPPUNIT_ASSERT(bf.toString() == bf2.toString());  \
    }                   

    BitField bf(1);
    CPPUNIT_ASSERT(bf.toString() == "00000000");
    COPY_OPERATOR_TEST(bf);
  
    bf.setBit(0);
    CPPUNIT_ASSERT(bf.isSet(0) == true);
    CPPUNIT_ASSERT(bf.isSet(1) == false);
    CPPUNIT_ASSERT(bf.toString() == "10000000");
    COPY_OPERATOR_TEST(bf);

    bf.setBit(2);
    CPPUNIT_ASSERT(bf.isSet(2) == true);
    CPPUNIT_ASSERT(bf.toString() == "10100000");
    COPY_OPERATOR_TEST(bf);

    bf.setBit(4);
    CPPUNIT_ASSERT(bf.isSet(4) == true);
    CPPUNIT_ASSERT(bf.toString() == "10101000");
    COPY_OPERATOR_TEST(bf);

    bf.setBit(4);
    CPPUNIT_ASSERT(bf.isSet(4) == true);
    CPPUNIT_ASSERT(bf.toString() == "10101000");
    COPY_OPERATOR_TEST(bf);

    bf.setBit(6);
    CPPUNIT_ASSERT(bf.isSet(6) == true);
    CPPUNIT_ASSERT(bf.toString() == "10101010");
    CPPUNIT_ASSERT(bf.isFull()  == false);
    CPPUNIT_ASSERT(bf.isEmpty() == false);
    COPY_OPERATOR_TEST(bf);
}

//-----------------------------------------------------------------------------

template <typename T>
void
TestBitField3D<T>::testResize() 
{
    DEFINE_TYPEDEFS;

    BitField bf(1);
    CPPUNIT_ASSERT(bf.toString() == "00000000");

    bf.setBit(0);
    bf.setBit(7);
    CPPUNIT_ASSERT(bf.toString() == "10000001");

    bf.resize(2);
    CPPUNIT_ASSERT(bf.toString() == "1000000000000000"
                                    "0000010000000000"
                                    "0000000000000000"
                                    "0000000000000000");

    bf.unsetBit(0);
    bf.setBit(21);
    bf.setBit(22);
    bf.setBit(25);
    bf.setBit(26);
    bf.setBit(37);
    bf.setBit(38);
    bf.setBit(41);
    bf.setBit(42);
    CPPUNIT_ASSERT(bf.toString() == "0000000000000000"
                                    "0000011001100000"
                                    "0000011001100000"
                                    "0000000000000000");

    bf.resize(1);
    CPPUNIT_ASSERT(bf.toString() == "00000001");

    bf.resize(2);
    CPPUNIT_ASSERT(bf.toString() == "0000000000000000"
                                    "0000010000000000"
                                    "0000000000000000"
                                    "0000000000000000");
}

//-----------------------------------------------------------------------------

template <typename T>
void
TestBitField3D<T>::testSetBitIndex() 
{
    DEFINE_TYPEDEFS;
 
    BitField bf(1);
    CPPUNIT_ASSERT(bf.toString() == "00000000");
  
    bf.setBit(0);
    CPPUNIT_ASSERT(bf.isSet(0) == true);
    CPPUNIT_ASSERT(bf.isSet(1) == false);
    CPPUNIT_ASSERT(bf.toString() == "10000000");
    bf.setBit(2);
    CPPUNIT_ASSERT(bf.isSet(2) == true);
    CPPUNIT_ASSERT(bf.toString() == "10100000");
    bf.setBit(4);
    CPPUNIT_ASSERT(bf.isSet(4) == true);
    CPPUNIT_ASSERT(bf.toString() == "10101000");
    bf.setBit(4);
    CPPUNIT_ASSERT(bf.isSet(4) == true);
    CPPUNIT_ASSERT(bf.toString() == "10101000");
    bf.setBit(6);
    CPPUNIT_ASSERT(bf.isSet(6) == true);
    CPPUNIT_ASSERT(bf.toString() == "10101010");
    CPPUNIT_ASSERT(bf.isFull()  == false);
    CPPUNIT_ASSERT(bf.isEmpty() == false);
}

//-----------------------------------------------------------------------------

template <typename T>
void
TestBitField3D<T>::testUnsetBitIndex() 
{
    DEFINE_TYPEDEFS;

    BitField bf(1);
    bf.invertBits();
    CPPUNIT_ASSERT(bf.toString() == "11111111");
  
    bf.unsetBit(1);
    CPPUNIT_ASSERT(bf.isSet(1) == false);
    CPPUNIT_ASSERT(bf.toString() == "10111111");
    bf.unsetBit(3);
    CPPUNIT_ASSERT(bf.isSet(3) == false);
    CPPUNIT_ASSERT(bf.toString() == "10101111");
    bf.unsetBit(5);
    CPPUNIT_ASSERT(bf.isSet(5) == false);
    CPPUNIT_ASSERT(bf.toString() == "10101011");
    bf.unsetBit(7);
    CPPUNIT_ASSERT(bf.isSet(7) == false);
    CPPUNIT_ASSERT(bf.toString() == "10101010");
    CPPUNIT_ASSERT(bf.isFull()  == false);
    CPPUNIT_ASSERT(bf.isEmpty() == false);
}

//-----------------------------------------------------------------------------

template <typename T>
void
TestBitField3D<T>::testInvertBits() 
{
    DEFINE_TYPEDEFS;

    BitField bf(1);
    CPPUNIT_ASSERT(bf.toString() == "00000000");

    bf.setBit(0);
    bf.setBit(2);
    bf.setBit(4);
    bf.setBit(4);
    bf.setBit(6);
    bf.invertBits();
    CPPUNIT_ASSERT(bf.toString() == "01010101");
    bf.invertBits();
    CPPUNIT_ASSERT(bf.toString() == "10101010");
}

//-----------------------------------------------------------------------------

template <typename T>
void
TestBitField3D<T>::testClearBits() 
{
    DEFINE_TYPEDEFS;

    BitField bf(1);
    CPPUNIT_ASSERT(bf.toString() == "00000000");

    bf.setBit(0);
    bf.setBit(2);
    bf.setBit(4);
    bf.setBit(4);
    bf.setBit(6);
    bf.invertBits();
    bf.clearBits();
    CPPUNIT_ASSERT(bf.toString() == "00000000");
    CPPUNIT_ASSERT(bf.isFull()  == false);
    CPPUNIT_ASSERT(bf.isEmpty() == true);
}

//-----------------------------------------------------------------------------

template <typename T>
void
TestBitField3D<T>::testFillBits() 
{
    DEFINE_TYPEDEFS;

    BitField bf(1);
    CPPUNIT_ASSERT(bf.toString() == "00000000");

    bf.setBit(0);
    bf.setBit(2);
    bf.setBit(4);
    bf.setBit(4);
    bf.setBit(6);
    bf.invertBits();
    bf.fillBits();
    CPPUNIT_ASSERT(bf.toString() == "11111111");
    CPPUNIT_ASSERT(bf.isFull()  == true);
    CPPUNIT_ASSERT(bf.isEmpty() == false);
}
  
//-----------------------------------------------------------------------------

template <typename T>
void
TestBitField3D<T>::testSetBitCoordinate() 
{
    DEFINE_TYPEDEFS;

    BitField bf(1);
    CPPUNIT_ASSERT(bf.toString() == "00000000");

    bf.setBit(0, 0, 0);
    CPPUNIT_ASSERT(bf.isSet(0) == true);
    CPPUNIT_ASSERT(bf.isSet(1) == false);
    CPPUNIT_ASSERT(bf.toString() == "10000000");
    bf.setBit(1, 1, 1);
    CPPUNIT_ASSERT(bf.isSet(7) == true);
    CPPUNIT_ASSERT(bf.toString() == "10000001");
    bf.setBit(0, 1, 0);
    CPPUNIT_ASSERT(bf.isSet(2) == true);
    CPPUNIT_ASSERT(bf.toString() == "10100001");
    bf.setBit(0, 1, 1);
    CPPUNIT_ASSERT(bf.isSet(6) == true);
    CPPUNIT_ASSERT(bf.toString() == "10100011");
    bf.setBit(1, 0, 1);
    CPPUNIT_ASSERT(bf.isSet(5) == true);
    CPPUNIT_ASSERT(bf.toString() == "10100111");
}

//-----------------------------------------------------------------------------

template <typename T>
void
TestBitField3D<T>::testUnsetBitCoordinate() 
{
    DEFINE_TYPEDEFS;

    BitField bf(1);
    CPPUNIT_ASSERT(bf.toString() == "00000000");
    
    bf.setBit(0, 0, 0);
    bf.setBit(1, 1, 1);
    bf.setBit(0, 1, 0);
    bf.setBit(0, 1, 1);
    bf.setBit(1, 0, 1);
    bf.invertBits();
    CPPUNIT_ASSERT(bf.toString() == "01011000");
        
    bf.unsetBit(1, 0, 0);
    CPPUNIT_ASSERT(bf.isSet(1) == false);
    CPPUNIT_ASSERT(bf.toString() == "00011000");
    bf.unsetBit(1, 1, 0);
    CPPUNIT_ASSERT(bf.isSet(3) == false);
    CPPUNIT_ASSERT(bf.toString() == "00001000");
    bf.unsetBit(0, 0, 1);
    CPPUNIT_ASSERT(bf.isSet(4) == false);
    CPPUNIT_ASSERT(bf.toString() == "00000000");
    CPPUNIT_ASSERT(bf.isEmpty() == true);
}

//-----------------------------------------------------------------------------

template <typename T>
void
TestBitField3D<T>::testSetIterator() 
{
    DEFINE_TYPEDEFS;

    BitField bf(1);
    CPPUNIT_ASSERT(bf.toString() == "00000000");

    vector v;
    VECTOR_FILL_RANGE(v, 0, 8);
    CPPUNIT_ASSERT(bf.toString() == "00000000");

    bf.setBit(5);
    bf.setBit(2);
    CPPUNIT_ASSERT(bf.toString() == "00100100");
    CPPUNIT_ASSERT(toStringSI(bf,v) == "2 5");

    bf.clearBits();
    CPPUNIT_ASSERT(bf.toString() == "00000000");
    CPPUNIT_ASSERT(toStringSI(bf,v) == "");

    bf.fillBits();
    CPPUNIT_ASSERT(bf.toString() == "11111111");
    CPPUNIT_ASSERT(toStringSI(bf,v) == "0 1 2 3 4 5 6 7");

    bf.unsetBit(0);
    bf.unsetBit(2);
    bf.unsetBit(4);
    bf.unsetBit(6);
    CPPUNIT_ASSERT(bf.toString() == "01010101");
    CPPUNIT_ASSERT(toStringSI(bf,v) == "1 3 5 7");

    bf.invertBits();
    CPPUNIT_ASSERT(bf.toString() == "10101010");
    CPPUNIT_ASSERT(toStringSI(bf,v) == "0 2 4 6");
}

//-----------------------------------------------------------------------------

template <typename T>
void
TestBitField3D<T>::testUnsetIterator() 
{
    DEFINE_TYPEDEFS;

    BitField bf(1);
    bf.fillBits();
    CPPUNIT_ASSERT(bf.toString() == "11111111");

    vector v;
    VECTOR_FILL_RANGE(v, 0, 8);
    CPPUNIT_ASSERT(bf.toString() == "11111111");

    bf.unsetBit(5);
    bf.unsetBit(2);
    CPPUNIT_ASSERT(bf.toString() == "11011011");
    CPPUNIT_ASSERT(toStringUI(bf,v) == "2 5");

    bf.fillBits();
    CPPUNIT_ASSERT(bf.toString() == "11111111");
    CPPUNIT_ASSERT(toStringUI(bf,v) == "");

    bf.clearBits();
    CPPUNIT_ASSERT(bf.toString() == "00000000");
    CPPUNIT_ASSERT(toStringUI(bf,v) == "0 1 2 3 4 5 6 7");

    bf.setBit(0);
    bf.setBit(2);
    bf.setBit(4);
    bf.setBit(6);
    CPPUNIT_ASSERT(bf.toString() == "10101010");
    CPPUNIT_ASSERT(toStringUI(bf,v) == "1 3 5 7");

    bf.invertBits();
    CPPUNIT_ASSERT(bf.toString() == "01010101");
    CPPUNIT_ASSERT(toStringUI(bf,v) == "0 2 4 6");
}

//------------------------------------------------------------------------------

template <typename T>
void
TestBitField3D<T>::testSetIteratorGetCoordinates()
{
    DEFINE_TYPEDEFS;

    BitField bf(1);
    CPPUNIT_ASSERT(bf.toString() == "00000000");

    vector v;
    VECTOR_FILL_RANGE(v, 0, 8);
    CPPUNIT_ASSERT(bf.toString() == "00000000");

    bf.setBit(5);
    bf.setBit(2);
    CPPUNIT_ASSERT(bf.toString() == "00100100");
    CPPUNIT_ASSERT(SIIndexString(bf,v) == "2 (0 1 0) 5 (1 0 1)");

    bf.clearBits();
    CPPUNIT_ASSERT(bf.toString() == "00000000");
    CPPUNIT_ASSERT(SIIndexString(bf,v) == "");

    bf.fillBits();
    CPPUNIT_ASSERT(bf.toString() == "11111111");
    CPPUNIT_ASSERT(SIIndexString(bf,v) == 
            "0 (0 0 0) 1 (1 0 0) 2 (0 1 0) 3 (1 1 0) 4 (0 0 1) 5 (1 0 1) 6 (0 1 1) 7 (1 1 1)");

    bf.unsetBit(0);
    bf.unsetBit(2);
    bf.unsetBit(4);
    bf.unsetBit(6);
    CPPUNIT_ASSERT(bf.toString() == "01010101");
    CPPUNIT_ASSERT(SIIndexString(bf,v) == 
            "1 (1 0 0) 3 (1 1 0) 5 (1 0 1) 7 (1 1 1)");

    bf.invertBits();
    CPPUNIT_ASSERT(bf.toString() == "10101010");
    CPPUNIT_ASSERT(SIIndexString(bf,v) == 
            "0 (0 0 0) 2 (0 1 0) 4 (0 0 1) 6 (0 1 1)");
}

//------------------------------------------------------------------------------

template <typename T>
void
TestBitField3D<T>::testIsSingleBitSet()
{
    DEFINE_TYPEDEFS;


    // set first bit.
    {
        // 128 bits
        BitField bf(7);

        bf.setBit(0);

        // check for true statement
        CPPUNIT_ASSERT(bf.isSingleBitSet(0));
        CPPUNIT_ASSERT(!bf.isSingleBitSet(64));

        // set bit in teh same block.
        bf.setBit(3);
        CPPUNIT_ASSERT(!bf.isSingleBitSet(0));

        // set bit in another block.
        bf.unsetBit(3);
        bf.setBit(72);
        CPPUNIT_ASSERT(!bf.isSingleBitSet(0));
    }

    // set last bit.
    {
        // 128 bits
        BitField bf(7);

        bf.setBit(127);

        // check for true statement
        CPPUNIT_ASSERT(bf.isSingleBitSet(127));
        CPPUNIT_ASSERT(!bf.isSingleBitSet(64));

        // set bit in teh same block.
        bf.setBit(123);
        CPPUNIT_ASSERT(!bf.isSingleBitSet(127));

        // set bit in another block.
        bf.unsetBit(123);
        bf.setBit(3);
        CPPUNIT_ASSERT(!bf.isSingleBitSet(127));
    }

    // set bit somewhere in the middle
    {
        // 128 bits
        BitField bf(7);

        bf.setBit(60);

        // check for true statement
        CPPUNIT_ASSERT(bf.isSingleBitSet(60));
        CPPUNIT_ASSERT(!bf.isSingleBitSet(64));

        // set bit in teh same block.
        bf.setBit(61);
        CPPUNIT_ASSERT(!bf.isSingleBitSet(60));

        // set bit in another block.
        bf.unsetBit(61);
        bf.setBit(0);
        CPPUNIT_ASSERT(!bf.isSingleBitSet(60));
    }
}

//-----------------------------------------------------------------------------

template <typename T>
void
TestBitField3D<T>::testCount() 
{
    DEFINE_TYPEDEFS;

    BitField bf(1);
    bf.fillBits();
    CPPUNIT_ASSERT(bf.toString() == "11111111");
    CPPUNIT_ASSERT(bf.count() == 8);

    bf.unsetBit(5);
    bf.unsetBit(2);
    CPPUNIT_ASSERT(bf.toString() == "11011011");
    CPPUNIT_ASSERT(bf.count() == 6);

    bf.clearBits();
    CPPUNIT_ASSERT(bf.toString() == "00000000");
    CPPUNIT_ASSERT(bf.count() == 0);

    bf.setBit(0);
    bf.setBit(2);
    bf.setBit(4);
    bf.setBit(6);
    CPPUNIT_ASSERT(bf.toString() == "10101010");
    CPPUNIT_ASSERT(bf.count() == 4);

    bf.invertBits();
    CPPUNIT_ASSERT(bf.toString() == "01010101");
    CPPUNIT_ASSERT(bf.count() == 4);
}

//-----------------------------------------------------------------------------

template <typename T>
void
TestBitField3D<T>::testCountRange() 
{
    DEFINE_TYPEDEFS;

    BitField bf(1);
    bf.fillBits();
    CPPUNIT_ASSERT(bf.toString() == "11111111");
    CPPUNIT_ASSERT(bf.countRange(0) == 0);
    CPPUNIT_ASSERT(bf.countRange(1) == 1);
    CPPUNIT_ASSERT(bf.countRange(3) == 3);
    CPPUNIT_ASSERT(bf.countRange(5) == 5);

    bf.unsetBit(5);
    bf.unsetBit(2);
    CPPUNIT_ASSERT(bf.toString() == "11011011");
    CPPUNIT_ASSERT(bf.countRange(3) == 2);
    CPPUNIT_ASSERT(bf.countRange(6) == 4);

    bf.clearBits();
    CPPUNIT_ASSERT(bf.toString() == "00000000");
    CPPUNIT_ASSERT(bf.countRange(7) == 0);
    CPPUNIT_ASSERT(bf.countRange(3) == 0);

    bf.setBit(0);
    bf.setBit(2);
    bf.setBit(4);
    bf.setBit(6);
    CPPUNIT_ASSERT(bf.toString() == "10101010");
    CPPUNIT_ASSERT(bf.countRange(2) == 1);
    CPPUNIT_ASSERT(bf.countRange(4) == 2);
    CPPUNIT_ASSERT(bf.countRange(6) == 3);

    bf.invertBits();
    CPPUNIT_ASSERT(bf.toString() == "01010101");
    CPPUNIT_ASSERT(bf.countRange(2) == 1);
    CPPUNIT_ASSERT(bf.countRange(4) == 2);
    CPPUNIT_ASSERT(bf.countRange(6) == 3);
}

//-----------------------------------------------------------------------------

template <typename T>
void
TestBitField3D<T>::testGetSetIndex() 
{
    DEFINE_TYPEDEFS;

    BitField bf(1);
    CPPUNIT_ASSERT(bf.toString() == "00000000");

    bf.setBit(5);
    CPPUNIT_ASSERT(bf.toString() == "00000100");
    CPPUNIT_ASSERT(bf.getSetIndex(0) == 5);

    bf.setBit(2);
    CPPUNIT_ASSERT(bf.toString() == "00100100");
    CPPUNIT_ASSERT(bf.getSetIndex(0) == 2);
    CPPUNIT_ASSERT(bf.getSetIndex(1) == 5);

    bf.setBit(7);
    CPPUNIT_ASSERT(bf.toString() == "00100101");
    CPPUNIT_ASSERT(bf.getSetIndex(0) == 2);
    CPPUNIT_ASSERT(bf.getSetIndex(1) == 5);
    CPPUNIT_ASSERT(bf.getSetIndex(2) == 7);

    bf.setBit(0);
    CPPUNIT_ASSERT(bf.toString() == "10100101");
    CPPUNIT_ASSERT(bf.getSetIndex(0) == 0);
    CPPUNIT_ASSERT(bf.getSetIndex(1) == 2);
    CPPUNIT_ASSERT(bf.getSetIndex(2) == 5);
    CPPUNIT_ASSERT(bf.getSetIndex(3) == 7);
}

//-----------------------------------------------------------------------------

template <typename T>
void
TestBitField3D<T>::testIO() 
{
    DEFINE_TYPEDEFS;

    #define IO_TEST(bfo) {                                          \
        std::ostringstream ostr(std::ios_base::binary);             \
        bfo.write(ostr);                                            \
        std::istringstream istr(ostr.str(), std::ios_base::binary); \
        BitField bfi;                                               \
        bfi.read(istr);                                             \
        CPPUNIT_ASSERT(bfo == bfi);                                 \
    } 

    BitField bf(1);
    bf.setBit(5);
    bf.setBit(1);
    bf.setBit(2);
    bf.setBit(7);
    IO_TEST(bf);

    bf.resize(3);
    bf.setBit(20);
    bf.setBit(16);
    IO_TEST(bf);

    bf.invertBits();
    IO_TEST(bf);
    
    bf.clearBits();
    IO_TEST(bf);

    bf.fillBits();
    IO_TEST(bf);

    #undef IO_TEST
}

//-----------------------------------------------------------------------------

template <typename T>
void
TestBitField3D<T>::testIOHDF5() 
{
    DEFINE_TYPEDEFS;
  
    #define IO_TEST(bfo) {                                          \
        remove("testingHDF5.hv");                                   \
        VolumeFile file("testingHDF5.hv", VoidFile::WRITE_TRUNC);   \
        HDF5Group root_group;                                       \
        root_group.open(file.m_id, NK_NS::String("/"));             \
        HDF5Id root_group_id = root_group.id();                     \
        bfo.write(root_group_id);                                   \
        file.close();                                               \
        BitField bfi;                                               \
        VolumeFile file2("testingHDF5.hv", VoidFile::READ_ONLY);    \
        HDF5Group root_group2;                                      \
        root_group2.open(file2.m_id, NK_NS::String("/"));           \
        HDF5Id root_group_id2 = root_group2.id();                   \
        bfi.read(root_group_id2);                                   \
        CPPUNIT_ASSERT(bfo == bfi);                                 \
        file2.close();                                              \
        remove("testingHDF5.hv");                                   \
    }

    BitField bf(1);
    bf.setBit(5);
    bf.setBit(1);
    bf.setBit(2);
    bf.setBit(7);
    IO_TEST(bf);

    bf.resize(3);
    bf.setBit(20);
    bf.setBit(16);
    IO_TEST(bf);

    bf.invertBits();
    IO_TEST(bf);
    
    bf.clearBits();
    IO_TEST(bf);

    bf.fillBits();
    IO_TEST(bf);

    #undef IO_TEST
}

//-----------------------------------------------------------------------------

template <typename T>
void
TestBitField3D<T>::testOperatorComparison() 
{
    DEFINE_TYPEDEFS;

    BitField bf1(1);
    BitField bf2(1);
    CPPUNIT_ASSERT(bf1 == bf2);

    bf1.setBit(5);
    bf2.setBit(5);
    CPPUNIT_ASSERT(bf1 == bf2);

    bf1.setBit(2);
    bf2.setBit(2);
    CPPUNIT_ASSERT(bf1 == bf2);

    bf1.setBit(7);
    bf2.setBit(7);
    CPPUNIT_ASSERT(bf1 == bf2);

    bf1.setBit(0);
    bf2.setBit(0);
    CPPUNIT_ASSERT(bf1 == bf2);

    bf1.invertBits();
    bf2.invertBits();
    CPPUNIT_ASSERT(bf1 == bf2);

    bf1.clearBits();
    bf2.clearBits();
    CPPUNIT_ASSERT(bf1 == bf2);

    bf1.fillBits();
    bf2.fillBits();
    CPPUNIT_ASSERT(bf1 == bf2);

    bf1.unsetBit(4);
    CPPUNIT_ASSERT(bf1 != bf2);

    bf2.unsetBit(4);
    CPPUNIT_ASSERT(bf1 == bf2);

    bf1.resize(2);
    CPPUNIT_ASSERT(bf1 != bf2);

    bf2.resize(2);
    CPPUNIT_ASSERT(bf1 == bf2);
}

