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
// TestAbstractIterators.cpp
//------------------------------------------------------------------------------

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <nkhive/volume/AbstractIterator.h>
#include <nkhive/volume/FilledBoundsIterator.h>
#include <nkhive/volume/CellSetIterator.h>

template <typename T>
class TestAbstractIterators : public CppUnit::TestFixture 
{
    CPPUNIT_TEST_SUITE(TestAbstractIterators);
    CPPUNIT_TEST(testFilledBoundsIterator);
    CPPUNIT_TEST(testCellSetIterator);
    CPPUNIT_TEST_SUITE_END();
    
public:
    void setUp() {}
    void tearDown() {}

    void testFilledBoundsIterator();
    void testCellSetIterator();
};

//-----------------------------------------------------------------------------
// test suite registration
//-----------------------------------------------------------------------------

CPPUNIT_TEST_SUITE_REGISTRATION(TestAbstractIterators<float>);
CPPUNIT_TEST_SUITE_REGISTRATION(TestAbstractIterators<double>);
CPPUNIT_TEST_SUITE_REGISTRATION(TestAbstractIterators<int32_t>);

//------------------------------------------------------------------------------
// tests
//------------------------------------------------------------------------------

template <typename T>
void
TestAbstractIterators<T>::testFilledBoundsIterator()
{
    USING_NK_NS
    USING_NKHIVE_NS

    index_bounds bounds(1, 5);
    FilledBoundsIterator<T> iter(bounds, T(1));
    CPPUNIT_ASSERT(iter.m_fill == T(1));
    CPPUNIT_ASSERT(iter.m_curr == 0);
    CPPUNIT_ASSERT(iter.m_end == 64);

    int count = 0;
    for (int i = 1; i < 5; ++i) {
        for (int j = 1; j < 5; ++j) {
            for (int k = 1; k < 5; ++k) {
                ++count;
            }
        }
    }

    int iter_count = 0;
    for ( ; !iter.atEnd(); iter.next()) {
        u32 i, j, k;
        iter.getCoordinates(i, j, k);

        u32 test_i, test_j, test_k;
        NKHIVE_NS::getCoordinates(iter_count, 2, test_i, test_j, test_k);
        test_i += 1;
        test_j += 1;
        test_k += 1;

        CPPUNIT_ASSERT(i == test_i && j == test_j && k == test_k);

        int test_index = NKHIVE_NS::getIndex(i-1, j-1, k-1, 2);
        CPPUNIT_ASSERT(test_index == iter_count);
        
        ++iter_count;
        CPPUNIT_ASSERT(iter.value() == T(1));
    }

    CPPUNIT_ASSERT(count == iter_count);
}

//------------------------------------------------------------------------------

template <typename T>
void
TestAbstractIterators<T>::testCellSetIterator()
{
    USING_NK_NS
    USING_NKHIVE_NS

    typedef Cell<T>     CellType;

    // Unfilled cell.
    {
        CellType cell(2, T(1));

        // store coordinate values for getCoordinate tests
        u32 test_coords[4][3];
        test_coords[0][0] = 0; test_coords[0][1] = 1; test_coords[0][2] = 0;
        test_coords[1][0] = 1; test_coords[1][1] = 0; test_coords[1][2] = 1;
        test_coords[2][0] = 2; test_coords[2][1] = 3; test_coords[2][2] = 2;
        test_coords[3][0] = 3; test_coords[3][1] = 3; test_coords[3][2] = 3;

        cell.set(test_coords[0][0], test_coords[0][1], test_coords[0][2], T(1));
        cell.set(test_coords[1][0], test_coords[1][1], test_coords[1][2], T(2));
        cell.set(test_coords[2][0], test_coords[2][1], test_coords[2][2], T(3));
        cell.set(test_coords[3][0], test_coords[3][1], test_coords[3][2], T(4));

        CPPUNIT_ASSERT(!cell.isFilled());
        
        CellSetIterator<CellType> iter(&cell);

        int iter_count = 0;
        u32 i, j, k;
        for ( ; !iter.atEnd(); iter.next()) {
            CPPUNIT_ASSERT(iter.value() == T(iter_count + 1));
            
            iter.getCoordinates(i,j,k);
            CPPUNIT_ASSERT(test_coords[iter_count][0] == i);
            CPPUNIT_ASSERT(test_coords[iter_count][1] == j);
            CPPUNIT_ASSERT(test_coords[iter_count][2] == k);
            ++iter_count;
        }

        CPPUNIT_ASSERT(iter_count == 4);
    }

    // Filled cell.
    {
        CellType cell(2, T(1));

        // store coordinate values for getCoordinate tests
        u32 test_coords[4][3];
        test_coords[0][0] = 0; test_coords[0][1] = 1; test_coords[0][2] = 0;
        test_coords[1][0] = 1; test_coords[1][1] = 0; test_coords[1][2] = 1;
        test_coords[2][0] = 2; test_coords[2][1] = 3; test_coords[2][2] = 2;
        test_coords[3][0] = 3; test_coords[3][1] = 3; test_coords[3][2] = 3;

        cell.set(test_coords[0][0], test_coords[0][1], test_coords[0][2], T(2));
        cell.set(test_coords[1][0], test_coords[1][1], test_coords[1][2], T(2));
        cell.set(test_coords[2][0], test_coords[2][1], test_coords[2][2], T(2));
        cell.set(test_coords[3][0], test_coords[3][1], test_coords[3][2], T(2));

        CPPUNIT_ASSERT(cell.isFilled());
        
        CellSetIterator<CellType> iter(&cell);

        int iter_count = 0;
        u32 i, j, k;
        for ( ; !iter.atEnd(); iter.next()) {
            CPPUNIT_ASSERT(iter.value() == T(2));
            
            iter.getCoordinates(i,j,k);
            CPPUNIT_ASSERT(test_coords[iter_count][0] == i);
            CPPUNIT_ASSERT(test_coords[iter_count][1] == j);
            CPPUNIT_ASSERT(test_coords[iter_count][2] == k);
            ++iter_count;
        }

        CPPUNIT_ASSERT(iter_count == 4);
    }

    {
        CellType cell(2, T(1));

        CellSetIterator<CellType> iter(&cell);

        int iter_count = 0;
        for ( ; !iter.atEnd(); iter.next()) {
            ++iter_count;
        }

        CPPUNIT_ASSERT(iter_count == 0);
    }
}

//------------------------------------------------------------------------------
