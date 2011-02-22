//------------------------------------------------------------------------------
//
// Copyright (c) 2011, NektarFX, Inc. (http://www.nektarfx.com)
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without modific-
//  ation, are permitted provided that the following conditions are met:
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
// Defs.h
//------------------------------------------------------------------------------

#ifndef __NKHIVE_DEFS_H__
#define __NKHIVE_DEFS_H__

#include <nkbase/Defs.h>

//------------------------------------------------------------------------------
// defines
//------------------------------------------------------------------------------

#define NKHIVE_NS       NK_NS::hive
#define USING_NKHIVE_NS using namespace NKHIVE_NS;
#define BEGIN_NKHIVE_NS namespace NK_NS { namespace hive {
#define END_NKHIVE_NS   } }

//------------------------------------------------------------------------------

#define NUM_QUADRANTS 8

#define VOXEL_NEIGHBORS 8

//------------------------------------------------------------------------------
// offsetof 
//  - macro to get offset of structs member variable
//  - usually addr 0 is used but this will cause issues with user defined ctors
//------------------------------------------------------------------------------

#ifdef offsetof
  #define offsetof_builtin offsetof
  #undef offsetof
#endif

#define offsetof(c, m) ((size_t)&(((c*)1)->m)-1)

//------------------------------------------------------------------------------

#endif // __NKHIVE_DEFS_H__
