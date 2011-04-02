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
// BitField3D.hpp
//------------------------------------------------------------------------------

// no includes allowed

//-----------------------------------------------------------------------------
// class implementation 
//-----------------------------------------------------------------------------

template <typename T, typename A>
inline
BitField3D<T, A>::BitField3D(const allocator_type& a) :
    m_allocator(a),
    m_blocks(NULL),
    m_capacity(0),
    m_lg_size(0)
{
    // nothing to see here
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline
BitField3D<T, A>::BitField3D(size_type lg_size, const allocator_type& a) :
    m_allocator(a),
    m_blocks(NULL),
    m_capacity(0),
    m_lg_size(lg_size)
{
    // figure out how many blocks are needed to represent the bits
    index_type bits         = numBits3D(m_lg_size);
    index_type bit_capacity = roundPow2(bits, bitsof(T));
    m_capacity              = bit_capacity / bitsof(T);

    // allocate memory for the bits
    allocate(&m_blocks, m_capacity);
    clear(m_blocks, m_capacity);
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline
BitField3D<T, A>::BitField3D(const BitField3D& that) :
    m_allocator(that.m_allocator),
    m_blocks(NULL),
    m_capacity(that.m_capacity),
    m_lg_size(that.m_lg_size)
{
    allocate(&m_blocks, m_capacity);
    copy(m_blocks, that.m_blocks, m_capacity);
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline
BitField3D<T, A>::~BitField3D()
{
    deallocate(m_blocks, m_capacity);
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline index_type
BitField3D<T, A>::getIndex(index_type i, index_type j, index_type k) const
{
    index_type index = NKHIVE_NS::getIndex(i, j, k, m_lg_size);
    return index;
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline void
BitField3D<T, A>::getCoordinates(
        index_type index, index_type& i, index_type& j, index_type& k) const
{
    NKHIVE_NS::getCoordinates(index, m_lg_size, i, j, k);
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline index_type
BitField3D<T, A>::getSetIndex(index_type si) const
{
    const_iterator i = begin();
    const_iterator e = end();

    ++si;
    while (i != e) {
        if (i.isSet() && !--si) {
            break;
        }
        ++i;
    }

    assert(!si);

    return i.getIndex();
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline bool 
BitField3D<T, A>::isSet(index_type i) const
{
    index_type block = i / bitsof(T);
    index_type bit   = i % bitsof(T);

    bool is_set = m_blocks[block] & getBitMask(T, bit);
    return is_set;
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline bool 
BitField3D<T, A>::isSet(index_type i, index_type j, index_type k) const
{
    index_type index = getIndex(i, j, k);
    return isSet(index);
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline void 
BitField3D<T, A>::setBit(index_type i)
{
    index_type block = i / bitsof(T);
    index_type bit   = i % bitsof(T);

    m_blocks[block] |= getBitMask(T, bit);
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline void 
BitField3D<T, A>::unsetBit(index_type i)
{
    index_type block = i / bitsof(T);
    index_type bit   = i % bitsof(T);

    m_blocks[block] &= ~getBitMask(T, bit);
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline void 
BitField3D<T, A>::setBit(index_type i, index_type j, index_type k)
{
    index_type index = getIndex(i, j, k);
    setBit(index);
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline void 
BitField3D<T, A>::unsetBit(index_type i, index_type j, index_type k)
{
    index_type index = getIndex(i, j, k);
    unsetBit(index);
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline typename BitField3D<T, A>::iterator 
BitField3D<T, A>::begin()
{
    return iterator(m_blocks, m_lg_size);
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline typename BitField3D<T, A>::const_iterator 
BitField3D<T, A>::begin() const
{
    return const_iterator(m_blocks, m_lg_size);
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline typename BitField3D<T, A>::iterator 
BitField3D<T, A>::end()
{
    return iterator(m_blocks, m_lg_size, numBits3D(size()));
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline typename BitField3D<T, A>::const_iterator 
BitField3D<T, A>::end() const
{
    return const_iterator(m_blocks, m_lg_size, numBits3D(size()));
}

//-----------------------------------------------------------------------------
    
template <typename T, typename A>
template <class FI>
inline typename BitField3D<T, A>::template set_iterator<FI>
BitField3D<T, A>::setIterator(FI iter) const
{
    return set_iterator<FI>(m_blocks, m_lg_size, 0, iter);
}

//-----------------------------------------------------------------------------
    
template <typename T, typename A>
template <class FI>
inline typename BitField3D<T, A>::template unset_iterator<FI>
BitField3D<T, A>::unsetIterator(FI iter) const
{
    return unset_iterator<FI>(m_blocks, m_lg_size, 0, iter);
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline typename BitField3D<T, A>::window_iterator 
BitField3D<T, A>::windowIterator(index_type b, index_type size) const 
{
    return window_iterator(m_blocks, m_lg_size, size, b, 0);
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline bool 
BitField3D<T, A>::isFull() const
{
    index_type s     = numBits3D(size());
    index_type block = s / bitsof(T);
    index_type bit   = s % bitsof(T);

    T filled = getBitMaskFilled(T);

    // check blocks
    for (index_type i = 0; i < block; ++i) {
        if (m_blocks[i] != filled) {
            return false;
        }
    }

    // check bits
    if (bit && (m_blocks[block] != (filled & getBitMaskRange(T, bit)))) {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline bool 
BitField3D<T, A>::isEmpty() const
{
    index_type s     = numBits3D(size());
    index_type block = s / bitsof(T);
    index_type bit   = s % bitsof(T);

    T empty = 0;

    // loop over partial used block as well
    block += !!bit;

    // check blocks
    for (index_type i = 0; i < block; ++i) {
        if (m_blocks[i] != empty) {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline void 
BitField3D<T, A>::fillBits()
{
    index_type s     = numBits3D(size());
    index_type block = s / bitsof(T);
    index_type bit   = s % bitsof(T);

    // fill blocks
    for (index_type i = 0; i < block; ++i) {
        m_blocks[i] = getBitMaskFilled(T);
    }

    // fill bits
    if (bit) {
        m_blocks[block] = getBitMaskFilled(T) & getBitMaskRange(T, bit);
    }
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline void 
BitField3D<T, A>::clearBits()
{
    index_type s     = numBits3D(size());
    index_type block = s / bitsof(T);
    index_type bit   = s % bitsof(T);

    // loop over partial used block as well
    block += !!bit;

    // clear blocks
    for (index_type i = 0; i < block; ++i) {
        m_blocks[i] = 0;
    }
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline void 
BitField3D<T, A>::invertBits()
{
    index_type s     = numBits3D(size());
    index_type block = s / bitsof(T);
    index_type bit   = s % bitsof(T);

    // invert blocks
    for (index_type i = 0; i < block; ++i) {
        m_blocks[i] = ~m_blocks[i];
    }

    // invert bits
    if (bit) {
        m_blocks[block] = ~m_blocks[block] & getBitMaskRange(T, bit);
    }
}

//------------------------------------------------------------------------------

template <typename T, typename A>
bool
BitField3D<T, A>::isSingleBitSet(index_type i) const 
{
    index_type block = i / bitsof(T);
    index_type bit   = i % bitsof(T);

    // Early check to see if this bit is the only one set in the block.
    if (m_blocks[block] != getBitMask(T, bit)) return false;

    // Check all other blocks for zero.
    for (index_type i = 0; i < m_capacity; ++i) {
        if (i == block) continue;
        if (m_blocks[i] != 0) return false;
    }

    return true;
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
index_type
BitField3D<T, A>::count() const
{
    return countRange(numBits3D(size()));
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
index_type
BitField3D<T, A>::countRange(index_type i) const 
{
    assert(i <= numBits3D(size()));

    index_type block = i / bitsof(T);
    index_type bit   = i % bitsof(T);
    index_type count = 0;

    // count blocks
    for (i = 0; i < block; ++i) {
        count += getBitCount<T>(m_blocks[i]);
    }

    // count partial block
    if (bit) {
        count += getBitCount<T>(m_blocks[block] & getBitMaskRange(T, bit));
    }

    return count;
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline typename BitField3D<T, A>::size_type 
BitField3D<T, A>::size() const
{
    return m_lg_size;
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline typename BitField3D<T, A>::size_type 
BitField3D<T, A>::capacity() const
{
    size_type capacity = m_capacity * bitsof(T);
    return capacity;
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline void 
BitField3D<T, A>::resize(size_type s)
{
    // is resize possible with current memory allocation
    index_type bits = numBits3D(s);
    bool realloc    = capacity() < bits;

    // allocate new bitfield
    if (realloc) {

        BitField3D that(s, m_allocator);

        index_type index, i, j, k;
        iterator iter = begin();
        iterator e    = end();

        // copy across contents
        for (; iter != e; ++iter) {
           index = iter.getIndex();
           getCoordinates(index, i, j, k);
           if (iter.isSet()) {
               that.setBit(i, j, k);
           }
        }

        // swap out the old with the new
        swap(that);

    // use existing memory, line up data correctly in new dimensions
    } else {

        // expand bitfield size
        if (m_lg_size < s) {
           
            // loop over smaller cube
            index_type index, i, j, k;
            index_type bit = numBits3D(m_lg_size) - 1;
            do {

                // get index into old bitifeld
                getCoordinates(bit, i, j, k);

                // update bit status
                bool is_set = isSet(i, j, k);
                unsetBit(i, j, k);
                if (is_set) {
                    index = NKHIVE_NS::getIndex(i, j, k, s);
                    setBit(index);
                }

            } while (bit-- != 0);

        // shrink bitfield size
        } else if (m_lg_size > s) {

            // loop over smaller cube
            index_type index, i, j, k;
            index_type bits  = numBits3D(s);
            for (index = 0; index < bits; ++index) {

                // get index into new bitifeld
                NKHIVE_NS::getCoordinates(index, s, i, j, k);

                // update bit status
                if (isSet(i, j, k)) {
                    setBit(index);
                } else {
                    unsetBit(index);
                }
            }

            index_type block = bits / bitsof(T);
            index_type bit   = bits % bitsof(T);

            // clean up remaining bits in block
            if (bit) {
                m_blocks[block] &= getBitMaskRange(T, bit);
                ++block;
            } 

            // clean up remaining blocks
            index_type extra = m_capacity - block; 
            if (extra) {
                clear(m_blocks + block, extra);
            }
        }

        // save new size
        m_lg_size = s;
    }
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline void
BitField3D<T, A>::swap(BitField3D& that)
{
    std::swap(m_allocator, that.m_allocator);
    std::swap(m_blocks, that.m_blocks);
    std::swap(m_capacity, that.m_capacity);
    std::swap(m_lg_size, that.m_lg_size);
}

//-----------------------------------------------------------------------------
    
template <typename T, typename A>
void
BitField3D<T, A>::read(std::istream &is) 
{
    // delete existing data
    if (m_blocks) {
        deallocate(m_blocks, m_capacity);
    }

    // read in size
    is.read((char*)&m_lg_size, sizeof(size_type));

    // figure out how many blocks are needed to represent the bits
    index_type bits         = numBits3D(m_lg_size);
    index_type bit_capacity = roundPow2(bits, bitsof(T));
    m_capacity              = bit_capacity / bitsof(T);

    index_type block = bits / bitsof(T);
    index_type bit   = bits % bitsof(T);

    // compensate for partially filled blocks
    if (bit) { ++block; }

    // allocate space
    allocate(&m_blocks, m_capacity);
        
    // read in bitfield
    is.read(reinterpret_cast<char*>(m_blocks), block * sizeof(T));
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
void
BitField3D<T, A>::read(HDF5Id parent_id)
{
    // delete existing data
    if (m_blocks) {
        deallocate(m_blocks, m_capacity);
    }

    // read in size
    HDF5DataSet bitfield_data_set;
    bitfield_data_set.open(parent_id, kBitFieldName);

    readScalarAttribute(bitfield_data_set.id(),
                        kBitFieldSize, 
                        TypeToHDF5Type<BOOST_TYPEOF(m_lg_size)>::type(),
                        &m_lg_size);

    // figure out how many blocks are needed to represent the bits
    index_type bits         = numBits3D(m_lg_size);
    index_type bit_capacity = roundPow2(bits, bitsof(T));
    m_capacity              = bit_capacity / bitsof(T);

    index_type block = bits / bitsof(T);
    index_type bit   = bits % bitsof(T);

    // compensate for partially filled blocks
    if (bit) { ++block; }

    // allocate space
    allocate(&m_blocks, m_capacity);

    // clear out bits
    clear(m_blocks, m_capacity);
    
    // read in bitfield
    readSimpleDataSet(parent_id, kBitFieldName, H5T_NATIVE_CHAR, m_blocks);
} 

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline void
BitField3D<T, A>::write(std::ostream &os) const 
{
    index_type s     = numBits3D(size());
    index_type block = s / bitsof(T);
    index_type bit   = s % bitsof(T);

    // compensate for partially filled blocks
    if (bit) { ++block; }
        
    // write out size
    os.write((char*)&m_lg_size, sizeof(size_type));

    // write out bitfield
    os.write(reinterpret_cast<char*>(m_blocks), block * sizeof(T));
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline void
BitField3D<T, A>::write(HDF5Id parent_id) const 
{
    index_type s     = numBits3D(size());
    index_type block = s / bitsof(T);
    index_type bit   = s % bitsof(T);

    // compensate for partially filled blocks
    if (bit) { ++block; }

    // write the bitfield as a dataset  
    HDF5Size data_size = block * sizeof(T);
    writeSimpleDataSet(parent_id, 
                       kBitFieldName, 
                       1, &data_size, 
                       H5T_NATIVE_CHAR, 
                       m_blocks);

    // write size as attribute on dataset
    // kind of annoying that I have to open the data set again.
    // TODO: do this without opening the data set twice
    HDF5DataSet bitfield_data_set;
    bitfield_data_set.open(parent_id, kBitFieldName);

    writeScalarAttribute(bitfield_data_set.id(),
                         kBitFieldSize, 
                         TypeToHDF5Type<BOOST_TYPEOF(m_lg_size)>::type(),
                         &m_lg_size);
} 

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline std::string
BitField3D<T, A>::toString()
{
    std::string str = "";

    // nothing to output if size is 0
    if (size()) {

        index_type s     = numBits3D(size());
        index_type block = s / bitsof(T);
        index_type bit   = s % bitsof(T);

        // loop over blocks first
        for (index_type i = 0; i < block; ++i) {
            T mask = getBitMask(T, 0);
            while (mask) {
                str.push_back(!!(m_blocks[i] & mask) + 48);
                mask >>= 1;
            }
        }

        // loop over the remaining bits
        T mask = getBitMask(T, 0);
        while (mask > getBitMask(T, bit)) {
            str.push_back(!!(m_blocks[block] & mask) + 48);
            mask >>= 1;
        }
    }

    return str;
}

//------------------------------------------------------------------------------

template <typename T, typename A>
inline BitField3D<T, A>&
BitField3D<T, A>::operator=(const BitField3D<T, A>& that)
{
    // deallocate first and then copy.
    if (m_blocks) {
        deallocate(m_blocks, m_capacity);
    }

    m_allocator = that.m_allocator;
    m_blocks    = NULL;
    m_capacity  = that.m_capacity;
    m_lg_size   = that.m_lg_size;

    allocate(&m_blocks, m_capacity);
    copy(m_blocks, that.m_blocks, m_capacity);

    return *this;
}

//-----------------------------------------------------------------------------
    
template <typename T, typename A>
inline bool
BitField3D<T, A>::operator==(const BitField3D<T, A>& that) const
{
    // early out
    if (size() != that.size()) {
        return false;
    }

    index_type s     = numBits3D(size());
    index_type block = s / bitsof(T);
    index_type bit   = s % bitsof(T);

    // compensate for partial blocks 
    if (bit) { ++block; }

    // check blocks
    return std::equal(m_blocks, m_blocks + block, that.m_blocks);
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline bool
BitField3D<T, A>::operator!=(const BitField3D<T, A>& that) const
{
    return !(*this == that);
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline std::istream& 
operator<<(std::istream& is, const BitField3D<T, A>& bitfield)
{
    bitfield.read(is);
    return is;
}

//-----------------------------------------------------------------------------
             
template <typename T, typename A>
inline std::ostream& 
operator<<(std::ostream& os, const BitField3D<T, A>& bitfield)
{
    bitfield.write(os);
    return os;
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline int
BitField3D<T, A>::sizeOf() const
{
    int sizeof_this   = sizeof(*this);
    int sizeof_blocks = sizeof(T) * m_capacity;
    return sizeof_this + sizeof_blocks;
}

//-----------------------------------------------------------------------------
             
#if 0
template <typename T, typename A>
inline std::ostream& 
operator<<(std::stream& os, const BitField3D<T, A>& bitfield) const
{
    os << "blocks: " << bitfield.m_blocks << std::endl;
    os << "capacity : " << bitfield.capacity() << std::endl;
    os << "size : 2^" << bitfield.size() << "^3" << std::endl;
    os << "data : " << bitfield.toString() << std::endl;

    return os;
}
#endif

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline bool
BitField3D<T, A>::valid() const
{
    bool is_valid = !m_blocks || (NKHIVE_NS::numBits3D(size()) <= capacity());
    return is_valid;
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline typename BitField3D<T, A>::block_iterator 
BitField3D<T, A>::blockBegin()
{
    return block_iterator(m_blocks, m_blocks, m_blocks + m_capacity);
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline typename BitField3D<T, A>::block_iterator 
BitField3D<T, A>::blockEnd()
{
    return block_iterator(m_blocks, m_blocks + m_capacity, m_blocks + m_capacity);
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline void 
BitField3D<T, A>::copy(pointer dst, const_pointer src, size_type s)
{
   assert(src);
   memcpy(dst, src, s * sizeof(T));
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline void 
BitField3D<T, A>::clear(pointer p, size_type s)
{
    assert(p);
    memset(p, 0, s * sizeof(T));
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline void 
BitField3D<T, A>::allocate(pointer* p, size_type s)
{
    *p = m_allocator.allocate(s);
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline void 
BitField3D<T, A>::deallocate(pointer p, size_type s)
{
    m_allocator.deallocate(p, s);
}

//-----------------------------------------------------------------------------
// BitField3D<T, A>::iterator
//-----------------------------------------------------------------------------

template <typename T, typename A>
inline
BitField3D<T, A>::iterator::iterator() :
    m_blocks(NULL),
    m_size(0),
    m_index(0)
{
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline
BitField3D<T, A>::iterator::iterator(pointer b, size_type s, size_type i) :
    m_blocks(b),
    m_size(s),
    m_index(i)
{
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline bool 
BitField3D<T, A>::iterator::valid() const
{
    bool is_valid = (!m_blocks && !m_size && !m_index) || 
                    (m_index <= numBits3D(m_size));
    return is_valid;
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
index_type
BitField3D<T, A>::iterator::getIndex() const
{
    return m_index;
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
bool 
BitField3D<T, A>::iterator::isSet() const 
{
    index_type block = m_index / bitsof(T);
    index_type bit   = m_index % bitsof(T);

    bool is_set = m_blocks[block] & getBitMask(T, bit);
    return is_set;
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
void 
BitField3D<T, A>::iterator::setBit() 
{
    index_type block = m_index / bitsof(T);
    index_type bit   = m_index % bitsof(T);

    m_blocks[block] |= getBitMask(T, bit);
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
void 
BitField3D<T, A>::iterator::unsetBit() 
{
    index_type block = m_index / bitsof(T);
    index_type bit   = m_index % bitsof(T);

    m_blocks[block] &= ~getBitMask(T, bit);
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline typename BitField3D<T, A>::iterator& 
BitField3D<T, A>::iterator::operator++()
{
    ++m_index;
    assert(valid());
    return *this;
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline typename BitField3D<T, A>::iterator 
BitField3D<T, A>::iterator::operator++(int)
{
    iterator i = *this;
    ++(*this);
    assert(valid());
    return i;
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline typename BitField3D<T, A>::iterator& 
BitField3D<T, A>::iterator::operator--()
{
    --m_index;
    assert(valid());
    return *this;
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline typename BitField3D<T, A>::iterator 
BitField3D<T, A>::iterator::operator--(int)
{
    iterator i = *this;
    --(*this);
    assert(valid());
    return i;
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline bool 
BitField3D<T, A>::iterator::operator==(const iterator& that) const
{
    bool is_equal = (m_blocks == that.m_blocks) &&
                    (m_size   == that.m_size)   &&
                    (m_index  == that.m_index);
    return is_equal;
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline bool 
BitField3D<T, A>::iterator::operator!=(const iterator& that) const
{
    return !(*this == that);
}

//-----------------------------------------------------------------------------
// BitField3D<T, A>::const_iterator
//-----------------------------------------------------------------------------

template <typename T, typename A>
inline
BitField3D<T, A>::const_iterator::const_iterator() :
    m_blocks(NULL),
    m_size(0),
    m_index(0)
{
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline
BitField3D<T, A>::const_iterator::const_iterator(pointer b, size_type s, size_type i) :
    m_blocks(b),
    m_size(s),
    m_index(i)
{
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline bool 
BitField3D<T, A>::const_iterator::valid() const
{
    bool is_valid = (!m_blocks && !m_size && !m_index) || 
                    (m_index <= numBits3D(m_size));
    return is_valid;
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
index_type
BitField3D<T, A>::const_iterator::getIndex() const
{
    return m_index;
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
bool 
BitField3D<T, A>::const_iterator::isSet() const 
{
    index_type block = m_index / bitsof(T);
    index_type bit   = m_index % bitsof(T);

    bool is_set = m_blocks[block] & getBitMask(T, bit);
    return is_set;
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline typename BitField3D<T, A>::const_iterator& 
BitField3D<T, A>::const_iterator::operator++()
{
    ++m_index;
    assert(valid());
    return *this;
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline typename BitField3D<T, A>::const_iterator 
BitField3D<T, A>::const_iterator::operator++(int)
{
    const_iterator i = *this;
    ++(*this);
    assert(valid());
    return i;
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline typename BitField3D<T, A>::const_iterator& 
BitField3D<T, A>::const_iterator::operator--()
{
    --m_index;
    assert(valid());
    return *this;
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline typename BitField3D<T, A>::const_iterator 
BitField3D<T, A>::const_iterator::operator--(int)
{
    const_iterator i = *this;
    --(*this);
    assert(valid());
    return i;
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline bool 
BitField3D<T, A>::const_iterator::operator==(const const_iterator& that) const
{
    bool is_equal = (m_blocks == that.m_blocks) &&
                    (m_size   == that.m_size)   &&
                    (m_index  == that.m_index);
    return is_equal;
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline bool 
BitField3D<T, A>::const_iterator::operator!=(const const_iterator& that) const
{
    return !(*this == that);
}

//-----------------------------------------------------------------------------
// BitField3D<T, A>::block_iterator
//-----------------------------------------------------------------------------

template <typename T, typename A>
inline
BitField3D<T, A>::block_iterator::block_iterator () :
    m_begin(NULL),
    m_ptr(NULL),
    m_end(NULL)
{
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline
BitField3D<T, A>::block_iterator::block_iterator(pointer b, pointer p, pointer e) :
    m_begin(b),
    m_ptr(p),
    m_end(e)
{
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline bool 
BitField3D<T, A>::block_iterator::valid() const
{
    bool is_valid = (!m_begin && !m_ptr && !m_end) || 
                    ((m_begin <= m_ptr) && (m_ptr <= m_end));
    return is_valid;
}

//-----------------------------------------------------------------------------
                
template <typename T, typename A>
inline typename BitField3D<T, A>::block_iterator::reference 
BitField3D<T, A>::block_iterator::operator*() const
{
    return *m_ptr;
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline typename BitField3D<T, A>::block_iterator::pointer 
BitField3D<T, A>::block_iterator::operator->() const
{
    return &**this;
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline typename BitField3D<T, A>::block_iterator& 
BitField3D<T, A>::block_iterator::operator++()
{
    ++m_ptr;
    assert(valid());
    return *this;
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline typename BitField3D<T, A>::block_iterator 
BitField3D<T, A>::block_iterator::operator++(int)
{
    block_iterator i = *this;
    ++(*this);
    assert(valid());
    return i;
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline typename BitField3D<T, A>::block_iterator& 
BitField3D<T, A>::block_iterator::operator--()
{
    --m_ptr;
    assert(valid());
    return *this;
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline typename BitField3D<T, A>::block_iterator 
BitField3D<T, A>::block_iterator::operator--(int)
{
    block_iterator i = *this;
    --(*this);
    assert(valid());
    return i;
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline bool 
BitField3D<T, A>::block_iterator::operator==(const block_iterator& that) const
{
    bool is_equal = (m_ptr == that.m_ptr);
    return is_equal;
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline bool 
BitField3D<T, A>::block_iterator::operator!=(const block_iterator& that) const
{
    return !(*this == that);
}

//-----------------------------------------------------------------------------
// BitField3D<T, A>::status_iterator
//-----------------------------------------------------------------------------

template <typename T, typename A>
template <typename FI, bool S>
inline
BitField3D<T, A>::status_iterator<FI, S>::status_iterator() :
    m_blocks(NULL),
    m_size(0),
    m_index(0),
    m_iter()
{
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
template <typename FI, bool S>
inline
BitField3D<T, A>::status_iterator<FI, S>::status_iterator(block_pointer b, 
        size_type s, size_type i, FI fi) :
    m_blocks(b),
    m_size(s),
    m_index(i),
    m_iter(fi)
{
    // increment to first bit w/ S set
    if ((*this)()) {
        index_type block = m_index / bitsof(T);
        index_type bit   = m_index % bitsof(T);
        if (!(m_blocks[block] & getBitMask(T, bit)) == S) {
            ++(*this);
        }
    }
}

//------------------------------------------------------------------------------

template <typename T, typename A>
template <typename FI, bool S>
inline index_type
BitField3D<T, A>::status_iterator<FI, S>::getIndex() const
{
    return m_index;
}

//------------------------------------------------------------------------------

template <typename T, typename A>
template <typename FI, bool S>
inline void
BitField3D<T, A>::status_iterator<FI, S>::getCoordinates(index_type &i, 
                                                         index_type &j,
                                                         index_type &k) const
{
    NKHIVE_NS::getCoordinates(m_index, m_size, i, j, k);
}

//------------------------------------------------------------------------------

template <typename T, typename A>
template <typename FI, bool S>
inline void
BitField3D<T, A>::status_iterator<FI, S>::getCoordinates(
        index_vec &coords) const
{
    getCoordinates(coords[0], coords[1], coords[2]);
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
template <typename FI, bool S>
inline bool 
BitField3D<T, A>::status_iterator<FI, S>::valid() const
{
    bool is_valid = (!m_blocks && !m_size && !m_index) || 
                    (m_index <= numBits3D(m_size));
    return is_valid;
}

//-----------------------------------------------------------------------------
                
template <typename T, typename A>
template <typename FI, bool S>
inline typename BitField3D<T, A>::template status_iterator<FI, S>::reference 
BitField3D<T, A>::status_iterator<FI, S>::operator*() const
{
    return *m_iter;
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
template <typename FI, bool S>
inline typename BitField3D<T, A>::template status_iterator<FI, S>::pointer 
BitField3D<T, A>::status_iterator<FI, S>::operator->() const
{
    return &**this;
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
template <typename FI, bool S>
inline typename BitField3D<T, A>::template status_iterator<FI, S>& 
BitField3D<T, A>::status_iterator<FI, S>::operator++()
{
    //  this can be sped up by checking for empty blocks
    //  might be good to specialize this function for 0 and 1

    index_type reset;
    index_type block = m_index / bitsof(T);
    index_type bit   = m_index % bitsof(T);

    // find next bit with status S
    do {
        ++m_index;
        ++m_iter;

        // keep track of block/bits for quicker checks
        reset = ++bit % bitsof(T);
        block = reset ? block : block + 1;
        bit   = reset ? bit : 0;

    } while ((*this)() && (!(m_blocks[block] & getBitMask(T, bit)) == S));

    assert(valid());
    return *this;
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
template <typename FI, bool S>
inline typename BitField3D<T, A>::template status_iterator<FI, S>
BitField3D<T, A>::status_iterator<FI, S>::operator++(int)
{
    status_iterator i = *this;
    ++(*this);
    assert(valid());
    return i;
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
template <typename FI, bool S>
inline bool 
BitField3D<T, A>::status_iterator<FI, S>::operator()() const
{
    return (m_index < numBits3D(m_size));
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
template <typename FI, bool S>
inline bool 
BitField3D<T, A>::status_iterator<FI, S>::operator==(const status_iterator& that) const
{
    bool is_equal = (m_blocks == that.m_blocks) &&
                    (m_size   == that.m_size)   &&
                    (m_index  == that.m_index)  &&
                    (m_iter   == that.m_iter);
    return is_equal;
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
template <typename FI, bool S>
inline bool 
BitField3D<T, A>::status_iterator<FI, S>::operator!=(const status_iterator& that) const
{
    return !(*this == that);
}

//-----------------------------------------------------------------------------
// BitField3D<T, A>::window_iterator
//-----------------------------------------------------------------------------

template <typename T, typename A>
inline
BitField3D<T, A>::window_iterator::window_iterator() :
    m_blocks(NULL),
    m_begin(0),
    m_end(0),
    m_index(0),
    m_wnd_index(0),
    m_wnd_size(0)
{
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline
BitField3D<T, A>::window_iterator::window_iterator(
        pointer b, size_type s, size_type ws, size_type i, size_type wi) :
    m_blocks(b)
{    
    // cache frequently referenced sizes
    index_type size = 1 << s;
    m_wnd_size      = ws;
    m_wnd_size_sqrd = m_wnd_size * m_wnd_size;

    // cache distance to shift between boundries
    m_row_shift  = size - m_wnd_size;
    m_page_shift = m_row_shift * size;

    // cache indices
    m_begin     = i;
    m_wnd_index = wi;

    // compensate for window index
    m_index = m_begin;
    m_index += (wi % m_wnd_size);
    m_index += (wi / m_wnd_size) * size;
    m_index += (wi / m_wnd_size_sqrd) * (size * size);

    // calculating the end index 
    //  1) start at the initial position
    //  2) move diagonally window size times
    //  3) go to next row/page boundry
    m_end = m_begin
            + ((((size * size) + size + 1) * (m_wnd_size - 1)) + 1) 
            + (m_row_shift + m_page_shift);
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline bool 
BitField3D<T, A>::window_iterator::valid() const
{
    bool is_valid = (!m_blocks && !m_begin && !m_end &&
                     !m_wnd_size && !m_wnd_index) || 
                    ((m_begin <= m_end) &&
                     (m_wnd_index <= numBits3D(m_wnd_size)));
    return is_valid;
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline index_type
BitField3D<T, A>::window_iterator::getIndex() const
{
    return m_index;
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline bool 
BitField3D<T, A>::window_iterator::isSet() const 
{
    index_type block = m_index / bitsof(T);
    index_type bit   = m_index % bitsof(T);

    bool is_set = m_blocks[block] & getBitMask(T, bit);
    return is_set;
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline typename BitField3D<T, A>::window_iterator& 
BitField3D<T, A>::window_iterator::operator++()
{
    // increment both window and bitfield index
    ++m_index; 
    ++m_wnd_index;
    
    // compensate for row and page edges 
    m_index += (m_wnd_index % m_wnd_size) == 0 ? m_row_shift : 0;
    m_index += (m_wnd_index % m_wnd_size_sqrd) == 0 ? m_page_shift : 0;

    return *this;
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline typename BitField3D<T, A>::window_iterator 
BitField3D<T, A>::window_iterator::operator++(int)
{
    window_iterator i = *this;
    ++(*this);
    assert(valid());
    return i;
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline typename BitField3D<T, A>::window_iterator& 
BitField3D<T, A>::window_iterator::operator--()
{
    // increment both window and bitfield index
    --m_index; 
    --m_wnd_index;
    
    // compensate for row and page edges 
    m_index -= (m_wnd_index % m_wnd_size) == 0 ? m_row_shift : 0;
    m_index -= (m_wnd_index % m_wnd_size_sqrd) == 0 ? m_page_shift : 0;

    assert(valid());
    return *this;
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline typename BitField3D<T, A>::window_iterator 
BitField3D<T, A>::window_iterator::operator--(int)
{
    window_iterator i = *this;
    --(*this);
    assert(valid());
    return i;
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline bool 
BitField3D<T, A>::window_iterator::operator()() const
{
    return (m_index != m_end);
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline bool 
BitField3D<T, A>::window_iterator::operator==(const window_iterator& that) const
{
    bool is_equal = (m_blocks    == that.m_blocks)   &&
                    (m_begin     == that.m_begin)    &&
                    (m_index     == that.m_index)    &&
                    (m_wnd_size  == that.m_wnd_size) &&
                    (m_wnd_index == that.m_wnd_index);
    return is_equal;
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline bool 
BitField3D<T, A>::window_iterator::operator!=(const window_iterator& that) const
{
    return !(*this == that);
}

