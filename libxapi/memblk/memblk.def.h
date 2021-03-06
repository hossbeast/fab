/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

   This file is part of fab.

   fab is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   fab is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with fab.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef _MEMBLK_DEF_H
#define _MEMBLK_DEF_H

/*

MODULE
 memblk.def

SUMMARY
 memblk definition and functions for working with memblks at a low level
 no dependency on libxapi

*/

struct memblk;
struct mempolicy;

typedef struct memblk {
  struct memblk_policy
  {
    struct mempolicy * mp;
    struct memblk * mb;
  } policy;

  int mapped;
  int prot;
  int flags;

  struct {
    char *  s;
    size_t  l;
    size_t  a;
    size_t  o;      // cumulative offset
  } *     blocks;
  size_t  blocksl;
  size_t  blocksa;
} memblk;

/// memblk_freeze
//
// SUMMARY
//  convert a pointer to memblk-allocd memory to an offset from the start of the memblk
//
// PARAMETERS
//  mb - memblk instance
//  p  - pointer to pointer to memory allocated with mb (T**)
//
void memblk_freeze(memblk * const restrict mb, void * restrict p)
  __attribute__((nonnull));

/// memblk_unfreeze
//
// SUMMARY
//  reverse the effects of memblk_freeze
//
// PARAMETERS
//  mb - memblk instance
//  p  - pointer to pointer to memory allocated with mb (T**)
//
void memblk_unfreeze(memblk * const restrict mb, void * restrict p)
  __attribute__((nonnull));

/// memblk_thaw
//
// SUMMARY
//  recover objects from a consolidated memblk
//
// PARAMETERS
//  mb - pointer to memory consolidated from an memblk (memblk_writeto, memblk_copyto)
//  p  - pointer to pointer to memory allocated with mb (T**)
//
void memblk_thaw(void * restrict mb, void * restrict p)
  __attribute__((nonnull));

/// memblk_copyto
//
// SUMMARY
//  consolidate the memblk
//
void memblk_copyto(memblk * const restrict mb, void * restrict dst, size_t sz)
  __attribute__((nonnull));

/// memblk_size
//
// SUMMARY
//  get the size of the memblk
//
size_t memblk_size(memblk * const restrict mb)
  __attribute__((nonnull));

#endif
