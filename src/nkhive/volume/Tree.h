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
// Tree.h
//------------------------------------------------------------------------------

#ifndef __NKHIVE_VOLUME_TREE_H__
#define __NKHIVE_VOLUME_TREE_H__

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------

#include <boost/typeof/typeof.hpp>

#include <nkbase/BinaryOps.h>

#include <nkhive/tiling/Stamp.h>
#include <nkhive/volume/Node.h>
#include <nkhive/volume/AbstractIterator.h>
#include <nkhive/volume/SetIterator.h>
#include <nkhive/volume/FilledBoundsIterator.h>

#include <nkhive/io/hdf5/HDF5Util.h>
#include <nkhive/io/hdf5/HDF5Group.h>
#include <nkhive/io/hdf5/HDF5Attribute.h>

//------------------------------------------------------------------------------
// forward declarations
//------------------------------------------------------------------------------

class TestTree;

//------------------------------------------------------------------------------
// class definition
//------------------------------------------------------------------------------

BEGIN_NKHIVE_NS

/** 
 * Characterizes the B+Tree structure to represented the volume. 
 */
template <typename CellType,
          typename A = std::allocator<typename CellType::value_type> >
class Tree
{

public:

    //--------------------------------------------------------------------------
    // typedefs
    //--------------------------------------------------------------------------

    typedef A                                         allocator_type;
    typedef typename allocator_type::value_type       value_type;
    typedef typename allocator_type::reference        reference;
    typedef typename allocator_type::const_reference  const_reference;

    //--------------------------------------------------------------------------
    // Forward declare internal classes.
    //--------------------------------------------------------------------------

    class set_iterator;

    //--------------------------------------------------------------------------
    // public interface
    //--------------------------------------------------------------------------

    Tree();
    Tree(uint8_t lg_branching_factor, uint8_t lg_cell_dim, 
         const_reference default_value);
    ~Tree();

    /**
     * Returns the height of the tree. 
     */
    index_type height(index_type quadrant) const;

    /**
     * Returns the stored default value of the tree. 
     */
    const_reference getDefault() const;

    /**
     * Get the value at the given coordinates. We can only return a
     * const_reference here b/c we don't want people to inadvertantly change
     * the default value in case that is returned when indices are accessed out
     * of bounds of the Tree. 
     */
    const_reference get(signed_index_type i, 
                        signed_index_type j, 
                        signed_index_type k) const;

    /** 
     * Set the value at a given voxel, growing the tree if necessary. 
     */
    void set(signed_index_type i, 
             signed_index_type j, 
             signed_index_type k, const_reference val);

    /**
     * Unsets a value at i,j,k. This will not shrink the topology of the tree
     * but will remove underlying nodes if it needs to.
     */
    void unset(signed_index_type i, signed_index_type j, signed_index_type k);

    /**
     * Update the value at a given voxel, growing the tree if necessary. The
     * type of update is determined by the given binary operator.
     */
    template <typename BinaryOp>
    void update(signed_index_type i, signed_index_type j, signed_index_type k,
                const_reference val, BinaryOp op);

    /**
     * Writes the stamp at the requested location using the given op.
     * The position indicates the origin of the stamps bounds.
     */
    template <typename U, template <typename> class Source>
    void stamp(const Stamp<U, Source> &stamp, 
               const signed_index_vec &position);

    /**
     * Return true if there are no set values in the tree. 
     */
    bool isEmpty() const;

    /**
     * Get the bounds of the set values in the tree. Return false if there are
     * no set values, true otherwise.
     */
    bool computeSetBounds(signed_index_bounds &bounds) const;

    /**
     * Return an instance of a set_iterator that allows iteration over all set
     * values in the Tree. 
     */
    set_iterator setIterator() const;

    /** 
     * Comparison operators.
     */
    bool operator==(const Tree &that) const;
    bool operator!=(const Tree &that) const;

    /**
     * I/O operations.
     */
    void read(std::istream &is);
    void write(std::ostream &os) const;

    void read(HDF5Id volume_group_id);
    void write(HDF5Id volume_group_id) const;

    /**
     * Returns memory used by class.
     */
    int sizeOf() const;

private:

    //--------------------------------------------------------------------------
    // internal helpers.
    //--------------------------------------------------------------------------

    void destruct();

    /**
     * Update the value at i, j, k for a particular quadrant. 
     */
    template <typename BinaryOp>
    void update(index_type quadrant, index_type i, index_type j, index_type k,
                const_reference val, BinaryOp op);
    /**
     * Check the quadrant to ensure values at i, j, k are contained within
     * the quadrants max dimensions. Quadrant will grow until max dims contains
     * the coordinates.
     */
    void grow(index_type quadrant, index_type i, index_type j, index_type k);

    /**
     * Convert the given quadrant index coordinates to their proper volume
     * coordinates. 
     */
    void getVolumeCoords(signed_index_vec &indices, uint8_t quadrant) const;

    /**
     * Convert the given volume signed index coordinates to their proper 
     * quadrant coordinates. 
     */
    index_vec getQuadrantCoords(const signed_index_vec &indices, 
                                uint8_t quadrant) const;

    /**
     * Convert a given set of unsigned quadrant coordinates into it's signed
     * equivalent for the given quadrant. 
     */
    void convertToSignedBounds(const index_bounds &node_bounds, 
                               signed_index_bounds &signed_node_bounds, 
                               uint8_t quadrant) const;

    /**
     * IO helper for reading in a leaf group into memory and constructing
     * the appropriate portion of the tree down to the leaf
     */
    void readLeaf(HDF5Id leaf_group_id);

    /**
     * Convert a given set of signed quadrant coordinates into it's local 
     * unsigned equivalent.
     */
    void convertToUnsignedBounds(const signed_index_bounds &signed_node_bounds,
                                 index_bounds &node_bounds) const;

    /**
     * Split the bounds up into seperate quadrants.  Each quadrant bounds will
     * contain the intersection of the quadrant with the bounds. A bitfield is
     * returned indicating which quadrant bounds contain intersections.
     */
    u8 getQuadrantBounds(signed_index_bounds bounds, 
                         signed_index_bounds quadrant_bounds[NUM_QUADRANTS]);

    /**
     * Creates either Cells or FillNodes that are in the volume to store in the
     * HDF5 file.
     */
    static HDF5Err createLeaf(HDF5Id volume_group_id,
                              const char *group_name,
                              const H5L_info_t *group_info,
                              void *op_data);

    //--------------------------------------------------------------------------
    // typedefs 
    //--------------------------------------------------------------------------
    
    typedef Node<CellType, A>       node_type;
    typedef node_type*              node_type_ptr;
    typedef const node_type_ptr     const_node_type_ptr;

#ifdef UNITTEST
    friend class ::TestTree;
#endif // UNITTEST

    //--------------------------------------------------------------------------
    // members
    //--------------------------------------------------------------------------

    /** 
     * A root node for each quadrant.
     */
    node_type_ptr m_root[NUM_QUADRANTS];

    /** 
     * Stores the current maximum dimensions that can be represented by each
     * root of the tree. These are max i,j,k ranges that can be stored. 
     */
    index_type m_max_dim[NUM_QUADRANTS];

    /**
     * Stores the tree's default value.
     */
    value_type m_default_value;

    //--------------------------------------------------------------------------
    // friends
    //--------------------------------------------------------------------------

    template <typename CT, typename CA>
    friend HDF5Err createLeaf(HDF5Id volume_group_id,
                              const char *group_name,
                              const H5L_info_t *group_info,
                              void *op_data);
};

//------------------------------------------------------------------------------
// set_iterator interface.
//------------------------------------------------------------------------------

template <typename CellType, 
          typename A = std::allocator<typename CellType::value_type> >
class Tree<CellType, A>::set_iterator
{
public:

    //--------------------------------------------------------------------------
    // typedefs.
    //--------------------------------------------------------------------------

    typedef std::forward_iterator_tag         iterator_category;
    typedef typename Tree::value_type         value_type;
    typedef typename Tree::reference          reference;
    typedef typename Tree::const_reference    const_reference;
    typedef AbstractIterator<value_type>*     abstract_iterator_pointer;
    typedef const Tree*                       const_tree_pointer;

    //--------------------------------------------------------------------------
    // public interface
    //--------------------------------------------------------------------------

    set_iterator(const_tree_pointer tree);
    ~set_iterator();

    /*
     * return index coordinates of the current iterator position. 
     * Note that this is relative to the tree's bounds
     */
    void getCoordinates(signed_index_type &i, 
                        signed_index_type &j, 
                        signed_index_type &k) const;
    void getCoordinates(signed_index_vec &coords) const;

    //--------------------------------------------------------------------------
    // operators
    //--------------------------------------------------------------------------

    const_reference operator*() const;

    set_iterator& operator++();

    /** 
     * Use the boolean operator to determine the validity of the iterator and
     * if one should continue iterating.
     */
    bool operator()() const;

private:

    //--------------------------------------------------------------------------
    // internal methods.
    //--------------------------------------------------------------------------

    /**
     * Destroys the current quadrant iterator.
     */
    void destroyQuadrantIterator();

    /**
     * Creates a new valid iterator to iterate over a quadrant (one that
     * contains values) of the tree. If no such quadrants exist, return NULL.
     */
    abstract_iterator_pointer createQuadrantIterator();

    /**
     * Creates the next valid iterator with values in the tree. If no such
     * iterator is found, return NULL.
     */
    abstract_iterator_pointer nextQuadrantIterator();

    //--------------------------------------------------------------------------
    // members
    //--------------------------------------------------------------------------

    /**
     * The tree we are iterating over.
     */
    const_tree_pointer m_tree;

    /**
     * The current iterator for iterating over a quadrant.
     */
    abstract_iterator_pointer m_quadrant_iter;

    /**
     * The current quadrant we are iterating over.
     */
    index_type m_quadrant;
};

END_NKHIVE_NS

//-----------------------------------------------------------------------------
// class implementation
//-----------------------------------------------------------------------------

BEGIN_NKHIVE_NS

#include <nkhive/volume/Tree.hpp>

END_NKHIVE_NS

//-----------------------------------------------------------------------------

#endif // __NKHIVE_VOLUME_ROOT_H__
