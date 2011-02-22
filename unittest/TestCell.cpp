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
// TestCell.cpp
//------------------------------------------------------------------------------

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <iterator>

#include <nkbase/BinaryOps.h>

#include <nkhive/volume/Cell.h>
#include <nkhive/Defs.h>
#include <nkhive/io/VolumeFile.h>

//-----------------------------------------------------------------------------
// definitions
//-----------------------------------------------------------------------------

#define BLOCK_2_TEST(a, b, v, c) {      \
    CPPUNIT_ASSERT(c->get(a,a,a) == v); \
    CPPUNIT_ASSERT(c->get(a,a,b) == v); \
    CPPUNIT_ASSERT(c->get(a,b,a) == v); \
    CPPUNIT_ASSERT(c->get(a,b,b) == v); \
    CPPUNIT_ASSERT(c->get(b,a,a) == v); \
    CPPUNIT_ASSERT(c->get(b,a,b) == v); \
    CPPUNIT_ASSERT(c->get(b,b,a) == v); \
    CPPUNIT_ASSERT(c->get(b,b,b) == v); \
}

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

/**
 * Get the cell group id from a volume file for reading test data under
 * a node
 */
#define GET_CELL_ID(vf, root_id, cell_id)                   \
    i32 size = H5Lget_name_by_idx(root_id, ".",             \
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
    NK_NS::String cell_name(buf);                           \
    HDF5Group cell_group;                                   \
    cell_group.open(root_id, cell_name);                    \
    cell_id = cell_group.id();                              \
   
//-----------------------------------------------------------------------------
// class definition
//-----------------------------------------------------------------------------

template<typename T>
class TestCell : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TestCell);
    CPPUNIT_TEST(testInitialization);
    CPPUNIT_TEST(testCopySingleSetValue);
    CPPUNIT_TEST(testCopyMultipleSetValue);
    CPPUNIT_TEST(testGetUnset);
    CPPUNIT_TEST(testSetSingleValue);
    CPPUNIT_TEST(testSetMultipleValue);
    CPPUNIT_TEST(testUpdate);
    CPPUNIT_TEST(testUpdateFillValue);
    CPPUNIT_TEST(testUpdateDefaultValue);
    CPPUNIT_TEST(testGet);
    CPPUNIT_TEST(testUnset);
    CPPUNIT_TEST(testUnsetNoAllocation);
    CPPUNIT_TEST(testIsSet);
    CPPUNIT_TEST(testClear);
    CPPUNIT_TEST(testClearFilledCell);
    CPPUNIT_TEST(testCompress);
    CPPUNIT_TEST(testCompressFilledCell);
    CPPUNIT_TEST(testCompressedCopy);
    CPPUNIT_TEST(testCompressCopyFilled);
    CPPUNIT_TEST(testFill);
    CPPUNIT_TEST(testFillSetUnset);
    CPPUNIT_TEST(testFilledSet);
    CPPUNIT_TEST(testIteration);
    CPPUNIT_TEST(testConstIterationFilled);
    CPPUNIT_TEST(testIsEmpty);
    CPPUNIT_TEST(testIO);
    CPPUNIT_TEST(testIOHDF5);
    CPPUNIT_TEST(testOperatorComparison);
    CPPUNIT_TEST(testCreateFilledCell);
    CPPUNIT_TEST(testUnsetBlock);
    CPPUNIT_TEST(testSetWindow);
    CPPUNIT_TEST(testUnsetBlockFilledCell);
    CPPUNIT_TEST(testSetBlockFilledCell);
    CPPUNIT_TEST(testComputeSetBounds);
    CPPUNIT_TEST(testWriteStamp);
    CPPUNIT_TEST_SUITE_END();

public:

    void setUp();
    void tearDown();
    
    void testInitialization();
    void testCopySingleSetValue();
    void testCopyMultipleSetValue();
    void testGetUnset();
    void testSetSingleValue();
    void testSetMultipleValue();
    void testUpdate();
    void testUpdateFillValue();
    void testUpdateDefaultValue();
    void testGet();
    void testUnset();
    void testUnsetNoAllocation();
    void testIsSet();
    void testClear();
    void testClearFilledCell();
    void testCompress();
    void testCompressFilledCell();
    void testCompressedCopy();
    void testCompressCopyFilled();
    void testFill();
    void testFillSetUnset();
    void testFilledSet();
    void testIteration();
    void testConstIterationFilled();
    void testIsEmpty();
    void testIO();
    void testIOHDF5();
    void testOperatorComparison();
    void testCreateFilledCell();
    void testUnsetBlock();
    void testSetWindow();
    void testUnsetBlockFilledCell();
    void testSetBlockFilledCell();
    void testComputeSetBounds();
    void testWriteStamp();
};

//-----------------------------------------------------------------------------
// test suite registration
//-----------------------------------------------------------------------------

CPPUNIT_TEST_SUITE_REGISTRATION(TestCell<int32_t>);
CPPUNIT_TEST_SUITE_REGISTRATION(TestCell<int64_t>);
CPPUNIT_TEST_SUITE_REGISTRATION(TestCell<uint32_t>);
CPPUNIT_TEST_SUITE_REGISTRATION(TestCell<uint64_t>);
CPPUNIT_TEST_SUITE_REGISTRATION(TestCell<float>);
CPPUNIT_TEST_SUITE_REGISTRATION(TestCell<double>);
CPPUNIT_TEST_SUITE_REGISTRATION(TestCell<half>);

//------------------------------------------------------------------------------

/**
 *  Writes out cell data to a string.
 */
template <typename T>
std::string
toString(const NKHIVE_NS::Cell<T>& cell) 
{
    using namespace std;

    string str = "";
    copy(cell.begin(), cell.end(), inserter(str, str.begin()));
    transform(str.begin(), str.end(), str.begin(), bind2nd(plus<char>(), 48));
    return str;
}

//------------------------------------------------------------------------------

template<typename T>
void
TestCell<T>::setUp()
{
}

//------------------------------------------------------------------------------

template<typename T>
void
TestCell<T>::tearDown()
{
}

//------------------------------------------------------------------------------

template<typename T>
void
TestCell<T>::testInitialization()
{
    USING_NKHIVE_NS

    typename Cell<T>::shared_ptr cell(new Cell<T>(1, T(0)));        
    
    // check dimensions
    CPPUNIT_ASSERT(cell->getDimension() == 2);

    // check allocation size
    CPPUNIT_ASSERT(cell->m_data_size == 0); 
    CPPUNIT_ASSERT(cell->m_data == NULL);
    CPPUNIT_ASSERT(cell->isFilled());

    // check memory contents
    CPPUNIT_ASSERT(toString(*cell) == "00000000");
}

//-----------------------------------------------------------------------------

template<typename T>
void
TestCell<T>::testCopySingleSetValue()
{
    USING_NKHIVE_NS

    T default_val(0);    
    T set_val(1);

    typename Cell<T>::shared_ptr cell(new Cell<T>(1, default_val));

    // assign a value
    cell->set(0, 0, 1, set_val);

    // check contents
    CPPUNIT_ASSERT(toString(*cell) == "00001000");

    // create a copy
    typename Cell<T>::shared_ptr cell_copy(new Cell<T>(*cell));

    // check contents of copy
    CPPUNIT_ASSERT(toString(*cell_copy) == "00001000");

    // test set place
    CPPUNIT_ASSERT(cell->get(0, 0, 1) == cell_copy->get(0, 0, 1));    

    // test unset place
    CPPUNIT_ASSERT(cell->get(0, 0, 0) == cell_copy->get(0, 0, 0));

    // test allocation size of copy
    CPPUNIT_ASSERT(cell_copy->m_data_size == 0);
    CPPUNIT_ASSERT(cell_copy->m_data == NULL);
}

//------------------------------------------------------------------------------

template<typename T>
void
TestCell<T>::testCopyMultipleSetValue()
{
    USING_NKHIVE_NS

    T default_val(0);    
    T set_val(1);

    typename Cell<T>::shared_ptr cell(new Cell<T>(1, default_val));

    // assign a value
    cell->set(0, 0, 1, set_val);
    cell->set(1, 1, 1, set_val + set_val);

    // check contents
    CPPUNIT_ASSERT(toString(*cell) == "00001002");

    // create a copy
    typename Cell<T>::shared_ptr cell_copy(new Cell<T>(*cell));

    // check contents of copy
    CPPUNIT_ASSERT(toString(*cell_copy) == "00001002");

    // test set place
    CPPUNIT_ASSERT(cell->get(0, 0, 1) == cell_copy->get(0, 0, 1));    
    CPPUNIT_ASSERT(cell->get(1, 1, 1) == cell_copy->get(1, 1, 1));    

    // test unset place
    CPPUNIT_ASSERT(cell->get(0, 0, 0) == cell_copy->get(0, 0, 0));

    // test allocation size of copy
    CPPUNIT_ASSERT(cell_copy->m_data_size == 2 * 2 * 2);
}

//-----------------------------------------------------------------------------

template<typename T>
void
TestCell<T>::testGetUnset()
{
    USING_NKHIVE_NS

    T default_val(0);
    typename Cell<T>::shared_ptr cell(new Cell<T>(1, default_val));        
    
    // get unset value
    T val = cell->get(0, 0, 0);
    CPPUNIT_ASSERT(default_val == val);
    
    // get unset value
    val = cell->get(1, 1, 1);
    CPPUNIT_ASSERT(default_val == val);
}

//-----------------------------------------------------------------------------

template<typename T>
void
TestCell<T>::testSetSingleValue()
{
    USING_NKHIVE_NS

    T default_val(0);
    T set_val(1);
    typename Cell<T>::shared_ptr cell(new Cell<T>(1, T(0)));        
    
    // set value
    cell->set(0, 0, 0,set_val);

    // check contents
    CPPUNIT_ASSERT(toString(*cell) == "10000000");
    
    // check set value
    CPPUNIT_ASSERT(cell->get(0, 0, 0) == set_val);
    
    // check bounding unset value
    CPPUNIT_ASSERT(cell->get(0, 0, 1) == default_val);

    // set another value
    cell->set(0, 1, 0,set_val);

    // check contents again
    CPPUNIT_ASSERT(toString(*cell) == "10100000");

    // no allocations should have been done
    CPPUNIT_ASSERT(cell->m_data_size == 0);
    CPPUNIT_ASSERT(cell->m_data == NULL);
}

//------------------------------------------------------------------------------

template<typename T>
void
TestCell<T>::testSetMultipleValue()
{
    USING_NKHIVE_NS

    T default_val(0);
    T set_val(1);
    typename Cell<T>::shared_ptr cell(new Cell<T>(1, T(0)));        
    
    // set value
    cell->set(0, 0, 0,set_val);

    // check contents
    CPPUNIT_ASSERT(toString(*cell) == "10000000");
    
    // check set value
    CPPUNIT_ASSERT(cell->get(0, 0, 0) == set_val);
    
    // check bounding unset value
    CPPUNIT_ASSERT(cell->get(0, 0, 1) == default_val);

    // set another value
    cell->set(0, 1, 0, set_val + set_val);

    // check contents again
    CPPUNIT_ASSERT(toString(*cell) == "10200000");

    CPPUNIT_ASSERT(cell->m_data_size == 2 * 2 * 2);
    CPPUNIT_ASSERT(cell->m_data != NULL);

    // check values.
    CPPUNIT_ASSERT(cell->get(0, 0, 0) == set_val);
    CPPUNIT_ASSERT(cell->get(0, 1, 0) == set_val + set_val);
}

//------------------------------------------------------------------------------

template<typename T>
void
TestCell<T>::testUpdate()
{
    USING_NKHIVE_NS

    T default_val(0);
    T set_val(1);
    typename Cell<T>::shared_ptr cell(new Cell<T>(1, default_val));        
    
    // set value
    cell->set(0, 0, 0, set_val);
    cell->set(0, 0, 1, set_val);

    CPPUNIT_ASSERT(cell->m_data_size == 0);
    CPPUNIT_ASSERT(cell->m_data == NULL);

    // check contents
    CPPUNIT_ASSERT(toString(*cell) == "10001000");
    
    // check set value
    CPPUNIT_ASSERT(cell->get(0, 0, 0) == set_val);

    cell->update(0, 0, 0, set_val, std::plus<T>());

    CPPUNIT_ASSERT(cell->m_data_size == 2 * 2 * 2);
    CPPUNIT_ASSERT(cell->m_data != NULL);

    // check set value
    CPPUNIT_ASSERT(cell->get(0, 0, 0) == set_val + set_val);
}

//------------------------------------------------------------------------------

template<typename T>
void
TestCell<T>::testUpdateFillValue()
{
    USING_NK_NS
    USING_NKHIVE_NS

    T default_val(0);
    T fill_val(1);
    typename Cell<T>::shared_ptr cell(new Cell<T>(1, default_val));        

    cell->fill(fill_val);

    CPPUNIT_ASSERT(cell->m_data_size == 0);
    CPPUNIT_ASSERT(cell->m_data == NULL);
    CPPUNIT_ASSERT(cell->isFilled() == true);

    // should not change the state of the cell.
    cell->update(1, 1, 1, fill_val, set_op<T>());

    CPPUNIT_ASSERT(cell->m_data_size == 0);
    CPPUNIT_ASSERT(cell->m_data == NULL);
    CPPUNIT_ASSERT(cell->isFilled() == true);

    // should update the state of the cell.
    cell->update(1, 1, 1, fill_val, std::plus<T>());

    CPPUNIT_ASSERT(cell->m_data_size == 8);
    CPPUNIT_ASSERT(cell->m_data != NULL);
    CPPUNIT_ASSERT(!cell->isFilled());
    CPPUNIT_ASSERT(cell->get(1, 1, 1) == fill_val + fill_val);
}

//------------------------------------------------------------------------------

template<typename T>
void
TestCell<T>::testUpdateDefaultValue()
{
    USING_NK_NS
    USING_NKHIVE_NS

    T default_val(1);
    T set_val(2);

    {
        typename Cell<T>::shared_ptr cell(new Cell<T>(1, default_val));        

        CPPUNIT_ASSERT(cell->isEmpty());

        // should set the value to set_val
        cell->update(1, 1, 1, set_val, set_op<T>());

        CPPUNIT_ASSERT(!cell->isEmpty());
        CPPUNIT_ASSERT(cell->get(1, 1, 1) == set_val);
    }

    {
        typename Cell<T>::shared_ptr cell(new Cell<T>(1, default_val));        

        CPPUNIT_ASSERT(cell->isEmpty());

        // should set the value to set_val + default_val
        cell->update(1, 1, 1, set_val, std::plus<T>());

        CPPUNIT_ASSERT(!cell->isEmpty());
        CPPUNIT_ASSERT(cell->isFilled());
        CPPUNIT_ASSERT(cell->m_data == 0);
        CPPUNIT_ASSERT(cell->get(1, 1, 1) == set_val + default_val);
    }

    {
        typename Cell<T>::shared_ptr cell(new Cell<T>(1, default_val));        

        CPPUNIT_ASSERT(cell->isEmpty());

        // should set the value to set_val + default_val
        cell->update(1, 1, 1, set_val, std::plus<T>());

        CPPUNIT_ASSERT(!cell->isEmpty());
        CPPUNIT_ASSERT(cell->get(1, 1, 1) == set_val + default_val);

        cell->update(1, 0, 1, set_val, std::plus<T>());
        CPPUNIT_ASSERT(cell->get(1, 0, 1) == set_val + default_val);
        CPPUNIT_ASSERT(cell->isFilled());
        CPPUNIT_ASSERT(cell->m_data == 0);

        cell->update(1, 0, 0, default_val, std::plus<T>());
        CPPUNIT_ASSERT(cell->get(1, 0, 0) == default_val + default_val);
        CPPUNIT_ASSERT(!cell->isFilled());
    }
}

//-----------------------------------------------------------------------------

template<typename T>
void
TestCell<T>::testGet()
{
    USING_NKHIVE_NS

    T set_val(1);
    T default_val(0);
    typename Cell<T>::shared_ptr cell(new Cell<T>(1,default_val));        
    
    // set value
    cell->set(0, 1, 0,set_val);

    // get value
    T& get_val = cell->get(0, 1, 0);

    // check get value
    CPPUNIT_ASSERT(get_val == set_val);

    // get const reference
    const T& get_const_val = cell->get(0, 1, 0);

    // check get value
    CPPUNIT_ASSERT(get_const_val == set_val); 
}

//-----------------------------------------------------------------------------

template<typename T>
void
TestCell<T>::testUnset()
{
    USING_NKHIVE_NS

    T set_val(1);
    T default_val(0);
    typename Cell<T>::shared_ptr cell(new Cell<T>(1,default_val));        
    
    // set value
    cell->set(0, 0, 0, set_val);
    cell->set(0, 0, 1, set_val + set_val);

    CPPUNIT_ASSERT(!cell->isFilled());

    // check contents
    CPPUNIT_ASSERT(toString(*cell) == "10002000");

    // unset value
    cell->unset(0, 0, 0);

    CPPUNIT_ASSERT(!cell->isFilled());

    // check contents
    CPPUNIT_ASSERT(toString(*cell) == "00002000");

    // make sure we get the default value.
    CPPUNIT_ASSERT(cell->get(0, 0, 0) == default_val);
}

//------------------------------------------------------------------------------

template <typename T>
void
TestCell<T>::testUnsetNoAllocation()
{
    USING_NKHIVE_NS

    T set_val(1);
    T default_val(0);
    typename Cell<T>::shared_ptr cell(new Cell<T>(1,default_val));        
    
    // set value
    cell->set(0, 0, 0, set_val);
    cell->set(1, 1, 1, set_val);

    CPPUNIT_ASSERT(cell->isFilled());
    CPPUNIT_ASSERT(cell->m_data == NULL);

    // unset value
    cell->unset(0, 0, 0);

    CPPUNIT_ASSERT(cell->isFilled());
    CPPUNIT_ASSERT(cell->m_data == NULL);

    cell->unset(1, 1, 1);

    CPPUNIT_ASSERT(cell->isFilled());
    CPPUNIT_ASSERT(cell->m_data == NULL);

    // Make sure we still get the default value.
    CPPUNIT_ASSERT(cell->get(0, 0, 0) == default_val);
    CPPUNIT_ASSERT(cell->get(1, 1, 1) == default_val);
}

//-----------------------------------------------------------------------------

template<typename T>
void
TestCell<T>::testIsSet()
{
    USING_NKHIVE_NS

    T set_val(1);
    typename Cell<T>::shared_ptr cell(new Cell<T>(1,T(0)));        
    
    // set value
    cell->set(0, 0, 0, set_val);

    // check some values that should be set
    CPPUNIT_ASSERT(cell->isSet(0, 0, 0) == true);
    
    // check for values that should not be set
    CPPUNIT_ASSERT(cell->isSet(0, 1, 1) == false);
}

//-----------------------------------------------------------------------------

template<typename T>
void
TestCell<T>::testClear()
{
    USING_NKHIVE_NS
    
    T set_val(1);
    T default_val(0);

    typename Cell<T>::shared_ptr cell(new Cell<T>(1, default_val));        

    // set some different values to make it not a filled cell.
    cell->set(0, 0, 1, set_val);
    cell->set(1, 1, 0, set_val + set_val);

    CPPUNIT_ASSERT(toString(*cell) == "00021000");

    CPPUNIT_ASSERT(!cell->isFilled());
    CPPUNIT_ASSERT(cell->m_data != NULL);

    // After this it should be a pristine fill state.
    cell->clear();

    CPPUNIT_ASSERT(toString(*cell) == "00000000");
    CPPUNIT_ASSERT(cell->isFilled());
    
    // set value
    cell->set(0, 0, 1, set_val);

    // set another value
    cell->set(1, 1, 0, set_val);

    // check contents
    CPPUNIT_ASSERT(toString(*cell) == "00011000");

    CPPUNIT_ASSERT(cell->isFilled());
    CPPUNIT_ASSERT(cell->m_data == NULL);

    // clear
    cell->clear();
    
    // check contents
    CPPUNIT_ASSERT(toString(*cell) == "00000000");

    // test previously filled in values
    CPPUNIT_ASSERT(cell->get(0, 0, 1) == default_val);
    CPPUNIT_ASSERT(cell->get(1, 1, 0) == default_val);

    // fill
    T fill_val(2);
    cell->fill(fill_val);

    // check contents. Nothing alloc'd
    CPPUNIT_ASSERT(cell->m_bitfield.isFull());
    CPPUNIT_ASSERT(cell->m_data == NULL);

    // check data size
    CPPUNIT_ASSERT(cell->m_data_size == 0);

    // clear after fill
    cell->clear();
    
    // check contents
    CPPUNIT_ASSERT(toString(*cell) == "00000000");

    // check state
    CPPUNIT_ASSERT(cell->isFilled());

    // check data size
    CPPUNIT_ASSERT(cell->m_data_size == 0);

    CPPUNIT_ASSERT(cell->get(0, 0, 1) == default_val);
}

template<typename T>
void
TestCell<T>::testClearFilledCell()
{
    USING_NKHIVE_NS
    
    T default_val(0);
    T fill_val(2);

    typename Cell<T>::shared_ptr cell(new Cell<T>(1, default_val, fill_val));

    // check contents. Nothing alloc'd
    CPPUNIT_ASSERT(cell->m_bitfield.isFull());
    CPPUNIT_ASSERT(cell->m_data == NULL);

    CPPUNIT_ASSERT(cell->get(0, 0, 1) == fill_val);

    // check data size
    CPPUNIT_ASSERT(cell->m_data_size == 0);

    // clear after fill
    cell->clear();
    
    // check contents
    CPPUNIT_ASSERT(toString(*cell) == "00000000");

    // check state
    CPPUNIT_ASSERT(cell->isFilled());

    // check data size
    CPPUNIT_ASSERT(cell->m_data_size == 0);

    // check the default value.
    CPPUNIT_ASSERT(cell->get(0, 0, 1) == default_val);
}

//-----------------------------------------------------------------------------

template<typename T>
void
TestCell<T>::testCompress()
{
    USING_NKHIVE_NS

    T set_val(1);
    T default_val(0);

    typename Cell<T>::shared_ptr cell(new Cell<T>(1, default_val));        
    
    // set value
    cell->set(0, 0, 0, set_val);

    // set another value
    cell->set(0, 0, 1, set_val + set_val);
    
    // compress cell
    cell->compress();

    // check state
    CPPUNIT_ASSERT(cell->isCompressed());

    // check compressed data size
    CPPUNIT_ASSERT(cell->m_data_size == 2);

    // check compressed contents
    CPPUNIT_ASSERT(toString(*cell) == "12");

    // check set value in compressed cell
    CPPUNIT_ASSERT(cell->get(0, 0, 0) == set_val);

    CPPUNIT_ASSERT(cell->get(0, 0, 1) == set_val + set_val);

    // check unset value in compressed cell
    CPPUNIT_ASSERT(cell->get(1, 0, 0) == default_val);
    
    // check for exception when trying to modify a compressed cell
    CPPUNIT_ASSERT_THROW(cell->set(0, 1, 0, set_val), Iex::LogicExc);    

    // check for exception when trying to fill a compressed cell
    CPPUNIT_ASSERT_THROW(cell->fill(set_val), Iex::LogicExc);

    // test uncompress
    cell->uncompress();

    // check state
    CPPUNIT_ASSERT(!cell->isCompressed());

    // check uncompressed contents
    CPPUNIT_ASSERT(toString(*cell) == "10002000");

    // check set value in uncompressed cell
    CPPUNIT_ASSERT(cell->get(0, 0, 0) == set_val);
    
    CPPUNIT_ASSERT(cell->get(0, 0, 1) == set_val + set_val);
    
    // check unset value in uncompressed cell
    CPPUNIT_ASSERT(cell->get(1, 0, 0) == default_val);

    // test insertion after uncompress
    cell->set(0, 1, 0, set_val);

    // test contents of insertion after uncompress
    CPPUNIT_ASSERT(toString(*cell) == "10102000");

    // check for successful insertion
    CPPUNIT_ASSERT(cell->get(0, 1, 0) == set_val);
}

//------------------------------------------------------------------------------

template<typename T>
void
TestCell<T>::testCompressFilledCell()
{
    USING_NKHIVE_NS

    T set_val(1);
    T default_val(0);

    typename Cell<T>::shared_ptr cell(new Cell<T>(1, default_val));        
    
    // set value
    cell->set(0, 0, 0, set_val);

    // set another value
    cell->set(0, 0, 1, set_val);
    
    // compress cell
    cell->compress();

    // check state
    CPPUNIT_ASSERT(cell->isCompressed());

    // check compressed data size
    CPPUNIT_ASSERT(cell->m_data_size == 0);
    CPPUNIT_ASSERT(cell->m_data == NULL);
    CPPUNIT_ASSERT(cell->isFilled());

    // check compressed contents
    CPPUNIT_ASSERT(toString(*cell) == "10001000");

    // check set value in compressed cell
    CPPUNIT_ASSERT(cell->get(0, 0, 0) == set_val);

    CPPUNIT_ASSERT(cell->get(0, 0, 1) == set_val);

    // check unset value in compressed cell
    CPPUNIT_ASSERT(cell->get(1, 0, 0) == default_val);
    
    // check for exception when trying to modify a compressed cell
    CPPUNIT_ASSERT_THROW(cell->set(0, 1, 0, set_val), Iex::LogicExc);    

    // check for exception when trying to fill a compressed cell
    CPPUNIT_ASSERT_THROW(cell->fill(set_val), Iex::LogicExc);

    // test uncompress
    cell->uncompress();

    // check state
    CPPUNIT_ASSERT(!cell->isCompressed());

    // check uncompressed contents
    CPPUNIT_ASSERT(toString(*cell) == "10001000");

    // check set value in uncompressed cell
    CPPUNIT_ASSERT(cell->get(0, 0, 0) == set_val);
    
    CPPUNIT_ASSERT(cell->get(0, 0, 1) == set_val);
    
    // check unset value in uncompressed cell
    CPPUNIT_ASSERT(cell->get(1, 0, 0) == default_val);

    // test insertion after uncompress
    cell->set(0, 1, 0, set_val + set_val);

    // test contents of insertion after uncompress
    CPPUNIT_ASSERT(toString(*cell) == "10201000");

    // check for successful insertion
    CPPUNIT_ASSERT(cell->get(0, 1, 0) == set_val + set_val);
}

//-----------------------------------------------------------------------------

// compress first then do copy and copy assignment.
template <typename T>
void
TestCell<T>::testCompressedCopy()
{
    USING_NKHIVE_NS

    T set_val(1);
    T default_val(0);

    typename Cell<T>::shared_ptr cell(new Cell<T>(1, default_val));        
    
    // set value
    cell->set(0, 0, 0, set_val);

    // set another value
    cell->set(1, 0, 0, set_val + set_val);
    
    // compress cell
    cell->compress();

    // check state
    CPPUNIT_ASSERT(cell->isCompressed());

    // Copy into another cell.
    typename Cell<T>::shared_ptr cell2(new Cell<T>(*cell));

    CPPUNIT_ASSERT(cell2->isCompressed());
    CPPUNIT_ASSERT(toString(*cell2) == "12");
    CPPUNIT_ASSERT(cell2->get(0, 0, 0) == set_val);
    CPPUNIT_ASSERT(cell2->get(1, 0, 0) == set_val + set_val);

    // copy into another cell using assignment.
    typename Cell<T>::shared_ptr cell3(new Cell<T>(1, default_val));        
    cell3->operator=(*cell);

    CPPUNIT_ASSERT(cell3->isCompressed());
    CPPUNIT_ASSERT(toString(*cell3) == "12");
    CPPUNIT_ASSERT(cell3->get(0, 0, 0) == set_val);
    CPPUNIT_ASSERT(cell3->get(1, 0, 0) == set_val + set_val);

    cell->uncompress();
    cell2->uncompress();
    cell3->uncompress();

    CPPUNIT_ASSERT(toString(*cell) == toString(*cell2));
    CPPUNIT_ASSERT(toString(*cell) == toString(*cell3));
}

//------------------------------------------------------------------------------

template <typename T>
void
TestCell<T>::testCompressCopyFilled()
{
    USING_NKHIVE_NS

    T set_val(1);
    T default_val(0);

    typename Cell<T>::shared_ptr cell(new Cell<T>(1, default_val));        
    
    // set value
    cell->set(0, 0, 0, set_val);

    // set another value
    cell->set(1, 0, 0, set_val);
    
    // compress cell
    cell->compress();

    // check state
    CPPUNIT_ASSERT(cell->isFilled());
    CPPUNIT_ASSERT(cell->isCompressed());

    // Copy into another cell.
    typename Cell<T>::shared_ptr cell2(new Cell<T>(*cell));

    CPPUNIT_ASSERT(cell2->isCompressed());
    CPPUNIT_ASSERT(cell2->isFilled());
    CPPUNIT_ASSERT(toString(*cell2) == "11000000");
    CPPUNIT_ASSERT(cell2->get(0, 0, 0) == set_val);
    CPPUNIT_ASSERT(cell2->get(1, 0, 0) == set_val);
    CPPUNIT_ASSERT(cell2->get(1, 1, 0) == default_val);

    // copy into another cell using assignment.
    typename Cell<T>::shared_ptr cell3(
            new Cell<T>(1, default_val + set_val));        
    cell3->operator=(*cell);

    CPPUNIT_ASSERT(cell3->isCompressed());
    CPPUNIT_ASSERT(cell3->isFilled());
    CPPUNIT_ASSERT(toString(*cell3) == "11000000");
    CPPUNIT_ASSERT(cell3->get(0, 0, 0) == set_val);
    CPPUNIT_ASSERT(cell3->get(1, 0, 0) == set_val);
    CPPUNIT_ASSERT(cell3->get(0, 1, 0) == default_val);

    cell->uncompress();
    cell2->uncompress();
    cell3->uncompress();

    CPPUNIT_ASSERT(cell->isFilled());
    CPPUNIT_ASSERT(cell2->isFilled());
    CPPUNIT_ASSERT(cell3->isFilled());

    CPPUNIT_ASSERT(toString(*cell) == toString(*cell2));
    CPPUNIT_ASSERT(toString(*cell) == toString(*cell3));
}

//-----------------------------------------------------------------------------

template<typename T>
void
TestCell<T>::testFill()
{
    USING_NKHIVE_NS

    T fill_val(1);
    T default_val(0);
    T set_val(2);

    typename Cell<T>::shared_ptr cell(new Cell<T>(1, default_val));        

    cell->set(0, 0, 1, set_val);
    cell->set(1, 1, 0, set_val);

    CPPUNIT_ASSERT(cell->isFilled());

    // test filling value
    cell->fill(fill_val);

    // check if allocation is down to one slot
    CPPUNIT_ASSERT(cell->m_data_size == 0);
    CPPUNIT_ASSERT(cell->m_data == NULL);

    // check state
    CPPUNIT_ASSERT(cell->isFilled() == true);
    CPPUNIT_ASSERT(cell->getFillValue() == fill_val);

    // test isset on filled cells
    CPPUNIT_ASSERT(cell->isSet(0, 0, 1) == true);
    CPPUNIT_ASSERT(cell->isSet(1, 0, 0) == true);

    // test access to a filled cell
    CPPUNIT_ASSERT(cell->get(0, 1, 0) == fill_val);

    // compression should just change state
    cell->compress();
    CPPUNIT_ASSERT(cell->isCompressed() == true);
    CPPUNIT_ASSERT(cell->isFilled() == true);

    // set should be illegal because of compressed state
    CPPUNIT_ASSERT_THROW(cell->set(0, 0, 1, fill_val), Iex::LogicExc);

    // uncompress to remove compressed state
    cell->uncompress();
    CPPUNIT_ASSERT(cell->isCompressed() == false);
    CPPUNIT_ASSERT(cell->isFilled() == true);
    
    // test access to a filled cell
    CPPUNIT_ASSERT(cell->get(0, 1, 0) == fill_val);

    // filling again should have no effect
    cell->fill(fill_val);
    CPPUNIT_ASSERT(cell->m_data == NULL);
    CPPUNIT_ASSERT(cell->isFilled() == true);
    CPPUNIT_ASSERT(cell->getFillValue() == fill_val);
    
    // test access to a filled cell
    CPPUNIT_ASSERT(cell->get(0, 1, 0) == fill_val);

    // set using fill value should have no effect
    cell->set(0, 0, 1, fill_val);
    CPPUNIT_ASSERT(cell->isFilled() == true);
    CPPUNIT_ASSERT(cell->getFillValue() == fill_val);
    
    // test access to a filled cell
    CPPUNIT_ASSERT(cell->get(0, 1, 0) == fill_val);

    // set with different value then fill
    cell->set(0, 0, 1, set_val);
    CPPUNIT_ASSERT(toString(*cell) == "11112111");
    CPPUNIT_ASSERT(cell->isFilled() == false);
    CPPUNIT_ASSERT(cell->m_data_size == 8);

    // fill again with fill val
    cell->fill(fill_val);
    CPPUNIT_ASSERT(cell->isFilled() == true);
    CPPUNIT_ASSERT(cell->getFillValue() == fill_val);
    CPPUNIT_ASSERT(cell->m_data == NULL);
    
    // test access to a filled cell
    CPPUNIT_ASSERT(cell->get(0, 1, 0) == fill_val);
    
    // unset a voxel
    cell->unset(0, 0, 1);
    CPPUNIT_ASSERT(toString(*cell) == "11110111");
    CPPUNIT_ASSERT(cell->isFilled() == true);    
    CPPUNIT_ASSERT(cell->m_data_size == 0);
}

//-----------------------------------------------------------------------------

template<typename T>
void
TestCell<T>::testFillSetUnset()
{
    USING_NKHIVE_NS

    T fill_val(1);
    T default_val(0);
    T set_val(2);

    typename Cell<T>::shared_ptr cell(new Cell<T>(1, default_val));        
    
    // test filling value
    cell->fill(fill_val);

    // check if allocation is down to one slot
    CPPUNIT_ASSERT(cell->m_data_size == 0);
    CPPUNIT_ASSERT(cell->m_data == NULL);

    // check state
    CPPUNIT_ASSERT(cell->isFilled() == true);

    // set a value
    cell->set(0, 0, 1, set_val);

    // check contents
    CPPUNIT_ASSERT(toString(*cell) == "11112111");

    // unset a value
    cell->unset(0, 1, 0);

    // check contents
    CPPUNIT_ASSERT(toString(*cell) == "11012111");

}

//------------------------------------------------------------------------------

template <typename T>
void
TestCell<T>::testFilledSet()
{
    USING_NKHIVE_NS

    T set_val(1);
    T default_val(0);
    T set_val2(2);

    typename Cell<T>::shared_ptr cell(new Cell<T>(1, default_val));        

    cell->set(0, 1, 0, set_val);

    CPPUNIT_ASSERT(cell->isFilled());
    CPPUNIT_ASSERT(cell->m_data == NULL);
    CPPUNIT_ASSERT(cell->getFillValue() == set_val);

    // set a different value at the same location. Should not allocate still
    // since we still only have a single value set.
    cell->set(0, 1, 0, set_val2);

    CPPUNIT_ASSERT(cell->isFilled());
    CPPUNIT_ASSERT(cell->m_data == NULL);
    CPPUNIT_ASSERT(cell->getFillValue() == set_val2);

    // set some more values.
    cell->set(1, 1, 0, set_val2);
    cell->set(0, 0, 1, set_val2);
    CPPUNIT_ASSERT(cell->isFilled());
    CPPUNIT_ASSERT(cell->m_data == NULL);
    CPPUNIT_ASSERT(cell->getFillValue() == set_val2);

    // now try to change the value for an already set bit, this time should
    // allocate.
    cell->set(0, 0, 1, set_val);
    CPPUNIT_ASSERT(!cell->isFilled());
    CPPUNIT_ASSERT(cell->m_data != NULL);

    // Also make sure the values are copied over.
    CPPUNIT_ASSERT(cell->get(1, 1, 0) == set_val2);
    CPPUNIT_ASSERT(cell->get(0, 1, 0) == set_val2);
    CPPUNIT_ASSERT(cell->get(0, 0, 1) == set_val);
    CPPUNIT_ASSERT(cell->get(0, 0, 0) == default_val);
}

//-----------------------------------------------------------------------------

template<typename T>
void
TestCell<T>::testIteration()
{
    USING_NKHIVE_NS

    T set_val(1);
    T default_val(0);

    typename Cell<T>::shared_ptr cell(new Cell<T>(1, default_val));        
    
    // set all odd bits
    cell->set(1, 0, 0, set_val);
    cell->set(1, 1, 0, set_val + set_val);
    cell->set(1, 0, 1, set_val + set_val + set_val);
    cell->set(1, 1, 1, set_val + set_val + set_val + set_val);

    // create iterator
    typename Cell<T>::iterator itr = cell->begin();
    
    // test forward iteration
    int i = 0; int odd_val = 0;
    for (; itr != cell->end(); ++itr, ++i) {
        if ((i % 2) == 0) 
            CPPUNIT_ASSERT(*itr == default_val);
        else {
            ++odd_val;
            CPPUNIT_ASSERT(*itr == T(odd_val));
        }
    }

    typename Cell<T>::shared_const_ptr c_cell(new const Cell<T>(*cell));
    
    // create const iterator
    typename Cell<T>::const_iterator c_itr = c_cell->begin();
    
    // test forward iteration
    i = 0; odd_val = 0;       
    for (; c_itr != c_cell->end(); ++c_itr, ++i) {
        if ((i % 2) == 0) 
            CPPUNIT_ASSERT(*c_itr == default_val);
        else {
            ++odd_val;
            CPPUNIT_ASSERT(*c_itr == T(odd_val));
        }
    }
}

//------------------------------------------------------------------------------

template <typename T>
void
TestCell<T>::testConstIterationFilled()
{
    USING_NKHIVE_NS

    T set_val(1);
    T default_val(0);

    {
        typename Cell<T>::shared_ptr cell(new Cell<T>(1, default_val));        
        
        // set all odd bits
        cell->set(1, 0, 0, set_val);
        cell->set(1, 0, 1, set_val);
        cell->set(1, 1, 0, set_val);
        cell->set(1, 1, 1, set_val);

        // create const iterator
        typename Cell<T>::shared_const_ptr c_cell(new const Cell<T>(*cell));
        typename Cell<T>::const_iterator c_itr = c_cell->begin();
        
        // test forward iteration
        int i = 0;        
        for (; c_itr != c_cell->end(); ++c_itr, ++i) {
            CPPUNIT_ASSERT(*c_itr == T(i % 2));
        }
    }

    // Test iteration over a filled cell
    {
        typename Cell<T>::shared_ptr cell(new Cell<T>(1, default_val,
                                                         set_val));        
        // create const iterator
        typename Cell<T>::shared_const_ptr c_cell(new const Cell<T>(*cell));
        typename Cell<T>::const_iterator c_itr = c_cell->begin();

        int count = 0;
        for ( ; c_itr != c_cell->end(); ++c_itr) {
            CPPUNIT_ASSERT(*c_itr == set_val);
            ++count;
        }
        CPPUNIT_ASSERT(count == 8);
    }
}

//------------------------------------------------------------------------------

template<typename T>
void
TestCell<T>::testIsEmpty()
{
    USING_NKHIVE_NS

    T set_val(1);
    T default_val(0);

    typename Cell<T>::shared_ptr cell(new Cell<T>(1, default_val));        

    // should be empty off start
    CPPUNIT_ASSERT(cell->isEmpty() == true);

    // add a value
    cell->set(0, 1, 0, set_val);

    // shouldn't be empty now
    CPPUNIT_ASSERT(cell->isEmpty() == false);

    // unset that value
    cell->unset(0, 1, 0);

    // should be empty again
    CPPUNIT_ASSERT(cell->isEmpty() == true);

    // fill
    cell->fill(set_val);

    // shouldn't be empty now
    CPPUNIT_ASSERT(cell->isEmpty() == false);

    // clear
    cell->clear();

     // should be empty again
    CPPUNIT_ASSERT(cell->isEmpty() == true);
}

//-----------------------------------------------------------------------------

template<typename T>
void
TestCell<T>::testIOHDF5()
{
    USING_NK_NS
    USING_NKHIVE_NS

    #define IO_TEST(co) {                                           \
        remove("testingHDF5.hv");                                   \
        VolumeFile file("testingHDF5.hv", VoidFile::WRITE_TRUNC);   \
        HDF5Group root_group;                                       \
        root_group.open(file.m_id, NK_NS::String("/"));             \
        HDF5Id root_group_id = root_group.id();                     \
        co->write(root_group_id, 2, index_vec(1,2,3));              \
        file.close();                                               \
        Cell<T> ci;                                                 \
        VolumeFile file2("testingHDF5.hv", VoidFile::READ_ONLY);    \
        HDF5Group root_group2;                                      \
        root_group2.open(file2.m_id, NK_NS::String("/"));           \
        HDF5Id root_group_id2 = root_group2.id();                   \
        HDF5Id cell_group_id;                                       \
        GET_CELL_ID(file2, root_group_id2, cell_group_id)           \
        ci.read(cell_group_id);                                     \
        CPPUNIT_ASSERT(ci == *co);                                  \
        file2.close();                                              \
        remove("testingHDF5.hv");                                   \
    }


    T fill_val(1);
    T default_val(0);

    typename Cell<T>::shared_ptr cell(new Cell<T>(1, default_val));        
    
    cell->set(0, 0, 1, fill_val);
    IO_TEST(cell);

    T set_val(2);
    cell->set(0, 0, 1, set_val);
    IO_TEST(cell);

    set_val = 3;
    cell->set(1, 1, 1, set_val);
    IO_TEST(cell);

    cell->fill(fill_val);
    IO_TEST(cell);
    
    cell->unset(0, 0, 1);
    IO_TEST(cell);

    cell->compress();
    IO_TEST(cell);

    cell->uncompress();
    IO_TEST(cell);

    #undef IO_TEST
}

//------------------------------------------------------------------------------

template <typename T>
void
TestCell<T>::testIO()
{
    USING_NKHIVE_NS

    #define IO_TEST(co) {                                           \
        std::ostringstream ostr(std::ios_base::binary);             \
        co->write(ostr);                                            \
        std::istringstream istr(ostr.str(), std::ios_base::binary); \
        Cell<T> ci;                                                 \
        ci.read(istr);                                              \
        CPPUNIT_ASSERT(ci == *co);                                  \
    } 

    T fill_val(1);
    T default_val(0);

    typename Cell<T>::shared_ptr cell(new Cell<T>(1, default_val));        
    
    cell->set(0, 0, 1, fill_val);
    IO_TEST(cell);

    T set_val(2);
    cell->set(0, 0, 1, set_val);
    IO_TEST(cell);

    set_val = 3;
    cell->set(1, 1, 1, set_val);
    IO_TEST(cell);

    cell->fill(fill_val);
    IO_TEST(cell);
    
    cell->unset(0, 0, 1);
    IO_TEST(cell);

    cell->compress();
    IO_TEST(cell);

    cell->uncompress();
    IO_TEST(cell);

    #undef IO_TEST
}

//-----------------------------------------------------------------------------

template<typename T>
void
TestCell<T>::testOperatorComparison()
{
    USING_NKHIVE_NS

    T fill_val(1);
    T default_val(0);

    typename Cell<T>::shared_ptr cell1(new Cell<T>(1, default_val));        
    typename Cell<T>::shared_ptr cell2(new Cell<T>(1, default_val));        
    CPPUNIT_ASSERT(*cell1 == *cell2);

    cell1->set(0, 0, 1, fill_val);
    cell2->set(0, 0, 1, fill_val);
    CPPUNIT_ASSERT(*cell1 == *cell2);

    T set_val(2);
    cell1->set(0, 1, 1, set_val);
    CPPUNIT_ASSERT(*cell1 != *cell2);
    cell2->set(0, 1, 1, set_val);
    CPPUNIT_ASSERT(*cell1 == *cell2);

    set_val = 3;
    cell1->set(1, 1, 1, set_val);
    CPPUNIT_ASSERT(*cell1 != *cell2);
    cell2->set(1, 1, 1, set_val);
    CPPUNIT_ASSERT(*cell1 == *cell2);

    cell1->fill(fill_val);
    CPPUNIT_ASSERT(*cell1 != *cell2);
    cell2->fill(fill_val);
    CPPUNIT_ASSERT(*cell1 == *cell2);
    
    cell1->unset(0, 0, 1);
    CPPUNIT_ASSERT(*cell1 != *cell2);
    cell2->unset(0, 0, 1);
    CPPUNIT_ASSERT(*cell1 == *cell2);

    cell1->compress();
    CPPUNIT_ASSERT(*cell1 != *cell2);
    cell2->compress();
    CPPUNIT_ASSERT(*cell1 == *cell2);

    cell1->uncompress();
    CPPUNIT_ASSERT(*cell1 != *cell2);
    cell2->uncompress();
    CPPUNIT_ASSERT(*cell1 == *cell2);
}

//------------------------------------------------------------------------------

template <typename T>
void
TestCell<T>::testCreateFilledCell()
{
    USING_NKHIVE_NS

    T default_val(0);
    T fill_val(1);

    Cell<T> cell(2, default_val, fill_val, typename Cell<T>::allocator_type());

    CPPUNIT_ASSERT(cell.m_data == NULL);
    CPPUNIT_ASSERT(cell.m_data_size == 0);
    CPPUNIT_ASSERT(cell.getFillValue() == fill_val);
    CPPUNIT_ASSERT(cell.getDefaultValue() == default_val);
    CPPUNIT_ASSERT(cell.isFilled());
    CPPUNIT_ASSERT(cell.m_bitfield.isFull());
}

//-----------------------------------------------------------------------------

template<typename T>
void
TestCell<T>::testUnsetBlock()
{
    USING_NK_NS
    USING_NKHIVE_NS
        
    T set_val(1);
    T default_val(0);

    typename Cell<T>::shared_ptr cell(new Cell<T>(1,default_val));        
    
    // set all odd bits
    cell->set(1, 0, 0, set_val);
    cell->set(1, 0, 1, set_val);
    cell->set(1, 1, 0, set_val);
    cell->set(1, 1, 1, set_val);

    // check contents
    CPPUNIT_ASSERT(toString(*cell) == "01010101");

    // unset value
    vec3ui min(0, 0, 0);

    // unset window
    cell->unsetBlock(min, 2);

    // check contents
    CPPUNIT_ASSERT(toString(*cell) == "00000000");

    // fill cell
    cell->fill(set_val);

    // unset window
    cell->unsetBlock(min, 2);

    // check contents
    CPPUNIT_ASSERT(toString(*cell) == "00000000");

    // create a 4x4x4 cell
    typename Cell<T>::shared_ptr cell4(new Cell<T>(2,default_val));

    // fill with set value
    cell4->fill(set_val);

    // unset a window in the middle
    min[0] = 1;
    min[1] = 1;
    min[2] = 1;

    // unset (1,1,1) to (2,2,2)
    cell4->unsetBlock(min, 2);

    // check contents
    BLOCK_2_TEST(1, 2, default_val, cell4);
   
    // fill
    cell4->fill(set_val);

    // unset (0,0,0) to (1,1,1)
    min[0] = 0;
    min[1] = 0;
    min[2] = 0;

    cell4->unsetBlock(min, 2);

    // check contents
    BLOCK_2_TEST(0, 1, default_val, cell4);

    // fill 
    cell4->fill(set_val);

    // unset (2,2,2) to (3,3,3)
    min[0] = 2;
    min[1] = 2;
    min[2] = 2;

    cell4->unsetBlock(min, 2);

    // check contents
    BLOCK_2_TEST(2, 3, default_val, cell4);
}

//-----------------------------------------------------------------------------

template<typename T>
void
TestCell<T>::testSetWindow()
{
    USING_NK_NS
    USING_NKHIVE_NS
        
    T set_val(1);
    T default_val(0);

    typename Cell<T>::shared_ptr cell(new Cell<T>(1,default_val));        
    
    // set all odd bits
    cell->set(1, 0, 0, set_val);
    cell->set(1, 0, 1, set_val);
    cell->set(1, 1, 0, set_val);
    cell->set(1, 1, 1, set_val);

    // check contents
    CPPUNIT_ASSERT(toString(*cell) == "01010101");

    // unset value
    vec3ui min(0, 0, 0);

    // unset window
    cell->unsetBlock(min, 2);

    // check contents
    CPPUNIT_ASSERT(toString(*cell) == "00000000");

    // fill cell
    cell->fill(set_val);

    // unset window
    cell->unsetBlock(min, 2);

    // check contents
    CPPUNIT_ASSERT(toString(*cell) == "00000000");

    // create a 4x4x4 cell
    typename Cell<T>::shared_ptr cell4(new Cell<T>(2,default_val));

    // set a window in the middle
    min[0] = 1;
    min[1] = 1;
    min[2] = 1;

    // unset (1,1,1) to (2,2,2)
    cell4->setBlock(min, 2, set_val);

    // check contents
    BLOCK_2_TEST(1, 2, set_val, cell4);
   
    // clear
    cell4->clear();

    // unset (0,0,0) to (1,1,1)
    min[0] = 0;
    min[1] = 0;
    min[2] = 0;

    cell4->setBlock(min, 2, set_val);

    // check contents
    BLOCK_2_TEST(0, 1, set_val, cell4);

    // clear 
    cell4->clear();

    // unset (2,2,2) to (3,3,3)
    min[0] = 2;
    min[1] = 2;
    min[2] = 2;

    cell4->setBlock(min, 2, set_val);

    // check contents
    BLOCK_2_TEST(2, 3, set_val, cell4);
}

//------------------------------------------------------------------------------

template <typename T>
void
TestCell<T>::testUnsetBlockFilledCell()
{
    USING_NK_NS
    USING_NKHIVE_NS

    T set_val(1);
    T default_val(0);

    // unset the entire cell and deallocate space and use default value if
    // unsetting the entire cell.
    {
        typename Cell<T>::shared_ptr cell(new Cell<T>(1,default_val));        

        vec3ui min(0, 0, 0);

        cell->fill(set_val);

        cell->unsetBlock(min, 2);

        CPPUNIT_ASSERT(cell->isFilled());
        CPPUNIT_ASSERT(cell->isEmpty());
        CPPUNIT_ASSERT(cell->getDefaultValue() == default_val);
    }

    // unset the fill value state when unsetting partial cell.
    {
        typename Cell<T>::shared_ptr cell(new Cell<T>(2, set_val));        

        vec3ui min(1, 1, 1);

        cell->fill(default_val);

        cell->unsetBlock(min, 2);

        CPPUNIT_ASSERT(cell->isFilled());
        BLOCK_2_TEST(1, 2, set_val, cell);
    }
}

//------------------------------------------------------------------------------

template <typename T>
void
TestCell<T>::testSetBlockFilledCell()
{
    USING_NK_NS
    USING_NKHIVE_NS

    T set_val(1);
    T default_val(0);
    T fill_val(2);

    // retain filled value status if setBlock sets the entire cell.
    {
        typename Cell<T>::shared_ptr cell(new Cell<T>(1,default_val));        

        vec3ui min(0, 0, 0);

        cell->fill(set_val);

        cell->setBlock(min, 2, fill_val);

        CPPUNIT_ASSERT(cell->isFilled());
        CPPUNIT_ASSERT(cell->getFillValue() == fill_val);
        CPPUNIT_ASSERT(cell->m_data == NULL);
    }

    // set fill value status if setBlocks fills entire cell.
    {
        typename Cell<T>::shared_ptr cell(new Cell<T>(1,default_val));        

        cell->set(0, 1, 1, set_val);

        vec3ui min(0, 0, 0);

        cell->setBlock(min, 2, fill_val);

        CPPUNIT_ASSERT(cell->isFilled());
        CPPUNIT_ASSERT(cell->getFillValue() == fill_val);
        CPPUNIT_ASSERT(cell->m_data == NULL);
    }

    // remove filled value status if setBlock only sets partial cell.
    {
        typename Cell<T>::shared_ptr cell(new Cell<T>(2,default_val));        

        vec3ui min(1, 1, 1);

        cell->fill(set_val);

        cell->setBlock(min, 2, fill_val);

        CPPUNIT_ASSERT(!cell->isFilled());
        BLOCK_2_TEST(1, 2, fill_val, cell);
    }
}

//------------------------------------------------------------------------------

template<typename T>
void
TestCell<T>::testComputeSetBounds()
{
    USING_NK_NS
    USING_NKHIVE_NS

    T set_val(1);

    vec3i min, max;

    {
        typename Cell<T>::shared_ptr cell(new Cell<T>(1, T(0)));        
        // set value
        cell->set(0, 0, 0,set_val);

        index_bounds bounds;
        cell->computeSetBounds(bounds);
        CPPUNIT_ASSERT(bounds.min() == index_bounds::vector_type(0, 0, 0));
        CPPUNIT_ASSERT(bounds.max() == index_bounds::vector_type(1, 1, 1));

        // set another value
        cell->set(0, 1, 0,set_val);
        cell->computeSetBounds(bounds);
        CPPUNIT_ASSERT(bounds.min() == index_bounds::vector_type(0, 0, 0));
        CPPUNIT_ASSERT(bounds.max() == index_bounds::vector_type(1, 2, 1));
    }

    {
        typename Cell<T>::shared_ptr cell(new Cell<T>(2, T(0)));        

        cell->set(1, 2, 3, set_val);
        cell->set(1, 2, 2, set_val);

        index_bounds bounds;
        cell->computeSetBounds(bounds);
        CPPUNIT_ASSERT(bounds.min() == index_bounds::vector_type(1, 2, 2));
        CPPUNIT_ASSERT(bounds.max() == index_bounds::vector_type(2, 3, 4));
    }

    // test over a filled cell.
    {
        typename Cell<T>::shared_ptr cell(new Cell<T>(2, T(0)));

        cell->fill(set_val);

        index_bounds bounds;
        cell->computeSetBounds(bounds);
        CPPUNIT_ASSERT(bounds.min() == index_bounds::vector_type(0, 0, 0));
        CPPUNIT_ASSERT(bounds.max() == index_bounds::vector_type(4, 4, 4));
    }
}

//------------------------------------------------------------------------------

template<typename T>
void
TestCell<T>::testWriteStamp() 
{
    USING_NKHIVE_NS

    typedef Stamp<T, One>                    StampOne;
    typedef Stamp<T, CoordinateMapper>       StampTester;
    typedef Cell<T>                          Cell;
    typedef index_bounds::vector_type        vector;
    typedef signed_index_bounds::vector_type signed_vector;

    const T default_val(0);    
    const T set_val(1);

    // stamp single value
    {
        const vector zero(0, 0, 0);
        const vector unit(1, 1, 1);

        StampOne stamp;
        index_bounds cell_bounds(zero, unit);
        signed_index_bounds voxel_bounds(zero, unit);

        typename Cell::shared_ptr cell(new Cell(2, default_val));        
        cell->stamp(stamp, voxel_bounds, cell_bounds, unit);
        CPPUNIT_ASSERT(cell->get(0, 0, 0) == set_val);    
        for (index_type k = -1; k <= 1; ++k) {
            for (index_type j = -1; j <= 1; ++j) {
                for (index_type i = -1; i <= 1; ++i) {
                    if (!(i + j + k)) continue;
                    CPPUNIT_ASSERT(cell->get(i, j, k) == default_val);    
                }
            }
        }
    }

    #define TEST_STAMP_BOUNDS(tx, ty, tz) {                                \
        const vector min( 1,  1,  1);                                      \
        const vector max(10, 10, 10);                                      \
        const signed_vector t(tx, ty, tz);                                 \
                                                                           \
        StampTester stamp;                                                 \
        stamp.bounds().setExtrema(min * t, max * t);                       \
        index_bounds cell_bounds(min, max);                                \
                                                                           \
        typename Cell::shared_ptr cell(new Cell(4, default_val));          \
        cell->stamp(stamp, stamp.bounds(), cell_bounds, t);                \
                                                                           \
        for (index_type k = 0; k <= max.z; ++k) {                          \
            for (index_type j = 0; j <= max.y; ++j) {                      \
                for (index_type i = 0; i <= max.x; ++i) {                  \
                    if (!cell_bounds.inRange(index_vec(i, j, k))) {        \
                        CPPUNIT_ASSERT(cell->get(i, j, k) == default_val); \
                    } else {                                               \
                        signed_index_type si, sj, sk;                      \
                        si = (t.x < 0 ? i + 1 : i) * t.x;                  \
                        sj = (t.y < 0 ? j + 1 : j) * t.y;                  \
                        sk = (t.z < 0 ? k + 1 : k) * t.z;                  \
                        CPPUNIT_ASSERT(cell->get(i, j, k) ==               \
                            CoordinateMapper<T>::get(si, sj, sk));         \
                    }                                                      \
                }                                                          \
            }                                                              \
        }                                                                  \
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

//------------------------------------------------------------------------------

