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
// BitOps.cpp
//------------------------------------------------------------------------------

#include <nkhive/bitfields/BitOps.h>

BEGIN_NKHIVE_NS

//------------------------------------------------------------------------------

/**
 * NOTE: The following will work for numbers up to 128 bits
 */
template <>
index_type
getBitCount(uint64_t v) 
{
    typedef uint64_t T;
    T c;
    v = v - ((v >> 1) & (T)~(T)0/3);                                // temp
    v = (v & (T)~(T)0/15*3) + ((v >> 2) & (T)~(T)0/15*3);           // temp
    v = (v + (v >> 4)) & (T)~(T)0/255*15;                           // temp
    c = (T)(v * ((T)~(T)0/255)) >> (sizeof(v) - 1) * BITS_PER_BYTE; // count
    return c;
}

//------------------------------------------------------------------------------

/**
 * NOTE: The following ONLY works for numbers up to 32-bits.
 */
template <typename T>
inline index_type
getBitCount(T v) 
{
    index_type c;
    v = v - ((v >> 1) & 0x55555555); // reuse input as temporary
    v = (v & 0x33333333) + ((v >> 2) & 0x33333333);     // temp
    c = (((v + (v >> 4)) & 0xF0F0F0F) * 0x1010101) >> 24; // count
    return c;
}

//------------------------------------------------------------------------------

template <>
uint8_t 
setMSB() 
{
    return 0x80;
}

//------------------------------------------------------------------------------

template <>
uint16_t 
setMSB() 
{
    return 0x8000;
}

//------------------------------------------------------------------------------

template <>
uint32_t 
setMSB() 
{
    return 0x80000000;
}

//------------------------------------------------------------------------------

template <>
uint64_t 
setMSB() 
{
    return 0x8000000000000000;
}

//------------------------------------------------------------------------------

/**
 * Note: This solution will only work for unsigned values of T.
 */
template <typename T>
inline index_type
getFirstSetBitIndex(T v)
{
    index_type i = 0; 
    if (v) {
        for (i = 1; ~v & 1; v = v >> 1, ++i) {}
    }
    return i;
}

//------------------------------------------------------------------------------

/**
 * Note: This solution will only work for unsigned values of T.
 */
template <typename T>
inline index_type
getLastSetBitIndex(T v)
{
    index_type i = 0; 
    T msb = setMSB<T>();
    if (v) {
        for (i = bitsof(T); ~v & msb; v = v << 1, --i) {}
    }
    return i;
}

//------------------------------------------------------------------------------

template <>
index_type
isNegative(int32_t v)
{
    return ((uint32_t)v) >> 31;
}

//------------------------------------------------------------------------------

template <>
index_type
isNegative(int64_t v)
{
    return ((uint64_t)v) >> 63;
}

//------------------------------------------------------------------------------

template <>
uint8_t
getQuadrant(int32_t i, int32_t j, int32_t k)
{
    // We need to shift all 31 bits to clear any lower-order bits that remain
    // from 2s-complement representation of a negative number.
    uint8_t quadrant = (((uint32_t)k) >> 31) | 
                       ((((uint32_t)j) >> 31) << 1) | 
                       ((((uint32_t)i) >> 31) << 2);
    return quadrant;
}

//------------------------------------------------------------------------------

template <>
uint8_t
getQuadrant(int64_t i, int64_t j, int64_t k)
{
    uint8_t quadrant = (((uint64_t)k) >> 63) | 
                       ((((uint64_t)j) >> 63) << 1) | 
                       ((((uint64_t)i) >> 63) << 2);
    return quadrant;
}

//------------------------------------------------------------------------------

template <typename T>
void
getQuadrantOffsets(T &i, T &j, T &k, uint8_t quadrant)
{
    i = !!T(quadrant & 0x04);
    j = !!T(quadrant & 0x02);
    k = !!T(quadrant & 0x01);
}

//------------------------------------------------------------------------------

template <typename T>
inline void
getQuadrantCoordinates(T &i, T &j, T &k, uint8_t quadrant) 
{
    i *= (quadrant & 0x04) ? -1 : 1;
    j *= (quadrant & 0x02) ? -1 : 1;
    k *= (quadrant & 0x01) ? -1 : 1;
}

//------------------------------------------------------------------------------

template <typename T, typename U>
U
abs(T v)
{
    const T mask = v >> (bitsof(T) - 1);
    U ret = (v ^ mask) - mask;
    return ret;
}

//------------------------------------------------------------------------------

template <>
index_vec
abs(signed_index_vec sv)
{
    index_vec v;
    v.x  = abs<signed_index_type, index_type>(sv.x);
    v.y  = abs<signed_index_type, index_type>(sv.y);
    v.z  = abs<signed_index_type, index_type>(sv.z);
    return v;
}

//------------------------------------------------------------------------------
// Template instantiations
//------------------------------------------------------------------------------

template index_type getBitCount<uint8_t>(uint8_t v);
template index_type getBitCount<uint16_t>(uint16_t v);
template index_type getBitCount<uint32_t>(uint32_t v);

template index_type getFirstSetBitIndex<uint8_t>(uint8_t v);
template index_type getFirstSetBitIndex<uint16_t>(uint16_t v);
template index_type getFirstSetBitIndex<uint32_t>(uint32_t v);
template index_type getFirstSetBitIndex<uint64_t>(uint64_t v);

template index_type getLastSetBitIndex<uint8_t>(uint8_t v);
template index_type getLastSetBitIndex<uint16_t>(uint16_t v);
template index_type getLastSetBitIndex<uint32_t>(uint32_t v);
template index_type getLastSetBitIndex<uint64_t>(uint64_t v);

template void getQuadrantOffsets<int32_t>(
        int32_t &i, int32_t &j, int32_t &k, uint8_t q);
template void getQuadrantOffsets<int64_t>(
        int64_t &i, int64_t &j, int64_t &k, uint8_t q);
template void getQuadrantOffsets<uint32_t>(
        uint32_t &i, uint32_t &j, uint32_t &k, uint8_t q);
template void getQuadrantOffsets<uint64_t>(
        uint64_t &i, uint64_t &j, uint64_t &k, uint8_t q);

template void getQuadrantCoordinates<int8_t>(
        int8_t &i, int8_t &j, int8_t &k, uint8_t q);
template void getQuadrantCoordinates<int16_t>(
        int16_t &i, int16_t &j, int16_t &k, uint8_t q);
template void getQuadrantCoordinates<int32_t>(
        int32_t &i, int32_t &j, int32_t &k, uint8_t q);
template void getQuadrantCoordinates<int64_t>(
        int64_t &i, int64_t &j, int64_t &k, uint8_t q);

template uint8_t abs<int8_t, uint8_t>(int8_t v);
template uint16_t abs<int16_t, uint16_t>(int16_t v);
template uint32_t abs<int32_t, uint32_t>(int32_t v);
template uint64_t abs<int64_t, uint64_t>(int64_t v);

END_NKHIVE_NS
