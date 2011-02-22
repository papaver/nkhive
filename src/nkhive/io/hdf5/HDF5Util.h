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
// HDF5Util.h
//------------------------------------------------------------------------------

#ifndef __NKHIVE_IO_HDF5_HDF5UTIL_H__
#define __NKHIVE_IO_HDF5_HDF5UTIL_H__

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------

#include <nkhive/io/hdf5/HDF5Base.h>
#include <nkhive/Types.h>
#include <nkbase/String.h>

BEGIN_NKHIVE_NS

//------------------------------------------------------------------------------
// defines
//------------------------------------------------------------------------------

#define NKHIVE_LEAF_GRP_MAX_STR_LEN 256

//------------------------------------------------------------------------------
// types
//------------------------------------------------------------------------------

enum LeafType {
    LEAF_TYPE_CELL = 0,
    LEAF_TYPE_FILL_NODE,
};

//------------------------------------------------------------------------------
// io constants
//------------------------------------------------------------------------------

/**
 * name of group that holds user attributes
 */
const String kUserAttrGroup = String("UserAttributes");

/**
 * volume name
 */
const String kVolumeNameAttr = String("name");

/**
 * volume description
 */
const String kVolumeDescAttr = String("description");

/**
 * xform attribute
 */
const String kLocalXformAttr = String("LocalXform");

/**
 * name of root group for all volumes in the file
 */
const String kVolumeRootGroup = String("HiveVolumeRoot");

/**
 * name of the volume's internal default value attribute
 */
const String kDefaultValueAttr = String("DefaultValue");

/**
 * name of the volume's internal branching factor attribute
 */
const String kBranchingFactorAttr = String("BranchingFactor");

/**
 * name of the volume's internal cell dimension attribute
 * NOTE: this is a lg dimension
 */
const String kCellDimAttr = String("CellDimensions");

/**
 * name of internal attribute storing index based offset to the 
 * current block of data.
 * Used by: 
 * - Tree
 * - Node
 * - Cell
 */
const String kIndexOffsetAttr = String("IndexOffset");

/**
 * name of internal attribute storing the quadrant for each leaf
 * stored in the file. This is to resolve ambiguity in the
 * index offset around 0 indices
 * Used by:
 * - Tree
 * - Node
 * - Cell
 */
const String kQuadrantAttr = String("Quadrant");

/**
 * name of internal attribute storing fill value for node
 */
const String kFillNodeValueAttr = String("FillValue");

/**
 * name of internal attribute storing level for filled bounds node
 */
const String kFillNodeLevelAttr = String("Level");

/**
 * name of internal attribute storing leaf type.
 * Values correspond to LeafType enum
 */
const String kLeafTypeAttr = String("LeafType");

/**
 * name of Cell data set for voxel data
 */
const String kCellDataSetName = String("VoxelData");

/**
 * name of internal attribute storing flags for Cell
 */
const String kCellFlagsAttr = String("Flags");

/**
 * name of internal attribute storing default or fill value for Cell
 */
const String kCellDefaultValueAttr = String("DefaultValue");
const String kCellFillValueAttr = String("FillValue");

/**
 * name of data set representing bit field
 */
const String kBitFieldName = String("BitField");

/**
 * internal attribute on bit field data set representing bitfield size
 */
const String kBitFieldSize = String("BitFieldSize");

//------------------------------------------------------------------------------
// struct definitions
//------------------------------------------------------------------------------

template <typename T>
struct TypeToHDF5Type
{
    static HDF5Id type();
};

//------------------------------------------------------------------------------
// io utility function declarations
//------------------------------------------------------------------------------

void readScalarAttribute(HDF5Id parent_id, const String &attr_name,
                         HDF5Id data_type_id, void *data);

void writeScalarAttribute(HDF5Id parent_id, const String &attr_name, 
                          HDF5Id data_type_id, const void *data);

hsize_t getVectorAttributeSize(HDF5Id parent_id, const String &attr_name);

void readVectorAttribute(HDF5Id parent_id, const String &attr_name, void *data);

void writeVectorAttribute(HDF5Id parent_id, const String &attr_name,
                          HDF5Id component_data_type_id, ssize_t num_components,
                          const void *data);

hsize_t getDataSetStorageSize(HDF5Id parent_id,
                              const String &name);

void readSimpleDataSet(HDF5Id parent_id, const String &name,
                       HDF5Id data_type_id, void *data);

void writeSimpleDataSet(HDF5Id parent_id, const String &name, i32 rank, 
                        const hsize_t *dims, HDF5Id data_type_id, 
                        const void *data);

void disableHDF5ErrorOutput();

void constructLeafGroupName(LeafType type, u8 quadrant, index_vec offset,
                            String &name);

//------------------------------------------------------------------------------
// template specializations
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// signed integer types
//------------------------------------------------------------------------------

template <>
inline HDF5Id 
TypeToHDF5Type<i8>::type()
{
    return H5T_NATIVE_SCHAR;
}

//------------------------------------------------------------------------------

template <>
inline HDF5Id 
TypeToHDF5Type<const i8>::type()
{
    return H5T_NATIVE_SCHAR;
}

//------------------------------------------------------------------------------

template <>
inline HDF5Id 
TypeToHDF5Type<i16>::type()
{
    return H5T_NATIVE_SHORT;
}

//------------------------------------------------------------------------------

template <>
inline HDF5Id 
TypeToHDF5Type<const i16>::type()
{
    return H5T_NATIVE_SHORT;
}

//------------------------------------------------------------------------------

template <>
inline HDF5Id 
TypeToHDF5Type<i32>::type()
{
    // H5T_NATIVE_LONG?
    return H5T_NATIVE_INT;
}

//------------------------------------------------------------------------------

template <>
inline HDF5Id 
TypeToHDF5Type<const i32>::type()
{
    // H5T_NATIVE_LONG?
    return H5T_NATIVE_INT;
}

//------------------------------------------------------------------------------

template <>
inline HDF5Id 
TypeToHDF5Type<i64>::type()
{
    return H5T_NATIVE_LLONG;
}

//------------------------------------------------------------------------------

template <>
inline HDF5Id 
TypeToHDF5Type<const i64>::type()
{
    return H5T_NATIVE_LLONG;
}

//------------------------------------------------------------------------------
// unsigned integer types
//------------------------------------------------------------------------------

template <>
inline HDF5Id 
TypeToHDF5Type<u8>::type()
{
    return H5T_NATIVE_CHAR;
}

//------------------------------------------------------------------------------

template <>
inline HDF5Id 
TypeToHDF5Type<const u8>::type()
{
    return H5T_NATIVE_CHAR;
}

//------------------------------------------------------------------------------

template <>
inline HDF5Id 
TypeToHDF5Type<u16>::type()
{
    return H5T_NATIVE_SHORT;
}

//------------------------------------------------------------------------------

template <>
inline HDF5Id 
TypeToHDF5Type<const u16>::type()
{
    return H5T_NATIVE_SHORT;
}

//------------------------------------------------------------------------------

template <>
inline HDF5Id 
TypeToHDF5Type<u32>::type()
{
    // H5T_NATIVE_ULONG?
    return H5T_NATIVE_UINT;
}

//------------------------------------------------------------------------------

template <>
inline HDF5Id 
TypeToHDF5Type<const u32>::type()
{
    // H5T_NATIVE_ULONG?
    return H5T_NATIVE_UINT;
}

//------------------------------------------------------------------------------

template <>
inline HDF5Id 
TypeToHDF5Type<u64>::type()
{
    return H5T_NATIVE_ULLONG;
}

//------------------------------------------------------------------------------

template <>
inline HDF5Id 
TypeToHDF5Type<const u64>::type()
{
    return H5T_NATIVE_ULLONG;
}

//------------------------------------------------------------------------------
// floating point types
//------------------------------------------------------------------------------

template <>
inline HDF5Id 
TypeToHDF5Type<f16>::type()
{
    return H5T_NATIVE_B16;
}

//------------------------------------------------------------------------------

template <>
inline HDF5Id 
TypeToHDF5Type<const f16>::type()
{
    return H5T_NATIVE_B16;
}

//------------------------------------------------------------------------------

template <>
inline HDF5Id 
TypeToHDF5Type<f32>::type()
{
    return H5T_NATIVE_FLOAT;
}

//------------------------------------------------------------------------------

template <>
inline HDF5Id 
TypeToHDF5Type<const f32>::type()
{
    return H5T_NATIVE_FLOAT;
}

//------------------------------------------------------------------------------

template <>
inline HDF5Id 
TypeToHDF5Type<f64>::type()
{
    return H5T_NATIVE_DOUBLE;
}

//------------------------------------------------------------------------------

template <>
inline HDF5Id 
TypeToHDF5Type<const f64>::type()
{
    return H5T_NATIVE_DOUBLE;
}

//------------------------------------------------------------------------------
// size type
//------------------------------------------------------------------------------

template <>
inline HDF5Id 
TypeToHDF5Type<size_t>::type()
{
    return H5T_NATIVE_HSIZE;
}

//------------------------------------------------------------------------------

template <>
inline HDF5Id 
TypeToHDF5Type<const size_t>::type()
{
    return H5T_NATIVE_HSIZE;
}

//------------------------------------------------------------------------------
// char type
//------------------------------------------------------------------------------

template <>
inline HDF5Id 
TypeToHDF5Type<char>::type()
{
    return H5T_NATIVE_CHAR;
}

//------------------------------------------------------------------------------

template <>
inline HDF5Id 
TypeToHDF5Type<const char>::type()
{
    return H5T_NATIVE_CHAR;
}

END_NKHIVE_NS

#endif // __NKHIVE_IO_HDF5_HDF5UTIL_H__
