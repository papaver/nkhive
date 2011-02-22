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
// VolumeFile.h
//------------------------------------------------------------------------------

#ifndef __NKHIVE_IO_VOLUMEFILE_H__
#define __NKHIVE_IO_VOLUMEFILE_H__

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------

#include <cassert>
#include <fstream>

#include <nkhive/Defs.h>
#include <nkhive/Types.h>

#include <nkbase/Exceptions.h>
#include <nkbase/String.h>

#include <nkhive/io/VoidFile.h>
#include <nkhive/volume/Volume.h>

//-----------------------------------------------------------------------------
// forward declarations
//-----------------------------------------------------------------------------

#ifdef UNITTEST
    template <typename T>
    class TestVolume;
#endif // UNITTEST

//-----------------------------------------------------------------------------
// interface definition
//-----------------------------------------------------------------------------

BEGIN_NKHIVE_NS

class VolumeFile : public VoidFile
{

public:

    //-------------------------------------------------------------------------
    // public interface
    //-------------------------------------------------------------------------
    
    /**
     * Constructors.
     */
    VolumeFile();
    VolumeFile(const String &file_path, OpenMode mode);
    ~VolumeFile();

    /**
     * Read volume from HDF5 file
     */
    template <typename V> typename V::shared_ptr read(String &name);
    template <typename V> typename V::shared_ptr read(u32 index = 0);

    /**
     * Read volume from stream.
     */
    template <typename V> typename V::shared_ptr read(std::istream &is);
    

    /**
     * Write volume to HDF5 file
     */
    template <typename V> void write(const V &volume);

    /**
     * Write volume to stream.
     */
    template <typename V> void write(std::ostream &os, const V &volume);

protected:

    //-------------------------------------------------------------------------
    // internal methods
    //-------------------------------------------------------------------------
    
    /**
     * Helper for reading a volume
     */
    template <typename V>
    typename V::shared_ptr readInternal(String &volume_name);
    template <typename V>
    typename V::shared_ptr readInternal(u32 index = 0);

    /**
     * Helper for writing out a volume
     */
    template <typename V>
    void writeInternal(const V &volume);

    //-------------------------------------------------------------------------
    // friends
    //-------------------------------------------------------------------------

#ifdef UNITTEST
    template <typename T>
    friend class ::TestVolume;
#endif // UNITTEST

private:

};

END_NKHIVE_NS

//------------------------------------------------------------------------------
// interface implementation
//------------------------------------------------------------------------------

BEGIN_NKHIVE_NS

#include <nkhive/io/VolumeFile.hpp>

END_NKHIVE_NS

//------------------------------------------------------------------------------

#endif // __NKHIVE_IO_VOLUMEFILE_H__
