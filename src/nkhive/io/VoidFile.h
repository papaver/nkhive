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
// VoidFile.h
//------------------------------------------------------------------------------

#ifndef __NKHIVE_IO_VOIDFILE_H__
#define __NKHIVE_IO_VOIDFILE_H__

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------

#include <cassert>
#include <fstream>

#include <nkhive/io/hdf5/HDF5Base.h>
#include <nkhive/Defs.h>
#include <nkhive/Types.h>
#include <nkbase/Exceptions.h>
#include <nkbase/String.h>

//-----------------------------------------------------------------------------
// forward declarations
//-----------------------------------------------------------------------------

#ifdef UNITTEST
    template <typename T>
    class TestVolume;

    template <typename T>
    class TestBitField3D;

    template <typename T>
    class TestCell;

    template <typename T>
    class TestPrimitiveAttributes;
    
    class TestNode;
    class TestTree;
    class TestAttributeCollection;
    class TestStringAttribute;
    class TestLocalXform;
#endif // UNITTEST

//-----------------------------------------------------------------------------
// interface definition
//-----------------------------------------------------------------------------

BEGIN_NKHIVE_NS

class VoidFile
{

public:

    //-------------------------------------------------------------------------
    // enums
    //-------------------------------------------------------------------------
    
    enum ContainerType
    {
        TYPE_INVALID = 0x0,
        TYPE_VOLUME  = 0x1,
    };

    enum OpenMode
    {
        READ_ONLY   = 0x0,
        READ_WRITE  = 0x1,
        WRITE_TRUNC = 0x2,
    };

    //-------------------------------------------------------------------------
    // structs
    //-------------------------------------------------------------------------
    
    struct Version 
    {
        u32 major;
        u32 minor;
        u32 revision;
    };
    
    //-------------------------------------------------------------------------
    // public interface
    //-------------------------------------------------------------------------
    
    /**
     * Constructors.
     */
    VoidFile(ContainerType type);
    VoidFile(const String &file_path, ContainerType type, OpenMode mode);
    ~VoidFile();

    /**
     * Container type accessors.
     */
    ContainerType getType();

    /**
     * Version accessors.
     */
    Version& getVersion();
    void setVersion(u32 major, u32 minor, u32 revision);

    /**
     * I/O operations.
     */
    void open(const String &file_path, OpenMode mode);
    void close();

protected:

    //-------------------------------------------------------------------------
    // internal methods
    //-------------------------------------------------------------------------
    
    void read(std::istream &is) throw (Iex::InputExc);
    void write(std::ostream &os) const;

    void read(); 
    void write() const;

    void writeTagAttribute(hid_t root_group_id) const;
    void writeContainerTypeAttribute(hid_t root_group_id) const;
    void writeVersionAttribute(hid_t root_group_id) const;

    void setType(ContainerType type) throw (Iex::TypeExc);

    //-------------------------------------------------------------------------
    // constants
    //-------------------------------------------------------------------------
    
    static const String TAG_STR;  

    /*
     * attribute names for headers
     */
    static const String kTagAttr;
    static const String kContainerAttr;
    static const String kVersionAttr;

    /*
     * attribute type names for attribute data types
     */
    static const String kContainerEnumType;
    static const String kVersionType;

    //-------------------------------------------------------------------------
    // friends
    //-------------------------------------------------------------------------

#ifdef UNITTEST
    template <typename T>
    friend class ::TestVolume;

    template <typename T>
    friend class ::TestBitField3D;

    template <typename T>
    friend class ::TestCell;
    
    template <typename T>
    friend class ::TestPrimitiveAttributes;
    
    friend class ::TestNode;
    friend class ::TestTree;
    friend class ::TestAttributeCollection;
    friend class ::TestStringAttribute;
    friend class ::TestLocalXform;
#endif // UNITTEST

    //-------------------------------------------------------------------------
    // members
    //-------------------------------------------------------------------------
    
    /**
     * Enum of the enclosed container.
     */
    ContainerType m_type;

    /**
     * File format version of the enclosed container.  
     */
    Version m_version;

    /**
     * HDF5 file id
     */
    HDF5Id m_id;

};

END_NKHIVE_NS

//------------------------------------------------------------------------------

#endif // __NKHIVE_IO_VOIDFILE_H__
