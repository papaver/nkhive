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
// BitField3D.h
//------------------------------------------------------------------------------

#ifndef __NKHIVE_BITFIELD_BITFIELD3D_H__
#define __NKHIVE_BITFIELD_BITFIELD3D_H__

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------

#include <cassert>   
#include <cstring>   
#include <string>   
#include <algorithm> 
#include <iterator>  
#include <memory>   
#include <stdexcept> 

#include <boost/typeof/typeof.hpp>

#include <nkhive/Defs.h>
#include <nkhive/Types.h>
#include <nkhive/bitfields/BitOps.h> 
#include <nkhive/io/hdf5/HDF5Util.h>
#include <nkhive/io/hdf5/HDF5DataSet.h>

//-----------------------------------------------------------------------------
// forward declarations 
//-----------------------------------------------------------------------------
  
BEGIN_NKHIVE_NS

template <typename T, typename A> 
class BitField3D;

template <typename T, typename A> 
std::istream& 
operator>>(std::istream& o, const BitField3D<T, A>& bitfield);

template <typename T, typename A> 
std::ostream& 
operator<<(std::ostream& o, const BitField3D<T, A>& bitfield);

END_NKHIVE_NS

//-----------------------------------------------------------------------------
// class declarations
//-----------------------------------------------------------------------------

BEGIN_NKHIVE_NS

/**
 *  The BitField3D container implements a 3D representation of a bitfield.  
 *  Bit operations can be performed using 3D coordinates or indexing.  Bit
 *  iterators are provided for sequential traversal.  
 *
 *  Special iterator adapters are provided for efficient traversal of the 
 *  data coupled with the bitfield.
 */
template < typename T, typename A = std::allocator<T> >
class BitField3D 
{
    
public:

    //-------------------------------------------------------------------------
    // typedefs
    //-------------------------------------------------------------------------

    typedef A                                        allocator_type;
    typedef typename allocator_type::value_type      value_type;

    typedef typename allocator_type::size_type       size_type;
    typedef typename allocator_type::difference_type difference_type;

    typedef typename allocator_type::pointer         pointer;
    typedef typename allocator_type::const_pointer   const_pointer;

    typedef typename allocator_type::reference       reference;
    typedef typename allocator_type::const_reference const_reference;
  
    //-----------------------------------------------------------------------
 
    class iterator;
    class const_iterator;
    class block_iterator;
    class window_iterator;

    template <class FI>
    class set_iterator;
    
    template <class FI>
    class unset_iterator;

    //-------------------------------------------------------------------------
    // public interface
    //-------------------------------------------------------------------------
      
    /**
     * Default constructor. Size set to zero, no memory allocated.
     */
    BitField3D(const allocator_type& a = allocator_type());

    /**
     *  Size in cubic dimensions.
     */
    BitField3D(size_type lg_size, const allocator_type& a = allocator_type());

    /**
     * Copy constructor.  Creates a deep copy of the bitfield.
     */
    BitField3D (const BitField3D& that);

    /**
     * Destructor. Deallocates any used memory.
     */
    ~BitField3D();

    /**
     * Returns index into bitfield for the specified 3D coordinates.
     */
    index_type getIndex(index_type i, index_type j, index_type k) const;

    /**
     * Returns i,j,k into bitfield for the specified index.
     */
    void getCoordinates(
        index_type index, index_type& i, index_type& j, index_type& k) const;

    /**
     * Returns index into bitfield for the ith set bit.
     */
    index_type getSetIndex(index_type si) const;

    /**
     * Check the status of a bit.
     */
    bool isSet(index_type i) const;
    bool isSet(index_type x, index_type y, index_type z) const;

    /**
     * Set the status of a single bit.
     */
    void setBit(index_type i);
    void unsetBit(index_type i);
    void setBit(index_type x, index_type y, index_type z);
    void unsetBit(index_type x, index_type y, index_type z);

    /**
     * Iterators used to traverse the bitfield, bit by bit.
     */
    iterator begin();
    const_iterator begin() const;
    iterator end();
    const_iterator end() const;

    /**
     * Iterators adapters used to traverse coupled data.
     */
    template <typename FI>
    typename BitField3D::template set_iterator<FI> setIterator(FI iter) const;

    template <typename FI>
    typename BitField3D::template unset_iterator<FI> unsetIterator(FI iter) const;

    /**
     * Iterators for traversing regions of the bitfield
     */
    window_iterator windowIterator(index_type b, index_type size) const;

    /**
     * Check if status of all bits are set to either on or off.
     */
    bool isFull() const;
    bool isEmpty() const;

    /**
     * Set all bits to either on or off, or invert status.
     */
    void fillBits();
    void clearBits();
    void invertBits();

    /**
     * Returns true if the bitfield has only one bit set and it is at the given
     * index i.
     */
    bool isSingleBitSet(index_type i) const;

    /**
     * Get number of set bits.
     */
    index_type count() const;

    /**
     * Get number of set bits up to the specified index.
     */
    index_type countRange(index_type i) const;

    /**
     * Get bitfield's log2 3D cube dimensions.
     */
    size_type size() const;

    /**
     * Get size of storage available for bitfield.
     */
    size_type capacity() const;

    /**
     * Resize bitfield to the given 3D cube dimensions.  If capacity is not 
     * large enough to accomadate new size, memory will be reallocated.  
     * The bitfield will be preserved in growth, and truncated when shrunk.
     */
    void resize(size_type s);

    /**
     * Swaps the contents of the bitfields.
     */
    void swap(BitField3D &that);

    /**
     * Read and write bitfield to/from input stream.
     */
    void read(HDF5Id parent_id);
    void write(HDF5Id parent_id) const;

    void read(std::istream &is);
    void write(std::ostream &os) const;

    /**
     * Returns string representation of bitfield.
     */
    std::string toString();

    /**
     * Assignment operator.
     */
    BitField3D& operator=(const BitField3D& that);

    /**
     * Comparison operators.
     */
    bool operator==(const BitField3D& that) const;
    bool operator!=(const BitField3D& that) const;

    /**
     * Read bitfield from input stream.
     */
    friend std::istream& operator>> <> (std::istream& is, 
                                        const BitField3D& bitfield);

    /**
     * Write bitfield to output stream.
     */
    friend std::ostream& operator<< <> (std::ostream& os, 
                                        const BitField3D& bitfield);

    /**
     * Returns memory used by class.
     */
    int sizeOf() const;

private:

    //-------------------------------------------------------------------------
    // internal methods
    //-------------------------------------------------------------------------

    /**
     * Validates state of bitfield container.
     */
    bool valid() const;

    /**
     * Iterators used to traverse the bitfield memory blocks.
     */
    block_iterator blockBegin();
    block_iterator blockEnd();

    /**
     * Copy size bytes from source to destination. 
     */
    void copy(pointer dst, const_pointer src, size_type s);
    
    /**
     * Clear size bytes, starting at pointer.
     */
    void clear(pointer p, size_type s);

    /**
     * Allocate/Deallocate memory blocks for bitfield storage.
     */
    void allocate(pointer* p, size_type s);
    void deallocate(pointer p, size_type s);

    //-------------------------------------------------------------------------
    // typedefs  
    //-------------------------------------------------------------------------
  
    template <class FI, bool S>
    class status_iterator;

    //-------------------------------------------------------------------------
    // members
    //-------------------------------------------------------------------------

    allocator_type m_allocator; // block allocator 
    pointer        m_blocks;    // block pointer
    size_type      m_capacity;  // block count 
    size_type      m_lg_size;   // Log2 of bitfield size
};

//-----------------------------------------------------------------------------
// BitField3D<T, A>::iterator
//-----------------------------------------------------------------------------

template < typename T, typename A = std::allocator<T> >
class BitField3D<T, A>::iterator 
{

public:

    //-------------------------------------------------------------------------
    // typedefs
    //-------------------------------------------------------------------------

    typedef std::bidirectional_iterator_tag      iterator_category;
    typedef typename BitField3D::size_type       size_type;
    typedef typename BitField3D::difference_type difference_type;
    typedef typename BitField3D::pointer         pointer;
    typedef typename BitField3D::reference       reference;

    //-------------------------------------------------------------------------
    // public interface 
    //-------------------------------------------------------------------------

    iterator();
    iterator(pointer b, size_type s, size_type i = 0);
    // iterator(const iterator&);
    // ~iterator();

    index_type getIndex() const;

    bool isSet() const;
    void setBit();
    void unsetBit();

    //-------------------------------------------------------------------------
    // operators
    //-------------------------------------------------------------------------

    iterator& operator++();
    iterator operator++(int);
    iterator& operator--();
    iterator operator--(int);

    bool operator==(const iterator& that) const;
    bool operator!=(const iterator& that) const;

private:

    //-------------------------------------------------------------------------
    // typedefs
    //-------------------------------------------------------------------------

    friend class BitField3D;

    //-------------------------------------------------------------------------
    // internal methods
    //-------------------------------------------------------------------------

    bool valid() const;

    //-------------------------------------------------------------------------
    // members
    //-------------------------------------------------------------------------
    
    pointer   m_blocks;  
    size_type m_size;
    size_type m_index;

};

//-----------------------------------------------------------------------------
// BitField3D<T, A>::const_iterator
//-----------------------------------------------------------------------------

template < typename T, typename A = std::allocator<T> >
class BitField3D<T, A>::const_iterator 
{ 

public:

    //-------------------------------------------------------------------------
    // typedefs
    //-------------------------------------------------------------------------

    typedef std::bidirectional_iterator_tag      iterator_category;
    typedef typename BitField3D::size_type       size_type;
    typedef typename BitField3D::difference_type difference_type;
    typedef typename BitField3D::const_pointer   pointer;
    typedef typename BitField3D::const_reference reference;

    //-------------------------------------------------------------------------
    // public interface
    //-------------------------------------------------------------------------

    const_iterator();
    const_iterator(pointer b, size_type s, size_type i = 0);
    // const_iterator(const const_iterator&);
    // ~const_iterator();

    index_type getIndex() const;

    bool isSet() const;

    //-------------------------------------------------------------------------
    // operators
    //-------------------------------------------------------------------------
    
    const_iterator& operator++();
    const_iterator operator++(int);
    const_iterator& operator--();
    const_iterator operator--(int);

    bool operator==(const const_iterator& that) const;
    bool operator!=(const const_iterator& that) const;

private:
    
    //-------------------------------------------------------------------------
    // typedefs
    //-------------------------------------------------------------------------

    friend class BitField3D; 

    //-------------------------------------------------------------------------
    // internal methods
    //-------------------------------------------------------------------------

    bool valid() const;

    //-------------------------------------------------------------------------
    // members
    //-------------------------------------------------------------------------

    pointer   m_blocks;  
    size_type m_size;
    size_type m_index;

};

//-----------------------------------------------------------------------------
// BitField3D<T, A>::block_iterator
//-----------------------------------------------------------------------------

template < typename T, typename A = std::allocator<T> >
class BitField3D<T, A>::block_iterator 
{

public:

    //-------------------------------------------------------------------------
    // typedefs
    //-------------------------------------------------------------------------

    typedef std::bidirectional_iterator_tag      iterator_category;
    typedef typename BitField3D::value_type      value_type;
    typedef typename BitField3D::size_type       size_type;
    typedef typename BitField3D::difference_type difference_type;
    typedef typename BitField3D::pointer         pointer;
    typedef typename BitField3D::reference       reference;

    //-------------------------------------------------------------------------
    // public interface
    //-------------------------------------------------------------------------

    block_iterator();
    block_iterator(pointer b, pointer p, pointer e);
    // block_iterator (const iterator&);
    // ~block_iterator ();
    
    //-------------------------------------------------------------------------
    // operators
    //-------------------------------------------------------------------------

    typename block_iterator::reference operator*() const;
    typename block_iterator::pointer operator->() const;
    // block_iterator& operator=(const iterator&);
    
    block_iterator& operator++();
    block_iterator operator++(int);
    block_iterator& operator--();
    block_iterator operator--(int);

    bool operator==(const block_iterator& that) const;
    bool operator!=(const block_iterator& that) const;
  
private:

    //-------------------------------------------------------------------------
    // typedefs
    //-------------------------------------------------------------------------

    friend class BitField3D;

    //-------------------------------------------------------------------------
    // internal methods
    //-------------------------------------------------------------------------

    bool valid() const;

    //-------------------------------------------------------------------------
    // methods
    //-------------------------------------------------------------------------
    
    pointer m_begin;
    pointer m_ptr;
    pointer m_end;

};

//-----------------------------------------------------------------------------
// BitField3D<T, A>::status_iterator
//-----------------------------------------------------------------------------

template <typename T, typename A> 
template <class FI, bool S>
class BitField3D<T, A>::status_iterator
{

public:
    
    //-------------------------------------------------------------------------
    // typedefs
    //-------------------------------------------------------------------------

    typedef std::forward_iterator_tag            iterator_category;
    typedef typename FI::value_type              value_type;
    typedef typename FI::difference_type         difference_type;
    typedef typename FI::pointer                 pointer;
    typedef typename FI::reference               reference;
    
    typedef typename BitField3D<T, A>::size_type size_type;
    typedef typename BitField3D<T, A>::pointer   block_pointer;
    typedef typename BitField3D<T, A>::reference block_reference;

    //-------------------------------------------------------------------------
    // public interface
    //-------------------------------------------------------------------------
   
    status_iterator();
    status_iterator(block_pointer b, size_type s, size_type i, FI fi);
    // status_iterator(const status_iterator&);
    // ~status_iterator();

    /**
     * Get the linear index of the current position of the iterator in the
     * bitfield.
     */
    index_type getIndex() const; 

    /**
     * Get the i,j,k coordinates of the current position of the iterator in the
     * 3D bitfield.
     */
    void getCoordinates(index_type &i, index_type &j, index_type &k) const;
    void getCoordinates(index_vec &coords) const;

    //-------------------------------------------------------------------------
    // operators
    //-------------------------------------------------------------------------

    typename status_iterator::reference operator*() const;
    typename status_iterator::pointer operator->() const;
    // status_iterator& operator=(const status_iterator&);
    
    status_iterator& operator++();
    status_iterator operator++(int);

    bool operator()() const;

    bool operator==(const status_iterator& that) const;
    bool operator!=(const status_iterator& that) const;
  
private:
    
    //-------------------------------------------------------------------------
    // internal methods
    //-------------------------------------------------------------------------

    bool valid() const;

    //-------------------------------------------------------------------------
    // members
    //-------------------------------------------------------------------------

    block_pointer m_blocks;
    size_type     m_size;
    size_type     m_index;
    FI            m_iter;

};

//-----------------------------------------------------------------------------
// BitField3D<T, A>::set_iterator
//-----------------------------------------------------------------------------

template <typename T, typename A> 
template <class FI>
class BitField3D<T, A>::set_iterator : public status_iterator<FI, 1>
{

public:
    
    //-------------------------------------------------------------------------
    // typedefs
    //-------------------------------------------------------------------------

    typedef typename BitField3D::template status_iterator<FI, 1> iterator;
    typedef typename iterator::size_type                         size_type;
    typedef typename iterator::block_pointer                     block_pointer;

    //-------------------------------------------------------------------------
    // public interface
    //-------------------------------------------------------------------------

    set_iterator() : iterator() {};
    set_iterator(block_pointer b, size_type s, size_type i, FI fi) :
        iterator(b, s, i, fi) {};

};

//-----------------------------------------------------------------------------
// BitField3D<T, A>::unset_iterator
//-----------------------------------------------------------------------------

template <typename T, typename A> 
template <class FI>
class BitField3D<T, A>::unset_iterator : public status_iterator<FI, 0>
{

public:

    //-------------------------------------------------------------------------
    // typedefs
    //-------------------------------------------------------------------------
    
    typedef typename BitField3D::template status_iterator<FI, 0> iterator;
    typedef typename iterator::size_type                         size_type;
    typedef typename iterator::block_pointer                     block_pointer;

    //-------------------------------------------------------------------------
    // public interface
    //-------------------------------------------------------------------------

    unset_iterator() : iterator() {};
    unset_iterator(block_pointer b, size_type s, size_type i, FI fi) :
        iterator(b, s, i, fi) {};

};

//-----------------------------------------------------------------------------
// BitField3D<T, A>::window_iterator
//-----------------------------------------------------------------------------

template < typename T, typename A = std::allocator<T> >
class BitField3D<T, A>::window_iterator 
{ 

public:
    
    //-------------------------------------------------------------------------
    // typedefs
    //-------------------------------------------------------------------------

    typedef std::bidirectional_iterator_tag      iterator_category;
    typedef typename BitField3D::size_type       size_type;
    typedef typename BitField3D::difference_type difference_type;
    typedef typename BitField3D::const_pointer   pointer;
    typedef typename BitField3D::const_reference reference;

    friend class BitField3D; 

    //-------------------------------------------------------------------------
    // public interface
    //-------------------------------------------------------------------------

    window_iterator();
    window_iterator(pointer b, size_type s, size_type ws, 
            size_type i = 0, size_type wi = 0);
    // window_iterator(const window_iterator&);
    // ~window_iterator();

    index_type getIndex() const;

    bool isSet() const;

    //-------------------------------------------------------------------------
    // operators
    //-------------------------------------------------------------------------
        
    window_iterator& operator++();
    window_iterator operator++(int);
    window_iterator& operator--();
    window_iterator operator--(int);

    bool operator()() const;

    bool operator==(const window_iterator& that) const;
    bool operator!=(const window_iterator& that) const;

private:

    //-------------------------------------------------------------------------
    // internal methods
    //-------------------------------------------------------------------------
    
    bool valid() const;

    //-------------------------------------------------------------------------
    // members
    //-------------------------------------------------------------------------

    pointer    m_blocks;  
    
    index_type m_begin;
    index_type m_end;
    index_type m_index;
    index_type m_wnd_index;
    index_type m_wnd_size;

    index_type m_row_shift;
    index_type m_page_shift;
    index_type m_wnd_size_sqrd;

};

END_NKHIVE_NS

//-----------------------------------------------------------------------------
// class implementation
//-----------------------------------------------------------------------------

BEGIN_NKHIVE_NS

#include <nkhive/bitfields/BitField3D.hpp>

END_NKHIVE_NS

//-----------------------------------------------------------------------------

#endif // __NKHIVE_BITFIELD_BITFIELD3D_H__
