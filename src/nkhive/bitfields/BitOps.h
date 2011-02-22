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
// BitOps.h
//  - This header contains useful functions to do some basic computations based 
//    on bits and numbers stored in Log2.
//------------------------------------------------------------------------------

#ifndef __NKHIVE_BITFIELDS_BITOPS_H__
#define __NKHIVE_BITFIELDS_BITOPS_H__

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------

#include <assert.h>

#include <nkhive/Defs.h>
#include <nkhive/Types.h>

//-----------------------------------------------------------------------------
// defines
//-----------------------------------------------------------------------------

#define BITS_PER_BYTE 8

/**
 * Returns number of bits in type.
 */
#define bitsof(t) (sizeof(t) << 3)

/**
 * Creates a bitmask for the requested type and index.
 */
#define getBitMask(t, b) ((t)1 << ((bitsof(t) - 1) - b))

/**
 * Creates a bitmask for the requested type from start to the requested bit.
 *  ex. u8,4 = 0x11110000
 */
#define getBitMaskRange(t, b) ((t)~((getBitMask(t, b) << 1) - 1))

/**
 * Returns a completely filled bit mask. ex. 0x11111111
 */
#define getBitMaskFilled(t) ((t)(~0))

//-----------------------------------------------------------------------------
// interface definition
//-----------------------------------------------------------------------------

BEGIN_NKHIVE_NS

/**
 * Returns the number of bits needed to represent the given size in Log2 in 3D.
 * Essentially the number of bits needed for a cube of size: (2^lg_size)^3
 */
index_type 
numBits3D(index_type lg_size);

/** 
 * Creates a linear index for a given 3D cube coordinate.
 */
index_type
getIndex(index_type i, index_type j, index_type k, 
         index_type w, index_type h, index_type d);

/** 
 * Creates a linear index for a given 3D cube coordinate of size lg_size,
 * which is Log2 the size of each i,j,k cube dimension. The real size of each
 * cube side is : 2^lg_size.
 */
index_type
getIndex(index_type i, index_type j, index_type k, index_type lg_size);

/** 
 * Computes the 3D cube coordinate of size lg_size (Log2 of cube dimension),
 * from a linear index.
 */
void
getCoordinates(index_type index, index_type lg_size, 
               index_type& i, index_type& j, index_type& k);

/** 
 * Gets number of set bits in value;
 */
template <typename T>
index_type
getBitCount(T v);

/** 
 * Gets the index of the first bit set. The returned index in 1 based. 1 is the
 * position of the least significant bit. Returns 0 is the value does not have
 * any bits set.
 */
template <typename T>
index_type
getFirstSetBitIndex(T v);


/**
 * Get the index of the last bit set. The returned index is 1 based. 1 is the
 * index of the least significant bit. Returns 0 if the value does not have any
 * bits set.
 */
template <typename T>
index_type
getLastSetBitIndex(T v);

/**
 * Returns 1 if the given numeric type is negative, false otherwise.
 */
template <typename T>
index_type
isNegative(T v);

/**
 * Given signed integers (i, j, k) determines which quadrant is represented in
 * euclidean space. The numeric value of the quadrant is determined by 3-bits
 * that are turned on if either i, j, and/or k are negative. The values
 * look something like this:
 *       i   j   k    bits    quadrant#
 *       1,  1,  1 -> 000 ->  0
 *       1,  1, -1 -> 001 ->  1
 *       1, -1,  1 -> 010 ->  2
 *      -1,  1,  1 -> 100 ->  4 
 *       1, -1, -1 -> 011 ->  3 
 *      -1,  1, -1 -> 101 ->  5 
 *      -1, -1,  1 -> 110 ->  6 
 *      -1, -1, -1 -> 111 ->  7 
 */
template <typename T>
uint8_t
getQuadrant(T i, T j, T k);

/**
 * Given the unsigned indices and the quadrant for those indices, convert the
 * given coordinates into signed values based on which quadrant they lie in. 
 */
template <typename T>
void
getQuadrantCoordinates(T &i, T &j, T &k, uint8_t quadrant);

/**
 * Retrieve the offset for the quadrant.  Since the origin can only live in 
 * one quadrant, the rest of the quadrants are offset accordingly.
 */
template <typename T>
void
getQuadrantOffsets(T &i, T &j, T &k, uint8_t quadrant);

/**
 * Return the number such that the given type's most significant bit is set. 
 */
template <typename T>
T
setMSB();

/**
 * Take the absolute value of the given signed integer without branching. T is
 * the signed integer type and U is the corresponding unsigned type.
 */
template <typename T, typename U>
U
abs(T v);

/** 
 * Checks weather a value is a power of two.  This is achieved by making sure
 * there is only one bit set in the value.
 */
bool
isPow2(index_type v);

/** 
 * Rounds the value up to the closest multiple of pow2.
 */
index_type
roundPow2(index_type v, index_type pow2);

/**
 * Returns (i % 2^lg_divisor)
 */
index_type
moduloLg(index_type i, index_type lg_divisor);

END_NKHIVE_NS

//------------------------------------------------------------------------------
// interface implementation
//------------------------------------------------------------------------------

BEGIN_NKHIVE_NS

#include <nkhive/bitfields/BitOps.hpp>

END_NKHIVE_NS

//------------------------------------------------------------------------------

#endif // __NKHIVE_BITFIELDS_BITOPS_H__
