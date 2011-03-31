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
// Volume.h
//------------------------------------------------------------------------------

#ifndef __NKHIVE_VOLUME_VOLUME_H__
#define __NKHIVE_VOLUME_VOLUME_H__

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------

#include <boost/shared_ptr.hpp>

#include <nkbase/BinaryOps.h>

#include <nkhive/Defs.h>
#include <nkhive/Types.h>
#include <nkhive/attributes/AttributeCollection.h>
#include <nkhive/tiling/Stamp.h>
#include <nkhive/volume/Cell.h>
#include <nkhive/volume/Tree.h>
#include <nkhive/xforms/LocalXform.h>
#include <nkhive/io/hdf5/HDF5Group.h>

//------------------------------------------------------------------------------
// forward delarations
//------------------------------------------------------------------------------

class TestVolumeFile;

//------------------------------------------------------------------------------
// class definition
//------------------------------------------------------------------------------

BEGIN_NKHIVE_NS

template <typename T>
class Volume
{

public:

    //--------------------------------------------------------------------------
    // typedefs
    //--------------------------------------------------------------------------
    
    typedef T                         value_type;
    typedef T&                        reference;
    typedef const T&                  const_reference;
    typedef boost::shared_ptr<Volume> shared_ptr;

    //--------------------------------------------------------------------------
    // public interface
    //--------------------------------------------------------------------------

    Volume();
    Volume(uint8_t lg_branching_factor, uint8_t lg_cell_dim, 
           const_reference default_value, const vec3d &localres = vec3d(1.0),
           const vec3d &kernel_offset = vec3d(0.0));
    ~Volume();

    /** 
     * Return the name of the type of volume this is. 
     */
    String typeName() const;

    /**
     * Return the discrete offset
     */
    vec3d kernelOffset() const;

    /**
     * Return the default value of the volume. 
     */
    const_reference getDefault() const;

    /** 
     * Get the value at voxel index i,j,k.
     */
    const_reference get(signed_index_type i, 
                        signed_index_type j, 
                        signed_index_type k) const;
    const_reference get(const signed_index_vec &coords) const;

    /**
     * Set the value at voxel index i,j,k.
     */
    void set(signed_index_type i, 
             signed_index_type j, 
             signed_index_type k, const_reference val);
    void set(const signed_index_vec &coords, const_reference val);

    /**
     * Unset the value at the voxel i,j,k if it is already set.
     */
    void unset(signed_index_type i, signed_index_type j, signed_index_type k);
    void unset(const signed_index_vec &coords);

    /**
     * Update the value at the voxel index i,j,k based on the type of the binary
     * operation given. 
     */
    template <typename BinaryOp>
    void update(signed_index_type i, signed_index_type j, signed_index_type k, 
                const_reference val, 
                BinaryOp op);
    template <typename BinaryOp>
    void update(const signed_index_vec &coords, const_reference val, 
                BinaryOp op);

    /**
     * Writes the stamp at the requested location using the given op.
     * The position indicates the minina of the stamps bounds.
     */
    template <typename U, template <typename> class Source>
    void stamp(Stamp<U, Source> &stamp, signed_index_vec &position);

    /**
     * Return true if there are no set values in the volume. 
     */
    bool isEmpty() const;

    /** 
     * Get the axis aligned bounds bounding all set values in the volume.
     * Return false if there are no values set in the volume.
     */
    bool computeSetBounds(signed_index_bounds &bounds) const;
   
    /**
     * Get the axis aligned bounds bounding all set values in local
     * space in the volume.
     * Return false if there are no values set in the volume.
     */
    bool computeSetBounds(Bounds3D<double> &bounds) const;

    /**
     * Returns memory used by class.
     */
    int sizeOf() const;

    //--------------------------------------------------------------------------
    // Local Xform methods.
    //--------------------------------------------------------------------------

    /** 
     * Set a local transform given the resolution.
     */
    void setLocalXform(const vec3d &res);

    /** 
     * The resolution of volume in local coordinates. 
     */
    vec3d res() const;

    void localToVoxel(const vec3d &l, vec3d &v) const;
    vec3d localToVoxel(const vec3d &l) const;

    void localToIndex(const vec3d &l, vec3i &i) const;
    vec3i localToIndex(const vec3d &l) const;

    void voxelToLocal(const vec3d &v, vec3d &l) const;
    vec3d voxelToLocal(const vec3d &v) const;

    void voxelToIndex(const vec3d &v, vec3i &i) const;
    vec3i voxelToIndex(const vec3d &v) const;

    void indexToVoxel(const vec3i &i, vec3d &v) const;
    vec3d indexToVoxel(const vec3i &i) const;

    void indexToLocal(const vec3i &i, vec3d &l) const;
    vec3d indexToLocal(const vec3i &i) const;

    //--------------------------------------------------------------------------
    // Attribute methods.
    //--------------------------------------------------------------------------

    AttributeCollection& getAttributeCollection();
    const AttributeCollection& getAttributeCollection() const;

    void   setName(String &name);
    String getName();
    const String& getName() const;

    void setDescription(String &description);
    String getDescription();
    const String& getDescription() const;

    //--------------------------------------------------------------------------
    // IO methods.
    //--------------------------------------------------------------------------

    void read(std::istream &is);
    void write(std::ostream &os) const;
    void read(HDF5Id file_id, String volume_name);
    void read(HDF5Id file_id, u32 index = 0);
    void write(HDF5Id file_id) const;

    //--------------------------------------------------------------------------
    // Operators.
    //--------------------------------------------------------------------------

    bool operator==(const Volume &that) const;
    bool operator!=(const Volume &that) const;

    //--------------------------------------------------------------------------
    // forward declare iterators and their access methods
    //--------------------------------------------------------------------------

    class set_iterator;

    /** 
     * Return an instance of set_iterator that allows iteration over all set
     * values in the volume.
     */
    set_iterator setIterator() const;

private:
    
    //--------------------------------------------------------------------------
    // typedefs
    //--------------------------------------------------------------------------
    
    typedef Cell<T>         cell_type;
    typedef Tree<cell_type> tree_type;

    //--------------------------------------------------------------------------
    // Internal helpers. 
    //--------------------------------------------------------------------------

    /**
     * Creates default attributes for each volume.
     */
    void createDefaultAttributes();

    /**
     * Handles reading of volume data 
     */
    void readVolume(HDF5Id volume_root_group_id, String &volume_name);
    void writeVolume(HDF5Id volume_root_group_id, 
                     const String &volume_name) const;

    //--------------------------------------------------------------------------
    // members
    //--------------------------------------------------------------------------

    /** 
     * Store the instance of the B+Tree that will store the values in the
     * volume.
     */
    tree_type m_tree;

    /**
     * The local transform for this volume. This is an intrinsic property of
     * the volume.
     */
    LocalXform m_local_xform;

    /**
     * Offset mapping from discrete space to voxel space
     */
    vec3d m_kernel_offset;
    
    /**
     * Attributes for the volume are stored in an AttributeCollection.
     */
    AttributeCollection m_attributes;

    //--------------------------------------------------------------------------
    // friends
    //--------------------------------------------------------------------------

    #ifdef UNITTEST
        friend class ::TestVolumeFile;
    #endif // UNITTEST 

};

//------------------------------------------------------------------------------
// set_iterator interface.
//------------------------------------------------------------------------------

template <typename T>
class Volume<T>::set_iterator
{
public:

    //--------------------------------------------------------------------------
    // typedefs.
    //--------------------------------------------------------------------------

    typedef std::forward_iterator_tag                   iterator_category;
    typedef typename Volume::value_type                 value_type;
    typedef typename Volume::reference                  reference;
    typedef typename Volume::const_reference            const_reference;
    typedef const Volume*                               const_volume_pointer;
    typedef typename Volume::tree_type::set_iterator    tree_set_iterator;

    //--------------------------------------------------------------------------
    // public interface
    //--------------------------------------------------------------------------

    set_iterator(const_volume_pointer volume);
    ~set_iterator();

    /*
     * return index coordinates of the current iterator position
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

protected:
private:

    //--------------------------------------------------------------------------
    // members
    //--------------------------------------------------------------------------

    /**
     * The iterator for the underlying tree. 
     */
    tree_set_iterator m_tree_set_iterator;

};

END_NKHIVE_NS

//-----------------------------------------------------------------------------
// class implementation
//-----------------------------------------------------------------------------

BEGIN_NKHIVE_NS

#include <nkhive/volume/Volume.hpp>

END_NKHIVE_NS

#endif //__NKHIVE_VOLUME_VOLUME_H__
