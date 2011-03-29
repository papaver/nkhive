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
// VolumeFile.hpp
//------------------------------------------------------------------------------

// no includes allowed

//------------------------------------------------------------------------------
// interface implementation
//------------------------------------------------------------------------------

inline
VolumeFile::VolumeFile() : 
    VoidFile(TYPE_VOLUME)
{
}

//------------------------------------------------------------------------------

inline
VolumeFile::VolumeFile(const String &file_path, OpenMode mode) :
    VoidFile(file_path, TYPE_VOLUME, mode)
{
}

//------------------------------------------------------------------------------

inline
VolumeFile::~VolumeFile()
{
    close();
}

//------------------------------------------------------------------------------

template <typename V>
inline typename V::shared_ptr
VolumeFile::read(String &volume_name)
{
    return readInternal<V>(volume_name); 
}

//------------------------------------------------------------------------------

template <typename V>
inline typename V::shared_ptr
VolumeFile::read(std::istream &is) 
{
    // read void header  
    VoidFile::read(is);

    // read in type name
    String type_name;
    type_name.read(is);

    // make sure type is valid 
    if (type_name.compare(typeid(typename V::value_type).name()) != 0) {
        THROW(Iex::InputExc, "Invalid volume type.");
    }

    // read in the volume
    typename V::shared_ptr volume(new V());
    volume->read(is);

    return volume;
}

//------------------------------------------------------------------------------

template <typename V>
inline void 
VolumeFile::write(std::ostream &os, const V &volume)
{
    // write out void header  
    VoidFile::write(os);

    // write out type id
    String type_name(typeid(typename V::value_type).name());
    type_name.write(os);

    // write out the volume
    volume.write(os);
}

//------------------------------------------------------------------------------

template <typename V>
inline typename V::shared_ptr
VolumeFile::read(u32 index)
{
    return readInternal<V>(index); 
}

//------------------------------------------------------------------------------

inline i32
VolumeFile::numVolumes() const
{
    HDF5Group volume_root;
    HDF5Group::getRootGroup(m_id, kVolumeRootGroup, volume_root);

    return volume_root.numChildren();
}

//------------------------------------------------------------------------------

template <typename V>
inline void 
VolumeFile::write(const V& volume)
{
    writeInternal(volume); 
}

//------------------------------------------------------------------------------

template <typename V>
inline typename V::shared_ptr
VolumeFile::readInternal(String &volume_name) 
{
    // read void header
    VoidFile::read();

    // read in the volume
    typename V::shared_ptr volume(new V());
    volume->read(m_id, volume_name);

    return volume;
}

//------------------------------------------------------------------------------

template <typename V>
inline typename V::shared_ptr
VolumeFile::readInternal(u32 index) 
{
    // read void header
    VoidFile::read();

    // read in the volume
    typename V::shared_ptr volume(new V());
    volume->read(m_id, index);

    return volume;
}

//------------------------------------------------------------------------------

template <typename V>
inline void 
VolumeFile::writeInternal(const V& volume)
{
    // write out void header  
    VoidFile::write();

    // write out the volume
    volume.write(m_id);
}

//------------------------------------------------------------------------------

