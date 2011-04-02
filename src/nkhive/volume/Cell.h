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
// Cell.h
//------------------------------------------------------------------------------

#ifndef __NKHIVE_VOLUME_CELL_H__
#define __NKHIVE_VOLUME_CELL_H__

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------

#include <assert.h>
#include <algorithm>
#include <memory>
#include <boost/shared_ptr.hpp>
#include <boost/typeof/typeof.hpp>

#include <nkbase/Exceptions.h>
#include <nkbase/BinaryOps.h>

#include <nkhive/Defs.h>
#include <nkhive/Types.h>
#include <nkhive/bitfields/BitField3D.h>
#include <nkhive/tiling/Stamp.h>
#include <nkhive/util/Bounds3D.h>
#include <nkhive/io/hdf5/HDF5Group.h>
#include <nkhive/io/hdf5/HDF5Util.h>
#include <nkhive/io/hdf5/HDF5DataType.h>

//------------------------------------------------------------------------------
// forward declarations
//------------------------------------------------------------------------------

#ifdef UNITTEST
class TestNode;
#endif //UNITTEST

BEGIN_NKHIVE_NS

template<typename T, typename A>
class Cell;

template <typename T> 
class CellSetIterator;

template<typename T, typename A>
std::istream& operator>>(std::istream& is, const Cell<T, A>& cell); 

template<typename T, typename A>
std::ostream& operator<<(std::ostream& os, const Cell<T, A>& cell); 

END_NKHIVE_NS

//------------------------------------------------------------------------------
// class definition
//------------------------------------------------------------------------------

BEGIN_NKHIVE_NS

template < typename T, typename A = std::allocator<T> >
class Cell
{
    
public:

    //--------------------------------------------------------------------------
    // typedefs
    //--------------------------------------------------------------------------
    
    typedef A                                        allocator_type;
    typedef typename allocator_type::value_type      value_type;
    
    typedef typename allocator_type::size_type       size_type;
    typedef typename allocator_type::difference_type difference_type;
    
    typedef typename allocator_type::pointer         pointer;
    typedef typename allocator_type::const_pointer   const_pointer;
    
    typedef typename allocator_type::reference       reference;
    typedef typename allocator_type::const_reference const_reference;
  
    typedef boost::shared_ptr<Cell>                  shared_ptr;
    typedef boost::shared_ptr<const Cell>            shared_const_ptr;

    class iterator;
    class const_iterator;

    typedef typename 
    bitfield_type::template set_iterator<iterator>          set_iterator;

    typedef typename
    bitfield_type::template set_iterator<const_iterator>    const_set_iterator;

    //--------------------------------------------------------------------------
    // enums
    //--------------------------------------------------------------------------
    
    enum Flags 
    {
        CELL_FLAG_COMPRESSED = 0x01,
        CELL_FLAG_FILLED     = 0x02
    };

    //--------------------------------------------------------------------------
    // public interface
    //--------------------------------------------------------------------------
    
    /**
     * Default contructor.
     */
    Cell();

    /**
     * dimSize is lg of actual dimensions to ensure power of 2.
     */
    Cell(u8 lg_dim_size, const_reference v = value_type(), 
         const allocator_type& a = allocator_type());

    /**
     * Constructor for initializing as a filled cell.
     */
    Cell(u8 lg_dim_size, 
         const_reference default_value, const_reference fill_value,
         const allocator_type& a = allocator_type());
                                        
    /**
     * Copy constructor.
     */
    Cell(const Cell& other);
 
    /**
     * Destructor.
     */
    ~Cell();
    
    /**
     * Returns the absolute dimensions of the cell.
     */
    size_type getDimension() const;

    /** 
     * Get bounds of the set data values.
     */
    void computeSetBounds(index_bounds &bounds) const;

    /**
     * Check state of cell.
     */
    bool isEmpty() const;
    bool isFilled() const;
    bool isCompressed() const;

    /**
     * Check the status of a voxel at the given coordinates.
     */
    bool isSet(size_type i, size_type j, size_type k) const;

    /**
     * Set/unset voxel at the given coordinates.
     */
    void set(size_type i, size_type j, size_type k, const_reference value);
    void unset(size_type i, size_type j, size_type k);

    /**
     * Set/unset voxel using bit index
     */
    void set(size_type index, const_reference value);
    void unset(size_type index);

    /** 
     * Update the value at the given coordinates/index with the given binary
     * operator.
     */
    template <typename BinaryOp>
    void update(size_type i, size_type j, size_type k, const_reference val, 
                BinaryOp op);
    template <typename BinaryOp>
    void update(size_type index, const_reference value, BinaryOp op);

    /**
     * Access the voxel at the given coordinates.
     */
    reference get(size_type i, size_type j, size_type k);
    const_reference get(size_type i, size_type j, size_type k) const;

    /**
     * Access to voxel at the linear index.
     */
    reference get(size_type index);
    const_reference get(size_type index) const;

    /**
     * Set/Unset a block of voxels
     */
    void unsetBlock(const vec3ui &min, size_type window_size);
    void setBlock(const vec3ui &min, size_type window_size, 
                  const_reference value);

    /**
     * Writes the stamp to the cell.  
     */
    template <typename U, template <typename> class Source>
    void stamp(const Stamp<U, Source> &stamp, signed_index_bounds stamp_bounds,
               const index_bounds &cell_bounds, 
               const signed_index_vec &transform);

    /**
     * Iterators for voxel traversal. Here the iterator and const_iterator
     * differ in functionality. Since values are allowed to change using the
     * *operator() in iterator, iterator is only valid if we have a non-fill
     * node with an allocated m_data member. const_iterator however, can be
     * used even when we have fill cells as it will properly use the set-bits
     * to iterate and get the default/fill values. 
     */
    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
    
    /**
     * Update the compression state.  Compressed cells cannot be modified.
     */
    void compress();
    void uncompress();

    /**
     * Clear all set voxels.
     */
    void clear();

    /**
     * Fill all voxels with value.
     */
    void fill(const_reference value);
 
    /**
     * Read and write cell to/from input stream.
     */
    void read(std::istream &is);
    void write(std::ostream &os) const;
    void read(HDF5Id leaf_group_id);
    void write(HDF5Id volume_group_id, 
               size_t quadrant, index_vec offset) const;
               

    /** 
     * Comparison operators.
     */
    bool operator==(const Cell& that) const;
    bool operator!=(const Cell& that) const;
   
    /**
     * Read cell from input stream.
     */
    friend std::istream& operator>> <> (std::istream& is, 
                                        const Cell<T, A>& cell);

    /**
     * Write cell to output stream.
     */
    friend std::ostream& operator<< <> (std::ostream& os, 
                                        const Cell<T, A>& cell);

    /**
     * Returns memory used by class.
     */
    int sizeOf() const;

private:

    //--------------------------------------------------------------------------
    // internal methods
    //--------------------------------------------------------------------------

    /**
     * Construct objects with value v for all voxels that are "set" as
     * indicated by the bitfield.
     */
    void initializeSet(const_reference v);
    
    /**
     * Construct available voxels with value v.
     */
    iterator uninitializedFill(iterator b, iterator e, const_reference v);

    /**
     * Deconstruct voxels in the cell. 
     */
    iterator deinitialize(iterator b, iterator e);

    /**
     * Compressed cell indexing.
     */
    void shallow_copy(pointer dst, pointer src, size_type size);

    /**
     * Flag management operations.
     */
    void setFlag(uint8_t flag);
    void unsetFlag(uint8_t flag);
    bool isFlagSet(uint8_t flag) const;

    /**
     * Operations for destructing the cell.
     */
    void destruct();

    /** 
     * Copy from another cell.
     */
    void copy(const Cell& that);

    /** 
     * Assignment operator.
     */
    Cell& operator=(const Cell& that);

    /**
     * Default and fill value getters and setters
     */
    const_reference   getDefaultValue() const;
    const_reference   getFillValue() const;
    reference         getDefaultValue();
    reference         getFillValue();
    void              setDefaultValue(const_reference val);
    void              setFillValue(const_reference val);

    /**
     * Internal write helper
     */
    void writeInternal(std::ostream &os) const;
    void writeInternal(HDF5Id cell_group_id) const;

    //--------------------------------------------------------------------------
    // friends
    //--------------------------------------------------------------------------
   
#ifdef UNITTEST
    template<typename U>
    friend class TestCell;

    friend class ::TestNode;
#endif // UNITTEST

    template <typename U>
    friend class CellSetIterator;

    //--------------------------------------------------------------------------
    // members
    //--------------------------------------------------------------------------
    
    allocator_type     m_allocator;
    pointer            m_data;
    size_type          m_data_size;   
    value_type         m_default_value;
    value_type         m_fill_value;
    bitfield_type      m_bitfield; 
    u8                 m_flags;
    
}; 

//-----------------------------------------------------------------------------
// iterator
//-----------------------------------------------------------------------------

template<typename T, typename A = std::allocator<T> >
class Cell<T, A>::iterator
{

public:

    //--------------------------------------------------------------------------
    // typedefs
    //--------------------------------------------------------------------------

    typedef std::bidirectional_iterator_tag iterator_category;
    typedef typename Cell::value_type       value_type;
    typedef typename Cell::difference_type  difference_type;
    typedef typename Cell::pointer          pointer;
    typedef typename Cell::reference        reference;
  
    //--------------------------------------------------------------------------
    // public interface
    //--------------------------------------------------------------------------

    iterator();
    iterator(pointer _begin, pointer _end, pointer _p);
  
    //--------------------------------------------------------------------------
    // operators
    //--------------------------------------------------------------------------
  
    typename Cell::iterator::reference operator*() const;
    typename Cell::iterator::pointer operator->() const;

    iterator& operator++();
    iterator operator++(int);
    iterator& operator--();
    iterator operator--(int);

    bool operator==(const iterator& that) const;
    bool operator!=(const iterator& that) const;
    bool operator&&(const iterator& that) const;
    bool operator<=(const iterator& that) const;
  
private:

    //--------------------------------------------------------------------------
    // internal methods
    //--------------------------------------------------------------------------
  
    bool isValid() const;
         
    //--------------------------------------------------------------------------
    // members
    //--------------------------------------------------------------------------

    pointer   m_begin; 
    pointer   m_end;   
    pointer   m_p;     
}; 

//-----------------------------------------------------------------------------
// const iterator
//-----------------------------------------------------------------------------

template<typename T, typename A = std::allocator<T> >
class Cell<T, A>::const_iterator
{

public:

    //--------------------------------------------------------------------------
    // typedefs
    //--------------------------------------------------------------------------

    typedef std::bidirectional_iterator_tag        iterator_category;
    typedef typename Cell::value_type              value_type;
    typedef typename Cell::difference_type         difference_type;
    typedef typename Cell::const_pointer           pointer;
    typedef typename Cell::const_reference         reference;
    typedef typename bitfield_type::const_iterator bitfield_iterator_type;
  
    //--------------------------------------------------------------------------
    // public interface
    //--------------------------------------------------------------------------

    const_iterator();
    const_iterator(const Cell *cell, const bitfield_iterator_type &biter, 
                   pointer p);

    //--------------------------------------------------------------------------
    // operators
    //--------------------------------------------------------------------------
  
    typename Cell::const_iterator::reference operator*() const;
    typename Cell::const_iterator::pointer operator->() const;

    const_iterator& operator++();
    const_iterator operator++(int);
    const_iterator& operator--();
    const_iterator operator--(int);

    bool operator==(const const_iterator& that) const;
    bool operator!=(const const_iterator& that) const;
    bool operator<=(const const_iterator& that) const;
  
private:

    //--------------------------------------------------------------------------
    // internal methods
    //--------------------------------------------------------------------------
         
    //--------------------------------------------------------------------------
    // members
    //--------------------------------------------------------------------------

    bitfield_iterator_type m_bitfield_iter;
    pointer   m_p;     
    bool      m_isFilled;

    /**
     * Keep a jump table that determines which value is returned during the
     * *operator() call.  
     *
     * The jump table is used to avoid branching in the iterator. The addresses
     * of the table are deciphered by first: the [0,1] value determined if a
     * it is a fill cell or not, and second: the [0,1] value determined if the
     * bit is set or not. Here are how the values are arranged in the
     * jump table: 
     *      [0][0] - m_p
     *      [0][1] - m_p
     *      [1][0] - &default_value
     *      [1][1] - &fill_value
     *
     * Note that indexing first by the fillCell and then by the set-bit also
     * handles the case when we have a compressed cell. Only when we do not
     * have a filled cell does the second index (set-bit) make a difference in
     * the return value. Otherwise, it just behaves like an array iterator
     * iterating over the m_data array.
     */
    pointer m_value_jumptable[2][2];
}; 

END_NKHIVE_NS

//-----------------------------------------------------------------------------
// class implementation
//-----------------------------------------------------------------------------

BEGIN_NKHIVE_NS

#include <nkhive/volume/Cell.hpp>

END_NKHIVE_NS

//-----------------------------------------------------------------------------

#endif // __NKHIVE_VOLUME_CELL_H_
