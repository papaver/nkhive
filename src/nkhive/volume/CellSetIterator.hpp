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
// CellSetIterator.hpp
//------------------------------------------------------------------------------

// no includes allowed

//------------------------------------------------------------------------------

template <typename CellType>
inline 
CellSetIterator<CellType>::CellSetIterator(const CellType *const cell) :
    m_cell(cell)
{
    // init the set_iterator with a const iterator for the cell..
    m_set_iterator = m_cell->m_bitfield.setIterator(m_cell->begin());
}

//------------------------------------------------------------------------------

template <typename CellType>
inline 
CellSetIterator<CellType>::~CellSetIterator()
{
}

//------------------------------------------------------------------------------

template <typename CellType>
inline bool
CellSetIterator<CellType>::atEnd() const
{
    return !m_set_iterator();
}

//------------------------------------------------------------------------------

template <typename CellType>
inline typename CellSetIterator<CellType>::const_reference
CellSetIterator<CellType>::value() const
{
    return *m_set_iterator;
}

//------------------------------------------------------------------------------

template <typename CellType>
inline void
CellSetIterator<CellType>::next()
{
    ++m_set_iterator;
}

//------------------------------------------------------------------------------

template <typename CellType>
inline void
CellSetIterator<CellType>::getCoordinates(index_type &i, 
                                          index_type &j, 
                                          index_type &k) const
{
    m_set_iterator.getCoordinates(i, j, k);
}

//------------------------------------------------------------------------------

template <typename CellType>
inline bool 
CellSetIterator<CellType>::isValueIterator() const
{
    return true;
}

//------------------------------------------------------------------------------
