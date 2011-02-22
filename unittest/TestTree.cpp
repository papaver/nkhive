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
// TestTree.cpp
//------------------------------------------------------------------------------

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <nkbase/BinaryOps.h>
#include <nkhive/volume/Tree.h>
#include <nkhive/volume/Cell.h>
#include <nkhive/io/VolumeFile.h>

//-----------------------------------------------------------------------------
// types
//-----------------------------------------------------------------------------

template <typename T> 
class CoordinateMapper
{
public:
    NKHIVE_NS::signed_index_bounds& bounds() 
    { 
        return m_bounds; 
    }
    const NKHIVE_NS::signed_index_bounds getBounds() const
    { 
        return m_bounds; 
    }
    static T get(NKHIVE_NS::signed_index_type i, NKHIVE_NS::signed_index_type j,
                 NKHIVE_NS::signed_index_type k) 
    {
        return T(i * 100 + j * 10 + k);
    }
private:
    NKHIVE_NS::signed_index_bounds m_bounds;
};

//-----------------------------------------------------------------------------
// interface declaration
//-----------------------------------------------------------------------------

class TestTree: public CppUnit::TestFixture {

    CPPUNIT_TEST_SUITE(TestTree);
    CPPUNIT_TEST(test);
    CPPUNIT_TEST(testSetQuadrant);
    CPPUNIT_TEST(testSetGrowingTree);
    CPPUNIT_TEST(testMultiLevelGrowing);
    CPPUNIT_TEST(testGet);
    CPPUNIT_TEST(testOperatorComparison);
    CPPUNIT_TEST(testSetSigned);
    CPPUNIT_TEST(testGetSigned);
    CPPUNIT_TEST(testUnsetEmpty);
    CPPUNIT_TEST(testUnset);
    CPPUNIT_TEST(testIO);
    CPPUNIT_TEST(testIOHDF5);
    CPPUNIT_TEST(testSetIteratorEmptyTree);
    CPPUNIT_TEST(testSetIteratorSingleQuadrant);
    CPPUNIT_TEST(testSetIteratorMultipleQuadrant);
    CPPUNIT_TEST(testSetIteratorWithFilledQuadrant);
    CPPUNIT_TEST(testComputeSetBounds);
    CPPUNIT_TEST(testGetQuadrantBounds);
    CPPUNIT_TEST(testWriteStamp);
    CPPUNIT_TEST(testWriteStampOrigin);
    CPPUNIT_TEST_SUITE_END();
    
public:
    void setUp() {}
    void tearDown() {}
    
    void test();
    void testSetQuadrant();
    void testSetGrowingTree();
    void testMultiLevelGrowing();
    void testGet();
    void testOperatorComparison();
    void testSetSigned();
    void testGetSigned();
    void testUnsetEmpty();
    void testUnset();
    void testIO();
    void testIOHDF5();
    void testSetIteratorEmptyTree();
    void testSetIteratorSingleQuadrant();
    void testSetIteratorMultipleQuadrant();
    void testSetIteratorWithFilledQuadrant();
    void testComputeSetBounds();
    void testGetQuadrantBounds();
    void testWriteStamp();
    void testWriteStampOrigin();
};

//-----------------------------------------------------------------------------
// interface implementation
//-----------------------------------------------------------------------------

CPPUNIT_TEST_SUITE_REGISTRATION(TestTree);

//------------------------------------------------------------------------------

void
TestTree::test()
{
    USING_NKHIVE_NS
    
    Tree<Cell<float> > tree(2, 2, 1.0f);

    for (size_t i = 0; i < NUM_QUADRANTS; ++i) {
        CPPUNIT_ASSERT(tree.height(i) == 1); 
        CPPUNIT_ASSERT(tree.m_max_dim[i] == 16);
    }
}

//------------------------------------------------------------------------------

void
TestTree::testSetQuadrant()
{
    USING_NK_NS
    USING_NKHIVE_NS

    for (size_t q = 0; q < NUM_QUADRANTS; ++q) {
        Tree<Cell<float> > tree(2, 2, 1.0f);

        tree.update(q, 0, 0, 0, 5.0f, set_op<float>());

        Cell<float> *cell = tree.m_root[q]->m_branches[0].cell;

        CPPUNIT_ASSERT(cell->get(0, 0, 0) == 5.0f);

        for (size_t i = 1; i < 3; ++i) {
            for (size_t j = 0; j < 3; ++j) {
                for (size_t k = 0; k < 3; ++k) {
                    CPPUNIT_ASSERT(cell->get(i, j, k) == 1.0f);
                }
            }
        }

        tree.update(q, 3, 3, 3, 5.0f, set_op<float>());

        CPPUNIT_ASSERT(cell->get(3, 3, 3) == 5.0f);

        tree.update(q, 15, 15, 15, 5.0f, set_op<float>());
        cell = tree.m_root[q]->m_branches[63].cell;
        CPPUNIT_ASSERT(cell->get(3, 3, 3) == 5.0f);

        for (size_t i = 0; i < 2; ++i) {
            for (size_t j = 0; j < 3; ++j) {
                for (size_t k = 0; k < 3; ++k) {
                    CPPUNIT_ASSERT(cell->get(i, j, k) == 1.0f);
                }
            }
        }

        CPPUNIT_ASSERT(tree.height(q) == 1);
        CPPUNIT_ASSERT(tree.m_max_dim[q] == 16);
    }
}

//------------------------------------------------------------------------------

void 
TestTree::testSetGrowingTree()
{
    USING_NK_NS
    USING_NKHIVE_NS

    for (size_t q = 0; q < NUM_QUADRANTS; ++q) {
        Tree<Cell<float> > tree(2, 2, 1.0f);

        tree.update(q, 16, 0, 0, 5.0f, set_op<float>());

        CPPUNIT_ASSERT(tree.height(q) == 2);
        CPPUNIT_ASSERT(tree.m_max_dim[q] == 64);
        CPPUNIT_ASSERT(!tree.m_root[q]->m_bitfield.isSet(0));
        CPPUNIT_ASSERT(tree.m_root[q]->m_branches[0].node == NULL);

        Cell<float> *cell = 
            tree.m_root[q]->m_branches[1].node->m_branches[0].cell;

        CPPUNIT_ASSERT(cell->get(0, 0, 0) == 5.0f);

        for (size_t i = 1; i < 3; ++i) {
            for (size_t j = 0; j < 3; ++j) {
                for (size_t k = 0; k < 3; ++k) {
                    CPPUNIT_ASSERT(cell->get(i, j, k) == 1.0f);
                }
            }
        }

        tree.update(q, 64, 64, 64, 5.0f, set_op<float>());

        cell = tree.m_root[q]->m_branches[21].node->m_branches[0].node->
               m_branches[0].cell;

        CPPUNIT_ASSERT(cell->get(0, 0, 0) == 5.0f);
        for (size_t i = 1; i < 3; ++i) {
            for (size_t j = 0; j < 3; ++j) {
                for (size_t k = 0; k < 3; ++k) {
                    CPPUNIT_ASSERT(cell->get(i, j, k) == 1.0f);
                }
            }
        }

        CPPUNIT_ASSERT(tree.height(q) == 3);
        CPPUNIT_ASSERT(tree.m_max_dim[q] == 256);
        // Make sure the the subtree bit is set.
        CPPUNIT_ASSERT(tree.m_root[q]->m_bitfield.isSet(0));

        // Test the original cell at m_branches[0] for 16,0,0
        cell = tree.m_root[q]->m_branches[0].node->m_branches[1].node->
               m_branches[0].cell;
        CPPUNIT_ASSERT(cell->get(0, 0, 0) == 5.0f);
    }
}

//------------------------------------------------------------------------------

// Test what happens hwen grow multiple levels at once.
void
TestTree::testMultiLevelGrowing()
{
    USING_NK_NS
    USING_NKHIVE_NS

    for (size_t q = 0; q < NUM_QUADRANTS; ++q) {
        Tree<Cell<float> > tree(2, 2, 1.0f);
        CPPUNIT_ASSERT(tree.height(q) == 1);
        CPPUNIT_ASSERT(tree.m_root[q]->isEmpty());

        tree.update(q, 256, 0, 0, 10.0f, set_op<float>());
        CPPUNIT_ASSERT(tree.height(q) == 4);
        CPPUNIT_ASSERT(!tree.m_root[q]->isEmpty());

        CPPUNIT_ASSERT(tree.m_root[q]->m_branches[0].node == NULL);
        CPPUNIT_ASSERT(!tree.m_root[q]->m_bitfield.isSet(0));
    }
}

//------------------------------------------------------------------------------

void
TestTree::testGet()
{
    USING_NK_NS
    USING_NKHIVE_NS

    Tree<Cell<float> > tree(2, 2, 1.0f);

    // Get something out of bounds.
    CPPUNIT_ASSERT(tree.get(64, 64, 64) == 1.0f);
    CPPUNIT_ASSERT(tree.height(0) == 1);
    CPPUNIT_ASSERT(tree.m_max_dim[0] == 16);

    tree.set(16, 0, 0, 10.0f);
    tree.set(64, 0, 0, 5.0f);
    tree.set(182, 12, 124, 5.0f);

    CPPUNIT_ASSERT(tree.get(64, 0, 0) == 5.0f);
    CPPUNIT_ASSERT(tree.get(16, 0, 0) == 10.0f);
    CPPUNIT_ASSERT(tree.get(182, 12, 124) == 5.0f);
    CPPUNIT_ASSERT(tree.get(182, 12, 125) == 1.0f);
}

//------------------------------------------------------------------------------

void
TestTree::testOperatorComparison()
{
    USING_NK_NS
    USING_NKHIVE_NS

    Tree<Cell<float> > tree(2, 2, 1.0f);

    Tree<Cell<float> > *tree2 = new Tree<Cell<float> >(2, 2, 1.0f);

    CPPUNIT_ASSERT(tree.operator==(*tree2));

    delete tree2;
    tree2 = new Tree<Cell<float> >(3, 2, 1.0f);
    CPPUNIT_ASSERT(tree.operator!=(*tree2));

    delete tree2;
    tree2 = new Tree<Cell<float> >(2, 3, 1.0f);
    CPPUNIT_ASSERT(tree.operator!=(*tree2));

    delete tree2;
    tree2 = new Tree<Cell<float> >(2, 2, 1.2f);
    CPPUNIT_ASSERT(tree.operator!=(*tree2));

    delete tree2;
    tree2 = new Tree<Cell<float> >(2, 2, 1.2f);

    delete tree2;
    tree2 = new Tree<Cell<float> >(2, 2, 1.0f);

    tree.set(0, 0, 0, 5.0f);
    tree2->set(16, 16, 16, 5.0f);
    CPPUNIT_ASSERT(tree.operator!=(*tree2));

    tree.set(16, 16, 16, 5.0f);
    tree2->set(0, 0, 0, 5.0f);
    CPPUNIT_ASSERT(tree.operator==(*tree2));

    delete tree2;
}

//------------------------------------------------------------------------------

void 
TestTree::testSetSigned()
{
    USING_NK_NS
    USING_NKHIVE_NS

    // i,j,k are either 1 or -1, they are multipliers 
    #define TEST_SIGNED(sx, sy, sz)                                    \
    {                                                                  \
        signed_index_type i = sx, j = sy, k = sz;                      \
        assert(abs(i) == 1 && abs(j) == 1 && abs(k) == 1);             \
        Tree<Cell<float> > tree(2, 2, 1.0f);                           \
        index_type q = getQuadrant(i, j, k);                           \
        tree.set(63 * i, 63 * j, 63 * k, 5.0f);                        \
        Cell<float> *cell =                                            \
            tree.m_root[q]->m_branches[63].node->m_branches[63].cell;  \
        vec3i check(3, 3, 3);                                          \
        index_type offset_i, offset_j, offset_k;                       \
        getQuadrantOffsets(offset_i, offset_j, offset_k, q);           \
        check.x -= offset_i; check.y -= offset_j; check.z -= offset_k; \
        CPPUNIT_ASSERT(cell->get(check.x, check.y, check.z) == 5.0f);  \
        CPPUNIT_ASSERT(tree.height(q) == 2);                           \
        CPPUNIT_ASSERT(tree.m_max_dim[q] == 64);                       \
        for (size_t i = 0; i < NUM_QUADRANTS; ++i) {                   \
            if (i == q) continue;                                      \
            CPPUNIT_ASSERT(tree.height(i) == 1);                       \
            CPPUNIT_ASSERT(tree.m_max_dim[i] == 16);                   \
            CPPUNIT_ASSERT(tree.m_root[i]->isEmpty());                 \
        }                                                              \
    }                                                                   

    // Test each quadrant.
    TEST_SIGNED( 1,  1, -1)
    TEST_SIGNED( 1, -1,  1)
    TEST_SIGNED(-1,  1,  1)
    TEST_SIGNED( 1, -1, -1)
    TEST_SIGNED(-1,  1, -1)
    TEST_SIGNED(-1, -1,  1)
    TEST_SIGNED(-1, -1, -1)

    // quadrant offset checks
    {
        Tree<Cell<float> > tree(2, 2, 0.0f);

        tree.set( 0,  0,  0, 1.0f);
        tree.set( 0,  0, -1, 2.0f);
        tree.set( 0, -1,  0, 3.0f);
        tree.set( 0, -1, -1, 4.0f);
        tree.set(-1,  0,  0, 5.0f);
        tree.set(-1,  0, -1, 6.0f);
        tree.set(-1, -1,  0, 7.0f);
        tree.set(-1, -1, -1, 8.0f);
        
        CPPUNIT_ASSERT(tree.m_root[0]->
                m_branches[0].cell->get(0,  0,  0) == 1.0f);
        CPPUNIT_ASSERT(tree.m_root[1]->
                m_branches[0].cell->get(0,  0,  0) == 2.0f);
        CPPUNIT_ASSERT(tree.m_root[2]->
                m_branches[0].cell->get(0,  0,  0) == 3.0f);
        CPPUNIT_ASSERT(tree.m_root[3]->
                m_branches[0].cell->get(0,  0,  0) == 4.0f);
        CPPUNIT_ASSERT(tree.m_root[4]->
                m_branches[0].cell->get(0,  0,  0) == 5.0f);
        CPPUNIT_ASSERT(tree.m_root[5]->
                m_branches[0].cell->get(0,  0,  0) == 6.0f);
        CPPUNIT_ASSERT(tree.m_root[6]->
                m_branches[0].cell->get(0,  0,  0) == 7.0f);
        CPPUNIT_ASSERT(tree.m_root[7]->
                m_branches[0].cell->get(0,  0,  0) == 8.0f);
    }
}

//------------------------------------------------------------------------------

void
TestTree::testGetSigned()
{
    USING_NK_NS
    USING_NKHIVE_NS

    Tree<Cell<float> > tree(2, 2, 1.0f);

    CPPUNIT_ASSERT(tree.get(-64, -64, 64) == 1.0f);

    tree.set(16, -3, -5, 10.0f);
    tree.set(-16, -4, -1, 10.0f);
    tree.set(56, 43, -23, 10.0f);
    tree.set(23, -34, 34, 10.0f);
    tree.set(-1, 3, 1, 10.0f);
    tree.set(-1, -3, 1, 5.0f);

    CPPUNIT_ASSERT(tree.get(16, -3, -5) == 10.0f);
    CPPUNIT_ASSERT(tree.get(-16, -4, -1) == 10.0f);
    CPPUNIT_ASSERT(tree.get(56, 43, -23) == 10.0f);
    CPPUNIT_ASSERT(tree.get(23, -34, 34) == 10.0f);
    CPPUNIT_ASSERT(tree.get(-1, 3, 1) == 10.0f);
    CPPUNIT_ASSERT(tree.get(-1, -3, 1) == 5.0f);
}

//------------------------------------------------------------------------------

void 
TestTree::testUnsetEmpty()
{
    USING_NK_NS
    USING_NKHIVE_NS

    // test for each quadrant
    for (size_t q = 0; q < NUM_QUADRANTS; ++q) {
        Tree<Cell<float> > tree(2, 2, 1.0f);
        CPPUNIT_ASSERT(tree.height(q) == 1);
        CPPUNIT_ASSERT(tree.m_root[q]->isEmpty());

        // do the unset past the set dimensions
        tree.unset(1024, 0, 0);
        CPPUNIT_ASSERT(tree.height(q) == 1);
        CPPUNIT_ASSERT(tree.m_root[q]->isEmpty());
    }
}

void
TestTree::testUnset()
{
    USING_NK_NS
    USING_NKHIVE_NS

    #define UNSET_TEST(i, j, k) \
    {                                                       \
        u8 q = getQuadrant<signed_index_type>(i, j, k);     \
        Tree<Cell<float> > tree(2, 2, 1.0f);                \
        CPPUNIT_ASSERT(tree.height(q) == 1);                \
        CPPUNIT_ASSERT(tree.m_root[q]->isEmpty());          \
                                                            \
        tree.set(i, j, k, 10.0f);                           \
        CPPUNIT_ASSERT(tree.height(q) == 4);                \
        CPPUNIT_ASSERT(!tree.m_root[q]->isEmpty());         \
                                                            \
        tree.unset(i, j, k);                                \
        CPPUNIT_ASSERT(tree.height(q) == 4);                \
        CPPUNIT_ASSERT(tree.m_root[q]->isEmpty());          \
                                                            \
        tree.set(i, j, k, 1.0f);                            \
        CPPUNIT_ASSERT(tree.height(q) == 4);                \
        CPPUNIT_ASSERT(!tree.m_root[q]->isEmpty());         \
        CPPUNIT_ASSERT(tree.get(i, j, k) == 1.0f);          \
    }                                                       

    // test for each quadrant
    UNSET_TEST(257, 0, 0)
    UNSET_TEST(-257, 0, 0)
    UNSET_TEST(0, 0, -257)
    UNSET_TEST(0, -257, 0)
    UNSET_TEST(257, -257, -257)
    UNSET_TEST(-257, -257, 0)
    UNSET_TEST(-257, -257, -257)
    UNSET_TEST(-257, 257, -257)

    #undef UNSET_TEST
}

//------------------------------------------------------------------------------

void
TestTree::testIO()
{
    USING_NK_NS
    USING_NKHIVE_NS

    #define TEST_IO(t, t2) {                                            \
        std::ostringstream ostr(std::ios_base::binary);                 \
        t.write(ostr);                                                  \
        std::istringstream istr(ostr.str(), std::ios_base::binary);     \
        t2.read(istr);                                                  \
        CPPUNIT_ASSERT(t == t2);                                        \
    }

    Tree<Cell<float> > tree(2, 2, 1.0f);

    for (int i = 0; i < NUM_QUADRANTS; ++i) {
        CPPUNIT_ASSERT(tree.m_root[i]->isEmpty());
    }

    Tree<Cell<float> > tree2(2, 2, 1.0f);
    Tree<Cell<float> > tree3(3, 3, 1.0f);

    tree2.set(-2, 4, -5, 10.0f);

    // test writing and reading empty tree
    TEST_IO(tree, tree2);
    TEST_IO(tree, tree3);

    tree.set(0, 0, 0, 10.0f);
    tree2.set(-2, 4, -5, 10.0f);
    TEST_IO(tree, tree2);
    TEST_IO(tree, tree3);

    tree.set(-1, -1, -4, 6.0f);
    tree2.set(-2, 4, -5, 10.0f);
    TEST_IO(tree, tree2);
    TEST_IO(tree, tree3);

    tree2.set(0, 10, 1024, 2.0f);
    tree2.set(-2, 4, -5, 10.0f);
    TEST_IO(tree, tree2);
    TEST_IO(tree, tree3);

    tree.set(64, 64, 64, 10.0f);
    tree2.set(-2, 4, -5, 10.0f);
    tree2.set(0, 10, 1024, 2.0f);
    TEST_IO(tree, tree2);
    TEST_IO(tree, tree3);

    #undef TEST_IO
}

//------------------------------------------------------------------------------

void
TestTree::testIOHDF5()
{
    USING_NK_NS
    USING_NKHIVE_NS

    #define TEST_IO(t, t2) {                                            \
        remove("testingHDF5.hv");                                       \
        VolumeFile file("testingHDF5.hv", VoidFile::WRITE_TRUNC);       \
        HDF5Group root_group;                                           \
        root_group.open(file.m_id, NK_NS::String("/"));                 \
        HDF5Id root_group_id = root_group.id();                         \
        HDF5Group volume_group;                                         \
        volume_group.create(root_group_id, "Volume1");                  \
        t.write(volume_group.id());                                     \
        file.close();                                                   \
        VolumeFile file2("testingHDF5.hv", VoidFile::READ_ONLY);        \
        HDF5Group root_group2;                                          \
        root_group2.open(file2.m_id, NK_NS::String("/"));               \
        HDF5Id root_group_id2 = root_group2.id();                       \
        HDF5Group volume_group2;                                        \
        volume_group2.open(root_group_id2, "Volume1");                  \
        t2.read(volume_group2.id());                                    \
        CPPUNIT_ASSERT(t == t2);                                        \
        file2.close();                                                  \
        remove("testingHDF5.hv");                                       \
    }

    Tree<Cell<float> > tree(2, 2, 1.0f);

    for (int i = 0; i < NUM_QUADRANTS; ++i) {
        CPPUNIT_ASSERT(tree.m_root[i]->isEmpty());
    }

    Tree<Cell<float> > tree2(2, 2, 1.0f);
    Tree<Cell<float> > tree3(2, 2, 1.0f);

    tree2.set(-2, 4, -5, 10.0f);

    // test writing and reading empty tree
    TEST_IO(tree, tree2);
    TEST_IO(tree, tree3);

    tree.set(0, 0, 0, 10.0f);
    tree2.set(-2, 4, -5, 10.0f);
    TEST_IO(tree, tree2);
    TEST_IO(tree, tree3);

    tree.set(-1, -1, -4, 6.0f);
    tree2.set(-2, 4, -5, 10.0f);
    TEST_IO(tree, tree2);
    TEST_IO(tree, tree3);

    tree2.set(0, 10, 1024, 2.0f);
    tree2.set(-2, 4, -5, 10.0f);
    TEST_IO(tree, tree2);
    TEST_IO(tree, tree3);

    tree.set(64, 64, 64, 10.0f);
    tree2.set(-2, 4, -5, 10.0f);
    tree2.set(0, 10, 1024, 2.0f);
    TEST_IO(tree, tree2);
    TEST_IO(tree, tree3);

    #undef TEST_IO
}

//------------------------------------------------------------------------------

void
TestTree::testSetIteratorEmptyTree()
{
    USING_NK_NS
    USING_NKHIVE_NS

    typedef Tree<Cell<float> >               tree_type;
    typedef tree_type::set_iterator          set_iterator;

    tree_type tree(2, 2, 1.0f);

    set_iterator sit = tree.setIterator();

    int iter_count = 0;
    for ( ; sit(); ++sit) {
        ++iter_count;
    }
    CPPUNIT_ASSERT(iter_count == 0);
}

//------------------------------------------------------------------------------

void
TestTree::testSetIteratorSingleQuadrant()
{
    USING_NK_NS
    USING_NKHIVE_NS

    typedef Tree<Cell<float> >               tree_type;
    typedef tree_type::set_iterator          set_iterator;

    tree_type tree(2, 2, 1.0f);

    tree.update(0, 0, 0, 0, 5.0f, set_op<float>());
    tree.update(0, 15, 15, 15, 5.0f, set_op<float>());

    set_iterator sit = tree.setIterator();

    int iter_count = 0;
    for ( ; sit(); ++sit) {
        ++iter_count;
        CPPUNIT_ASSERT(*sit == 5.0f);
    }
    CPPUNIT_ASSERT(iter_count == 2);
}

//------------------------------------------------------------------------------

void
TestTree::testSetIteratorMultipleQuadrant()
{
    USING_NK_NS
    USING_NKHIVE_NS

    typedef Tree<Cell<float> >               tree_type;
    typedef tree_type::set_iterator          set_iterator;

    // Two internal quadrants set.
    {
        tree_type tree(2, 2, 1.0f);

        tree.update(2, 1, 1, 1, 5.0f, set_op<float>());
        tree.update(4, 2, 2, 2, 5.0f, set_op<float>());

        set_iterator sit = tree.setIterator();

        int iter_count = 0;
        for ( ; sit(); ++sit) {
            ++iter_count;
            CPPUNIT_ASSERT(*sit == 5.0f);
        }
        CPPUNIT_ASSERT(iter_count == 2);
    }

    // boundary quadrants set.
    {
        tree_type tree(2, 2, 1.0f);

        tree.update(0, 1, 1, 1, 5.0f, set_op<float>());
        tree.update(7, 2, 2, 2, 5.0f, set_op<float>());

        set_iterator sit = tree.setIterator();

        int iter_count = 0;
        for ( ; sit(); ++sit) {
            ++iter_count;
            CPPUNIT_ASSERT(*sit == 5.0f);
        }
        CPPUNIT_ASSERT(iter_count == 2);
    }

    // internal and boundary quadrants set.
    {
        tree_type tree(2, 2, 1.0f);

        tree.update(0, 1, 1, 1, 5.0f, set_op<float>());
        tree.update(7, 2, 2, 2, 5.0f, set_op<float>());
        tree.update(2, 3, 3, 3, 5.0f, set_op<float>());
        tree.update(4, 2, 1, 5, 5.0f, set_op<float>());

        set_iterator sit = tree.setIterator();

        int iter_count = 0;
        for ( ; sit(); ++sit) {
            ++iter_count;
            CPPUNIT_ASSERT(*sit == 5.0f);
        }
        CPPUNIT_ASSERT(iter_count == 4);
    }
}

//------------------------------------------------------------------------------

void
TestTree::testSetIteratorWithFilledQuadrant()
{
    USING_NK_NS
    USING_NKHIVE_NS

    typedef Tree<Cell<float> >      tree_type;
    typedef tree_type::set_iterator set_iterator;

    // only a single fill node.
    {
        tree_type tree(2, 2, 1.0f);

        delete tree.m_root[1];
        tree_type::node_type_ptr n = 
            new tree_type::node_type(1, 2, 2, 2.0f, true);
        tree.m_root[1] = n;

        set_iterator sit = tree.setIterator();
        int iter_count = 0;
        for ( ; sit(); ++sit) {
            ++iter_count;
            CPPUNIT_ASSERT(*sit == 2.0f);
        }
        CPPUNIT_ASSERT(iter_count == 4096);
    }

    // filled values at boundary
    {
        tree_type tree(2, 2, 1.0f);

        delete tree.m_root[0];
        tree_type::node_type_ptr n = 
            new tree_type::node_type(1, 2, 2, 2.0f, true);
        tree.m_root[0] = n;

        delete tree.m_root[7];
        n = new tree_type::node_type(1, 2, 2, 2.0f, true);
        tree.m_root[7] = n;

        set_iterator sit = tree.setIterator();
        int iter_count = 0;
        for ( ; sit(); ++sit) {
            ++iter_count;
            CPPUNIT_ASSERT(*sit == 2.0f);
        }
        CPPUNIT_ASSERT(iter_count == 4096*2);
    }

    // filled nodes + set values
    {
        tree_type tree(2, 2, 1.0f);

        delete tree.m_root[2];
        tree_type::node_type_ptr n = 
            new tree_type::node_type(1, 2, 2, 2.0f, true);
        tree.m_root[2] = n;

        delete tree.m_root[4];
        n = new tree_type::node_type(1, 2, 2, 2.0f, true);
        tree.m_root[4] = n;

        tree.update(3, 1, 1, 1, 2.0f, set_op<float>());
        tree.update(5, 2, 2, 2, 2.0f, set_op<float>());
        tree.update(7, 3, 3, 3, 2.0f, set_op<float>());

        set_iterator sit = tree.setIterator();
        int iter_count = 0;
        for ( ; sit(); ++sit) {
            ++iter_count;
            CPPUNIT_ASSERT(*sit == 2.0f);
        }
        CPPUNIT_ASSERT(iter_count == 4096*2 + 3);
    }
}

//------------------------------------------------------------------------------

void 
TestTree::testComputeSetBounds()
{
    USING_NK_NS
    USING_NKHIVE_NS

    typedef Tree<Cell<float> >               tree_type;
    typedef tree_type::set_iterator          set_iterator;

    // an empty tree.
    {
        tree_type tree(2, 2, 1.0f);

        signed_index_bounds bounds;
        CPPUNIT_ASSERT(!tree.computeSetBounds(bounds));
    }

    // only positive quadrants set
    {
        tree_type tree(2, 2, 1.0f);

        tree.set(1, 1, 1, 5.0f);
        tree.set(2, 2, 2, 5.0f);

        signed_index_bounds bounds;
        CPPUNIT_ASSERT(tree.computeSetBounds(bounds));

        CPPUNIT_ASSERT(bounds.min() == vec3i(1, 1, 1));
        CPPUNIT_ASSERT(bounds.max() == vec3i(3, 3, 3));
    }

    // mix neg and pos
    {
        tree_type tree(2, 2, 1.0f);

        tree.set(-1,  1, -1, 5.0f);
        tree.set( 2, -3,  2, 5.0f);
        tree.set( 2,  2,  2, 5.0f);

        signed_index_bounds bounds;
        CPPUNIT_ASSERT(tree.computeSetBounds(bounds));

        CPPUNIT_ASSERT(bounds.min() == vec3i(-1, -3, -1));
        CPPUNIT_ASSERT(bounds.max() == vec3i( 3,  3,  3));
    }

    // all negative
    {
        tree_type tree(2, 2, 1.0f);

        tree.set(-1, -1, -1, 5.0f);
        tree.set(-2, -2, -2, 5.0f);
        tree.set(-4, -2, -5, 5.0f);

        signed_index_bounds bounds;
        CPPUNIT_ASSERT(tree.computeSetBounds(bounds));

        CPPUNIT_ASSERT(bounds.min() == vec3i(-4, -2, -5));
        CPPUNIT_ASSERT(bounds.max() == vec3i(0, 0, 0));
    }

    // positive and negative.
    {
        tree_type tree(2, 2, 1.0f);

        tree.set(-10, -2, -2, 5.0f);
        tree.set(2, 20, 1, 5.0f);

        signed_index_bounds bounds;
        CPPUNIT_ASSERT(tree.computeSetBounds(bounds));

        CPPUNIT_ASSERT(bounds.min() == vec3i(-10, -2, -2));
        CPPUNIT_ASSERT(bounds.max() == vec3i(3, 21, 2));
    }
}

//------------------------------------------------------------------------------

void 
TestTree::testGetQuadrantBounds()
{
    USING_NK_NS
    USING_NKHIVE_NS

    typedef Tree<Cell<float> >               tree_type;
    typedef signed_index_bounds::vector_type vector_type;

    #define TEST_QUADRANT(q, vec_min, vec_max) {                         \
        tree_type tree(2, 2, 1.0f);                                      \
        signed_index_bounds bounds;                                      \
        bounds.min() = vec_min;                                          \
        bounds.max() = vec_max;                                          \
        signed_index_bounds quadrant_bounds[NUM_QUADRANTS];              \
        u8 quadrants = tree.getQuadrantBounds(bounds, quadrant_bounds);  \
        CPPUNIT_ASSERT(quadrants == (1 << q));                           \
        CPPUNIT_ASSERT(quadrant_bounds[q].min() == vec_min);             \
        CPPUNIT_ASSERT(quadrant_bounds[q].max() == vec_max);             \
    }

    // test all the boundry cases to ensure no overflow exists

    // FTR -> 0, 0, 0 -> 000 ->  0
    TEST_QUADRANT(0, vector_type(  0,   0,   0), vector_type(10, 10, 10));
    TEST_QUADRANT(0, vector_type(  5,   5,   5), vector_type(10, 10, 10));
    // NTR -> 0, 0, 1 -> 001 ->  1
    TEST_QUADRANT(1, vector_type(  0,   0, -10), vector_type(10, 10,  0));
    TEST_QUADRANT(1, vector_type(  5,   5, -10), vector_type(10, 10, -5));
    // FBR -> 0, 1, 0 -> 010 ->  2
    TEST_QUADRANT(2, vector_type(  0, -10,   0), vector_type(10,  0, 10));
    TEST_QUADRANT(2, vector_type(  5, -10,   5), vector_type(10, -5, 10));
    // NBR -> 0, 1, 1 -> 011 ->  3 
    TEST_QUADRANT(3, vector_type(  0, -10, -10), vector_type(10,  0,  0));
    TEST_QUADRANT(3, vector_type(  5, -10, -10), vector_type(10, -5, -5));
    // FTL -> 1, 0, 0 -> 100 ->  4 
    TEST_QUADRANT(4, vector_type(-10,   0,   0), vector_type( 0, 10, 10));
    TEST_QUADRANT(4, vector_type(-10,   5,   5), vector_type(-5, 10, 10));
    // NTL -> 1, 0, 1 -> 101 ->  5 
    TEST_QUADRANT(5, vector_type(-10,   0, -10), vector_type( 0, 10,  0));
    TEST_QUADRANT(5, vector_type(-10,   5, -10), vector_type(-5, 10, -5));
    // FBL -> 1, 1, 0 -> 110 ->  6 
    TEST_QUADRANT(6, vector_type(-10, -10,   0), vector_type( 0,  0, 10));
    TEST_QUADRANT(6, vector_type(-10, -10,   5), vector_type(-5, -5, 10));
    // NBL -> 1, 1, 1 -> 111 ->  7 
    TEST_QUADRANT(7, vector_type(-10, -10, -10), vector_type( 0,  0,  0));
    TEST_QUADRANT(7, vector_type(-10, -10, -10), vector_type(-5, -5, -5));

    // test multiquadrant split
    {                                                                    
        tree_type tree(2, 2, 1.0f);                                      

        signed_index_bounds bounds;                                      
        bounds.min() = vector_type(-10, -10, -10);                                          
        bounds.max() = vector_type( 10,  10,  10);                                          

        signed_index_bounds quadrant_bounds[NUM_QUADRANTS];              
        u8 quadrants = tree.getQuadrantBounds(bounds, quadrant_bounds);  

        CPPUNIT_ASSERT(quadrants == 0xff);                           
        CPPUNIT_ASSERT(quadrant_bounds[0].min() == vector_type(  0,   0,   0));
        CPPUNIT_ASSERT(quadrant_bounds[0].max() == vector_type( 10,  10,  10));
        CPPUNIT_ASSERT(quadrant_bounds[1].min() == vector_type(  0,   0, -10));
        CPPUNIT_ASSERT(quadrant_bounds[1].max() == vector_type( 10,  10,   0));
        CPPUNIT_ASSERT(quadrant_bounds[2].min() == vector_type(  0, -10,   0));
        CPPUNIT_ASSERT(quadrant_bounds[2].max() == vector_type( 10,   0,  10));
        CPPUNIT_ASSERT(quadrant_bounds[3].min() == vector_type(  0, -10, -10));
        CPPUNIT_ASSERT(quadrant_bounds[3].max() == vector_type( 10,   0,   0));
        CPPUNIT_ASSERT(quadrant_bounds[4].min() == vector_type(-10,   0,   0));
        CPPUNIT_ASSERT(quadrant_bounds[4].max() == vector_type(  0,  10,  10));
        CPPUNIT_ASSERT(quadrant_bounds[5].min() == vector_type(-10,   0, -10));
        CPPUNIT_ASSERT(quadrant_bounds[5].max() == vector_type(  0,  10,   0));
        CPPUNIT_ASSERT(quadrant_bounds[6].min() == vector_type(-10, -10,   0));
        CPPUNIT_ASSERT(quadrant_bounds[6].max() == vector_type(  0,   0,  10));
        CPPUNIT_ASSERT(quadrant_bounds[7].min() == vector_type(-10, -10, -10));
        CPPUNIT_ASSERT(quadrant_bounds[7].max() == vector_type(  0,   0,   0));
    }
}

//-----------------------------------------------------------------------------

void
TestTree::testWriteStamp() 
{
    USING_NK_NS
    USING_NKHIVE_NS

    typedef float                            T;
    typedef Stamp<T, CoordinateMapper>       StampTester;
    typedef Tree<Cell<T> >                   tree_type;
    typedef index_bounds::vector_type        vector;
    typedef signed_index_bounds::vector_type signed_vector;

    const T default_val(0);    

    const signed_vector min(-64, -64, -64);                                       
    const signed_vector max(64, 64, 64);                                       
    const signed_vector pos(5, 5, 5);                                       
                                                                            
    StampTester stamp;                                                  
    stamp.bounds().setExtrema(min, max);                        
                                                                            
    tree_type tree(2, 2, default_val);                     
    tree.stamp(stamp, pos);                    

    stamp.bounds().translate(pos);
                                                                            
    for (signed_index_type k = min.z - 1; k <= max.z; ++k) {                           
        for (signed_index_type j = min.y - 1; j <= max.y; ++j) {                       
            for (signed_index_type i = min.x - 1; i <= max.x; ++i) {                   
                if (!stamp.bounds().inRange(index_vec(i, j, k))) {         
                    CPPUNIT_ASSERT(tree.get(i, j, k) == default_val);  
                } else {                                                
                    CPPUNIT_ASSERT(tree.get(i, j, k) ==                
                        CoordinateMapper<T>::get(
                            i - pos.x, j - pos.y, k - pos.z));          
                }                                                       
            }                                                           
        }                                                               
    }                                                                   
}

//-----------------------------------------------------------------------------

void
TestTree::testWriteStampOrigin() 
{
    USING_NK_NS
    USING_NKHIVE_NS

    typedef float                            T;
    typedef Stamp<T, CoordinateMapper>       StampTester;
    typedef Tree<Cell<T> >                   tree_type;
    typedef index_bounds::vector_type        vector;
    typedef signed_index_bounds::vector_type signed_vector;

    const T default_val(0);    

    const signed_vector min(-64, -64, -64);                                       
    const signed_vector max(64, 64, 64);                                       

    // Place the point at the origin
    const signed_vector pos(0, 0, 0);                                       
                                                                            
    StampTester stamp;                                                  
    stamp.bounds().setExtrema(min, max);                        
                                                                            
    tree_type tree(2, 2, default_val);                     
    tree.stamp(stamp, pos);                    

    stamp.bounds().translate(pos);
                                                                            
    for (signed_index_type k = min.z - 1; k <= max.z; ++k) {                           
        for (signed_index_type j = min.y - 1; j <= max.y; ++j) {                       
            for (signed_index_type i = min.x - 1; i <= max.x; ++i) {                   
                if (!stamp.bounds().inRange(index_vec(i, j, k))) {         
                    CPPUNIT_ASSERT(tree.get(i, j, k) == default_val);  
                } else {                                                
                    CPPUNIT_ASSERT(tree.get(i, j, k) ==                
                        CoordinateMapper<T>::get(i, j, k));          
                }                                                       
            }                                                           
        }                                                               
    }                                                                   
}

//------------------------------------------------------------------------------

