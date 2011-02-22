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
// TestSetIterator.cpp
//------------------------------------------------------------------------------

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <nkhive/volume/SetIterator.h>

template <typename T>
class TestSetIterator : public CppUnit::TestFixture 
{

    CPPUNIT_TEST_SUITE(TestSetIterator);
    CPPUNIT_TEST(testDirectCellParent);
    CPPUNIT_TEST(testMultilevelNode);
    CPPUNIT_TEST(testFillNodeIteration);
    CPPUNIT_TEST_SUITE_END();
    
public:
    void setUp() {}
    void tearDown() {}

    void testDirectCellParent();
    void testMultilevelNode();
    void testFillNodeIteration();
};

//-----------------------------------------------------------------------------
// test suite registration
//-----------------------------------------------------------------------------

CPPUNIT_TEST_SUITE_REGISTRATION(TestSetIterator<float>);
CPPUNIT_TEST_SUITE_REGISTRATION(TestSetIterator<double>);
CPPUNIT_TEST_SUITE_REGISTRATION(TestSetIterator<int32_t>);

//------------------------------------------------------------------------------
// tests
//------------------------------------------------------------------------------

template <typename T>
void
TestSetIterator<T>::testDirectCellParent()
{
    USING_NK_NS
    USING_NKHIVE_NS

    typedef Cell<T> CellType;

    // empty node
    {
        Node<CellType> n(1, 2, 2, T(0));

        SetIterator<CellType> iter(&n);

        int iter_count = 0;
        for ( ; !iter.atEnd(); iter.next()) {
            ++iter_count;
        }
        CPPUNIT_ASSERT(iter_count == 0);
    }

    // a single cell with single value (fill cell).
    {
        Node<CellType> n(1, 2, 2, T(1));

        // store coordinate values for getCoordinate tests
        u32 test_coords[3][3];
        test_coords[0][0] = 0; test_coords[0][1] = 0; test_coords[0][2] = 0;
        test_coords[1][0] = 1; test_coords[1][1] = 2; test_coords[1][2] = 3;
        test_coords[2][0] = 3; test_coords[2][1] = 3; test_coords[2][2] = 3;

        n.set(test_coords[0][0], test_coords[0][1], test_coords[0][2], T(2));
        n.set(test_coords[1][0], test_coords[1][1], test_coords[1][2], T(2));
        n.set(test_coords[2][0], test_coords[2][1], test_coords[2][2], T(2));

        SetIterator<CellType> iter(&n);

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
        CPPUNIT_ASSERT(iter_count == 3);
    }

    // a single cell with multiple values (non-fill cell);
    {
        Node<CellType> n(1, 2, 2, T(1));

        // store coordinate values for getCoordinate tests
        u32 test_coords[3][3];
        test_coords[0][0] = 0; test_coords[0][1] = 0; test_coords[0][2] = 0;
        test_coords[1][0] = 1; test_coords[1][1] = 2; test_coords[1][2] = 3;
        test_coords[2][0] = 3; test_coords[2][1] = 3; test_coords[2][2] = 3;

        n.set(test_coords[0][0], test_coords[0][1], test_coords[0][2], T(1));
        n.set(test_coords[1][0], test_coords[1][1], test_coords[1][2], T(2));
        n.set(test_coords[2][0], test_coords[2][1], test_coords[2][2], T(3));

        SetIterator<CellType> iter(&n);

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
        CPPUNIT_ASSERT(iter_count == 3);
    }

    // test multiple cells
    {
        Node<CellType> n(1, 2, 2, T(1));

        u32 test_coords[3][3];
        test_coords[0][0] =  0; test_coords[0][1] =  0; test_coords[0][2] =  0;
        test_coords[1][0] =  3; test_coords[1][1] =  3; test_coords[1][2] =  3;
        test_coords[2][0] = 12; test_coords[2][1] = 13; test_coords[2][2] = 14;

        n.set(test_coords[0][0], test_coords[0][1], test_coords[0][2], T(1));
        n.set(test_coords[1][0], test_coords[1][1], test_coords[1][2], T(2));
        n.set(test_coords[2][0], test_coords[2][1], test_coords[2][2], T(3));

        SetIterator<CellType> iter(&n);

        int iter_count = 0;
        u32 i, j, k;
        for ( ; !iter.atEnd(); iter.next()) {
            ++iter_count;
            CPPUNIT_ASSERT(iter.value() == T(iter_count));

            iter.getCoordinates(i, j, k);
            CPPUNIT_ASSERT(test_coords[iter_count-1][0] == i);
            CPPUNIT_ASSERT(test_coords[iter_count-1][1] == j);
            CPPUNIT_ASSERT(test_coords[iter_count-1][2] == k);
        }
        CPPUNIT_ASSERT(iter_count == 3);
    }
}

//------------------------------------------------------------------------------

template <typename T>
void
TestSetIterator<T>::testMultilevelNode()
{
    USING_NK_NS
    USING_NKHIVE_NS

    typedef Cell<T> CellType;

    // a multilevel, single cell
    {
        Node<CellType> n(2, 2, 2, T(1));
        
        
        u32 test_coords[3][3];
        test_coords[0][0] =  0; test_coords[0][1] =  0; test_coords[0][2] =  0;
        test_coords[1][0] =  1; test_coords[1][1] =  2; test_coords[1][2] =  3;
        test_coords[2][0] =  3; test_coords[2][1] =  3; test_coords[2][2] =  3;

        n.set(test_coords[0][0], test_coords[0][1], test_coords[0][2], T(1));
        n.set(test_coords[1][0], test_coords[1][1], test_coords[1][2], T(2));
        n.set(test_coords[2][0], test_coords[2][1], test_coords[2][2], T(3));
        
        SetIterator<CellType> iter(&n);

        int iter_count = 0;
        u32 i, j, k;
        for ( ; !iter.atEnd(); iter.next()) {
            ++iter_count;
            CPPUNIT_ASSERT(iter.value() == T(iter_count));

            iter.getCoordinates(i, j, k);
            CPPUNIT_ASSERT(test_coords[iter_count - 1][0] == i);
            CPPUNIT_ASSERT(test_coords[iter_count - 1][1] == j);
            CPPUNIT_ASSERT(test_coords[iter_count - 1][2] == k);

        }
        CPPUNIT_ASSERT(iter_count == 3);
    }

    // test multilevel, multiple cells, same branch
    {
        Node<CellType> n(2, 2, 2, T(1));
        
        u32 test_coords[4][3];
        test_coords[0][0] =  0; test_coords[0][1] =  0; test_coords[0][2] =  0;
        test_coords[1][0] =  3; test_coords[1][1] =  3; test_coords[1][2] =  3;
        test_coords[2][0] =  5; test_coords[2][1] =  6; test_coords[2][2] =  7;
        test_coords[3][0] = 15; test_coords[3][1] = 15; test_coords[3][2] = 15;

        n.set(test_coords[0][0], test_coords[0][1], test_coords[0][2], T(1));
        n.set(test_coords[1][0], test_coords[1][1], test_coords[1][2], T(2));
        n.set(test_coords[2][0], test_coords[2][1], test_coords[2][2], T(3));
        n.set(test_coords[3][0], test_coords[3][1], test_coords[3][2], T(4));
       
        SetIterator<CellType> iter(&n);

        int iter_count = 0;
        u32 i, j, k;
        for ( ; !iter.atEnd(); iter.next()) {
            ++iter_count;
            CPPUNIT_ASSERT(iter.value() == T(iter_count));

            iter.getCoordinates(i, j, k);
            CPPUNIT_ASSERT(test_coords[iter_count-1][0] == i);
            CPPUNIT_ASSERT(test_coords[iter_count-1][1] == j);
            CPPUNIT_ASSERT(test_coords[iter_count-1][2] == k);
        }
        CPPUNIT_ASSERT(iter_count == 4);
    }

    // test multilevel, multiple cells, multiple branches
    {
        Node<CellType> n(2, 2, 2, T(1));
        
        u32 test_coords[4][3];
        test_coords[0][0] =  3; test_coords[0][1] =  3; test_coords[0][2] =  3;
        test_coords[1][0] =  5; test_coords[1][1] =  6; test_coords[1][2] =  7;
        test_coords[2][0] = 15; test_coords[2][1] = 15; test_coords[2][2] = 15;
        test_coords[3][0] = 16; test_coords[3][1] = 17; test_coords[3][2] = 18;

        n.set(test_coords[0][0], test_coords[0][1], test_coords[0][2], T(1));
        n.set(test_coords[1][0], test_coords[1][1], test_coords[1][2], T(2));
        n.set(test_coords[2][0], test_coords[2][1], test_coords[2][2], T(3));
        n.set(test_coords[3][0], test_coords[3][1], test_coords[3][2], T(4));

        SetIterator<CellType> iter(&n);

        int iter_count = 0;
        u32 i, j, k;
        for ( ; !iter.atEnd(); iter.next()) {
            ++iter_count;
            CPPUNIT_ASSERT(iter.value() == T(iter_count));

            iter.getCoordinates(i, j, k);
            CPPUNIT_ASSERT(test_coords[iter_count-1][0] == i);
            CPPUNIT_ASSERT(test_coords[iter_count-1][1] == j);
            CPPUNIT_ASSERT(test_coords[iter_count-1][2] == k);
        }
        CPPUNIT_ASSERT(iter_count == 4);
    }
}

//------------------------------------------------------------------------------

template <typename T>
void
TestSetIterator<T>::testFillNodeIteration()
{
    USING_NK_NS
    USING_NKHIVE_NS

    typedef Cell<T> CellType;

    index_type index_i, index_j, index_k;

    // single cell among fill nodes
    {
        Node<CellType> n(2, 2, 2, T(5), true);

        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                for (int k = 0; k < 4; ++k) 
                    n.set(i, j, k, T(2));

        SetIterator<CellType> iter(&n);

        // first 64 values are in the cell
        int iter_count = 0;
        for ( ; !iter.atEnd() && iter_count < 64; iter.next()) {
            ++iter_count;
            CPPUNIT_ASSERT(iter.value() == T(2));
        }

        iter.getCoordinates(index_i, index_j, index_k);
        CPPUNIT_ASSERT(index_i == 4);
        CPPUNIT_ASSERT(index_j == 0);
        CPPUNIT_ASSERT(index_k == 0);

        // the next set of values are in the branch but in filled cells.
        for ( ; !iter.atEnd() && iter_count < 64*64; iter.next())
        {
            ++iter_count;
            CPPUNIT_ASSERT(iter.value() == T(5));
        }
        CPPUNIT_ASSERT(iter_count == 4096);

        iter.getCoordinates(index_i, index_j, index_k);
        CPPUNIT_ASSERT(index_i == 16);
        CPPUNIT_ASSERT(index_j == 0);
        CPPUNIT_ASSERT(index_k == 0);

        // iterate over filled nodes.
        iter_count = 0;
        for ( ; !iter.atEnd(); iter.next()) {
            ++iter_count;
            CPPUNIT_ASSERT(iter.value() == T(5));
        }
        CPPUNIT_ASSERT(iter_count == 258048);
    }
}

//------------------------------------------------------------------------------
