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

#ifndef _NARRATOR_FD_H
#define _NARRATOR_FD_H

/*

MODULE
 narrator/fd

SUMMARY
 narrator that passes all writes to a file descriptor

*/

#include "types.h"

#include "narrator.h"

typedef struct narrator_fd
{
  narrator base;
  int fd;
} narrator_fd;

extern struct narrator_vtable narrator_fd_vtable;

/// narrator_fd_create
//
// SUMMARY
//  allocate a fd narrator
//
// PARAMETERS
//  n  - (returns) narrator
//  fd - file descriptor
//
void narrator_fd_create(narrator_fd ** const restrict n, int fd)
  __attribute__((nonnull));

void narrator_fd_free(narrator_fd * const restrict n);

#endif
