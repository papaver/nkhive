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
// HDF5Util.cpp
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------

#include <nkbase/Exceptions.h>
#include <nkhive/io/hdf5/HDF5Util.h>
#include <nkhive/io/hdf5/HDF5Attribute.h>
#include <nkhive/io/hdf5/HDF5DataSpace.h>
#include <nkhive/io/hdf5/HDF5DataType.h>
#include <nkhive/io/hdf5/HDF5DataSet.h>

BEGIN_NKHIVE_NS

//------------------------------------------------------------------------------
// interface implementation
//------------------------------------------------------------------------------

void
readScalarAttribute(HDF5Id parent_id, const String &attr_name,
                    HDF5Id data_type_id, void *data)
{

    // check if the attribute exists
    HDF5Tri attr_exists = H5Aexists(parent_id, attr_name.c_str());
    if (attr_exists == 0) {
        THROW(Iex::IoExc, "readScalarAttribute - Attribute " << 
                          attr_name.c_str() << " doesn't exist");
    }

    // open the attribute
    HDF5Attribute attr;
    attr.open(parent_id, attr_name);

    // check if data type is valid
    HDF5DataType attr_data_type;
    if (data_type_id == -1) {
        // if no data type, get it from the attribute
        attr_data_type.getFromAttribute(attr.id());
        data_type_id = attr_data_type.id();
    }

    // read the attribute
    attr.read(data_type_id, data);
}

//------------------------------------------------------------------------------

void
writeScalarAttribute(HDF5Id parent_id, const String &attr_name, 
                     HDF5Id data_type_id, const void *data)
{
    // check if data type is valid
    if (data_type_id == -1) {
        THROW(Iex::IoExc, "writeScalarAttribute - Invalid data type for "
                          "attribute " << attr_name.c_str());
    }

    // check if the attribute already exists
    HDF5Tri attr_exists = H5Aexists(parent_id, attr_name.c_str());

    // attribute doesn't exist. Create it. 
    HDF5Attribute attr; 
    if (attr_exists == 0) {
             
        // create the scalar data space
        HDF5DataSpace data_space;
        data_space.createScalar();

        // create the attribute 
        attr.create(parent_id, attr_name, 
                    data_type_id, data_space.id());

        // write out the data
        attr.write(data_type_id, data);

    } else {

        // attribute exists. Open it for writing. 
        attr.open(parent_id, attr_name);

        // write out the data
        attr.write(data_type_id, data);
    }
}

//------------------------------------------------------------------------------

hsize_t
getVectorAttributeSize(HDF5Id parent_id, const String &attr_name)
{
    // check if the attribute exists
    HDF5Tri attr_exists = H5Aexists(parent_id, attr_name.c_str());
    if (attr_exists == 0) {
        THROW(Iex::IoExc, "readScalarAttribute - Attribute " << 
                          attr_name.c_str() << " doesn't exist");
    }

    // open the attribute
    HDF5Attribute attr;
    attr.open(parent_id, attr_name);

    // get the compound data type from the attribute for reading in data
    HDF5DataType cmpd_data_type;
    cmpd_data_type.getFromAttribute(attr.id());

    // return size in bytes    
    return H5Tget_size(cmpd_data_type.id()); 
}

//------------------------------------------------------------------------------

void
readVectorAttribute(HDF5Id parent_id, const String &attr_name, void *data)
{
    // check if the attribute exists
    HDF5Tri attr_exists = H5Aexists(parent_id, attr_name.c_str());
    if (attr_exists == 0) {
        THROW(Iex::IoExc, "readScalarAttribute - Attribute " << 
                          attr_name.c_str() << " doesn't exist");
    }

    // open the attribute
    HDF5Attribute attr;
    attr.open(parent_id, attr_name);

    // get the compound data type from the attribute for reading in data
    HDF5DataType cmpd_data_type;
    cmpd_data_type.getFromAttribute(attr.id());
    
    // read the attribute
    attr.read(cmpd_data_type.id(), data);
}

//------------------------------------------------------------------------------

void
writeVectorAttribute(HDF5Id parent_id, const String &attr_name,
                     HDF5Id component_data_type_id,
                     ssize_t num_components,
                     const void *data)
{
    // check if data type is valid
    if (component_data_type_id == -1) {
        THROW(Iex::IoExc, "writeVectorAttribute - Invalid data type for "
                          "attribute " << attr_name.c_str());
    }
    
    // check if the attribute already exists
    HDF5Tri attr_exists = H5Aexists(parent_id, attr_name.c_str());

    // create the array data type
    HDF5DataType data_type;
    u32 rank = 1;
    hsize_t type_dims[rank];
    type_dims[0] = num_components;
    data_type.createArray(component_data_type_id, rank, type_dims);

    // attribute doesn't exist. Create it. 
    HDF5Attribute attr; 
    if (attr_exists == 0) {

        // create the scalar data space
        HDF5DataSpace data_space;
        data_space.createScalar();

        // create the attribute
        attr.create(parent_id, attr_name, 
                    data_type.id(), data_space.id());
        
        // write out the data
        attr.write(data_type.id(), data);

    } else {

        // attribute exists. Open it for writing. 
        attr.open(parent_id, attr_name);

        // write out the data
        attr.write(data_type.id(), data);
    }
}

//------------------------------------------------------------------------------

hsize_t
getDataSetStorageSize(HDF5Id parent_id, const String &name)
{
    // open the data set
    HDF5DataSet data_set;
    data_set.open(parent_id, name);

    if (!data_set.isValid()) {
        THROW(Iex::IoExc, "getDataSetStorageSize - Invalid data set " 
            << name.c_str());
    }

    // get the storage size
    return H5Dget_storage_size(data_set.id());
}

//------------------------------------------------------------------------------

void
readSimpleDataSet(HDF5Id parent_id, const String &name, HDF5Id data_type_id,
                  void *data)
{
    // open the data set
    HDF5DataSet data_set;
    data_set.open(parent_id, name);

    if (!data_set.isValid()) {
        THROW(Iex::IoExc, "readSimpleDataSet - Invalid data set " 
                          << name.c_str());
    }

    // determine data type
    HDF5DataType data_type;
    if (data_type_id == -1) {
        // get the data type from the data set
        data_type.getFromDataSet(data_set.id());
        data_type_id = data_type.id();
    }

    // read in the data
    data_set.read(data_type_id, data);
}

//------------------------------------------------------------------------------

void
writeSimpleDataSet(HDF5Id parent_id, const String &name, i32 rank, 
                   const hsize_t *dims, HDF5Id data_type_id, const void *data)
{
    // check if data type is valid
    if (data_type_id == -1) {
        THROW(Iex::IoExc, "writeSimpleDataSet - Invalid data type for "
                          "attribute " << name.c_str());
    }

    // try to open first
    HDF5DataSet data_set;
    data_set.open(parent_id, name);

    // open failed. Create new.
    if (!data_set.isValid()) {
        
        // create the data space
        HDF5DataSpace data_space;
        data_space.createSimple(rank, dims, dims);

        // create the data set
        data_set.create(parent_id, name, data_type_id, data_space.id());
    }

    // write the data out
    data_set.write(data_type_id, data);
}

//------------------------------------------------------------------------------

void
disableHDF5ErrorOutput()
{
    // Disable the error reporting on the default error stack.
    H5Eset_auto(H5E_DEFAULT, NULL, NULL);
}

//------------------------------------------------------------------------------

void
constructLeafGroupName(LeafType type, u8 quadrant, index_vec offset, 
                       String &name)
{
    // set the group name prefix based on leaf type
    String prefix;
    if (type == LEAF_TYPE_CELL) {
        prefix = String("cell");
    } else if (type == LEAF_TYPE_FILL_NODE) {
        prefix = String("leaf");
    }
    
    char buf[NKHIVE_LEAF_GRP_MAX_STR_LEN];
    sprintf(buf, "%s_q%u_%u_%u_%u", prefix.c_str(), quadrant, 
                                    offset[0], offset[1], offset[2]);

    name = String(buf);
}

//------------------------------------------------------------------------------

END_NKHIVE_NS
