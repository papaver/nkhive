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
// Node.h
//------------------------------------------------------------------------------

#ifndef __NKHIVE_VOLUME_NODE_H__
#define __NKHIVE_VOLUME_NODE_H__

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------

#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/typeof/typeof.hpp>

#include <nkbase/BinaryOps.h>
#include <nkbase/Exceptions.h>

#include <nkhive/Defs.h>
#include <nkhive/Types.h>
#include <nkhive/bitfields/BitField3D.h>
#include <nkhive/tiling/Stamp.h>
#include <nkhive/util/Bounds3D.h>
#include <nkhive/io/hdf5/HDF5Group.h>
#include <nkhive/io/hdf5/HDF5DataType.h>

//------------------------------------------------------------------------------
// forward declarations
//------------------------------------------------------------------------------

#ifdef UNITTEST
class TestNode;
class TestTree;
#endif //UNITTEST

BEGIN_NKHIVE_NS

template <typename C, typename Alloc>
class Tree;

template <typename CT>
class NodeSetIterator;

template <typename CT>
class SetIterator;

END_NKHIVE_NS

//------------------------------------------------------------------------------
// class definition
//------------------------------------------------------------------------------

BEGIN_NKHIVE_NS

/** 
 * Declares a node in the B+-Tree like structure for storing the volume. 
 */
template <typename CellType, 
          typename A = std::allocator<typename CellType::value_type> >
class Node
{

public:

    //--------------------------------------------------------------------------
    // typedefs
    //--------------------------------------------------------------------------
    
    typedef A                                           allocator_type;
    typedef typename allocator_type::value_type         value_type;
    typedef typename allocator_type::reference          reference;
    typedef typename allocator_type::const_reference    const_reference;

    typedef typename allocator_type::pointer            pointer;
    typedef typename allocator_type::const_pointer      const_pointer;

    typedef boost::shared_ptr<Node>                     shared_ptr;
    typedef boost::shared_ptr<const Node>               const_shared_ptr;

    //--------------------------------------------------------------------------
    // public interface
    //--------------------------------------------------------------------------

    /**
     * Params: 
     *   level: the current level of the node in the tree. 
     *   lg_branching_factor: Log2 of the node's branching factor.
     *   lg_cell_dim: Log2 of the cell dimension.
     *   default_value: the default value to set for this node.
     *   as_fill: creates a fill node or a branching node. If this is true,
     *            then the default_value parameter is treated as a fill value.
     */
    Node(index_type level, 
         uint8_t lg_branching_factor, uint8_t lg_cell_dim,
         const_reference default_value, bool as_fill = false);
    ~Node();

    /** 
     * The current level of the node in the tree.
     */
    index_type level() const;

    /**
     * Returns true if the node has branches, i.e., it is not a single fill
     * value.
     */
    bool isBranching() const; 

    /**
     * Returns true if the node is a fill node, i.e., it has a single fill
     * value.
     */
    bool isFill() const;

    /**
     * Returns true if this node has no set values under it.
     */
    bool isEmpty() const;

    /**
     * Returns the maximum dimensions that can be represented by this node.
     * This takes into account all nodes underneath this node. 
     */
    index_type computeMaxDim() const;

    /**
     * Returns the dimension of each child branch. Takes into account all the
     * branches underneath the child.
     */
    index_type computeChildDim() const;

    /**
     * Returns Log2 of the current node's branching factor.
     */
    uint8_t getLgBranchingFactor() const;
    
    /**
     * Returns Log2 of the current node's cell dimension size.
     */
    uint8_t getLgCellDim() const;

    /**
     * Get the bounding box for set values under this node. The returned
     * index bounds are relative to this node.
     */
    void computeSetBounds(index_bounds &bounds) const;

    /** 
     * Get the value stored at given i, j, k index relative to this node.
     */
    reference get(index_type i, index_type j, index_type k);
    const_reference get(index_type i, index_type j, index_type k) const;

    /**
     * Set the value at a particular i, j, k index relative to this node.
     */
    void set(index_type i, index_type j, index_type k, const_reference value);

    /**
     * Updates the value at a particular i, j, k index relative to this node,
     * using a specified binary operator.
     */
    template <typename BinaryOp>
    void update(index_type i, index_type j, index_type k, const_reference val,
                BinaryOp op);

    /** 
     * Unsets the value at a particular i, j, k index relative to this node. 
     * This will recursively remove child nodes as values are cleared. Also 
     * takes in a "default_value" that will be set if nodes are deleted, to 
     * support adaptive grids.
     */
    void unset(index_type i, index_type j, index_type k, const_reference
               default_value);

    /**
     * Writes the stamp to the node.  The stamp will be passed down the tree
     * until they get written by cells.
     */
    template <typename U, template <typename> class Source>
    void stamp(const Stamp<U, Source> &stamp, const index_bounds &stamp_bounds,
               const index_bounds &node_bounds, 
               const signed_index_vec &transform);

    /**
     * I/O methods.
     */
    void read(std::istream &is);
    void write(std::ostream &os) const;
    void read(HDF5Id leaf_group_id, index_vec index_offset);
    void write(HDF5Id volume_group_id, 
               size_t quadrant,
               index_vec offset) const;

    /**
     * Comparision operators.
     */
    bool operator==(const Node &that) const;
    bool operator!=(const Node &that) const;

    /**
     * Returns memory used by class.
     */
    int sizeOf() const;

private:

    //--------------------------------------------------------------------------
    // internal typedefs.
    //--------------------------------------------------------------------------

    /**
     * Define the branching type. If this node is a branching node, then we
     * branch into another node, if it is the direct parent of leaf nodes, then
     * the children are all cells.
     */
    typedef union {
        Node *node;
        CellType *cell;
    } branch_type;

    /** 
     * Vector to hold the branches and it's iterator.
     */
    typedef std::vector<branch_type>                    branch_vector;
    typedef typename branch_vector::iterator            bv_iterator;
    typedef typename branch_vector::const_iterator      const_bv_iterator;

    /**
     * Type to iterator over set branches using the bitfield's set iterator.
     */
    typedef typename bitfield_type::template set_iterator<bv_iterator>
                                                        branch_iterator;
    typedef typename bitfield_type::template set_iterator<const_bv_iterator>
                                                        const_branch_iterator;

    //--------------------------------------------------------------------------
    // internal methods.
    //--------------------------------------------------------------------------

    /**
     * Manages the deallocation of memory when the node is destroyed.
     */
    void destruct();

    /**
     * Get the default value of this node. 
     */
    value_type& defaultValue(); 
    const value_type& defaultValue() const;

    /**
     * Get the fill value of this node. 
     */
    value_type& fillValue();
    const value_type& fillValue() const;

    /**
     * Returns true if the current node directly parents cells.
     */
    bool isCellParent() const;
    
    /** 
     * Computes the total number of divisions in each i, j, k dimension that
     * exist under the current node.
     */
    void computeChildDivisions();

    /**
     * Allocates to total number of branches and the correct type of branches
     * necessary for the current node.
     */
    void allocateBranches();

    /**
     * Creates a child branch of the proper type at the given branch index. 
     * This should not be used if node is filled.
     */
    void createBranch(index_type branch);

    /**
     * Create and/or set the branch associated with the given i, j, k values.
     * Return the branch index and the child i,j,k coordinates that can be used
     * to traverse down that branch. 
     */
    void setBranch(index_type i, index_type j, index_type k, 
                   index_type &branch, 
                   index_type &i_child, 
                   index_type &j_child,
                   index_type &k_child);

    /**
     * Creates fill branches under the current node using the current fill
     * value and the given default value.
     */
    void createFillBranches(const_reference default_val);

    /** 
     * Computes the local branch index for the given i, j, k coordinates. The
     * coordinates are given local Node coordinates.
     */
    index_type computeBranchIndex(index_type i, 
                                  index_type j, 
                                  index_type k) const;

    /**
     * This computes the new coordinates for a child node. It converts the
     * given node coodinates in i, j, k into child coodinates
     * i_child, j_child, k_child. 
     */
    void computeChildCoordinates(index_type i, index_type j, index_type k,
                                 index_type &i_child, 
                                 index_type &j_child, 
                                 index_type &k_child) const;

    /** 
     * Computes the bounds of the child node at the given branch coordinates,
     * in this nodes space.
     */
    index_bounds computeChildBounds(index_type i, 
                                    index_type j, 
                                    index_type k) const;

    /**
     * Sets the given sub-tree as the first child of the current tree. This
     * essentially makes the given node the first branch of the current node.
     * Assumes that the given node is not empty.
     */
    void setSubtree(Node<CellType, A> *subtree);

    /**
     * Computes the intersection of the bounds in voxel coordinates with the 
     * child branches. The returned bounds is in branch coordinates and 
     * covers all branches with intersection.
     */
    index_bounds calculateBranchIntersection(index_bounds voxel_bounds);
   
    /**
     * computes total index space offset to child pointed at by 
     * branch coordinates i, j, k in node space
     */
    index_vec accumulateBranchOffset(index_vec offset, index_type &child_dim,
                                     index_type i, 
                                     index_type j, 
                                     index_type k) const;

    //--------------------------------------------------------------------------
    // friends.
    //--------------------------------------------------------------------------

    template <typename C, typename Alloc>
    friend class Tree;

    template <typename CT>
    friend class NodeSetIterator;
    
    template <typename CT>
    friend class SetIterator;

#ifdef UNITTEST
    friend class ::TestNode;
    friend class ::TestTree;
#endif // UNITTEST

    //--------------------------------------------------------------------------
    // members.
    //--------------------------------------------------------------------------
    
    /**
     * The current level of the node in the tree. Level 0 is the leaf node,
     * which is dictated by a Cell, thus Nodes can never be of level < 1.
     */
    index_type m_level;

    /**
     * The branching factor for this node, if it is a branching node. If there
     * are no branches on this node. This is stored as a log2
     * value to ensure that branching is always a power of 2. 
     */
    uint8_t m_lg_branching_factor;

    /** 
     * The dimensions of the cell stored under this node. This is log2 dimension
     * of one side of the cell cube.
     */
    uint8_t m_lg_cell_dim;

    /**
     * Stores Log2 of the total number of divisions in i, j, k dimensions 
     * for child nodes under this one.
     */
    index_type m_lg_child_divisions;

    /**
     * The value of this node. This stores the volume's default value if
     * the node is a branching node, otherwise it stores the fill value for all
     * children under this node.
     */
    value_type m_value;

    /**
     * The bit field that tracks which branch has set values underneath them. 
     */
    bitfield_type m_bitfield;

    /**
     * The branches from this node. This can either be branching into other
     * branch nodes or into cells. The cells are the leaf nodes of the tree.
     * This can be an empty array if the node is not a branch node and only
     * contains a single fillvalue.
     */
    branch_vector m_branches;

};

END_NKHIVE_NS

//-----------------------------------------------------------------------------
// class implementation
//-----------------------------------------------------------------------------

BEGIN_NKHIVE_NS

#include <nkhive/volume/Node.hpp>

END_NKHIVE_NS

//-----------------------------------------------------------------------------

#endif // __NKHIVE_VOLUME_NODE_H__
