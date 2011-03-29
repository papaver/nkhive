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
// HDF5Group.h
//------------------------------------------------------------------------------

#ifndef __NKHIVE_IO_HDF5_HDF5GROUP_H__
#define __NKHIVE_IO_HDF5_HDF5GROUP_H__

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------

#include <nkhive/io/hdf5/HDF5Base.h>
#include <nkbase/String.h>

//------------------------------------------------------------------------------
// interface
//------------------------------------------------------------------------------

BEGIN_NKHIVE_NS

class HDF5Group : public HDF5Base
{

public:

    //--------------------------------------------------------------------------
    // constructors/destructors
    //--------------------------------------------------------------------------
    
    HDF5Group();
    ~HDF5Group();

    //--------------------------------------------------------------------------
    // public interface
    //--------------------------------------------------------------------------

    void create(const HDF5Id location_id, const String &name);
    void open(const HDF5Id location_id, const String &name);

    /**
     * if you want control over closing the group earlier than 
     * destruction of this object
     */
    void close();

    /**
     * return the number of children linked from this group
     */
    i32 numChildren() const;

    //--------------------------------------------------------------------------
    // static methods
    //--------------------------------------------------------------------------

    /**
     * Returns the root group of the given name from a HDF5 file.
     */
    static void getRootGroup(HDF5Id file_id, const String &group_name,
                             HDF5Group &named_root_group);

    /**
     * Returns the named root group if it exists, it it does not, then it will
     * be created. 
     */
    static void getOrCreateRootGroup(HDF5Id file_id, const String &name,
                                     HDF5Group &named_root_group);

    static HDF5Err deleteSubtree(HDF5Id group_id, const char *group_name,
                                 const H5L_info_t *group_info, void *op_data);
};

END_NKHIVE_NS

#endif // __NKHIVE_IO_HDF5_HDF5GROUP_H__
