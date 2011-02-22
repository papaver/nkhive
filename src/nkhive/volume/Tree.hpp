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
// Tree.hpp
//------------------------------------------------------------------------------

// no includes allowed

//------------------------------------------------------------------------------
// class implemenation
//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline
Tree<CellType, A>::Tree() : 
    m_default_value(typename CellType::value_type(0))
{
    for (size_t i = 0; i < NUM_QUADRANTS; ++i) {
        m_root[i] = new Node<CellType, A>(1, 2, 2, m_default_value);
        m_max_dim[i] = m_root[i]->computeMaxDim();
    }
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline
Tree<CellType, A>::Tree(uint8_t lg_branching_factor, uint8_t lg_cell_dim,
                        const_reference default_value)
{
    for (size_t i = 0; i < NUM_QUADRANTS; ++i) {
        m_root[i] = new Node<CellType, A>(1, lg_branching_factor, lg_cell_dim,
                                          default_value);
        m_max_dim[i] = m_root[i]->computeMaxDim();
    }
    m_default_value = default_value;
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline
Tree<CellType, A>::~Tree()
{
    destruct();
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline void
Tree<CellType, A>::destruct()
{
    for (size_t i = 0; i < NUM_QUADRANTS; ++i) {
        delete m_root[i];
    }
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline index_type
Tree<CellType, A>::height(index_type quadrant) const
{
    return m_root[quadrant]->level();
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline typename Tree<CellType, A>::const_reference
Tree<CellType, A>::getDefault() const
{
    return m_default_value;
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline typename Tree<CellType, A>::const_reference
Tree<CellType, A>::get(signed_index_type i, 
                       signed_index_type j, 
                       signed_index_type k) const
{
    // Choose the quadrant based on the indices. 
    u8 q = getQuadrant<signed_index_type>(i, j, k);

    // Convert coords into quadrant coords.
    index_vec qc = getQuadrantCoords(signed_index_vec(i, j, k), q);

    // Check max dimensions
    if (qc.x >= m_max_dim[q] || qc.y >= m_max_dim[q] || qc.z >= m_max_dim[q]) {
        return m_default_value;
    }

    // Call the appropriate node's get method.
    return m_root[q]->get(qc.x, qc.y, qc.z);
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline void
Tree<CellType, A>::set(signed_index_type i,
                       signed_index_type j,
                       signed_index_type k,
                       const_reference val)
{
    update(i, j, k, val, set_op<value_type>());
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline void
Tree<CellType, A>::unset(signed_index_type i,
                         signed_index_type j,
                         signed_index_type k)
{
    // choose the quadrant
    u8 q = getQuadrant<signed_index_type>(i, j, k);

    // Convert coords into quadrant coords.
    index_vec qc = getQuadrantCoords(signed_index_vec(i, j, k), q);

    // do not allow unsets past hte maximum dimensions of set value.
    if (qc.x >= m_max_dim[q] || qc.y >= m_max_dim[q] || qc.z >= m_max_dim[q]) {
        return;
    }

    // call the quadrant node's unset function
    m_root[q]->unset(qc.x, qc.y, qc.z, getDefault());
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
template <typename BinaryOp>
inline void
Tree<CellType, A>::update(signed_index_type i, 
                          signed_index_type j, 
                          signed_index_type k, 
                          const_reference val, BinaryOp op)
{
    // Choose the quadrant.
    u8 q = getQuadrant<signed_index_type>(i, j, k);

    // Convert coords into quadrant coords.
    index_vec qc = getQuadrantCoords(signed_index_vec(i, j, k), q);

    // Call set on the quadrant's unsigned indices
    update(q, qc.x, qc.y, qc.z, val, op);
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
template <typename BinaryOp>
inline void
Tree<CellType, A>::update(index_type q,
                          index_type i, index_type j, index_type k, 
                          const_reference val, BinaryOp op)
{
    // make sure quadrant spans large enough to contain the index
    grow(q, i, j, k);

    // call set on the quadrant's unsigned indices
    m_root[q]->update(i, j, k, val, op);
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline void
Tree<CellType, A>::grow(index_type q, 
                        index_type i, index_type j, index_type k)
{
    // Keep increasing the depth of the tree until we can allocate the value
    // at the given i, j, k
    while (i >= m_max_dim[q] || j >= m_max_dim[q] || k >= m_max_dim[q]) {

        // Allocate the new root.
        Node<CellType, A> *root = 
            new Node<CellType, A>(height(q) + 1, 
                                  m_root[q]->getLgBranchingFactor(),
                                  m_root[q]->getLgCellDim(), m_default_value);

        // If the current tree is empty, then we delete it and just use the new
        // node as root. Otherwise, we need to make the current tree a subtree
        // of the new node.
        if (m_root[q]->isEmpty()) {
            delete m_root[q];
            m_root[q] = NULL;
        } else {
            root->setSubtree(m_root[q]);
        }

        // Set the new root and update the max dimension
        m_root[q] = root;
        m_max_dim[q] = m_root[q]->computeMaxDim();
    }
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
template <typename U, template <typename> class Source>
inline void 
Tree<CellType, A>::stamp(const Stamp<U, Source> &stamp, 
                         const signed_index_vec &position) 
{
    typedef signed_index_bounds::vector_type      vector;
    typedef signed_index_bounds::vector_reference vector_ref;
    
    signed_index_bounds bounds = stamp.getBounds();

    // get bounds in volume space
    bounds.translate(position);

    // split up the bounds by quadrants
    signed_index_bounds quadrant_bounds[NUM_QUADRANTS];
    u8 quadrants = getQuadrantBounds(bounds, quadrant_bounds);

    // loop variables; shortcut loop if possible
    u8 q   = 0;
    u8 end = NUM_QUADRANTS;

    // if the stamp is bound to only one quadrant 
    if (getBitCount(quadrants) == 1) {
        q   = getFirstSetBitIndex(quadrants);
        end = q + 1;
    }

    // submit the stamp to intersecting quadrants
    for (u8 q = 0; q < end; ++q) {
        if (!(quadrants & (1 << q))) continue;

        // get the unsigned quadrant coordinates 
        index_bounds unsigned_bounds;
        convertToUnsignedBounds(quadrant_bounds[q], unsigned_bounds);

        // make sure quadrant has allocated space to hold the values
        index_vec &corner = unsigned_bounds.max();
        grow(q, corner.x, corner.y, corner.z);

        // calculate the quadrants transform
        signed_index_vec transform(1, 1, 1);
        getQuadrantCoordinates(transform.x, transform.y, transform.z, q);

        // calculate the stamps coordinates
        index_bounds stamp_bounds = unsigned_bounds;
        stamp_bounds.translate(-position * transform);

        // write to quadrant
        m_root[q]->stamp(stamp, stamp_bounds, unsigned_bounds, transform);
    }
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline bool
Tree<CellType, A>::isEmpty() const
{
    for (u8 i = 0; i < NUM_QUADRANTS; ++i) {
        if (!m_root[i]->isEmpty()) return false;
    }

    return true;
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline void
Tree<CellType, A>::getVolumeCoords(signed_index_vec &indices, 
                                   u8 quadrant) const
{
    // Offset coordinates to quadrant.
    index_type offset_i, offset_j, offset_k;
    getQuadrantOffsets(offset_i, offset_j, offset_k, quadrant);
    indices.x += offset_i;
    indices.y += offset_j;
    indices.z += offset_k;

    getQuadrantCoordinates<signed_index_type>(indices.x, 
                                              indices.y, 
                                              indices.z, 
                                              quadrant);
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline index_vec
Tree<CellType, A>::getQuadrantCoords(const signed_index_vec &indices, 
                                     u8 quadrant) const
{
    // Take absolute value of indices.
    index_type ui = abs<signed_index_type, index_type>(indices.x);
    index_type uj = abs<signed_index_type, index_type>(indices.y);
    index_type uk = abs<signed_index_type, index_type>(indices.z);

    // Offset coordinates to quadrant.
    index_type offset_i, offset_j, offset_k;
    getQuadrantOffsets(offset_i, offset_j, offset_k, quadrant);
    ui -= offset_i;
    uj -= offset_j;
    uk -= offset_k;

    return index_vec(ui, uj, uk);
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline void
Tree<CellType, A>::convertToSignedBounds(
    const index_bounds &node_bounds,
    signed_index_bounds &signed_node_bounds, 
    uint8_t quadrant) const
{
    // change the sign of the given sign based on the quadrant..
    signed_index_vec min = node_bounds.min();
    signed_index_vec max = node_bounds.max();
    getQuadrantCoordinates<signed_index_type>(min.x, min.y, min.z, quadrant);
    getQuadrantCoordinates<signed_index_type>(max.x, max.y, max.z, quadrant);

    // Update extrema with new signed bounds
    signed_node_bounds.setExtrema(min, max);
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline void
Tree<CellType, A>::convertToUnsignedBounds(
    const signed_index_bounds &signed_node_bounds,
    index_bounds &node_bounds) const
{
    // Take absolute value of indices and update bounds with new extrema
    index_vec min = abs<signed_index_vec, index_vec>(signed_node_bounds.min());
    index_vec max = abs<signed_index_vec, index_vec>(signed_node_bounds.max());
    node_bounds.setExtrema(min, max);
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline bool
Tree<CellType, A>::computeSetBounds(signed_index_bounds &bounds) const
{
    typedef typename signed_index_bounds::value_type bounds_value_type;
    signed_index_bounds::vector_type inf(
            std::numeric_limits<bounds_value_type>::max());

    // initialize the bounds.
    bounds = signed_index_bounds(inf, -inf);

    bool setvalues = false;
    for (uint8_t q = 0; q < NUM_QUADRANTS; ++q) {
        if (m_root[q]->isEmpty()) continue;

        setvalues = true;
        index_bounds node_bounds;
        m_root[q]->computeSetBounds(node_bounds);

        // convert to signed bounds
        signed_index_bounds signed_node_bounds;
        convertToSignedBounds(node_bounds, signed_node_bounds, q);
        bounds.updateExtrema(signed_node_bounds);
    }

    return setvalues;
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline u8 
Tree<CellType, A>::getQuadrantBounds(
        signed_index_bounds bounds, 
        signed_index_bounds quadrant_bounds[NUM_QUADRANTS])
{
    typedef signed_index_bounds::vector_type      vector;
    typedef signed_index_bounds::vector_reference vector_ref;

    // unit vector
    vector unit(1, 1, 1);

    // keep track of the intersecting quadrants
    u8 quadrants = 0;

    // remove the 'edge' from the bounds to prevent false positives
    bounds.translateMax(-unit);

    // loop through all the corners, find the quadrant the corner is in and
    // update the bounds for that quadrant, if a bounds doesn't exist for
    // the quadrant create a new bounds, initialized to quadrants bounds
    for (int index = 0; index < signed_index_bounds::CORNERS; ++index) {

        // calculate the quadrant
        vector corner = bounds.get(index);
        u8 q = getQuadrant<signed_index_type>(corner.x, corner.y, corner.z);

        // check if this quadrant has already been visited, if not a bounds
        // needs to be setup for this quadrant
        if (!(quadrants & (1 << q))) {

            // mark quadrant visisted
            quadrants |= (1 << q);

            // calculate quadrants 'origin'
            vector origin;
            getQuadrantOffsets(origin.x, origin.y, origin.z, q);
            getQuadrantCoordinates(origin.x, origin.y, origin.z, q);

            // calculate quadrants 'infinity'
            vector inf(signed_index_bounds::limits::max());
            getQuadrantCoordinates(inf.x, inf.y, inf.z, q);

            // initialize the bounds to the quadrants extents
            quadrant_bounds[q].setExtrema(origin, inf);
        } 

        // clamp the quadrant bounds by the volume bounds, since the 
        // quadrants only encapsulate thier respective area, clamping 
        // to the bounds cleanly splits the bounds over the different
        // quadrants
        quadrant_bounds[q].clamp(corner, index);
    }

    // add the 'edge' back to intersecting bounds
    for (int q = 0; q < NUM_QUADRANTS; ++q) {
        if (quadrants & (1 << q)) {
            quadrant_bounds[q].translateMax(unit);
        }
    }

    return quadrants;
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline typename Tree<CellType, A>::set_iterator
Tree<CellType, A>::setIterator() const
{
    return set_iterator(this);
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline bool
Tree<CellType, A>::operator==(const Tree &that) const
{
    if (m_default_value != that.m_default_value) return false;

    for (size_t i = 0; i < NUM_QUADRANTS; ++i) {
        if (m_max_dim[i] != that.m_max_dim[i]) return false;
        if (m_root[i]->operator!=(*(that.m_root[i]))) return false;
    }

    return true;
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline bool 
Tree<CellType, A>::operator!=(const Tree &that) const
{
    return !(operator==(that));
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline void
Tree<CellType, A>::read(std::istream &is)
{
    // Destroy the current tree.
    destruct();

    // Read in the default value
    is.read((char*)&m_default_value, sizeof(value_type));

    // Allocate and read in each node.
    for (size_t i = 0; i < NUM_QUADRANTS; ++i) {
        // Create node with bogus parameters.
        m_root[i] = new Node<CellType, A>(1, 2, 2, m_default_value);

        // Read in the data from the stream of this node.
        m_root[i]->read(is);

        // Re-evaluate the maximum dimensions per node.
        m_max_dim[i] = m_root[i]->computeMaxDim();
    }
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline void
Tree<CellType, A>::read(HDF5Id volume_group_id)
{
    // Destroy the current tree
    destruct();

    // read in default value
    HDF5Attribute default_val_attr;
    default_val_attr.open(volume_group_id, kDefaultValueAttr);

    default_val_attr.read(TypeToHDF5Type<value_type>::type(), 
                          &m_default_value);
    
    // read in branching factor. Assumed uniform across all nodes
    HDF5Attribute branching_factor_attr;
    branching_factor_attr.open(volume_group_id, kBranchingFactorAttr);

    u8 lg_branching_factor;
    branching_factor_attr.read(
            TypeToHDF5Type<BOOST_TYPEOF(lg_branching_factor)>::type(),
            &lg_branching_factor);

    // read in the cell dimensions. Assumed uniform cell size.
    HDF5Attribute cell_dim_attr;
    cell_dim_attr.open(volume_group_id, kCellDimAttr);

    u8 lg_cell_dim;
    cell_dim_attr.read(TypeToHDF5Type<BOOST_TYPEOF(lg_cell_dim)>::type(), 
                       &lg_cell_dim);

    // create the root nodes 
    for (size_t i = 0; i < NUM_QUADRANTS; ++i) {
        m_root[i] = 
            new Node<CellType, A>(1, lg_branching_factor, lg_cell_dim,
                                  m_default_value);
        m_max_dim[i] = m_root[i]->computeMaxDim();
    }

    // iterate over leaf groups and construct the tree
    hsize_t iter_index = 0;
    H5Literate(volume_group_id, H5_INDEX_CRT_ORDER, H5_ITER_NATIVE, 
               &iter_index, Tree<CellType, A>::createLeaf, 
               reinterpret_cast<void *>(this));
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline void
Tree<CellType, A>::write(std::ostream &os) const
{
    // write out the default value.
    os.write((char*)&m_default_value, sizeof(value_type));

    // Write out each root node.
    for (size_t i = 0; i < NUM_QUADRANTS; ++i) {
        m_root[i]->write(os);
    }
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline void
Tree<CellType, A>::write(HDF5Id volume_group_id) const
{
    // write out the default value
    writeScalarAttribute(volume_group_id,
                         kDefaultValueAttr,
                         TypeToHDF5Type<value_type>::type(),
                         &m_default_value);

    // write out the branching factor
    // NOTE: this assumes uniform branching factor across all nodes.
    u8 branching_factor = m_root[0]->getLgBranchingFactor();
    writeScalarAttribute(volume_group_id,
                         kBranchingFactorAttr,
                         TypeToHDF5Type<BOOST_TYPEOF(branching_factor)>::type(),
                         &branching_factor);

    // write out the cell dimensions
    // NOTE: this assumes uniform branching factor across all nodes.
    u8 cell_dim = m_root[0]->getLgCellDim();
    writeScalarAttribute(volume_group_id,
                         kCellDimAttr,
                         TypeToHDF5Type<BOOST_TYPEOF(cell_dim)>::type(),
                         &cell_dim);
 
    // traverse the tree, writing the leaves 
    index_vec origin(0, 0, 0);
    for (size_t i=0; i < NUM_QUADRANTS; ++i) {
        m_root[i]->write(volume_group_id, i, origin);
    }
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline int
Tree<CellType, A>::sizeOf() const
{
    int sizeof_this = sizeof(*this);

    int sizeof_quadrants = 0;
    for (int index = 0; index < NUM_QUADRANTS; ++index) {
        sizeof_quadrants += m_root[index]->sizeOf();
    }

    return sizeof_this + sizeof_quadrants;
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline HDF5Err
Tree<CellType, A>::createLeaf(HDF5Id volume_group_id, 
                              const char *group_name, 
                              const H5L_info_t *NK_UNUSED_PARAM(group_info),
                              void *op_data)
{
    // skip the User Attributes group
    if (String(group_name) == kUserAttrGroup) {
        return 0;
    }
    
    // open the leaf group
    HDF5Group leaf_group;
    leaf_group.open(volume_group_id, String(group_name));
    if (!leaf_group.isValid()) {
        THROW(Iex::IoExc, "Invalid leaf group " << group_name);
    }

    // cast user data back to tree pointer
    Tree<CellType, A> *tree = reinterpret_cast< Tree<CellType, A>* >(op_data);

    // read in data and construct portion of tree for this leaf 
    tree->readLeaf(leaf_group.id());

    return 0;
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline void
Tree<CellType, A>::readLeaf(HDF5Id leaf_group_id)
{
    // get the offset
    index_vec index_offset;
    readVectorAttribute(leaf_group_id, kIndexOffsetAttr, &index_offset);
    
    // get the quadrant
    size_t q;
    readScalarAttribute(leaf_group_id, kQuadrantAttr,
                        TypeToHDF5Type<BOOST_TYPEOF(q)>::type(),
                        &q);

    // Keep  increasing the depth of the tree until we can allocate the value
    // at the given i, j, k
    grow(q, index_offset[0], index_offset[1], index_offset[2]);

    m_root[q]->read(leaf_group_id, index_offset);
}

//------------------------------------------------------------------------------
// set_iterator implementation
//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline
Tree<CellType, A>::set_iterator::set_iterator(const_tree_pointer tree) :
    m_tree(tree),
    m_quadrant_iter(NULL),
    m_quadrant(0)
{
    m_quadrant_iter = createQuadrantIterator();
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline
Tree<CellType, A>::set_iterator::~set_iterator()
{
    destroyQuadrantIterator();
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline void
Tree<CellType, A>::set_iterator::getCoordinates(signed_index_type &i, 
                                                signed_index_type &j, 
                                                signed_index_type &k) const
{
    // quadrant iterator and down is all unsigned since there are
    // no negative indices
    index_vec quadrant_coords;

    // get the index coordinates relative to the quadrant
    m_quadrant_iter->getCoordinates(quadrant_coords);

    // offset coordinates to quadrant.
    index_vec offset;
    getQuadrantOffsets(offset[0], offset[1], offset[2], m_quadrant);
    quadrant_coords += offset;
    
    // cast to signed
    i = static_cast<signed_index_type>(quadrant_coords[0]);
    j = static_cast<signed_index_type>(quadrant_coords[1]);
    k = static_cast<signed_index_type>(quadrant_coords[2]);

    // transform from quadrant local indices to tree local indices
    getQuadrantCoordinates<signed_index_type>(i, j, k, m_quadrant);
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline void
Tree<CellType, A>::set_iterator::getCoordinates(signed_index_vec &coords) const
{
    getCoordinates(coords[0], coords[1], coords[2]);
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline void
Tree<CellType, A>::set_iterator::destroyQuadrantIterator()
{
    if (m_quadrant_iter) delete m_quadrant_iter;
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline typename Tree<CellType, A>::set_iterator::abstract_iterator_pointer
Tree<CellType, A>::set_iterator::createQuadrantIterator()
{
    // Finding next non-empty node.
    node_type_ptr node = NULL;
    while (m_quadrant < NUM_QUADRANTS) {
        node = m_tree->m_root[m_quadrant];
        if (!node->isEmpty())
            break;
        // else
        ++m_quadrant;
    }

    if (m_quadrant == NUM_QUADRANTS) 
        return NULL;

    // Check if the node is a fill node.
    if (node->isFill()) {
        index_bounds bounds;
        node->computeSetBounds(bounds);
        return new FilledBoundsIterator<value_type>(bounds,
                                                    node->fillValue());
    }
    //else,
    return new SetIterator<CellType>(node);
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline typename Tree<CellType, A>::set_iterator::abstract_iterator_pointer
Tree<CellType, A>::set_iterator::nextQuadrantIterator()
{
    ++m_quadrant;
    destroyQuadrantIterator();
    return createQuadrantIterator();
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline typename Tree<CellType, A>::const_reference
Tree<CellType, A>::set_iterator::operator*() const
{
    return m_quadrant_iter->value();
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline typename Tree<CellType, A>::set_iterator&
Tree<CellType, A>::set_iterator::operator++()
{
    m_quadrant_iter->next();

    // If exhausted the iterator, go to the next quadrant if there is a valid
    // quadrant available.
    if (m_quadrant_iter->atEnd() && m_quadrant < NUM_QUADRANTS) {
        m_quadrant_iter = nextQuadrantIterator();
    }
    
    return *this;
}

//------------------------------------------------------------------------------

template <typename CellType, typename A>
inline bool
Tree<CellType, A>::set_iterator::operator()() const
{
    // Check if we are at the end of the iterator. This should only happen when
    // there are no more quadrants to iterator over.
    return m_quadrant_iter != NULL && !m_quadrant_iter->atEnd();
}

//------------------------------------------------------------------------------
