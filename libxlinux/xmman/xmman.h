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

#ifndef _XMMAN_H
#define _XMMAN_H

#include <sys/mman.h>

#include "xapi.h"

#define restrict __restrict

/// xmmap
//
// SUMMARY
//  proxy for mmap
//
xapi xmmap(void * restrict addr, size_t length, int prot, int flags, int fd, off_t offset, void ** restrict r);

/// xmunmap
//
// SUMMARY
//  proxy for munmap
//
xapi xmunmap(void * addr, size_t length);

/// ixmunmap
//
// SUMMARY
//  idempotent proxy for munmap
//
// PARAMETERS
//  [addr] - if not zero, munmap *addr
//  length - size of mapping
//
xapi ixmunmap(void * addr, size_t length);

#undef restrict
#endif
