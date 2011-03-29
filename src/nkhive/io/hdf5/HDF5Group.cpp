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
// HDF5Group.cpp
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------

#include <nkhive/io/hdf5/HDF5Group.h>
#include <nkhive/io/hdf5/HDF5Property.h>
#include <nkbase/Exceptions.h>

BEGIN_NKHIVE_NS

//------------------------------------------------------------------------------
// interface
//------------------------------------------------------------------------------

HDF5Group::HDF5Group() :
    HDF5Base()
{
}

//------------------------------------------------------------------------------

HDF5Group::~HDF5Group()
{
    close();
}

//------------------------------------------------------------------------------

void
HDF5Group::create(const HDF5Id location_id, const String &name)
{
    // set up some default group creation properties for tracking 
    // group links by index

    // create group creation property list
    HDF5Property create_prop_list(H5P_GROUP_CREATE);

    // add creation order tracking and indexing on creation order
    H5Pset_link_creation_order(create_prop_list.id(),
                               H5P_CRT_ORDER_TRACKED | H5P_CRT_ORDER_INDEXED);

    m_id = H5Gcreate(location_id, name.c_str(), 
                     H5P_DEFAULT,
                     create_prop_list.id(), 
                     H5P_DEFAULT);
}

//------------------------------------------------------------------------------

void
HDF5Group::open(const HDF5Id location_id, const String &name)
{
    m_id = H5Gopen(location_id, name.c_str(), H5P_DEFAULT);
}

//------------------------------------------------------------------------------

void HDF5Group::close()
{
    if (isValid()) {
        H5Gclose(m_id);
        m_id = -1;
    }
}

//------------------------------------------------------------------------------

i32 HDF5Group::numChildren() const
{
    // get the group info
    H5G_info_t group_info;
    HDF5Err res = H5Gget_info(m_id, &group_info);

    if (res < 0) {
        THROW(Iex::IoExc, "Could not get info for group");
    }

    // return the number of links from the group info struct
    return group_info.nlinks;
}

//------------------------------------------------------------------------------

void
HDF5Group::getRootGroup(HDF5Id file_id, const String &name, 
                        HDF5Group &named_root_group)
{
    // first get the root group.
    HDF5Group root_group;
    root_group.open(file_id, String("/"));

    // Now get the named root group.
    named_root_group.open(root_group.id(), name);

    if (!named_root_group.isValid()) {
        THROW(Iex::IoExc, "Root group " << name << " not found.");
    }
}

//------------------------------------------------------------------------------

void
HDF5Group::getOrCreateRootGroup(HDF5Id file_id, const String &name,
                                HDF5Group &named_root_group)
{
    // first get the root group.
    HDF5Group root_group;
    root_group.open(file_id, String("/"));

    // Now get the named root group.
    named_root_group.open(root_group.id(), name);

    if (!named_root_group.isValid()) {
        named_root_group.create(root_group.id(), name);
    }
}

//------------------------------------------------------------------------------

HDF5Err
HDF5Group::deleteSubtree(HDF5Id group_id, const char *group_name,
                         const H5L_info_t *NK_UNUSED_PARAM(group_info), 
                         void *NK_UNUSED_PARAM(op_data))
{
    // open the child group
    HDF5Group child_group;
    child_group.open(group_id, String(group_name));

    // iterate over the group's children
    HDF5Size iter_index = 0;
    H5Literate(child_group.id(), H5_INDEX_CRT_ORDER, H5_ITER_NATIVE, 
               &iter_index, deleteSubtree, NULL);

    // close the child group
    child_group.close();

    // delete the link
    H5Ldelete(group_id, group_name, H5P_DEFAULT);

    return 0;
}

END_NKHIVE_NS
