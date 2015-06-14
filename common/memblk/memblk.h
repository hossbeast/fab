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

#ifndef _MEMBLK_H
#define _MEMBLK_H

#include <sys/types.h>

struct mempolicy;		// xlinux/mempolicy.h

struct memblk;
typedef struct memblk memblk;

#define restrict __restrict

/// memblk_mk
//
// SUMMARY
//  create an memblk
//
int memblk_mk(memblk ** mb)
	__attribute__((nonnull));

/// memblk_mk_mapped
//
// SUMMARY
//  create an memblk whose allocations are backed by mmapped pages
//
int memblk_mk_mapped(memblk ** mb, int prot, int flags)
	__attribute__((nonnull));

/// memblk_alloc
//
// SUMMARY
//  request an allocation
//
int memblk_alloc(memblk * restrict mb, void * restrict p, size_t sz)
	__attribute__((nonnull));

/// memblk_realloc
//
// SUMMARY
//  request a reallocation
//
int memblk_realloc(memblk * restrict mb, void * restrict p, size_t es, size_t ec, size_t oec)
	__attribute__((nonnull));

/// memblk_free
//
// SUMMARY
//  free an memblk with free semantics
//
void memblk_free(memblk * mb);

/// memblk_xfree
//
// SUMMARY
//  free an memblk with xfree semantics
//
void memblk_xfree(memblk ** mb)
	__attribute__((nonnull));

/// mempolicy 
//
// SUMMARY
//  get the mempolicy associated with this memblk
//
struct mempolicy * memblk_getpolicy(memblk * mb);

/// memblk_size
//
// SUMMARY
//  get the size of the memblk
//
size_t memblk_size(memblk * const restrict mb)
	__attribute__((nonnull));

/// memblk_writeto
//
// SUMMARY
//  consolidate the memblk and write it to a file descriptor with a single writev
//
// ERRORS
//  as for writev, plus LINUX_LESS : actual bytes != expected bytes
//
int memblk_writeto(memblk * const restrict mb, const int fd)
	__attribute__((nonnull));

/// memblk_bwriteto
//
// SUMMARY
//  consolidate the memblk and write it to a file descriptor with a single writev
//
// ERRORS
//  as for writev, plus LINUX_LESS : actual bytes != expected bytes
//
// REMARKS
//  this is a non-xapi function
//
int memblk_bwriteto(memblk * const restrict mb, const int fd)
	__attribute__((nonnull));

/// memblk_copyto
//
// SUMMARY
//  consolidate the memblk
//
void memblk_copyto(memblk * const restrict mb, char * const restrict dst, size_t sz)
	__attribute__((nonnull));

#undef restrict
#endif
