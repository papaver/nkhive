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
// Node.hpp
//------------------------------------------------------------------------------

// no includes allowed

//------------------------------------------------------------------------------
// class implementation
//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline
Node<CellType, A>::Node(index_type level, 
                        uint8_t lg_branching_factor,
                        uint8_t lg_cell_dim,
                        const_reference default_value, 
                        bool as_fill) : 
    m_level(level),
    m_lg_branching_factor(lg_branching_factor),
    m_lg_cell_dim(lg_cell_dim),
    m_value(default_value),
    m_bitfield(lg_branching_factor, bitfield_alloc())
{
    assert(level > 0);

    if (as_fill) {
        m_branches.clear();
        m_bitfield.fillBits();
    } else {
        allocateBranches();
    }

    computeChildDivisions();
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline 
Node<CellType, A>::~Node()
{
    destruct();
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline index_type
Node<CellType, A>::level() const
{
    return m_level;
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline bool
Node<CellType, A>::isBranching() const
{
    return m_branches.size() > 0;
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline bool
Node<CellType, A>::isFill() const
{
    return !isBranching();
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline bool
Node<CellType, A>::isEmpty() const
{
    return m_bitfield.isEmpty();
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline index_type
Node<CellType, A>::computeMaxDim() const
{
    // NOTE: This assumes that each level of the tree has equal branching
    // factor.
    return (1 << (m_lg_cell_dim + (m_lg_branching_factor * m_level)));
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline index_type
Node<CellType, A>::computeChildDim() const
{
    // NOTE: This assumes that each level of the tree has equal branching
    // factor.
    return (1 << m_lg_child_divisions);
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline uint8_t 
Node<CellType, A>::getLgBranchingFactor() const
{
    return m_lg_branching_factor;
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline uint8_t 
Node<CellType, A>::getLgCellDim() const
{
    return m_lg_cell_dim;
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline void
Node<CellType, A>::computeSetBounds(index_bounds &bounds) const
{
    index_type dim = computeMaxDim();

    // Should never have an empty node.
    assert(!isEmpty());

    // Trivial case. Just return the extents of the node.
    if (isFill()) {
        bounds = index_bounds(0, dim); 
    } else {
        // Set the min to dimension and max to 0
        bounds = index_bounds(dim, 0);

        // Iterate over all set branches.
        const_branch_iterator iter = m_bitfield.setIterator(m_branches.begin());
        for ( ; iter(); ++iter) {
            // Local node's branch 3D coordinates and compute the offset based
            // on the 3D coordinates for the child node/cell division.
            index_bounds::vector_type offset;
            iter.getCoordinates(offset);
            offset *= computeChildDim();

            // Get the child's bounds and add the offset to get coordinates
            // relative to current node.
            index_bounds childBounds;
            if (isCellParent()) { 
                iter->cell->computeSetBounds(childBounds);
            } else {
                iter->node->computeSetBounds(childBounds);
            }
            childBounds.translate(offset);

            // extremize based the child bounds
            bounds.updateExtrema(childBounds);
        }
    }
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline typename Node<CellType, A>::reference
Node<CellType, A>::get(index_type i, index_type j, index_type k)
{
    // If no branches, then return the fill value.
    if (m_branches.size() == 0) {
        return fillValue();
    }

    // The branch index. 
    index_type branch = computeBranchIndex(i, j, k);

    // If branch does not have a value, then return default value.
    if (!m_bitfield.isSet(branch)) {
        return defaultValue();
    }

    // Compute the local coordinates for the _child node. 
    index_type i_child, j_child, k_child;
    computeChildCoordinates(i, j, k, i_child, j_child, k_child);

    // Call get on the child.
    if (isCellParent()) {
        return m_branches[branch].cell->get(i_child, j_child, k_child);
    } // else
    return m_branches[branch].node->get(i_child, j_child, k_child);
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline typename Node<CellType, A>::const_reference
Node<CellType, A>::get(index_type i, index_type j, index_type k) const
{
    const_cast<Node&>(*this).get(i, j, k);
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline void
Node<CellType, A>::set(index_type i, index_type j, index_type k, 
                       const_reference val)
{
    if (isFill()) {
        // Do not set if value is same as current fill value.
        if (fillValue() == val) {
            return;
        }

        // Allocate and create LOD tree with fill branches.
        createFillBranches(fillValue());
    }

    // Get/Allocate the right branch and get the child coordinates to traverse
    // down it. 
    index_type branch, i_child, j_child, k_child;
    setBranch(i, j, k, branch, i_child, j_child, k_child);

    // Recurse down tree
    if (isCellParent()) {
        m_branches[branch].cell->set(i_child, j_child, k_child, val);
    } else {
        m_branches[branch].node->set(i_child, j_child, k_child, val);
    }
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
template <typename BinaryOp>
inline void
Node<CellType, A>::update(index_type i, index_type j, index_type k, 
                          const_reference val, BinaryOp op)
{
    if (isFill()) {
        // Update the value first
        value_type new_val = op(fillValue(), val);

        // Now set the value in the node.
        set(i, j, k, new_val);
    } else {
        // For a non-fill node, simply call the update() function on the child
        // node. This will traverse down to the Cell or FillNode.

        // Get/Allocate the right branch and get the child coordinates to
        // traverse down it. 
        index_type branch, i_child, j_child, k_child;
        setBranch(i, j, k, branch, i_child, j_child, k_child);

        // Recurse down the tree. 
        if (isCellParent()) {
            m_branches[branch].cell->update(i_child, j_child, k_child, val, op);
        } else {
            m_branches[branch].node->update(i_child, j_child, k_child, val, op);
        }
    }
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline void
Node<CellType, A>::unset(index_type i, index_type j, index_type k, 
                         const_reference default_val)
{
    if (isFill()) {
        createFillBranches(default_val);
    }

    // set the default value.
    defaultValue() = default_val;

    // the branch index.
    index_type branch = computeBranchIndex(i, j, k);

    // Early out.
    if (!m_bitfield.isSet(branch)) return;

    // Compute the local child coordinates for the child node.
    index_type i_child, j_child, k_child;
    computeChildCoordinates(i, j, k, i_child, j_child, k_child);

    // Call unset recursively on child nodes.
    if (isCellParent()) {
        m_branches[branch].cell->unset(i_child, j_child, k_child);

        // If the cell is empty, deallocate it and unset this branch.
        if (m_branches[branch].cell->isEmpty()) {
            delete m_branches[branch].cell;
            m_branches[branch].cell = NULL;
            m_bitfield.unsetBit(branch);
        }
    } else {
        m_branches[branch].node->unset(i_child, j_child, k_child, default_val);

        // Check if the branch is empty and should be unset.
        if (m_branches[branch].node->isEmpty()) {
            delete m_branches[branch].node;
            m_branches[branch].node = NULL;
            m_bitfield.unsetBit(branch);
        }
    }
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
template <typename U, template <typename> class Source>
inline void
Node<CellType, A>::stamp(const Stamp<U, Source> &stamp, 
                         const index_bounds &stamp_bounds,
                         const index_bounds &node_bounds, 
                         const signed_index_vec &transform) 
{
    typedef index_bounds::vector_type      vector;
    typedef index_bounds::vector_reference vector_ref;

    // allocate branches if fill node.
    if (isFill()) createFillBranches(defaultValue());

    // calculate the intersecting branches
    index_bounds branch_bounds = calculateBranchIntersection(node_bounds);

    // offset for calculating stamp position relative to children
    vector stamp_offset = stamp_bounds.min() - node_bounds.min();

    // TODO: use window iterator on the inner most set of nodes, they
    //       don't need bounds checks done since they should all be 
    //       completely filled

    vector_ref min = branch_bounds.min();
    vector_ref max = branch_bounds.max();
    for (index_type k = min.z; k < max.z; ++k) {
        for (index_type j = min.y; j < max.y; ++j) {
            for (index_type i = min.x; i < max.x; ++i) {

                // figure out the bounds of the current child
                index_bounds child_bounds = computeChildBounds(i, j, k);

                // intersect with the stamps bounds
                index_bounds intersection = 
                    node_bounds.intersection(child_bounds);

                // compute the intersection in child's space
                index_bounds child_intersection = intersection;
                child_intersection.translate(-child_bounds.min());

                // Set the bit and allocate child node/cell
                index_type branch = getIndex(i, j, k, m_lg_branching_factor);
                m_bitfield.setBit(branch);
                createBranch(branch);

                // compute the intersection in stamp's space
                index_bounds stamp_intersection(
                    stamp_offset + intersection.min(),
                    stamp_offset + intersection.max());

                // write to node or cell
                if (isCellParent()) {

                    // calculate stamp signed bounds
                    signed_index_bounds signed_bounds;
                    vector max = stamp_intersection.min() * transform; 
                    vector min = stamp_intersection.max() * transform; 
                    signed_bounds.setExtrema(min, max);

                    m_branches[branch].cell->stamp(stamp, signed_bounds, 
                                                   child_intersection, 
                                                   transform);
                } else {

                    m_branches[branch].node->stamp(stamp, stamp_intersection, 
                                                   child_intersection, 
                                                   transform);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline void
Node<CellType, A>::read(std::istream &is)
{
    // Destroy the current node.
    destruct();

    // Read in the basic info.
    is.read((char*)&m_level, sizeof(BOOST_TYPEOF(m_level)));
    is.read((char*)&m_lg_branching_factor,
            sizeof(BOOST_TYPEOF(m_lg_branching_factor)));
    is.read((char*)&m_lg_cell_dim, sizeof(BOOST_TYPEOF(m_lg_cell_dim)));
    is.read((char*)&m_lg_child_divisions, 
            sizeof(BOOST_TYPEOF(m_lg_child_divisions)));

    // Read in the bitfield.
    // TODO: Don't read it in if it is a fill node.
    m_bitfield.read(is);

    // Determine if this node is a branching node or a fill node.
    unsigned char branching = 0;
    is.read((char*)&branching, sizeof(BOOST_TYPEOF(branching)));

    // Read the default value or the fill value.
    if (branching) {
        // Allocate space for all branches
        allocateBranches();

        is.read((char*)&defaultValue(), sizeof(value_type));

        // Create the nodes/cells on all active branches and read them in.
        if (isCellParent()) {
            branch_iterator iter = m_bitfield.setIterator(m_branches.begin());
            for ( ; iter(); ++iter) {
                iter->cell = new CellType(m_lg_cell_dim, defaultValue());
                iter->cell->read(is);
            }
        } else {
            branch_iterator iter = m_bitfield.setIterator(m_branches.begin());
            for ( ; iter(); ++iter) {
                iter->node = new Node(m_level - 1, m_lg_branching_factor, 
                                      m_lg_cell_dim, defaultValue());
                iter->node->read(is);
            }
        }

    } else {
        is.read((char*)&fillValue(), sizeof(value_type));

        // no branches to read in.
    }
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline void
Node<CellType, A>::read(HDF5Id leaf_group_id, index_vec index_offset)
{
    // get the type of leaf
    LeafType type;
    readScalarAttribute(leaf_group_id, kLeafTypeAttr, -1, &type);

    if (type == LEAF_TYPE_FILL_NODE) {
        // get the level
        index_type level;
        readScalarAttribute(leaf_group_id,
                            kFillNodeLevelAttr,
                            TypeToHDF5Type<BOOST_TYPEOF(m_level)>::type(),
                            &level);

        if (level == m_level) {
            // construct the fill node
            // get the fill value
            readScalarAttribute(leaf_group_id, 
                                kFillNodeValueAttr,
                                TypeToHDF5Type<value_type>::type(),
                                &m_value);

            m_branches.clear();
            m_bitfield.fillBits();
            return;
        }
    } 
    
    // The branch index 
    index_type branch = computeBranchIndex(index_offset[0],
                                           index_offset[1], 
                                           index_offset[2]);

    // Set the bit
    m_bitfield.setBit(branch);

    // Allocate a child node or cell.
    if (isCellParent()) {
        assert(type == LEAF_TYPE_CELL);

        // cell end case
        if (m_branches[branch].cell == NULL) {
            m_branches[branch].cell = new CellType(m_lg_cell_dim, 
                                                   defaultValue());
        }
        m_branches[branch].cell->read(leaf_group_id);

    // recurse down the tree
    } else {

        // Compute the local coordinates for the child node. 
        index_vec child_offset;
        computeChildCoordinates(
            index_offset[0], index_offset[1], index_offset[2],
            child_offset[0], child_offset[1], child_offset[2]); 

       if (m_branches[branch].node == NULL) {
            m_branches[branch].node = new Node(m_level - 1,
                                               m_lg_branching_factor,
                                               m_lg_cell_dim,
                                               defaultValue());
        }
        m_branches[branch].node->read(leaf_group_id, child_offset);
    }
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline void
Node<CellType, A>::write(std::ostream &os) const
{
    // Write out basic info
    os.write((char*)&m_level, sizeof(BOOST_TYPEOF(m_level)));
    os.write((char*)&m_lg_branching_factor,
             sizeof(BOOST_TYPEOF(m_lg_branching_factor)));
    os.write((char*)&m_lg_cell_dim, sizeof(BOOST_TYPEOF(m_lg_cell_dim)));
    os.write((char*)&m_lg_child_divisions,
             sizeof(BOOST_TYPEOF(m_lg_child_divisions)));

    // Write out the bitfield.
    // TODO: We don't need to write this out if the node is a fill node. 
    m_bitfield.write(os);

    // Write out if it is a branching node or it has a single fill value.
    unsigned char branching = isBranching();
    os.write((char*)&branching, sizeof(BOOST_TYPEOF(branching)));

    // Write the default value or the fill value.
    if (branching) {
        os.write((char*)&defaultValue(), sizeof(value_type));

        // Call write on the set branches.
        if (isCellParent()) {
            const_branch_iterator iter = 
                m_bitfield.setIterator(m_branches.begin());
            for ( ; iter(); ++iter) {
                iter->cell->write(os);
            }
        } else {
            const_branch_iterator iter = 
                m_bitfield.setIterator(m_branches.begin());
            for ( ; iter(); ++iter) {
                iter->node->write(os);
            }
        }

    } else {
        os.write((char*)&fillValue(), sizeof(value_type));
        // no branches to write out.
    }
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline void
Node<CellType, A>::write(HDF5Id volume_group_id, size_t quadrant,
                         index_vec offset) const
{
    if (isBranching()) {
        index_type child_dim = computeChildDim();

        // accumulate offset and recurse
        if (isCellParent()) {

            // iterate over set branches
            const_branch_iterator iter = 
                m_bitfield.setIterator(m_branches.begin());
            for ( ; iter(); ++iter) {
                index_type i, j, k;
                iter.getCoordinates(i, j, k);

                // accumulate offsets to pass down the tree
                index_vec accum_offset = 
                    accumulateBranchOffset(offset, child_dim, i, j, k);

                // write out the cell  
                iter->cell->write(volume_group_id, quadrant, accum_offset);
            }

        } else {

            // iterate over set branches
            const_branch_iterator iter = 
                m_bitfield.setIterator(m_branches.begin());
            for ( ; iter(); ++iter) {
                index_type i, j, k;
                iter.getCoordinates(i, j, k);

                // accumulate offsets to pass down the tree
                index_vec accum_offset = 
                    accumulateBranchOffset(offset, child_dim, i, j, k);
               
                // recurse on the child 
                iter->node->write(volume_group_id, quadrant, accum_offset);
            }
        }

    // only write out if it's a fill node
    } else if (isFill()) {
        
        // open or create a group for the cell or fill node
        HDF5Group leaf_group;
       
        // construct a unique name for this leaf_group  
        String leaf_group_name;
        constructLeafGroupName(LEAF_TYPE_FILL_NODE, quadrant, offset, 
                               leaf_group_name);

        // if leaf group exists open it
        leaf_group.open(volume_group_id, leaf_group_name); 
        if (!leaf_group.isValid()) {
            // if it doesn't, create it
            leaf_group.create(volume_group_id, leaf_group_name);
        }

        // write out the level
        writeScalarAttribute(leaf_group.id(), 
                             kFillNodeLevelAttr,  
                             TypeToHDF5Type<BOOST_TYPEOF(m_level)>::type(),
                             &m_level);

        // write out the fill value
        writeScalarAttribute(leaf_group.id(),
                             kFillNodeValueAttr,
                             TypeToHDF5Type<value_type>::type(),
                             &m_value);

        // write out the index offset
        writeVectorAttribute(leaf_group.id(),
                             kIndexOffsetAttr,
                             TypeToHDF5Type<index_type>::type(),
                             3,
                             &offset);

        // write out the quadrant
        writeScalarAttribute(leaf_group.id(),
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
        type = LEAF_TYPE_FILL_NODE; 
        writeScalarAttribute(leaf_group.id(), kLeafTypeAttr, 
                             leaf_type_enum.id(), &type);
    }
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline bool
Node<CellType, A>::operator==(const Node &that) const
{
    bool simpleChecks = 
        (m_level == that.m_level) && 
        (m_lg_branching_factor == that.m_lg_branching_factor) &&
        (m_lg_cell_dim == that.m_lg_cell_dim) &&
        (m_lg_child_divisions == that.m_lg_child_divisions) &&
        (m_branches.size() == that.m_branches.size()) && 
        (isFill() == that.isFill());

    if (!simpleChecks) return false;

    // Check the fill value or the default value
    if (isFill()) {
        if (fillValue() != that.fillValue()) return false;
    } else {
        if (defaultValue() != that.defaultValue()) return false;
    }

    // Check bitfield.
    if (m_bitfield != that.m_bitfield) return false;

    // Don't check branches if we have a fill node.
    if (isFill()) return true;

    // Recursively check each branch for branching nodes.
    const_branch_iterator this_it = m_bitfield.setIterator(m_branches.begin());
    const_branch_iterator that_it = 
        m_bitfield.setIterator(that.m_branches.begin());
    for ( ; this_it(); ++this_it, ++that_it) {
        if (isCellParent()) {
            if (this_it->cell->operator!=(*(that_it->cell))) return false;
        } else {
            if (this_it->node->operator!=(*(that_it->node))) return false;
        }
    }

    return true;
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline bool
Node<CellType, A>::operator!=(const Node &that) const
{
    return !(operator==(that));
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline int
Node<CellType, A>::sizeOf() const
{
    int sizeof_this     = sizeof(*this);
    int sizeof_bitfield = m_bitfield.sizeOf();

    int sizeof_branches = sizeof(branch_type*) * m_branches.capacity();
    const_branch_iterator iter = m_bitfield.setIterator(m_branches.begin());
    for ( ; iter(); ++iter) {
        if (isCellParent()) { 
            sizeof_branches += iter->cell->sizeOf();
        } else {
            sizeof_branches += iter->node->sizeOf();
        }
    }

    return sizeof_this + sizeof_bitfield + sizeof_branches;
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline void
Node<CellType, A>::allocateBranches()
{
    // Allocate the branch nodes.
    m_branches.resize(numBits3D(m_lg_branching_factor));
    if (isCellParent()) {
        for (size_t i = 0; i < m_branches.size(); ++i) {
            m_branches[i].cell = NULL;
        }
    } else {
        for (size_t i = 0; i < m_branches.size(); ++i) {
            m_branches[i].node = NULL;
        }
    }
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline void
Node<CellType, A>::setBranch(index_type i, index_type j, index_type k,
                             index_type &branch,
                             index_type &i_child, 
                             index_type &j_child, 
                             index_type &k_child)
{
    // The branch index 
    branch = computeBranchIndex(i, j, k);

    // Set the bit
    m_bitfield.setBit(branch);

    // Compute the local coordinates for the child node. 
    computeChildCoordinates(i, j, k, i_child, j_child, k_child);

    // Allocate a child node or cell.
    createBranch(branch);
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline void
Node<CellType, A>::createBranch(index_type branch)
{
    // this should only be called if the node is a normal node.
    assert(!isFill());

    value_type value = defaultValue();

    // allocate a child node or cell.
    if (isCellParent()) {
        if (m_branches[branch].cell == NULL) {
            m_branches[branch].cell = new CellType(m_lg_cell_dim, value);
        }
    } else {
        if (m_branches[branch].node == NULL) {
            m_branches[branch].node = new Node(
                m_level - 1, m_lg_branching_factor, m_lg_cell_dim, value);
        }
    }
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline void
Node<CellType, A>::createFillBranches(const_reference default_val)
{
    // This should only be called if the node is a fill node.
    assert(isFill());

    value_type fill_val = fillValue();

    // Allocate branch nodes.
    m_branches.resize(numBits3D(m_lg_branching_factor));
    if (isCellParent()) {
        for (size_t i = 0; i < m_branches.size(); ++i) {
            m_branches[i].cell = 
                new CellType(m_lg_cell_dim, default_val, fill_val, 
                             typename CellType::allocator_type());
        }
    } else {
        for (size_t i = 0; i < m_branches.size(); ++i) {
            m_branches[i].node = 
                new Node(m_level - 1, m_lg_branching_factor, 
                         m_lg_cell_dim, fill_val, true);
        }
    }
}

//------------------------------------------------------------------------------ 

template <typename CellType, typename A>
inline index_type
Node<CellType, A>::computeBranchIndex(index_type i,
                                      index_type j, 
                                      index_type k) const
{
    // Convert each given coordinate into local node coordinates by dividing by
    // the total number of branches under the node.
    return getIndex(i >> m_lg_child_divisions,
                    j >> m_lg_child_divisions,
                    k >> m_lg_child_divisions,
                    m_lg_branching_factor);
}

//------------------------------------------------------------------------------ 
template <typename CellType, typename A>
inline void
Node<CellType, A>::computeChildCoordinates(index_type i,
                                        index_type j, 
                                        index_type k,
                                        index_type &i_child,
                                        index_type &j_child,
                                        index_type &k_child) const
{
    i_child = moduloLg(i, m_lg_child_divisions);
    j_child = moduloLg(j, m_lg_child_divisions);
    k_child = moduloLg(k, m_lg_child_divisions);
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline index_bounds 
Node<CellType, A>::computeChildBounds(index_type i, 
                                      index_type j, 
                                      index_type k) const
{
    index_type child_dim = computeChildDim();

    // calculate the bounds for the child node
    index_bounds bounds;
    bounds.min() = index_vec(i, j, k) * child_dim;
    bounds.max() = bounds.min() + index_vec(child_dim);

    return bounds;
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline void 
Node<CellType, A>::destruct()
{
    if (isCellParent()) {
        for (size_t i = 0; i < m_branches.size(); ++i) {
            if (m_branches[i].cell) {
                delete m_branches[i].cell;
            }
        }
    } else {
        for (size_t i = 0; i < m_branches.size(); ++i) {
            if (m_branches[i].node) {
                delete m_branches[i].node;
            }
        }
    }
    m_branches.clear();
    m_bitfield.clearBits();
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline typename Node<CellType, A>::value_type&
Node<CellType, A>::defaultValue()
{
    assert(m_branches.size() == numBits3D(m_bitfield.size()));
    return m_value;
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline const typename Node<CellType, A>::value_type&
Node<CellType, A>::defaultValue() const
{
    return const_cast<Node&>(*this).defaultValue();
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline typename Node<CellType, A>::value_type&
Node<CellType, A>::fillValue()
{
    assert(m_branches.size() == 0);
    assert(m_bitfield.isFull());
    return m_value;
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline const typename Node<CellType, A>::value_type&
Node<CellType, A>::fillValue() const
{
    return const_cast<Node&>(*this).fillValue();
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline bool
Node<CellType, A>::isCellParent() const
{
    return (m_level == 1);
}

//------------------------------------------------------------------------------ 

template <typename CellType, typename A>
inline void
Node<CellType, A>::computeChildDivisions() 
{
    // NOTE: This function assumes that all nodes in the tree have the same
    // branching factor with the exception of the cell nodes.
    m_lg_child_divisions = 
        (m_level - 1) * m_lg_branching_factor + m_lg_cell_dim;
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline void
Node<CellType, A>::setSubtree(Node<CellType, A> *subtree) 
{
    // Assure thet the given node has values in it. If it is empty, then it
    // should never be used as a subtree of the current node.
    assert(!subtree->isEmpty());
    assert(isBranching());
    assert(m_branches[0].node == NULL);

    // Assign the branch and set the bit to signify set values.
    m_branches[0].node = subtree;
    m_bitfield.setBit(0);
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline index_bounds 
Node<CellType, A>::calculateBranchIntersection(index_bounds voxel_bounds)
{
    const index_vec unit(1, 1, 1);

    // remove the 'edge' from the bounds.
    voxel_bounds.translateMax(-unit);

    // calculate the intersecting corners of the bounds w/ the children
    index_type branch_min_index = computeBranchIndex(
        voxel_bounds.min().x, voxel_bounds.min().y, voxel_bounds.min().z);
    index_type branch_max_index = computeBranchIndex(
        voxel_bounds.max().x, voxel_bounds.max().y, voxel_bounds.max().z);

    // convert the min/max into branch coordinates
    index_bounds branch_bounds;
    getCoordinates(branch_min_index, m_lg_branching_factor, 
        branch_bounds.min().x, branch_bounds.min().y, branch_bounds.min().z);
    getCoordinates(branch_max_index, m_lg_branching_factor, 
        branch_bounds.max().x, branch_bounds.max().y, branch_bounds.max().z);

    // add 'edge' to bounds
    branch_bounds.translateMax(unit);

    return branch_bounds;
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline index_vec
Node<CellType, A>::accumulateBranchOffset(index_vec offset, 
                                          index_type &child_dim,
                                          index_type i,
                                          index_type j,
                                          index_type k) const
{
    // get the coordinate offset of this branch
    index_vec branch_coords(i, j, k);
    branch_coords *= child_dim;

    // add it to the current offset
    offset += branch_coords;
    return offset;
}               

