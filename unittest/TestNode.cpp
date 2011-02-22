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
// TestNode.cpp
//------------------------------------------------------------------------------

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <nkhive/bitfields/BitOps.h>
#include <nkhive/volume/Cell.h>
#include <nkhive/io/VolumeFile.h>
#include <nkhive/volume/Node.h>

//-----------------------------------------------------------------------------
// defines
//-----------------------------------------------------------------------------

/**
 * Get the cell group id from a volume file for reading test data under
 * a node
 */
#define GET_LEAF_ID(vf, root_id, leaf_id)                   \
    NK_NS::i32 size = H5Lget_name_by_idx(root_id, ".",      \
                                  H5_INDEX_NAME,            \
                                  H5_ITER_INC,              \
                                  0,                        \
                                  NULL,                     \
                                  0,                        \
                                  H5P_DEFAULT);             \
    ++size;                                                 \
    char buf[size];                                         \
    H5Lget_name_by_idx(root_id, ".",                        \
                       H5_INDEX_NAME,                       \
                       H5_ITER_INC,                         \
                       0,                                   \
                       buf,                                 \
                       size,                                \
                       H5P_DEFAULT);                        \
    NK_NS::String leaf_name(buf);                           \
    HDF5Group leaf_group;                                   \
    leaf_group.open(root_id, leaf_name);                    \
    leaf_id = leaf_group.id();                              \

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

template <typename T> 
class One
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
    static T get(NKHIVE_NS::signed_index_type, NKHIVE_NS::signed_index_type,
                 NKHIVE_NS::signed_index_type) 
    {
        return T(1);
    }
private:
    NKHIVE_NS::signed_index_bounds m_bounds;
};

//-----------------------------------------------------------------------------
// interface declaration
//-----------------------------------------------------------------------------

class TestNode : public CppUnit::TestFixture {

    CPPUNIT_TEST_SUITE(TestNode);
    CPPUNIT_TEST(testComputeChildDivisions);
    CPPUNIT_TEST(testComputeBranchIndex);
    CPPUNIT_TEST(testComputeChildCoordinates);
    CPPUNIT_TEST(testComputeMaxDim);
    CPPUNIT_TEST(testSetSingleLevel);
    CPPUNIT_TEST(testSetMultiLevel);
    CPPUNIT_TEST(testUpdateMultiLevel);
    CPPUNIT_TEST(testUpdateWithFillNode);
    CPPUNIT_TEST(testGet);
    CPPUNIT_TEST(testOperatorComparisonSingleLevel);
    CPPUNIT_TEST(testOperatorComparison);
    CPPUNIT_TEST(testIO);
    CPPUNIT_TEST(testFillNodeCreation);
    CPPUNIT_TEST(testFillNodeIO);
    CPPUNIT_TEST(testFillNodeIOHDF5);
    CPPUNIT_TEST(testFillSetSingleLevel);
    CPPUNIT_TEST(testFillSetMultiLevel);
    CPPUNIT_TEST(testUnsetSingleLevel);
    CPPUNIT_TEST(testUnsetMultiLevel);
    CPPUNIT_TEST(testFillUnsetSingleLevel);
    CPPUNIT_TEST(testFillUnsetMultiLevel);
    CPPUNIT_TEST(testComputeSetBounds);
    CPPUNIT_TEST(testWriteStamp);
    CPPUNIT_TEST_SUITE_END();
    
public:
    void setUp() {}
    void tearDown() {}
    
    void testComputeChildDivisions();
    void testComputeBranchIndex();
    void testComputeChildCoordinates();
    void testComputeMaxDim();
    void testSetSingleLevel();
    void testSetMultiLevel();
    void testUpdateMultiLevel();
    void testUpdateWithFillNode();
    void testGet();
    void testOperatorComparisonSingleLevel();
    void testOperatorComparison();
    void testIO();
    void testFillNodeCreation();
    void testFillNodeIO();
    void testFillNodeIOHDF5();
    void testFillSetSingleLevel();
    void testFillSetMultiLevel();
    void testUnsetSingleLevel();
    void testUnsetMultiLevel();
    void testFillUnsetSingleLevel();
    void testFillUnsetMultiLevel();
    void testComputeSetBounds();
    void testWriteStamp();
};

//-----------------------------------------------------------------------------
// interface implementation
//-----------------------------------------------------------------------------

CPPUNIT_TEST_SUITE_REGISTRATION(TestNode);

//-----------------------------------------------------------------------------

void 
TestNode::testComputeChildDivisions()
{
    USING_NKHIVE_NS

    Node<Cell<float> > *node = new Node<Cell<float> >(1, 4, 3, 1.0f);
    CPPUNIT_ASSERT(node->m_lg_child_divisions == 3);
    CPPUNIT_ASSERT(node->isEmpty());
    delete node;
    
    node = new Node<Cell<float> >(3, 4, 3, 1.0f);
    CPPUNIT_ASSERT(node->m_lg_child_divisions == 11);
    CPPUNIT_ASSERT(node->isEmpty());
    delete node;
}


//-----------------------------------------------------------------------------

void 
TestNode::testComputeBranchIndex()
{
    USING_NKHIVE_NS

    Node<Cell<float> > *node = new Node<Cell<float> >(1, 2, 2, 1.0f);
    index_type branch = node->computeBranchIndex(3, 3, 3);
    CPPUNIT_ASSERT(branch == 0);
    branch = node->computeBranchIndex(4, 1, 2);
    CPPUNIT_ASSERT(branch == 1);
    branch = node->computeBranchIndex(4, 5, 2);
    CPPUNIT_ASSERT(branch == 5);
    branch = node->computeBranchIndex(4, 5, 10);
    CPPUNIT_ASSERT(branch == 37);
    delete node;

    node = new Node<Cell<float> >(2, 2, 2, 1.0f);
    branch = node->computeBranchIndex(4, 5, 10);
    CPPUNIT_ASSERT(branch == 0);
    branch = node->computeBranchIndex(17, 5, 10);
    CPPUNIT_ASSERT(branch == 1);
    branch = node->computeBranchIndex(16, 0, 16);
    CPPUNIT_ASSERT(branch == 17);
    delete node;
}

//-----------------------------------------------------------------------------

void 
TestNode::testComputeChildCoordinates()
{
    USING_NKHIVE_NS

    index_type ic, jc, kc;
    Node<Cell<float> > *node = new Node<Cell<float> >(1, 2, 2, 1.0f);
    node->computeChildCoordinates(1, 2, 3, ic, jc, kc);
    CPPUNIT_ASSERT(ic == 1);
    CPPUNIT_ASSERT(jc == 2);
    CPPUNIT_ASSERT(kc == 3);

    node->computeChildCoordinates(4, 5, 6, ic, jc, kc);
    CPPUNIT_ASSERT(ic == 0);
    CPPUNIT_ASSERT(jc == 1);
    CPPUNIT_ASSERT(kc == 2);
    delete node;

    node = new Node<Cell<float> >(2, 2, 2, 1.0f);
    node->computeChildCoordinates(8, 9, 15, ic, jc, kc);
    CPPUNIT_ASSERT(ic == 8);
    CPPUNIT_ASSERT(jc == 9);
    CPPUNIT_ASSERT(kc == 15);

    node->computeChildCoordinates(18, 24, 15, ic, jc, kc);
    CPPUNIT_ASSERT(ic == 2);
    CPPUNIT_ASSERT(jc == 8);
    CPPUNIT_ASSERT(kc == 15);
    delete node;
}

//-----------------------------------------------------------------------------

void 
TestNode::testComputeMaxDim()
{
    USING_NKHIVE_NS

    Node<Cell<float> > *node = new Node<Cell<float> >(1, 2, 2, 1.0f);
    CPPUNIT_ASSERT(node->computeMaxDim() == 16);
    delete node;

    node = new Node<Cell<float> >(2, 3, 2, 1.0f);
    CPPUNIT_ASSERT(node->computeMaxDim() == 256);
    delete node;
}

//-----------------------------------------------------------------------------

void 
TestNode::testSetSingleLevel()
{
    USING_NKHIVE_NS

    Node<Cell<float> > *node = new Node<Cell<float> >(1, 2, 2, 1.0f);
    node->set(0, 0, 0, 5.0f);
    CPPUNIT_ASSERT(node->m_branches[0].cell->get(0, 0, 0) == 5.0f);

    CPPUNIT_ASSERT(!node->isEmpty());
    CPPUNIT_ASSERT(node->isBranching());

    node->set(1, 2, 3, 5.0f);
    CPPUNIT_ASSERT(node->m_branches[0].cell->get(1, 2, 3) == 5.0f);
    CPPUNIT_ASSERT(node->m_branches[0].cell->get(0, 0, 0) == 5.0f);
    CPPUNIT_ASSERT(node->m_branches[0].cell->get(1, 1, 1) == 1.0f);

    node->set(12, 15, 13, 5.0f);
    CPPUNIT_ASSERT(node->m_branches[63].cell->get(0, 3, 1) == 5.0f);
    CPPUNIT_ASSERT(node->m_branches[63].cell->get(0, 0, 0) == 1.0f);

    // check that all other cell branches are null
    for (size_t i = 1; i < node->m_branches.size() - 1; ++i) {
        CPPUNIT_ASSERT(node->m_branches[i].node == NULL);
    }
    delete node;
}

//-----------------------------------------------------------------------------

void 
TestNode::testSetMultiLevel()
{
    USING_NKHIVE_NS

    Cell<float> *cell;

    Node<Cell<float> > *node = new Node<Cell<float> >(2, 2, 2, 1.0f);
    node->set(0, 0, 0, 5.0f);
    cell = node->m_branches[0].node->m_branches[0].cell;
    CPPUNIT_ASSERT(cell->get(0, 0, 0) == 5.0f);
    CPPUNIT_ASSERT(cell->get(1, 1, 1) == 1.0f);

    // make sure all other nodes are NULL
    for (size_t i = 1; i < node->m_branches.size(); ++i) 
        CPPUNIT_ASSERT(node->m_branches[i].node == NULL);

    node->set(16, 1, 2, 5.0f);
    cell = node->m_branches[1].node->m_branches[0].cell;
    CPPUNIT_ASSERT(cell->get(0, 1, 2) == 5.0f);

    // make sure all other nodes are NULL
    for (size_t i = 2; i < node->m_branches.size(); ++i) 
        CPPUNIT_ASSERT(node->m_branches[i].node == NULL);

    node->set(16, 16, 16, 5.0f);
    cell = node->m_branches[21].node->m_branches[0].cell;
    CPPUNIT_ASSERT(cell->get(0, 0, 0) == 5.0f);
    CPPUNIT_ASSERT(cell->get(1, 0, 0) == 1.0f);

    delete node;
}

//------------------------------------------------------------------------------

void 
TestNode::testUpdateMultiLevel()
{
    USING_NKHIVE_NS

    Cell<float> *cell;

    Node<Cell<float> > *node = new Node<Cell<float> >(2, 2, 2, 1.0f);
    node->set(0, 0, 0, 5.0f);
    cell = node->m_branches[0].node->m_branches[0].cell;
    CPPUNIT_ASSERT(cell->get(0, 0, 0) == 5.0f);
    CPPUNIT_ASSERT(cell->get(1, 1, 1) == 1.0f);

    // make sure all other nodes are NULL
    for (size_t i = 1; i < node->m_branches.size(); ++i) 
        CPPUNIT_ASSERT(node->m_branches[i].node == NULL);

    node->set(16, 1, 2, 5.0f);
    cell = node->m_branches[1].node->m_branches[0].cell;
    CPPUNIT_ASSERT(cell->get(0, 1, 2) == 5.0f);

    // make sure all other nodes are NULL
    for (size_t i = 2; i < node->m_branches.size(); ++i) 
        CPPUNIT_ASSERT(node->m_branches[i].node == NULL);

    node->set(16, 16, 16, 5.0f);
    cell = node->m_branches[21].node->m_branches[0].cell;
    CPPUNIT_ASSERT(cell->get(0, 0, 0) == 5.0f);
    CPPUNIT_ASSERT(cell->get(1, 0, 0) == 1.0f);

    node->update(0, 0, 0, 1.0f, std::minus<float>());
    node->update(16, 16, 16, 1.0f, std::plus<float>());

    CPPUNIT_ASSERT(node->get(0, 0, 0) == 4.0f);
    CPPUNIT_ASSERT(node->get(16, 16, 16) == 6.0f);

    delete node;
}

//------------------------------------------------------------------------------

void
TestNode::testUpdateWithFillNode()
{
    USING_NK_NS
    USING_NKHIVE_NS

    float fill_val = 2.0f;

    Node<Cell<float> > n(1, 2, 2, fill_val, true);

    CPPUNIT_ASSERT(n.isFill());
    CPPUNIT_ASSERT(!n.isEmpty());
    CPPUNIT_ASSERT(!n.isBranching());
    CPPUNIT_ASSERT(n.m_branches.size() == 0);

    CPPUNIT_ASSERT(n.m_bitfield.isFull());

    // Should not update the node state.
    n.update(1, 2, 3, fill_val, set_op<float>());

    CPPUNIT_ASSERT(n.isFill());
    CPPUNIT_ASSERT(!n.isEmpty());
    CPPUNIT_ASSERT(!n.isBranching());
    CPPUNIT_ASSERT(n.m_branches.size() == 0);
    CPPUNIT_ASSERT(n.m_bitfield.isFull());

    // Should update the node state and allocate cell.
    n.update(1, 2, 3, fill_val, std::plus<float>());

    CPPUNIT_ASSERT(!n.isFill());
    CPPUNIT_ASSERT(!n.isEmpty());
    CPPUNIT_ASSERT(n.isBranching());
    CPPUNIT_ASSERT(n.m_branches.size() != 0);
    CPPUNIT_ASSERT(n.m_bitfield.isFull());

    Cell<float> *cell = n.m_branches[0].cell;
    CPPUNIT_ASSERT(cell->get(1, 2, 3) == fill_val + fill_val);
}

//-----------------------------------------------------------------------------

void 
TestNode::testGet()
{
    USING_NKHIVE_NS

    Node<Cell<float> > *node = new Node<Cell<float> >(2, 2, 2, 1.0f);
    
    // ensure all other values are still 1.0;
    for (size_t i = 0; i < 64; ++i) {
        for (size_t j = 0; j < 64; ++j) {
            for (size_t k = 0; k < 64; ++k) {
                CPPUNIT_ASSERT(node->get(i, j, k) == 1.0);
            }
        }
    }

    node->set(0, 0, 0, 5.0);

    CPPUNIT_ASSERT(node->get(0, 0, 0) == 5.0);

    // ensure all other values are still 1.0;
    for (size_t i = 1; i < 64; ++i) {
        for (size_t j = 0; j < 64; ++j) {
            for (size_t k = 0; k < 64; ++k) {
                CPPUNIT_ASSERT(node->get(i, j, k) == 1.0);
            }
        }
    }

    node->set(23, 54, 63, 10.0);
    CPPUNIT_ASSERT(node->get(23, 54, 63) == 10.0);
    delete node;
}

//-----------------------------------------------------------------------------

void 
TestNode::testOperatorComparisonSingleLevel()
{
    USING_NKHIVE_NS

    Node<Cell<float> > n(1, 2, 2, 1.0f);
    Node<Cell<float> > n1(1, 2, 2, 1.0f);

    n.set(0, 1, 1, 5.0f);
    n1.set(0, 1, 1, 5.0f);
    CPPUNIT_ASSERT(n == n1);
}

//-----------------------------------------------------------------------------

void
TestNode::testOperatorComparison()
{
    USING_NKHIVE_NS
    
    Node<Cell<float> > *node = new Node<Cell<float> >(2, 2, 2, 1.0f);

    Node<Cell<float> > *node2 = new Node<Cell<float> >(2, 3, 2, 1.0f);

    CPPUNIT_ASSERT(node != node2);

    delete node2;
    node2 = new Node<Cell<float> >(4, 2, 2, 1.0f);
    CPPUNIT_ASSERT(node->operator!=(*node2));

    delete node2;
    node2 = new Node<Cell<float> >(2, 2, 4, 1.0f);
    CPPUNIT_ASSERT(node->operator!=(*node2));

    delete node2;
    node2 = new Node<Cell<float> >(2, 2, 2, 1.1f);
    CPPUNIT_ASSERT(node->operator!=(*node2));

    delete node2;
    node2 = new Node<Cell<float> >(2, 2, 2, 1.0f);
    CPPUNIT_ASSERT(node->operator==(*node2));

    // check branches.
    node->set(0, 8, 63, 5.0f);
    node2->set(0, 0, 0, 5.0f);
    CPPUNIT_ASSERT(node->operator!=(*node2));

    node->set(0, 0, 0, 5.0f);
    node2->set(0, 8, 63, 5.0f);
    CPPUNIT_ASSERT(node->operator==(*node2));

    delete node;
}

//-----------------------------------------------------------------------------

void
TestNode::testIO()
{
    USING_NKHIVE_NS

    #define IO_TEST(co) {                                           \
        std::ostringstream ostr(std::ios_base::binary);             \
        co.write(ostr);                                             \
        std::istringstream istr(ostr.str(), std::ios_base::binary); \
        Node<Cell<float> > ci(3, 3, 3, 5.0f);                       \
        ci.set(0, 0, 0, 10.0f);                                     \
        ci.set(10, 0, 0, 6.0f);                                     \
        ci.read(istr);                                              \
        CPPUNIT_ASSERT(co == ci);                                   \
    } 

    Node<Cell<float> > n(2, 2, 2, 1.0f);
    IO_TEST(n);

    n.set(0, 0, 0, 10.0f);
    IO_TEST(n);

    n.set(10, 0, 0, 5.0f);
    IO_TEST(n);

    n.set(1, 2, 3, 4.0f);
    IO_TEST(n);
}

//------------------------------------------------------------------------------

void
TestNode::testFillNodeCreation()
{
    USING_NKHIVE_NS

    Node<Cell<float> > n(2, 2, 2, 2.0f, true);

    CPPUNIT_ASSERT(n.isFill());
    CPPUNIT_ASSERT(!n.isEmpty());
    CPPUNIT_ASSERT(!n.isBranching());
    CPPUNIT_ASSERT(n.m_branches.size() == 0);

    CPPUNIT_ASSERT(n.m_bitfield.isFull());


    // Test get.
    CPPUNIT_ASSERT(n.get(1, 2, 3) == 2.0f);
}

//------------------------------------------------------------------------------

void
TestNode::testFillNodeIO()
{
    USING_NKHIVE_NS

    Node<Cell<float> > n(2, 2, 2, 1.0f, true);

    std::ostringstream ostr(std::ios_base::binary);             
    n.write(ostr);                                             
    std::istringstream istr(ostr.str(), std::ios_base::binary); 
    Node<Cell<float> > ci(3, 3, 3, 5.0f);                       
    ci.set(0, 0, 0, 10.0f);                                     
    ci.set(10, 0, 0, 6.0f);                                     

    CPPUNIT_ASSERT(!ci.isFill());

    ci.read(istr);                                              

    CPPUNIT_ASSERT(ci.isFill());
    CPPUNIT_ASSERT(n == ci);                                   
}

//------------------------------------------------------------------------------

void
TestNode::testFillNodeIOHDF5()
{
    USING_NKHIVE_NS

    Node<Cell<float> > n(2, 2, 2, 1.0f, true);
    
    remove("testingHDF5.hv");                                   
    VolumeFile file("testingHDF5.hv", VoidFile::WRITE_TRUNC);                         
    HDF5Group root_group;                                 
    root_group.open(file.m_id, NK_NS::String("/"));            
    HDF5Id root_group_id = root_group.id();                            
    index_vec offset(1,2,3);
    n.write(root_group_id, 2, offset);          
    file.close();                                          
    Node<Cell<float> > ni(2, 2, 2, 5.0f);                 
    ni.set(0, 0, 0, 10.0f);                              
    ni.set(10, 0, 0, 6.0f);                             
    CPPUNIT_ASSERT(!ni.isFill());
    
    VolumeFile file2("testingHDF5.hv", VoidFile::READ_ONLY);                
    HDF5Group root_group2;                                 
    root_group2.open(file2.m_id, NK_NS::String("/"));            
    HDF5Id root_group_id2 = root_group2.id();                            
    HDF5Id leaf_group_id;
    GET_LEAF_ID(file2, root_group_id2, leaf_group_id)
    ni.read(leaf_group_id, offset);
    CPPUNIT_ASSERT(ni.isFill());
    CPPUNIT_ASSERT(ni == n);                                  
    file2.close();                                              
    remove("testingHDF5.hv");                                   
}

//------------------------------------------------------------------------------

void
TestNode::testFillSetSingleLevel()
{
    USING_NKHIVE_NS

    float fill_val = 2.0f;
    float val = 5.0f;

    Node<Cell<float> > node(1, 2, 2, fill_val, true);

    CPPUNIT_ASSERT(node.isFill());
    CPPUNIT_ASSERT(node.m_branches.size() == 0);
    CPPUNIT_ASSERT(!node.isEmpty());
    CPPUNIT_ASSERT(!node.isBranching());

    node.set(1, 2, 3, val);
    Cell<float> *cell = node.m_branches[0].cell;
    CPPUNIT_ASSERT(cell->get(1, 2, 3) == val);
    // All other cell components should have the fill value.
    for (size_t i = 0; i < cell->getDimension(); ++i) {
        for (size_t j = 0; j < cell->getDimension(); ++j) {
            for (size_t k = 0; k < cell->getDimension(); ++k) {
                if (i == 1 && j == 2 && k == 3) continue;
                CPPUNIT_ASSERT(cell->get(i, j, k) == fill_val);
            }
        }
    }

    // Check that all other cell's are fill cells.
    CPPUNIT_ASSERT(node.m_bitfield.isFull());
    for (size_t i = 1; i < node.m_branches.size(); ++i) {
        CPPUNIT_ASSERT(node.m_branches[i].cell != NULL);
        CPPUNIT_ASSERT(node.m_branches[i].cell->isFilled());
        CPPUNIT_ASSERT(node.m_branches[i].cell->getFillValue() == fill_val);
        CPPUNIT_ASSERT(node.m_branches[i].cell->m_bitfield.isFull());
    }
}

//------------------------------------------------------------------------------

void 
TestNode::testFillSetMultiLevel()
{
    USING_NKHIVE_NS

    float fill_val = 2.0f;
    float val = 5.0f;

    Cell<float> *cell;

    Node<Cell<float> > node(2, 2, 2, fill_val, true);
    CPPUNIT_ASSERT(node.isFill());
    CPPUNIT_ASSERT(node.m_branches.size() == 0);
    CPPUNIT_ASSERT(!node.isEmpty());
    CPPUNIT_ASSERT(!node.isBranching());

    node.set(0, 0, 0, val);
    cell = node.m_branches[0].node->m_branches[0].cell;
    CPPUNIT_ASSERT(cell->get(0, 0, 0) == val);
    CPPUNIT_ASSERT(cell->m_bitfield.isFull());
    // All other cell components should have the fill value.
    for (size_t i = 0; i < cell->getDimension(); ++i) {
        for (size_t j = 0; j < cell->getDimension(); ++j) {
            for (size_t k = 0; k < cell->getDimension(); ++k) {
                if (i == 0 && j == 0 && k == 0) continue;
                CPPUNIT_ASSERT(cell->get(i, j, k) == fill_val);
            }
        }
    }

    CPPUNIT_ASSERT(node.m_bitfield.isFull());

    // make sure all other nodes are fill nodes.
    for (size_t i = 1; i < node.m_branches.size(); ++i) {
        CPPUNIT_ASSERT(node.m_branches[i].node != NULL);
        CPPUNIT_ASSERT(node.m_branches[i].node->isFill());
        CPPUNIT_ASSERT(node.m_branches[i].node->m_branches.size() == 0);
        CPPUNIT_ASSERT(node.m_branches[i].node->fillValue() == fill_val);
        CPPUNIT_ASSERT(node.m_branches[i].node->m_bitfield.isFull());
    }

    node.set(16, 1, 2, val);
    cell = node.m_branches[1].node->m_branches[0].cell;
    CPPUNIT_ASSERT(cell->get(0, 1, 2) == val);
    CPPUNIT_ASSERT(cell->m_bitfield.isFull());
    CPPUNIT_ASSERT(node.m_bitfield.isFull());

    // make sure all other nodes are fill nodes 
    for (size_t i = 2; i < node.m_branches.size(); ++i) {
        CPPUNIT_ASSERT(node.m_branches[i].node != NULL);
        CPPUNIT_ASSERT(node.m_branches[i].node->isFill());
        CPPUNIT_ASSERT(node.m_branches[i].node->m_branches.size() == 0);
        CPPUNIT_ASSERT(node.m_branches[i].node->fillValue() == fill_val);
        CPPUNIT_ASSERT(node.m_branches[i].node->m_bitfield.isFull());
    }

    node.set(16, 16, 16, 5.0f);
    cell = node.m_branches[21].node->m_branches[0].cell;
    CPPUNIT_ASSERT(cell->get(0, 0, 0) == val);
    CPPUNIT_ASSERT(cell->m_bitfield.isFull());
    CPPUNIT_ASSERT(node.m_bitfield.isFull());

    // make sure all other nodes are fill nodes 
    for (size_t i = 0; i < node.m_branches.size(); ++i) {
        if (i == 0 || i == 1 || i == 21) continue;
        CPPUNIT_ASSERT(node.m_branches[i].node != NULL);
        CPPUNIT_ASSERT(node.m_branches[i].node->isFill());
        CPPUNIT_ASSERT(node.m_branches[i].node->m_branches.size() == 0);
        CPPUNIT_ASSERT(node.m_branches[i].node->fillValue() == fill_val);
        CPPUNIT_ASSERT(node.m_branches[i].node->m_bitfield.isFull());
    }
}

//------------------------------------------------------------------------------

void 
TestNode::testUnsetSingleLevel()
{
    USING_NKHIVE_NS

    float default_val = 1.0f;
    float val = 5.0f;

    Node<Cell<float> > node(1, 2, 2, default_val);
    node.set(0, 0, 0, val);
    node.set(1, 2, 3, val);
    node.set(12, 15, 13, val);

    // do the unset
    node.unset(1, 2, 3, default_val);

    CPPUNIT_ASSERT(node.m_branches[0].cell->get(1, 2, 3) == default_val);
    CPPUNIT_ASSERT(!node.m_branches[0].cell->m_bitfield.isSet(1, 2, 3));
    CPPUNIT_ASSERT(node.m_branches[0].cell->m_bitfield.isSet(0, 0, 0));

    node.unset(12, 15, 13, default_val);

    // Make sure the cell was removed. 
    CPPUNIT_ASSERT(node.m_branches[63].cell == NULL);
    CPPUNIT_ASSERT(!node.m_bitfield.isSet(63));

    // check that all other cell branches are null
    for (size_t i = 1; i < node.m_branches.size(); ++i) {
        CPPUNIT_ASSERT(node.m_branches[i].cell == NULL);
    }
}

//------------------------------------------------------------------------------

void 
TestNode::testUnsetMultiLevel()
{
    USING_NKHIVE_NS

    float val = 5.0f;
    float default_val = 1.0f;

    Cell<float> *cell;

    Node<Cell<float> > node(2, 2, 2, default_val);
    node.set(0, 0, 0, val);
    node.set(1, 1, 1, val);
    node.set(4, 0, 0, val);

    // do unset operations.
    node.unset(0, 0, 0, default_val);

    cell = node.m_branches[0].node->m_branches[0].cell;
    CPPUNIT_ASSERT(!cell->m_bitfield.isSet(0, 0, 0));
    CPPUNIT_ASSERT(cell->get(0, 0, 0) == default_val);
    CPPUNIT_ASSERT(cell->get(1, 1, 1) == val);

    // another unset that removes the cell, but not the node.
    node.unset(4, 0, 0, default_val);

    CPPUNIT_ASSERT(node.get(4, 0, 0) == default_val);
    CPPUNIT_ASSERT(node.m_branches[0].node != NULL);
    CPPUNIT_ASSERT(!node.m_branches[0].node->m_bitfield.isSet(1));
    CPPUNIT_ASSERT(node.m_branches[0].node->m_branches[1].cell == NULL);

    // another unset that removes the node and the cell
    node.unset(1, 1, 1, default_val);
    CPPUNIT_ASSERT(node.m_bitfield.isEmpty());
    CPPUNIT_ASSERT(node.m_bitfield.isEmpty());
    CPPUNIT_ASSERT(node.defaultValue() == default_val);
    CPPUNIT_ASSERT(node.m_branches.size() == numBits3D(node.m_bitfield.size()));

    // make sure all nodes are null
    for (size_t i = 0; i < node.m_branches.size(); ++i) {
        CPPUNIT_ASSERT(node.m_branches[i].node == NULL);
    }
}

//------------------------------------------------------------------------------

void 
TestNode::testFillUnsetSingleLevel()
{
    USING_NKHIVE_NS

    float default_val = 1.0f;
    float fill_val = 2.0f;

    Cell<float> *cell;
    Node<Cell<float> > node(1, 2, 2, fill_val, true);

    CPPUNIT_ASSERT(node.m_bitfield.isFull());
    CPPUNIT_ASSERT(node.m_branches.size() == 0);

    // do the unset
    node.unset(1, 2, 3, default_val);

    CPPUNIT_ASSERT(node.m_bitfield.isFull());
    cell = node.m_branches[0].cell;
    CPPUNIT_ASSERT(cell != NULL);
    CPPUNIT_ASSERT(!cell->m_bitfield.isFull());
    CPPUNIT_ASSERT(cell->get(1, 2, 3) == default_val);
    CPPUNIT_ASSERT(cell->isFilled());

    // make sure all other values are the fill value
    for (size_t i = 0; i < 4; ++i) {
        for (size_t j = 0; j < 4; ++j) {
            for (size_t k = 0; k < 4; ++k) {
                if (i == 1 && j == 2 && k == 3) continue;
                CPPUNIT_ASSERT(cell->get(i, j, k) == fill_val);
            }
        }
    }

    // maek sure all other branches are filled cells.
    for (size_t i = 1; i < numBits3D(node.m_bitfield.size()); ++i) {
        CPPUNIT_ASSERT(node.m_branches[i].cell->isFilled());
        CPPUNIT_ASSERT(node.m_branches[i].cell->m_bitfield.isFull());
    }

    node.unset(12, 15, 13, default_val);

    // Make sure the cell was converted to a allocated cell. 
    cell = node.m_branches[63].cell;
    CPPUNIT_ASSERT(cell->isFilled());
    CPPUNIT_ASSERT(node.m_bitfield.isFull());
    CPPUNIT_ASSERT(cell->get(0, 3, 1) == default_val);

    // make sure all other values are the fill value
    for (size_t i = 0; i < 4; ++i) {
        for (size_t j = 0; j < 4; ++j) {
            for (size_t k = 0; k < 4; ++k) {
                if (i == 0 && j == 3 && k == 1) continue;
                CPPUNIT_ASSERT(cell->get(i, j, k) == fill_val);
            }
        }
    }
}

//------------------------------------------------------------------------------

void 
TestNode::testFillUnsetMultiLevel()
{
    USING_NKHIVE_NS

    float fill_val = 5.0f;
    float default_val = 1.0f;

    Cell<float> *cell;
    Node<Cell<float> > *node2;

    Node<Cell<float> > node(2, 2, 2, fill_val, true);

    CPPUNIT_ASSERT(node.isFill());
    CPPUNIT_ASSERT(node.m_branches.size() == 0);

    // do unset operations.
    node.unset(0, 0, 0, default_val);

    CPPUNIT_ASSERT(!node.isFill());
    CPPUNIT_ASSERT(node.m_bitfield.isFull());
    CPPUNIT_ASSERT(node.m_branches.size() == numBits3D(2));

    // make sure that all subsequent nodes are created and are fill nodes.
    for (size_t i = 1; i < node.m_branches.size(); ++i) {
        CPPUNIT_ASSERT(node.m_branches[i].node != NULL);
        CPPUNIT_ASSERT(node.m_branches[i].node->isFill());
        CPPUNIT_ASSERT(node.m_branches[i].node->fillValue() == fill_val);
    }

    // The node that has the cell.
    node2 = node.m_branches[0].node;
    CPPUNIT_ASSERT(!node2->isFill());
    CPPUNIT_ASSERT(node2->m_bitfield.isFull());

    cell = node2->m_branches[0].cell;

    // make sure all other cell branches are filled cells.
    CPPUNIT_ASSERT(node2->m_branches.size() == numBits3D(2));
    for (size_t i = 1; i < node2->m_branches.size(); ++i) {
        CPPUNIT_ASSERT(node2->m_branches[i].cell->isFilled());
        CPPUNIT_ASSERT(node2->m_branches[i].cell->getFillValue() == fill_val);
    }

    CPPUNIT_ASSERT(cell->get(0, 0, 0) == default_val);
    CPPUNIT_ASSERT(!cell->m_bitfield.isSet(0, 0, 0));

    // check that all other values are fill values.
    for (size_t i = 0; i < 4; ++i) {
        for (size_t j = 0; j < 4; ++j) {
            for (size_t k = 0; k < 4; ++k) {
                if (i == 0 && j == 0 && k == 0) continue;
                CPPUNIT_ASSERT(cell->get(i ,j, k) == fill_val);
            }
        }
    }
}

//------------------------------------------------------------------------------

void
TestNode::testComputeSetBounds()
{
    USING_NKHIVE_NS

    index_bounds bounds;

    {
        Node<Cell<float> > node(1, 2, 2, 1.0f);
        node.set(3, 4, 5, 1.0);

        node.computeSetBounds(bounds);
        CPPUNIT_ASSERT(bounds.min() == index_bounds::vector_type(3, 4, 5));
        CPPUNIT_ASSERT(bounds.max() == index_bounds::vector_type(4, 5, 6));

        node.set(10, 12, 13, 1.0);

        node.computeSetBounds(bounds);
        CPPUNIT_ASSERT(bounds.min() == index_bounds::vector_type(3, 4, 5));
        CPPUNIT_ASSERT(bounds.max() == index_bounds::vector_type(11, 13, 14));
    }

    {
        Node<Cell<float> > node(1, 2, 2, 1.0f);
        node.set(0, 0, 0, 1.0);

        node.computeSetBounds(bounds);
        CPPUNIT_ASSERT(bounds.min() == index_bounds::vector_type(0, 0, 0));
        CPPUNIT_ASSERT(bounds.max() == index_bounds::vector_type(1, 1, 1));

        node.set(15, 15, 15, 1.0);

        node.computeSetBounds(bounds);
        CPPUNIT_ASSERT(bounds.min() == index_bounds::vector_type(0, 0, 0));
        CPPUNIT_ASSERT(bounds.max() == index_bounds::vector_type(16, 16, 16));
    }

    // Multi-level
    {
        Node<Cell<float> > node(2, 2, 2, 1.0f);
        node.set(16, 17, 18, 1.0);

        node.computeSetBounds(bounds);
        CPPUNIT_ASSERT(bounds.min() == index_bounds::vector_type(16, 17, 18));
        CPPUNIT_ASSERT(bounds.max() == index_bounds::vector_type(17, 18, 19));

        node.set(32, 28, 15, 1.0);

        node.computeSetBounds(bounds);
        CPPUNIT_ASSERT(bounds.min() == index_bounds::vector_type(16, 17, 15));
        CPPUNIT_ASSERT(bounds.max() == index_bounds::vector_type(33, 29, 19));
    }

    // test for a filled node.
    {
        Node<Cell<float> > node(2, 2, 2, 1.0f, true);

        node.computeSetBounds(bounds);
        CPPUNIT_ASSERT(bounds.min() == index_bounds::vector_type(0, 0, 0));
        CPPUNIT_ASSERT(bounds.max() == index_bounds::vector_type(64, 64, 64));
    }
}

//-----------------------------------------------------------------------------

void
TestNode::testWriteStamp() 
{
    USING_NKHIVE_NS

    typedef float                            T;
    typedef Stamp<T, One>                    StampOne;
    typedef Stamp<T, CoordinateMapper>       StampTester;
    typedef Node<Cell<T> >                   Node;
    typedef index_bounds::vector_type        vector;
    typedef signed_index_bounds::vector_type signed_vector;

    const T default_val(0);    
    const T set_val(1);

    // stamp single value
    {
        const vector zero(0, 0, 0);
        const vector unit(1, 1, 1);

        StampOne stamp;
        index_bounds node_bounds(zero, unit);

        Node::shared_ptr node(new Node(1, 4, 3, default_val));
        node->stamp(stamp, node_bounds, node_bounds, unit);
        CPPUNIT_ASSERT(node->get(0, 0, 0) == set_val);    
        for (index_type k = -1; k <= 1; ++k) {
            for (index_type j = -1; j <= 1; ++j) {
                for (index_type i = -1; i <= 1; ++i) {
                    if (!(i + j + k)) continue;
                    CPPUNIT_ASSERT(node->get(i, j, k) == default_val);    
                }
            }
        }
    }

    #define TEST_STAMP_BOUNDS(tx, ty, tz)  {                                \
        const vector min( 1,  1,  1);                                       \
        const vector max(20, 20, 20);                                       \
        const signed_vector t(tx, ty, tz);                                  \
                                                                            \
        StampTester stamp;                                                  \
        stamp.bounds().setExtrema(min * t, max * t);                        \
        index_bounds node_bounds(min, max);                                 \
                                                                            \
        Node::shared_ptr node(new Node(1, 2, 3, default_val));              \
        node->stamp(stamp, node_bounds, node_bounds, t);                    \
                                                                            \
        for (index_type k = 0; k <= max.z; ++k) {                           \
            for (index_type j = 0; j <= max.y; ++j) {                       \
                for (index_type i = 0; i <= max.x; ++i) {                   \
                    if (!node_bounds.inRange(index_vec(i, j, k))) {         \
                        CPPUNIT_ASSERT(node->get(i, j, k) == default_val);  \
                    } else {                                                \
                        signed_index_type si, sj, sk;                       \
                        si = (t.x < 0 ? i + 1 : i) * t.x;                   \
                        sj = (t.y < 0 ? j + 1 : j) * t.y;                   \
                        sk = (t.z < 0 ? k + 1 : k) * t.z;                   \
                        CPPUNIT_ASSERT(node->get(i, j, k) ==                \
                            CoordinateMapper<T>::get(si, sj, sk));          \
                    }                                                       \
                }                                                           \
            }                                                               \
        }                                                                   \
    }

    // stamp bounds, test all quadrants

    TEST_STAMP_BOUNDS( 1,  1,  1);
    TEST_STAMP_BOUNDS( 1,  1, -1);
    TEST_STAMP_BOUNDS( 1, -1,  1);
    TEST_STAMP_BOUNDS( 1, -1, -1);
    TEST_STAMP_BOUNDS(-1,  1,  1);
    TEST_STAMP_BOUNDS(-1,  1, -1);
    TEST_STAMP_BOUNDS(-1, -1,  1);
    TEST_STAMP_BOUNDS(-1, -1, -1);
}

