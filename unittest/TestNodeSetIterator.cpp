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
// TestNodeSetIterator.cpp
//------------------------------------------------------------------------------

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <nkhive/volume/NodeSetIterator.h>

template <typename T>
class TestNodeSetIterator: public CppUnit::TestFixture 
{
    CPPUNIT_TEST_SUITE(TestNodeSetIterator);
    CPPUNIT_TEST(testDirectCellParent);
    CPPUNIT_TEST(testMultilevelNode);
    CPPUNIT_TEST_SUITE_END();
    
public:
    void setUp() {}
    void tearDown() {}

    void testDirectCellParent();
    void testMultilevelNode();
};

//-----------------------------------------------------------------------------
// test suite registration
//-----------------------------------------------------------------------------

CPPUNIT_TEST_SUITE_REGISTRATION(TestNodeSetIterator<float>);
CPPUNIT_TEST_SUITE_REGISTRATION(TestNodeSetIterator<double>);
CPPUNIT_TEST_SUITE_REGISTRATION(TestNodeSetIterator<int32_t>);

//------------------------------------------------------------------------------
// tests
//------------------------------------------------------------------------------

template <typename T>
void
TestNodeSetIterator<T>::testDirectCellParent()
{
    USING_NK_NS
    USING_NKHIVE_NS

    typedef Cell<T> CellType;

    // test empty cell
    {
        Node<CellType> n(1, 2, 2, T(0));

        NodeSetIterator<CellType> iter(&n);

        int iter_count = 0;
        for ( ; !iter.atEnd(); iter.next()) { ++iter_count; }

        CPPUNIT_ASSERT(iter_count == 0);
    }

    // a single cell.
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

        NodeSetIterator<CellType> iter(&n);

        int iter_count = 0;
        u32 i, j, k;
        for ( ; !iter.atEnd(); iter.next()) {
            CPPUNIT_ASSERT_THROW(iter.value(), Iex::LogicExc);

            iter.getCoordinates(i,j,k);
            CPPUNIT_ASSERT(0 == i);
            CPPUNIT_ASSERT(0 == j);
            CPPUNIT_ASSERT(0 == k);

            ++iter_count;
        }
        CPPUNIT_ASSERT(iter_count == 1);
    }

    // test multiple cells
    {
        Node<CellType> n(1, 2, 2, T(1));

        u32 test_coords[3][3];
        test_coords[0][0] =  0; test_coords[0][1] =  0; test_coords[0][2] =  0;
        test_coords[1][0] =  1; test_coords[1][1] =  1; test_coords[1][2] =  1;
        test_coords[2][0] =  3; test_coords[2][1] =  3; test_coords[2][2] =  3;

        n.set(0, 0, 0, T(1));
        n.set(4, 4, 4, T(2));
        n.set(12, 15, 13, T(3));

        NodeSetIterator<CellType> iter(&n);

        int iter_count = 0;
        u32 i, j, k;
        for ( ; !iter.atEnd(); iter.next()) {
            iter.getCoordinates(i, j, k);
            CPPUNIT_ASSERT(test_coords[iter_count][0] == i);
            CPPUNIT_ASSERT(test_coords[iter_count][1] == j);
            CPPUNIT_ASSERT(test_coords[iter_count][2] == k);
             
            ++iter_count;
        }
        CPPUNIT_ASSERT(iter_count == 3);
    }
}

//------------------------------------------------------------------------------

template <typename T>
void
TestNodeSetIterator<T>::testMultilevelNode()
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
        
        NodeSetIterator<CellType> iter(&n);

        int iter_count = 0;
        u32 i, j, k;
        for ( ; !iter.atEnd(); iter.next()) {
            iter.getCoordinates(i, j, k);
            CPPUNIT_ASSERT(0 == i);
            CPPUNIT_ASSERT(0 == j);
            CPPUNIT_ASSERT(0 == k);

            ++iter_count;
        }
        CPPUNIT_ASSERT(iter_count == 1);
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
       
        NodeSetIterator<CellType> iter(&n);

        int iter_count = 0;
        u32 i, j, k;
        for ( ; !iter.atEnd(); iter.next()) {

            iter.getCoordinates(i, j, k);
            CPPUNIT_ASSERT(0 == i);
            CPPUNIT_ASSERT(0 == j);
            CPPUNIT_ASSERT(0 == k);

            ++iter_count;
        }
        CPPUNIT_ASSERT(iter_count == 1);
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

        NodeSetIterator<CellType> iter(&n);

        u32 coords[2][3];
        coords[0][0] = 0; coords[0][1] = 0; coords[0][2] = 0;
        coords[1][0] = 1; coords[1][1] = 1; coords[1][2] = 1;

        int iter_count = 0;
        u32 i, j, k;
        for ( ; !iter.atEnd(); iter.next()) {
            iter.getCoordinates(i, j, k);
            CPPUNIT_ASSERT(coords[iter_count][0] == i);
            CPPUNIT_ASSERT(coords[iter_count][1] == j);
            CPPUNIT_ASSERT(coords[iter_count][2] == k);

            ++iter_count;
        }
        CPPUNIT_ASSERT(iter_count == 2);
    }
}

//------------------------------------------------------------------------------
