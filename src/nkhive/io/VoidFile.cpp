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
// VoidFile.cpp
//------------------------------------------------------------------------------

#include <nkhive/io/VoidFile.h>
#include <cstring>
#include <nkhive/io/hdf5/HDF5Group.h>
#include <nkhive/io/hdf5/HDF5Property.h>
#include <nkhive/io/hdf5/HDF5Attribute.h>
#include <nkhive/io/hdf5/HDF5DataSpace.h>
#include <nkhive/io/hdf5/HDF5DataType.h>
#include <nkhive/io/hdf5/HDF5Util.h>

BEGIN_NKHIVE_NS

//------------------------------------------------------------------------------
// class globals
//------------------------------------------------------------------------------

const String VoidFile::TAG_STR            = String("nektar_");

const String VoidFile::kContainerEnumType = String("container_enum_type");
const String VoidFile::kVersionType       = String("version_type");

const String VoidFile::kTagAttr           = String("tag");
const String VoidFile::kContainerAttr     = String("container_type");
const String VoidFile::kVersionAttr       = String("version");

//------------------------------------------------------------------------------
// interface implementation
//------------------------------------------------------------------------------

VoidFile::VoidFile(ContainerType type) :
    m_type(type),
    m_id(-1)
{
    m_version.major    = 0;
    m_version.minor    = 0;
    m_version.revision = 0;

    disableHDF5ErrorOutput();
}

//------------------------------------------------------------------------------

VoidFile::VoidFile(const String &file_path, ContainerType type, OpenMode mode) :
    m_type(type),
    m_id(-1)
{
    open(file_path, mode);

    disableHDF5ErrorOutput();
}

//------------------------------------------------------------------------------

VoidFile::~VoidFile()
{
    close();
}

//------------------------------------------------------------------------------

VoidFile::Version& 
VoidFile::getVersion()
{
    return m_version;
}

//------------------------------------------------------------------------------

void 
VoidFile::setVersion(u32 major, u32 minor, u32 revision)
{
    m_version.major    = major;
    m_version.minor    = minor;
    m_version.revision = revision;
}

//------------------------------------------------------------------------------

VoidFile::ContainerType 
VoidFile::getType()
{
    return m_type;
}

//------------------------------------------------------------------------------

void 
VoidFile::setType(ContainerType type) throw (Iex::TypeExc)
{
    if (type == TYPE_INVALID) {
        THROW(Iex::TypeExc, "Invalid container type.");
    }

    m_type = type;
}

//------------------------------------------------------------------------------

void
VoidFile::open(const String &file_path, OpenMode mode)
{
    // set the creation parameters
    HDF5Property ap_params(H5P_FILE_ACCESS);
    HDF5Property cp_params(H5P_FILE_CREATE);

    // attempt to open an existing file
    if (mode == READ_ONLY) {
        m_id = H5Fopen(file_path.c_str(), H5F_ACC_RDONLY, ap_params.id());
    // open file writeable without deleting contents
    } else if (mode == READ_WRITE) {
        // try open first
        m_id = H5Fopen(file_path.c_str(), H5F_ACC_RDWR, ap_params.id());

        // file doesn't exist. Create it.
        if (m_id < 0) {
            m_id = H5Fcreate(file_path.c_str(), H5F_ACC_TRUNC, cp_params.id(), 
                             ap_params.id());
        
            // if this failed we should throw an exception
            if (m_id < 0) {
                THROW(Iex::IoExc, "Could not open file " << file_path);
            }
        }
    // open file writeable and delete contents or create new file for write
    } else if (mode == WRITE_TRUNC) {
        
        // create the file
        m_id = H5Fcreate(file_path.c_str(), H5F_ACC_TRUNC, cp_params.id(), 
                         ap_params.id());

        // if this failed we should throw an exception
        if (m_id < 0) {
            THROW(Iex::IoExc, "Could not open file " << file_path);
        }
    } else {
        THROW(Iex::IoExc, "Unsupported file access mode");
    }
}

//------------------------------------------------------------------------------

void
VoidFile::close()
{
    if (m_id != -1) {
        H5Fflush(m_id, H5F_SCOPE_GLOBAL);
        H5Fclose(m_id);

        // reset file handle
        m_id = -1;
    }
}    

//------------------------------------------------------------------------------

void 
VoidFile::read(std::istream &is) throw (Iex::InputExc)
{
    // read in tag and verify 
    String tag;
    tag.read(is);
    if (tag != TAG_STR) {
        THROW(Iex::InputExc, "Invalid file type: nektar tag not found.");
    }

    // read in type and verify 
    is.read((char*)&m_type, sizeof(ContainerType));
    if (m_type == TYPE_INVALID) {
        THROW(Iex::InputExc, "Invalid file type: invalid container type.");
    }

    // read in version
    is.read((char*)&m_version, sizeof(Version));
}

//------------------------------------------------------------------------------

void 
VoidFile::write(std::ostream &os) const
{
    // write out tag
    TAG_STR.write(os);
    
    // write out container type 
    os.write((char*)&m_type, sizeof(ContainerType));

    // write out version
    os.write((char*)&m_version, sizeof(Version));
}

//------------------------------------------------------------------------------

void
VoidFile::read()
{
    // first get the root group
    HDF5Group root_group;
    root_group.open(m_id, String("/"));

    // get the size of the tag string
    HDF5Size tag_data_size = getVectorAttributeSize(root_group.id(), kTagAttr);
    char data[tag_data_size];

    // read in the tag attribute
    readVectorAttribute(root_group.id(), kTagAttr, &(data[0]));

    String tag(data);
    if (tag != TAG_STR) {
        THROW(Iex::InputExc, "Invalid file type: nektar tag not found.");
    }

    // get the container type attribute
    ContainerType type;
    readScalarAttribute(root_group.id(), kContainerAttr, -1, &type);

    // ensure the file type matches the type we're trying to load
    if (type != m_type || type == TYPE_INVALID) {
        THROW(Iex::InputExc, "Invalid file type: invalid container type.");
    }

    // read the version attribute
    readScalarAttribute(root_group.id(), kVersionAttr, -1, &m_version);
}

//------------------------------------------------------------------------------

void
VoidFile::write() const
{
    // write out the header as attributes on the root group

    // first get the root group
    HDF5Group root_group;
    root_group.open(m_id, String("/"));

    // handle the tag attribute
    writeTagAttribute(root_group.id());

    // handle the container type attribute
    writeContainerTypeAttribute(root_group.id());

    // handle the version attribute 
    writeVersionAttribute(root_group.id());
}

//------------------------------------------------------------------------------

void
VoidFile::writeTagAttribute(HDF5Id root_group_id) const
{
    // write out the tag attribute
    // add 1 to string length to include null terminator
    writeVectorAttribute(root_group_id, kTagAttr, 
                         TypeToHDF5Type<char>::type(),
                         TAG_STR.length() + 1, TAG_STR.c_str());
}

//------------------------------------------------------------------------------

void
VoidFile::writeContainerTypeAttribute(HDF5Id root_group_id) const
{
    // check for the container data type
    HDF5DataType container_type_enum;
    if (!container_type_enum.open(m_id, kContainerEnumType)) {

        // data type doesn't exist so create it
        container_type_enum.create(H5T_ENUM, sizeof(ContainerType));

        // add the enum entries
        ContainerType type = TYPE_INVALID;
        container_type_enum.enumInsert("TYPE_INVALID", &type);
        type = TYPE_VOLUME;
        container_type_enum.enumInsert("TYPE_VOLUME", &type);
    }

    // write the container type attribute to the file
    writeScalarAttribute(root_group_id, kContainerAttr,
                         container_type_enum.id(), &m_type);
}

//------------------------------------------------------------------------------

void
VoidFile::writeVersionAttribute(HDF5Id root_group_id) const
{
    // check for the version data type
    HDF5DataType version_type;
    if (!version_type.open(m_id, kVersionType.c_str())) {

        // data type doesn't exist so create it
        version_type.create(H5T_COMPOUND, sizeof(Version));

        // add the struct member entries
        String str_major("major");
        version_type.compoundInsert(str_major, 
                                    HOFFSET(Version, major),
                                    H5T_NATIVE_UINT);
        String str_minor("minor");
        version_type.compoundInsert(str_minor,
                                    HOFFSET(Version, minor),
                                    H5T_NATIVE_UINT);
        String str_revision("revision");
        version_type.compoundInsert(str_revision,
                                    HOFFSET(Version, revision),
                                    H5T_NATIVE_UINT);
    }

    // write the version attribute
    writeScalarAttribute(root_group_id, kVersionAttr,
                         version_type.id(), &m_version);
}

//------------------------------------------------------------------------------

END_NKHIVE_NS
