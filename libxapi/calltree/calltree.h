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

#ifndef _XAPI_CALLTREE_H
#define _XAPI_CALLTREE_H

#if XAPI_STACKTRACE

struct memblk;      // memblk.h
struct calltree;

/// xapi_calltree_unwindto
//
// SUMMARY
//  unwind to the specified frame, discarding any error
//
// PARAMETERS
//  frame - frame to unwind to
//
void xapi_calltree_unwind(void);

/// xapi_calltree_freeze
//
// SUMMARY
//  freeze the calltree for the current thread
//
// RETURNS
//  containing memblk
//
struct memblk * xapi_calltree_freeze(void);

/// xapi_calltree_unfreeze
//
// SUMMARY
//  reverse the effects of xapi_calltree_freeze
//
// RETURNS
//  containing memblk
//
void xapi_calltree_unfreeze(void);

/// xapi_calltree_thaw
//
// SUMMARY
//  recover a calltree frozen with xapi_calltree_freeze
//
// RETURNS
//  calltree instance (pointer-equivalent with mb)
//
struct calltree * xapi_calltree_thaw(void * mb)
  __attribute__((nonnull));

/// xapi_calltree_export
//
// SUMMARY
//  export the calltree for the current thread while unwinding
//
// PARAMETERS
//  dst - buffer to copy the calltree to
//  sz  - size of dst
//
// RETURNS
//  pointer to dst, or 0 if dst is not large enough
//
struct calltree * xapi_calltree_export(void * dst, size_t sz)
  __attribute__((nonnull));

#endif
#endif
