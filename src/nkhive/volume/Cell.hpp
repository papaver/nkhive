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
// Cell.hpp
//------------------------------------------------------------------------------

// no includes allowed

//-----------------------------------------------------------------------------
// class implementation 
//-----------------------------------------------------------------------------

template < typename T, typename A>
inline
Cell<T, A>::Cell() :
    m_allocator(),
    m_data(0),
    m_data_size(0),
    m_default_value(),
    m_fill_value(),
    m_bitfield(),
    m_flags(0)
{
}

//-----------------------------------------------------------------------------

template < typename T, typename A>
inline
Cell<T, A>::Cell(u8 lg_dim_size,  const_reference v,
                 const allocator_type& a) :
    m_allocator(a),
    m_fill_value(v),
    m_bitfield(lg_dim_size, bitfield_alloc()),
    m_flags(0)
{
    // Do not allocate data until the first set.
    m_data = NULL;
    m_data_size = 0;

    // Set the default value.
    setDefaultValue(v);

    // Set it to be a filled cell state and let the bitfield determine which
    // voxels are set.
    setFlag(CELL_FLAG_FILLED);
}

//------------------------------------------------------------------------------

template < typename T, typename A>
inline
Cell<T, A>::Cell(u8 lg_dim_size,  const_reference default_value,
                 const_reference fill_value, const allocator_type& a) :
    m_allocator(a),
    m_bitfield(lg_dim_size, bitfield_alloc()),
    m_flags(0)
{
    // Do not allocate data until the first set.
    m_data = NULL;
    m_data_size = 0;

    // Set the default value.
    setDefaultValue(default_value);

    // Set it to be a filled cell state and let the bitfield determine which
    // voxels are set.
    setFlag(CELL_FLAG_FILLED);

    // Declaring as a filled cell, then set a fill value and fill all the
    // set bits.
    setFillValue(fill_value);
    m_bitfield.fillBits();
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline
Cell<T, A>::Cell(const Cell& other)
{
    copy(other);
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline
Cell<T, A>::~Cell()
{
    destruct();
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline typename Cell<T, A>::reference
Cell<T, A>::get(size_type i, size_type j, size_type k)
{
    size_type index = m_bitfield.getIndex(i, j, k);
    return get(index);
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline typename Cell<T, A>::const_reference
Cell<T, A>::get(size_type i, size_type j, size_type k) const
{
    return const_cast<Cell&>(*this).get(i, j, k);
}

//------------------------------------------------------------------------------

template <typename T, typename A>
inline typename Cell<T, A>::reference
Cell<T, A>::get(size_type index)
{
    if (!m_bitfield.isSet(index)) {
        return getDefaultValue();
    } else if (isFilled()) {
        // the bit is set and is a filled node.
        return getFillValue();
    } else {
        if (isCompressed()) {
            // get the number of set bits up to and including this index 
            // subtract 1 to get the index  
            size_type compressed_index = m_bitfield.countRange(index + 1) - 1;
            return m_data[compressed_index];
        } else {
            return m_data[index];
        }
    }
}

//------------------------------------------------------------------------------

template<typename T, typename A>
inline typename Cell<T, A>::const_reference
Cell<T, A>::get(size_type index) const
{
    return const_cast<Cell&>(*this).get(index);
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline void 
Cell<T, A>::unsetBlock(const vec3ui &min, size_type window_size)
{
    // get the min raw bit index
    size_type min_index = m_bitfield.getIndex(min.x, min.y, min.z);
    
    // create a window iterator on the bitfield
    bitfield_type::window_iterator window_iter = 
        m_bitfield.windowIterator(min_index, window_size);

    // iterate through and unset each set index
    for ( ; window_iter(); ++window_iter) {
        // might be faster to avoid this branch
        if (window_iter.isSet()) {
            size_type index = window_iter.getIndex();
            // unset by index
            unset(index);
        }
    }    
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline void 
Cell<T, A>::setBlock(const vec3ui &min, size_type window_size, 
                    const_reference value)
{
    // get the min raw bit index
    size_type min_index = m_bitfield.getIndex(min.x, min.y, min.z);

    // If the extents of the set cover the entire cell, then just
    // deallocate and fill the cell with a single value using fill(value).
    if (min_index == 0 && window_size == getDimension()) {
        fill(value);
    } else {
        // create a window iterator on the bitfield
        bitfield_type::window_iterator window_iter = 
            m_bitfield.windowIterator(min_index, window_size);

        // iterate through and set each index
        for ( ; window_iter(); ++window_iter) {
            size_type index = window_iter.getIndex();
            // set by index
            set(index, value);
        }    
    }
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
template <typename U, template <typename> class Source>
inline void 
Cell<T, A>::stamp(const Stamp<U, Source> &stamp, 
                  signed_index_bounds stamp_bounds, 
                  const index_bounds &cell_bounds, 
                  const signed_index_vec &transform) 
{
    index_type ci, cj, ck;
    signed_index_type si, sj, sk;

    // compensate stamp's minima/maxima, since starting at cell's minima
    if (transform.x < 0) {
        std::swap(--stamp_bounds.min().x, --stamp_bounds.max().x);
    }
    if (transform.y < 0) {
        std::swap(--stamp_bounds.min().y, --stamp_bounds.max().y);
    }
    if (transform.z < 0) {
        std::swap(--stamp_bounds.min().z, --stamp_bounds.max().z);
    }

    // loop over cell to ensure cache coherency in cell queries
    ck = cell_bounds.min().z; sk = stamp_bounds.min().z;
    for ( ; ck < cell_bounds.max().z; ++ck, sk += transform.z) {
        cj = cell_bounds.min().y; sj = stamp_bounds.min().y;
        for ( ; cj < cell_bounds.max().y; ++cj, sj += transform.y) {
            ci = cell_bounds.min().x; si = stamp_bounds.min().x;
            for ( ; ci < cell_bounds.max().x; ++ci, si += transform.x) {
                U value = stamp.get(si, sj, sk);
                set(ci, cj, ck, T(value));
            }
        }
    }
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline typename Cell<T, A>::size_type
Cell<T, A>::getDimension() const
{
    return (1 << m_bitfield.size());
}

//------------------------------------------------------------------------------

template<typename T, typename A>
inline void
Cell<T, A>::computeSetBounds(index_bounds &bounds) const
{
    // We should never have empty cells.
    assert(!isEmpty());

    if (m_bitfield.isFull()) {
        bounds = index_bounds(0, getDimension());
    } else {
        // Set min to greatest value, max to smallest value.
        bounds = index_bounds(getDimension(), 0);

        const_set_iterator iter = m_bitfield.setIterator(begin());
        for ( ; iter(); ++iter) {
            index_type i, j, k;
            iter.getCoordinates(i, j, k);
            bounds.updateExtrema(i, j, k);
        }

        // Increment the max bounds by one to account for the inclusive max set
        // bounds tests. 
        bounds.translateMax(vec3i(1));
    }
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline bool
Cell<T, A>::isCompressed() const
{
    return isFlagSet(CELL_FLAG_COMPRESSED);
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline bool
Cell<T, A>::isFilled() const
{
    return isFlagSet(CELL_FLAG_FILLED);
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline bool
Cell<T, A>::isSet(size_type i, size_type j, size_type k) const
{
    return m_bitfield.isSet(i, j, k);
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline bool
Cell<T, A>::isEmpty() const
{
    return m_bitfield.isEmpty();
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline typename Cell<T, A>::iterator 
Cell<T, A>::begin()
{
    assert(!isFilled());
    return iterator(m_data, m_data + m_data_size, m_data);
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline typename Cell<T, A>::iterator 
Cell<T, A>::end()
{
    return iterator(m_data, m_data + m_data_size, m_data + m_data_size);
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline typename Cell<T, A>::const_iterator 
Cell<T, A>::begin() const
{
    return const_iterator(this, m_bitfield.begin(), m_data);
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline typename Cell<T, A>::const_iterator 
Cell<T, A>::end() const
{
    if (isFilled()) {
        // HACK: Since we have a filled cell we can pass in the largest
        // available pointer value for the end condition.
        return const_iterator(this, m_bitfield.end(), m_data - 1);
    } 
    // else
    return const_iterator(this, m_bitfield.end(), m_data + m_data_size);
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline void 
Cell<T, A>::set(size_type i, size_type j, size_type k, const_reference value)
{
    size_type index = m_bitfield.getIndex(i, j, k);
    set(index, value);
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline void 
Cell<T, A>::set(size_type index, const_reference value)
{
    if (isCompressed()) {
        // can't modify a compressed cell
        throw Iex::LogicExc("Can't modify a compressed cell");
    }
    
    // If this is the first set, then set as a fill node.
    if (m_bitfield.isEmpty()) {
        destruct();
        setFlag(CELL_FLAG_FILLED);
        m_bitfield.setBit(index);
        setFillValue(value);
        return;
    }

    if (isFilled()) {
        // if trying to set with fill val then just set another set bit.
        const_reference fill_value = getFillValue();
        if (fill_value == value) {
            m_bitfield.setBit(index);
            return;
        }

        // If we are setting the same and only value again in teh bit field,
        // then just update the fill value.
        if (m_bitfield.isSingleBitSet(index)) {
            setFillValue(value);
            return;
        }

        // clear the filled flag
        unsetFlag(CELL_FLAG_FILLED);

        // allocate the full cell and set desired value
        m_data_size = numBits3D(m_bitfield.size());
        m_data = m_allocator.allocate(m_data_size);
            
        // construct objects
        initializeSet(fill_value);
    }

    // set the bit in the bit field
    m_bitfield.setBit(index);
    
    // set the value in the data block  
    m_data[index] = value;
}

//------------------------------------------------------------------------------

template <typename T, typename A>
template <typename BinaryOp>
inline void
Cell<T, A>::update(size_type i, size_type j, size_type k,
                   const_reference value, BinaryOp op)
{
    size_type index = m_bitfield.getIndex(i, j, k);
    update<BinaryOp>(index, value, op);
}

//------------------------------------------------------------------------------

template <typename T, typename A>
template <typename BinaryOp>
inline void
Cell<T, A>::update(size_type index, const_reference value, BinaryOp op)
{
    // Update the value first in a temporary variable.
    value_type val = get(index);
    val = op(val, value);

    // Now call set on that value.
    set(index, val);
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline void 
Cell<T, A>::unset(size_type i, size_type j, size_type k)
{
    size_type index = m_bitfield.getIndex(i, j, k);
    unset(index);
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline void 
Cell<T, A>::unset(size_type index)
{
    if (isCompressed()) {
        // can't modify a compressed cell
        throw new Iex::LogicExc("Can't modify a compressed cell");
    }
     
    // unset the bit in the bit field
    m_bitfield.unsetBit(index);

    // unset the value in the data block only if not a filled cell.
    if (!isFilled()) {
        m_data[index] = getDefaultValue();
    }
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline void 
Cell<T, A>::compress()
{
    if (!isCompressed() && !isFilled()) {

        size_type num_set_bits = m_bitfield.count();
 
        // allocate compressed data
        pointer compressed_data = m_allocator.allocate(num_set_bits);
  
        // iterate through set bits, constructing only with set data
        pointer cmp_data_ptr = compressed_data;
        iterator uncmp_data_itr = begin();
        set_iterator set_bit_itr = m_bitfield.setIterator(uncmp_data_itr);
        while (set_bit_itr()) {
            m_allocator.construct(cmp_data_ptr, *set_bit_itr);
            ++set_bit_itr;
            ++cmp_data_ptr;
        }

        // delete the uncompressed data
        
        // destroy objects
        deinitialize(begin(), end());
    
        // free memory
        m_allocator.deallocate(m_data, m_data_size);

        // point to the compressed data
        m_data = compressed_data;
        m_data_size = num_set_bits;
    } 
        
    // set the compressed flag
    setFlag(CELL_FLAG_COMPRESSED);
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline void 
Cell<T, A>::uncompress()
{
    if (isCompressed() && !isFilled()) {
        // allocate space for uncompressed data
        size_type total_voxels = numBits3D(m_bitfield.size());
        pointer uncompressed_data = m_allocator.allocate(total_voxels);
    
        // construct objects
        iterator begin_itr(uncompressed_data, 
            uncompressed_data + total_voxels, uncompressed_data);
                           
        iterator end_itr(uncompressed_data, 
            uncompressed_data + total_voxels, uncompressed_data + total_voxels);
             
        uninitializedFill(begin_itr, end_itr, getDefaultValue());

        // set data in uncompressed memory block    
        iterator uncmp_data_itr(uncompressed_data,
            uncompressed_data + total_voxels, uncompressed_data);
        set_iterator set_bit_itr = m_bitfield.setIterator(uncmp_data_itr);
        iterator cmp_data_itr = begin();
        for ( ; cmp_data_itr != end(); ++cmp_data_itr, ++set_bit_itr) {
            *set_bit_itr = *cmp_data_itr;
        }

        // delete the compressed data
        
        // destroy objects
        deinitialize(begin(), end());
    
        // free memory
        m_allocator.deallocate(m_data, m_data_size);

        // point to the uncompressed data
        m_data = uncompressed_data;
        m_data_size = total_voxels;

    }    
        
    // unset the compressed state variable
    unsetFlag(CELL_FLAG_COMPRESSED);
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline void
Cell<T, A>::fill(const_reference value)
{
    // can't fill a compressed cell
    if (isCompressed()) {
        // can't modify a compressed cell
        throw Iex::LogicExc("Can't modify a compressed cell");
    }

    if (!isFilled()) {        
        // destroy
        destruct();

        // set the fill value
        setFillValue(value);

        // fill the bitfield
        m_bitfield.fillBits();
    
        // set the filled flag
        setFlag(CELL_FLAG_FILLED);
    } else {
        m_bitfield.fillBits();
        setFillValue(value);
    }
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline void
Cell<T, A>::clear()
{
    // can't clear a compressed cell
    if (isCompressed()) {
        // can't modify a compressed cell
        throw Iex::LogicExc("Can't modify a compressed cell");
    }

    if (!isFilled()) {
        destruct();
    }

    // Make it a filled cell.
    setFlag(CELL_FLAG_FILLED);

    // clear the bit field
    m_bitfield.clearBits();

}    
    
//-----------------------------------------------------------------------------

template<typename T, typename A>
inline void
Cell<T, A>::read(std::istream &is)
{
    destruct();

    // read cell flags
    is.read((char*)&m_flags, sizeof(BOOST_TYPEOF(m_flags)));

    // read in bitfield
    m_bitfield.read(is);

    // read default and fill value
    is.read((char*)&m_default_value, sizeof(T));
    is.read((char*)&m_fill_value, sizeof(T));

    // allocate space
    is.read((char*)&m_data_size, sizeof(size_type));

    if (!isFilled()) { 
        m_data = m_allocator.allocate(m_data_size);  
        // read in voxels
        is.read(reinterpret_cast<char*>(m_data), m_data_size * sizeof(T));
    } else {
        m_data = NULL;
    }
    
    // check state to see if we should uncompress
    if (!isCompressed())
    {
        // uncompress since it's always compressed on disk
        // first set the flag to compressed otherwise uncompress
        // won't do anything
        setFlag(CELL_FLAG_COMPRESSED);

        uncompress();
    }
}

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline void
Cell<T, A>::read(HDF5Id leaf_group_id)
{
    destruct();

    // read in flags
    readScalarAttribute(leaf_group_id,
                        kCellFlagsAttr,
                        TypeToHDF5Type<BOOST_TYPEOF(m_flags)>::type(),
                        &m_flags);
    
    // read in bitfield
    m_bitfield.read(leaf_group_id);

    // read in default value
    readScalarAttribute(leaf_group_id, 
                        kCellDefaultValueAttr,
                        TypeToHDF5Type<T>::type(),
                        &m_default_value);

    // read the fill value
    readScalarAttribute(leaf_group_id, 
                        kCellFillValueAttr,
                        TypeToHDF5Type<T>::type(),
                        &m_fill_value);

    // read in data
    if (!isFilled()) {
        // get data size
        m_data_size = 
            getDataSetStorageSize(leaf_group_id, kCellDataSetName) / sizeof(T);

        // allocate data size
        m_data = m_allocator.allocate(m_data_size);  

        // read in voxel data
        readSimpleDataSet(leaf_group_id, 
                          kCellDataSetName, 
                          TypeToHDF5Type<T>::type(), 
                          m_data);
    }

    // check state to see if we should uncompress
    if (!isCompressed()) {
        // uncompress since it's always compressed on disk
        // first set the flag to compressed otherwise uncompress
        // won't do anything
        setFlag(CELL_FLAG_COMPRESSED);
        uncompress();
    }
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline void
Cell<T, A>::write(std::ostream &os) const
{
    // write cell flags
    os.write((char*)&m_flags, sizeof(BOOST_TYPEOF(m_flags)));

    // make a copy to compress
    // this is so the compression doesn't affect the state
    // of this object.
    // the other alternative is uncompressing after the write is done
    // which would be slower
    Cell<T, A> cellCopy(*this);

    // compress
    cellCopy.compress();

    // write the data out
    cellCopy.writeInternal(os);
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline void
Cell<T, A>::write(HDF5Id volume_group_id, size_t quadrant,
                  index_vec offset) const
{
    // open or create a group for the cell
    HDF5Group cell_group;
   
    // construct a unique name for this leaf group 
    String cell_group_name;
    constructLeafGroupName(LEAF_TYPE_CELL, quadrant, offset, cell_group_name);

    // if leaf group exists open it
    cell_group.open(volume_group_id, cell_group_name); 
    if (!cell_group.isValid()) {
        // if it doesn't, create it
        cell_group.create(volume_group_id, cell_group_name);
    }

    // write flags
    writeScalarAttribute(cell_group.id(),
                         kCellFlagsAttr,
                         TypeToHDF5Type<BOOST_TYPEOF(m_flags)>::type(),
                         &m_flags);

    // write out the index offset
    writeVectorAttribute(cell_group.id(),
                         kIndexOffsetAttr,
                         TypeToHDF5Type<index_type>::type(),
                         3,
                         &offset);

    // write out the quadrant
    writeScalarAttribute(cell_group.id(),
                         kQuadrantAttr,
                         TypeToHDF5Type<BOOST_TYPEOF(quadrant)>::type(),
                         &quadrant);
  
    // define the leaf type enum data type
    HDF5DataType leaf_type_enum;
    leaf_type_enum.create(H5T_ENUM, sizeof(LeafType));

    // add the enum entries
    LeafType type = LEAF_TYPE_CELL;
    leaf_type_enum.enumInsert("LEAF_TYPE_CELL", &type);
    type = LEAF_TYPE_FILL_NODE;
    leaf_type_enum.enumInsert("LEAF_TYPE_FILL_NODE", &type);

    // now write the actual attribute out   
    type = LEAF_TYPE_CELL;
    writeScalarAttribute(cell_group.id(), kLeafTypeAttr, 
                         leaf_type_enum.id(), &type);
  
    // make a copy to compress
    // this is so the compression doesn't affect the state
    // of this object.
    // the other alternative is uncompressing after the write is done
    // which would be slower
    Cell<T, A> cellCopy(*this);

    // compress
    cellCopy.compress();

    // write the data out
    cellCopy.writeInternal(cell_group.id());
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline std::istream& 
operator>>(std::istream& is, const Cell<T, A>& cell)
{
    cell.read(is);
    return is;
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline std::ostream& 
operator<<(std::ostream& os, const Cell<T, A>& cell)
{
    cell.write(os);
    return os;
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline int
Cell<T, A>::sizeOf() const
{
    int sizeof_this     = sizeof(*this);
    int sizeof_data     = sizeof(T) * m_data_size;
    int sizeof_bitfield = m_bitfield.sizeOf();
    return sizeof_this + sizeof_data + sizeof_bitfield;
}

//-----------------------------------------------------------------------------

#if 0
template<typename T, typename A>
inline std::ostream& 
operator<<(std::ostream& o, const Cell<T, A>& cell) const
{
    o << "data: " << cell.m_data << std::endl;
    o << "default value: " << cell.m_default_val << std::endl;
    o << "bitfield: " << cell.m_bitfield << std::endl;
    o << "is compressed: " << cell.isCompressed() << std::endl;
    o << "data allocation size: " << cell.m_data_size << std::endl;

    return o;
}
#endif 

//-----------------------------------------------------------------------------

template <typename T, typename A>
inline void
Cell<T, A>::initializeSet(const_reference v)
{
    // First intialize the data with the default value.
    uninitializedFill(begin(), end(), getDefaultValue());
    
    // Now set the appropriate values for each set bit.
    set_iterator iter = m_bitfield.setIterator(begin());
    for ( ; iter(); ++iter) {
        m_allocator.construct(&*iter, v);
    }
}

//------------------------------------------------------------------------------

template<typename T, typename A>
inline typename Cell<T, A>::iterator 
Cell<T, A>::uninitializedFill(iterator b, iterator e, const_reference v) 
{
    iterator p = b;   // save start position
    try 
    {
        while (b != e) {                // construct while not at end
            m_allocator.construct(&*b, v);
            ++b;
        }
    }
    catch (...)                              // if exeption thrown, undo
    {
        deinitialize(p, b);
        throw;
    }
    return e;
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline typename Cell<T, A>::iterator 
Cell<T, A>::deinitialize(iterator b, iterator e)
{
    assert((b <= e) && (b && e));
    while (b != e) {
        m_allocator.destroy(&*--e);
    }
    return b;
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline void
Cell<T, A>::shallow_copy(pointer dst, pointer src, size_type size)
{
    if (src) {
        memcpy(dst, src, size * sizeof(T));
    }
}

//------------------------------------------------------------------------------

template <typename T, typename A>
inline void
Cell<T, A>::setFlag(uint8_t flag)
{
    m_flags |= flag;
}

//------------------------------------------------------------------------------

template <typename T, typename A>
inline void
Cell<T, A>::unsetFlag(uint8_t flag)
{
    m_flags &= ~flag;
}

//------------------------------------------------------------------------------

template <typename T, typename A>
inline bool 
Cell<T, A>::isFlagSet(uint8_t flag) const
{
    return (m_flags & flag);
}

//------------------------------------------------------------------------------

template <typename T, typename A>
inline void
Cell<T, A>::destruct()
{
    if (m_data) {

        // destroy objects
        deinitialize(begin(), end());
        
        // free memory
        m_allocator.deallocate(m_data, m_data_size);
        m_data = NULL;
        m_data_size = 0;
    }
}

//------------------------------------------------------------------------------

template <typename T, typename A>
inline void
Cell<T, A>::copy(const Cell& that)
{
    m_allocator     = that.m_allocator;
    m_data_size     = that.m_data_size;
    m_default_value = that.m_default_value;
    m_fill_value    = that.m_fill_value;
    m_bitfield      = that.m_bitfield;
    m_flags         = that.m_flags;

    m_data = NULL;

    if (!isFilled()) {
        // allocate memory  
        m_data = m_allocator.allocate(m_data_size);  

        // shallow copy since we don't support user defined structs atm
        shallow_copy(m_data, that.m_data, m_data_size);
    }
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline Cell<T, A>&
Cell<T, A>::operator=(const Cell& that)
{
    destruct(); 
    copy(that);
    return *this;
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline typename Cell<T, A>::const_reference
Cell<T, A>::getDefaultValue() const
{
    return const_cast<Cell&>(*this).getDefaultValue();
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline typename Cell<T, A>::const_reference
Cell<T, A>::getFillValue() const
{
    return const_cast<Cell&>(*this).getFillValue();
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline typename Cell<T, A>::reference
Cell<T, A>::getDefaultValue()
{
    return m_default_value;
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline typename Cell<T, A>::reference
Cell<T, A>::getFillValue()
{
    assert(isFilled());
    return m_fill_value;
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline void
Cell<T, A>::setDefaultValue(const_reference val)
{
    m_default_value = val;
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline void
Cell<T, A>::setFillValue(const_reference val)
{
    m_fill_value = val;
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline void
Cell<T, A>::writeInternal(std::ostream &os) const
{
    // write out bitfield
    m_bitfield.write(os);

    // write default and fill value
    os.write((char*)&m_default_value, sizeof(T));
    os.write((char*)&m_fill_value, sizeof(T));

    // write out data size
    os.write((char*)&m_data_size, sizeof(size_type));
    
    if (!isFilled()) {
        // write out voxels
        os.write(reinterpret_cast<char*>(m_data), m_data_size * sizeof(T));
    }
}


//-----------------------------------------------------------------------------

template<typename T, typename A>
inline void
Cell<T, A>::writeInternal(HDF5Id cell_group_id) const
{
    // write out bitfield
    m_bitfield.write(cell_group_id);

    // write out default value
    writeScalarAttribute(cell_group_id, 
                         kCellDefaultValueAttr,
                         TypeToHDF5Type<T>::type(),
                         &m_default_value);

    // write out fill value
    writeScalarAttribute(cell_group_id, 
                         kCellFillValueAttr,
                         TypeToHDF5Type<T>::type(),
                         &m_fill_value);

    if (!isFilled()) {
        // write out voxel data
        // have to convert to hsize_t manually here
        hsize_t data_size = m_data_size;
        writeSimpleDataSet(cell_group_id,
                           kCellDataSetName,
                           1, &data_size,
                           TypeToHDF5Type<T>::type(),
                           m_data);
    }
}
                           
//-----------------------------------------------------------------------------

template<typename T, typename A>
inline bool
Cell<T, A>::operator==(const Cell& that) const
{
    // check setup
    bool is_equal = true;
    is_equal = m_flags == that.m_flags;
    is_equal = is_equal && m_data_size == that.m_data_size;
    is_equal = is_equal && m_default_value == that.m_default_value;
    if (!is_equal) {
        return false;
    }

    // check bitfield 
    if (m_bitfield != that.m_bitfield) {
        return false;
    }

    // check fill value if both are indicated as fill nodes.
    if (isFilled() && m_fill_value != that.m_fill_value) {
        return false;
    }

    // check data
    for (index_type i = 0; i < m_data_size; ++i) {
        if (m_data[i] != that.m_data[i]) {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline bool
Cell<T, A>::operator!=(const Cell& that) const
{
    return !(*this == that);
}

//-----------------------------------------------------------------------------
// Cell<T, A>::iterator
//-----------------------------------------------------------------------------

template<typename T, typename A>
inline
Cell<T, A>::iterator::iterator() :
    m_begin(0),
    m_end(0),
    m_p(0)
{
    assert(isValid());
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline
Cell<T, A>::iterator::iterator(pointer _begin, pointer _end, pointer _p) :
    m_begin(_begin),
    m_end(_end),
    m_p(_p)
{
    assert(isValid());
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline typename Cell<T, A>::iterator::reference 
Cell<T, A>::iterator::operator*() const
{
    return *m_p;
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline typename Cell<T, A>::iterator::pointer 
Cell<T, A>::iterator::operator->() const
{
    return &**this;
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline typename Cell<T, A>::iterator& 
Cell<T, A>::iterator::operator++()
{
    ++m_p;
    assert(isValid());
    return *this;
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline typename Cell<T, A>::iterator 
Cell<T, A>::iterator::operator++(int)
{
    iterator x = *this;
    ++(*this);
    assert(isValid());
    return x;
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline typename Cell<T, A>::iterator& 
Cell<T, A>::iterator::operator--()
{
    --m_p;
    assert(isValid());
    return *this;
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline typename Cell<T, A>::iterator 
Cell<T, A>::iterator::operator--(int)
{
    iterator x = *this;
    --(*this);
    assert(isValid());
    return x;
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline bool 
Cell<T, A>::iterator::operator==(const iterator& that) const
{
    return m_p == that.m_p;
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline bool 
Cell<T, A>::iterator::operator!=(const iterator& that) const
{
    return !(*this == that);
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline bool 
Cell<T, A>::iterator::operator&&(const iterator& that) const
{
    return this->isValid() && that.isValid();
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline bool 
Cell<T, A>::iterator::operator<=(const iterator& that) const
{
    return this->m_p <= that.m_p;
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline bool 
Cell<T, A>::iterator::isValid() const
{
    return m_p >= m_begin && m_p <= m_end;
}

//-----------------------------------------------------------------------------
// Cell<T, A>::const_iterator
//-----------------------------------------------------------------------------

template<typename T, typename A>
inline
Cell<T, A>::const_iterator::const_iterator() :
    m_bitfield_iter(),
    m_p(0),
    m_isFilled(false)
{
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline
Cell<T, A>::const_iterator::const_iterator(const Cell *cell,
                                           const bitfield_iterator_type &bit,
                                           pointer p) :
    m_bitfield_iter(bit),
    m_p(p),
    m_isFilled(cell->isFilled())
{
    // Initialize the jump table.
    m_value_jumptable[0][0] = &(cell->getDefaultValue());
    m_value_jumptable[0][1] = m_p;
    m_value_jumptable[1][0] = &(cell->getDefaultValue());
    if (m_isFilled) m_value_jumptable[1][1] = &(cell->getFillValue());
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline typename Cell<T, A>::const_iterator::reference 
Cell<T, A>::const_iterator::operator*() const
{
    pointer val = m_value_jumptable[m_isFilled][m_bitfield_iter.isSet()];
    return *val;
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline typename Cell<T, A>::const_iterator::pointer 
Cell<T, A>::const_iterator::operator->() const
{
    return &**this;
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline typename Cell<T, A>::const_iterator& 
Cell<T, A>::const_iterator::operator++()
{
    // Update the bitfield iterator.
    ++m_bitfield_iter;
    
    // Blindly increment m_p. This value is only used when m_p is valid, so it
    // is safe to blindly increment it.
    ++m_p;

    // Update the jump table with new pointer value of m_p.
    m_value_jumptable[0][0] = m_p;
    m_value_jumptable[0][1] = m_p;

    return *this;
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline typename Cell<T, A>::const_iterator 
Cell<T, A>::const_iterator::operator++(int)
{
    const_iterator x = *this;
    ++(*this);
    return x;
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline typename Cell<T, A>::const_iterator& 
Cell<T, A>::const_iterator::operator--()
{
    // Update the bitfield iterator.
    --m_bitfield_iter;
    
    // Blindly decrement m_p. This value is only used when m_p is valid, so it
    // is safe to blindly decrement it.
    --m_p;

    // Update the jump table with new pointer value of m_p.
    m_value_jumptable[0][0] = m_p;
    m_value_jumptable[0][1] = m_p;

    return *this;
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline typename Cell<T, A>::const_iterator 
Cell<T, A>::const_iterator::operator--(int)
{
    const_iterator x = *this;
    --(*this);
    return x;
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline bool 
Cell<T, A>::const_iterator::operator==(const const_iterator& that) const
{
    // Either check bitfields if we have a filled cell or the pointer if we
    // don't.
    return (m_bitfield_iter == that.m_bitfield_iter && 
            m_isFilled == that.m_isFilled) || (m_p == that.m_p);
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline bool 
Cell<T, A>::const_iterator::operator!=(const const_iterator& that) const
{
    return !(*this == that);
}

//-----------------------------------------------------------------------------

template<typename T, typename A>
inline bool 
Cell<T, A>::const_iterator::operator<=(const const_iterator& that) const
{
    return this->m_bitfield_iter <= that.m_bitfield_iter;
}

//-----------------------------------------------------------------------------
