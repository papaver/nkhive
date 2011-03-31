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
// Volume.hpp
//------------------------------------------------------------------------------

// no includes allowed

//------------------------------------------------------------------------------
// typedefs
//------------------------------------------------------------------------------

typedef Volume<f16> VolumeHalf;
typedef Volume<f32> VolumeFloat;
typedef Volume<f64> VolumeDouble;
typedef Volume<i32> VolumeInt;
typedef Volume<u32> VolumeUInt;
typedef Volume<i64> VolumeLong;
typedef Volume<u64> VolumeULong;

//------------------------------------------------------------------------------
// class implemenation
//------------------------------------------------------------------------------

template <typename T>
inline
Volume<T>::Volume() :
    m_tree(), 
    m_local_xform(),
    m_attributes()
{
}

//------------------------------------------------------------------------------

template <typename T>
inline
Volume<T>::Volume(uint8_t lg_branching_factor, 
                  uint8_t lg_cell_dim,
                  const_reference default_value, 
                  const vec3d &res,
                  const vec3d &kernel_offset) : 
    m_tree(lg_branching_factor, lg_cell_dim, default_value), 
    m_local_xform(res), 
    m_kernel_offset(kernel_offset),
    m_attributes()
{
    createDefaultAttributes();
}

//------------------------------------------------------------------------------

template <typename T>
inline
Volume<T>::~Volume()
{
}

//------------------------------------------------------------------------------

template <typename T>
inline String 
Volume<T>::typeName() const
{
    return typeid(T).name();
}

//------------------------------------------------------------------------------

template <typename T>
inline vec3d
Volume<T>::kernelOffset() const
{
    return m_kernel_offset;
}

//------------------------------------------------------------------------------

template <typename T>
inline typename Volume<T>::const_reference
Volume<T>::getDefault() const
{
    return m_tree.getDefault();
}

//------------------------------------------------------------------------------

template <typename T>
inline typename Volume<T>::const_reference
Volume<T>::get(signed_index_type i, 
               signed_index_type j, 
               signed_index_type k) const
{
    return m_tree.get(i, j, k);
}

//------------------------------------------------------------------------------

template <typename T>
inline typename Volume<T>::const_reference
Volume<T>::get(const signed_index_vec &coords) const
{
    return get(coords[0], coords[1], coords[2]);
}

//------------------------------------------------------------------------------

template <typename T>
inline void
Volume<T>::set(signed_index_type i, 
               signed_index_type j, 
               signed_index_type k, const_reference val)
{
    m_tree.set(i, j, k, val);
}

//------------------------------------------------------------------------------

template <typename T>
inline void
Volume<T>::set(const signed_index_vec &coords, const_reference val)
{
    set(coords[0], coords[1], coords[2], val);
}

//------------------------------------------------------------------------------

template <typename T>
inline void
Volume<T>::unset(signed_index_type i, signed_index_type j, signed_index_type k)
{
    m_tree.unset(i, j, k);
}

//------------------------------------------------------------------------------

template <typename T>
inline void
Volume<T>::unset(const signed_index_vec &coords)
{
    unset(coords[0], coords[1], coords[2]);
}

//------------------------------------------------------------------------------

template <typename T>
template <typename BinaryOp>
inline void
Volume<T>::update(signed_index_type i, 
                  signed_index_type j, 
                  signed_index_type k, 
                  const_reference val, BinaryOp op)
{
    m_tree.update(i, j, k, val, op);
}

//------------------------------------------------------------------------------

template <typename T>
template <typename BinaryOp>
inline void
Volume<T>::update(const signed_index_vec &coords, 
                  const_reference val, BinaryOp op)
{
    update(coords[0], coords[1], coords[2], val, op);
}

//------------------------------------------------------------------------------

template <typename T>
template <typename U, template <typename> class Source>
inline void 
Volume<T>::stamp(Stamp<U, Source> &stamp, signed_index_vec &position) 
{
    m_tree.stamp(stamp, position);
}

//------------------------------------------------------------------------------

template <typename T>
inline bool
Volume<T>::isEmpty() const
{
    return m_tree.isEmpty();
}

//------------------------------------------------------------------------------

template <typename T>
inline bool
Volume<T>::computeSetBounds(signed_index_bounds &bounds) const
{
    return m_tree.computeSetBounds(bounds);
}

//------------------------------------------------------------------------------

template <typename T>
inline bool
Volume<T>::computeSetBounds(Bounds3D<double> &bounds) const
{
    signed_index_bounds index_bounds;
    if (!m_tree.computeSetBounds(index_bounds)) {
        return false;
    } 

    Bounds3D<double> voxel_bounds;
    indexToVoxel(index_bounds.min(), voxel_bounds.min());
    indexToVoxel(index_bounds.max(), voxel_bounds.max());
    
    voxelToLocal(voxel_bounds.min(), bounds.min());
    voxelToLocal(voxel_bounds.max(), bounds.max());

    return true;
}

//------------------------------------------------------------------------------

template <typename T>
inline int
Volume<T>::sizeOf() const
{
    int sizeof_this = sizeof(*this);
    int sizeof_tree = m_tree.sizeOf();

    return sizeof_this + sizeof_tree;
}

//------------------------------------------------------------------------------

template <typename T>
inline void
Volume<T>::setLocalXform(const vec3d &res)
{
    m_local_xform = LocalXform(res);
}

//------------------------------------------------------------------------------

template <typename T>
inline vec3d
Volume<T>::res() const
{
    return m_local_xform.res();
}

//------------------------------------------------------------------------------

template <typename T>
inline void
Volume<T>::localToVoxel(const vec3d &l, vec3d &v) const
{
    m_local_xform.localToVoxel(l, v);
}

//------------------------------------------------------------------------------

template <typename T>
inline vec3d
Volume<T>::localToVoxel(const vec3d &l) const
{
    return m_local_xform.localToVoxel(l);
}

//------------------------------------------------------------------------------

template <typename T>
inline void
Volume<T>::localToIndex(const vec3d &l, vec3i &i) const
{
    vec3d v = localToVoxel(l);
    voxelToIndex(v, i);
}

//------------------------------------------------------------------------------

template <typename T>
inline vec3i
Volume<T>::localToIndex(const vec3d &l) const
{
    vec3d v = localToVoxel(l);
    return voxelToIndex(v);
}

//------------------------------------------------------------------------------

template <typename T>
inline void
Volume<T>::voxelToLocal(const vec3d &v, vec3d &l) const
{
    m_local_xform.voxelToLocal(v, l);
}

//------------------------------------------------------------------------------

template <typename T>
inline vec3d
Volume<T>::voxelToLocal(const vec3d &v) const
{
    return m_local_xform.voxelToLocal(v);
}

//------------------------------------------------------------------------------

template <typename T>
inline void
Volume<T>::voxelToIndex(const vec3d &v, vec3i &i) const
{
    vec3d inv_xformed_v = v - m_kernel_offset;

    i.x = (vec3i::BaseType)floor(inv_xformed_v.x);
    i.y = (vec3i::BaseType)floor(inv_xformed_v.y);
    i.z = (vec3i::BaseType)floor(inv_xformed_v.z);
}

//------------------------------------------------------------------------------

template <typename T>
inline vec3i
Volume<T>::voxelToIndex(const vec3d &v) const
{
    vec3i i(0,0,0);
    voxelToIndex(v, i);
    return i;
}

//------------------------------------------------------------------------------

template <typename T>
inline void
Volume<T>::indexToVoxel(const vec3i &i, vec3d &v) const
{
    v.x = ((vec3d::BaseType)(i.x)) + m_kernel_offset.x;
    v.y = ((vec3d::BaseType)(i.y)) + m_kernel_offset.y;
    v.z = ((vec3d::BaseType)(i.z)) + m_kernel_offset.z;
}

//------------------------------------------------------------------------------

template <typename T>
inline vec3d
Volume<T>::indexToVoxel(const vec3i &i) const
{
    vec3d v(0.0, 0.0, 0.0);
    indexToVoxel(i, v);
    return v; 
}

//------------------------------------------------------------------------------

template <typename T>
inline void
Volume<T>::indexToLocal(const vec3i &i, vec3d &l) const
{
    vec3d v = indexToVoxel(i);
    voxelToLocal(v, l);
}

//------------------------------------------------------------------------------

template <typename T>
inline vec3d
Volume<T>::indexToLocal(const vec3i &i) const
{
    vec3d v = indexToVoxel(i);
    return voxelToLocal(v);
}

//------------------------------------------------------------------------------

template <typename T>
inline AttributeCollection&
Volume<T>::getAttributeCollection()
{
    return m_attributes;
}

//------------------------------------------------------------------------------

template <typename T>
inline const AttributeCollection&
Volume<T>::getAttributeCollection() const
{
    return m_attributes;
}

//------------------------------------------------------------------------------

template <typename T>
inline void
Volume<T>::read(std::istream &is)
{
    // read in the type.
    String type_name;
    type_name.read(is);

    // Make sure we have the right type.
    if (type_name != typeName()) {
        THROW(Iex::TypeExc, "Invalid volume type.");
    }

    // read in the attributes.
    m_attributes.read(is);

    // read in the local xform.
    m_local_xform.read(is);

    // read in the tree.
    m_tree.read(is);
}

//------------------------------------------------------------------------------

template <typename T>
inline void
Volume<T>::setName(String &name)
{
    m_attributes.insert(kVolumeNameAttr, StringAttribute(name));
}

//------------------------------------------------------------------------------

template <typename T>
inline String
Volume<T>::getName()
{
    return m_attributes.value<String>(kVolumeNameAttr);
}

//------------------------------------------------------------------------------

template <typename T>
inline const String&
Volume<T>::getName() const
{
    return m_attributes.value<String>(kVolumeNameAttr);
}

//------------------------------------------------------------------------------

template <typename T>
inline void
Volume<T>::setDescription(String &description)
{
    m_attributes.insert(kVolumeDescAttr, StringAttribute(description));
}

//------------------------------------------------------------------------------

template <typename T>
inline String
Volume<T>::getDescription()
{
    return m_attributes.value<String>(kVolumeDescAttr);
}

//------------------------------------------------------------------------------

template <typename T>
inline const String&
Volume<T>::getDescription() const
{
    return m_attributes.value<String>(kVolumeDescAttr);
}

//------------------------------------------------------------------------------

template <typename T>
inline void
Volume<T>::read(HDF5Id file_id, String volume_name)
{
    HDF5Group volume_root_group;
    HDF5Group::getRootGroup(file_id, kVolumeRootGroup, volume_root_group);

    readVolume(volume_root_group.id(), volume_name);
}

//------------------------------------------------------------------------------

template <typename T>
inline void
Volume<T>::read(HDF5Id file_id, u32 index)
{
    HDF5Group volume_root_group;
    HDF5Group::getRootGroup(file_id, kVolumeRootGroup, volume_root_group);

    // get the size of the volume group name
    u32 size = H5Lget_name_by_idx(volume_root_group.id(), ".",
                                  H5_INDEX_CRT_ORDER,
                                  H5_ITER_INC,
                                  index,
                                  NULL,
                                  0,
                                  H5P_DEFAULT);
    
    // for some reason HDF5 shorts the name size by 1. Restore it to
    // proper length here.     
    ++size;

    // get the name of the link at index
    char buf[size];
    H5Lget_name_by_idx(volume_root_group.id(), ".",
                       H5_INDEX_CRT_ORDER,
                       H5_ITER_INC,
                       index,
                       buf,
                       size,
                       H5P_DEFAULT);
    
    String volume_name(buf);
    
    // now read in this volume by name
    readVolume(volume_root_group.id(), volume_name);
}

//------------------------------------------------------------------------------

template <typename T>
inline void
Volume<T>::write(std::ostream &os) const
{
    // write out the type of the volume.
    String type_name = typeName();
    type_name.write(os);
    
    // write out the attributes.
    m_attributes.write(os);

    // write out the local xform.
    m_local_xform.write(os);

    // write out the tree.
    m_tree.write(os);
}

//------------------------------------------------------------------------------

template <typename T>
inline void
Volume<T>::write(HDF5Id file_id) const
{
    // a volume root group is needed in order to allow random access to
    // groups by creation index. We can't set this index creation property
    // on the root group so we create a volume root group under the root
    
    // check if the volume root exists
    HDF5Group volume_root_group;
    HDF5Group::getOrCreateRootGroup(file_id, kVolumeRootGroup, 
                                    volume_root_group);

    // get the volume name 
    const String volume_name = 
        m_attributes.value<const String>(kVolumeNameAttr);

    // write the volume
    writeVolume(volume_root_group.id(), volume_name); 
}

//------------------------------------------------------------------------------

template <typename T>
inline bool
Volume<T>::operator==(const Volume<T> &that) const
{
    if ((m_local_xform != that.m_local_xform) ||
        (m_attributes != that.m_attributes)   ||
        (m_tree != that.m_tree)) {
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------

template <typename T>
inline bool
Volume<T>::operator!=(const Volume<T> &that) const
{
    return !(operator==(that));
}

//------------------------------------------------------------------------------

template <typename T>
inline typename Volume<T>::set_iterator
Volume<T>::setIterator() const
{
    return set_iterator(this);
}

//------------------------------------------------------------------------------

template <typename T>
inline void
Volume<T>::createDefaultAttributes()
{
    m_attributes.insert(kVolumeNameAttr, StringAttribute("unknown"));
    m_attributes.insert(kVolumeDescAttr, StringAttribute(""));
}

//------------------------------------------------------------------------------

template <typename T>
inline void
Volume<T>::readVolume(HDF5Id volume_root_group_id, String &volume_name)
{
    // read in the volume group 
    HDF5Group volume_group;
    volume_group.open(volume_root_group_id, volume_name);
    if (!volume_group.isValid()) {
        THROW(Iex::IoExc, "Error file has no HiVE volume");
    }    
    
    // read in the local xform attribute
    m_local_xform.read(volume_group.id());

    // read in the attributes.
    m_attributes.read(volume_group.id());
    
    // read in the leaves and construct the tree
    m_tree.read(volume_group.id());
}

//------------------------------------------------------------------------------

template <typename T>
inline void
Volume<T>::writeVolume(HDF5Id volume_root_group_id, 
                       const String &volume_name) const
{
    // check if a group for the volume exists by attempting to open
    HDF5Group volume_group;
    volume_group.open(volume_root_group_id, volume_name);

    if (volume_group.isValid()) {
        // close the volume group so we can delete it
        volume_group.close();

        // delete the volume group so we can overwrite it with a new one
        HDF5Group::deleteSubtree(volume_root_group_id, volume_name.c_str(),
                                 NULL, NULL);
    }

    // create the group for this volume
    volume_group.create(volume_root_group_id, volume_name);
    
    // if something needs to be done about type it should be done here
    
    // write out the attributes
    m_attributes.write(volume_group.id());

    // write out the local xform
    m_local_xform.write(volume_group.id());

    // write out the tree
    m_tree.write(volume_group.id());
}

//------------------------------------------------------------------------------
// set_iterator implementation
//------------------------------------------------------------------------------

template <typename T>
inline
Volume<T>::set_iterator::set_iterator(const_volume_pointer volume) : 
    m_tree_set_iterator(volume->m_tree.setIterator())
{
}

//------------------------------------------------------------------------------

template <typename T>
inline
Volume<T>::set_iterator::~set_iterator()
{
}

//------------------------------------------------------------------------------

template <typename T>
inline void
Volume<T>::set_iterator::getCoordinates(signed_index_type &i, 
                                        signed_index_type &j, 
                                        signed_index_type &k) const
{
    m_tree_set_iterator.getCoordinates(i, j, k);
}

//------------------------------------------------------------------------------

template <typename T>
inline void
Volume<T>::set_iterator::getCoordinates(signed_index_vec &coords) const
{
    getCoordinates(coords[0], coords[1], coords[2]);
}

//------------------------------------------------------------------------------

template <typename T>
inline typename Volume<T>::const_reference
Volume<T>::set_iterator::operator*() const
{
    return *m_tree_set_iterator;
}

//------------------------------------------------------------------------------

template <typename T>
inline typename Volume<T>::set_iterator&
Volume<T>::set_iterator::operator++()
{
    ++m_tree_set_iterator;
    return *this;
}

//------------------------------------------------------------------------------

template <typename T>
inline bool
Volume<T>::set_iterator::operator()() const
{
    return m_tree_set_iterator();
}

//------------------------------------------------------------------------------
