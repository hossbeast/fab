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

#ifndef _NARRATOR_ROLLING_H
#define _NARRATOR_ROLLING_H

/*

MODULE
 narrator/rolling

SUMMARY
 narrator that writes to a rolling set of files

REMARKS
 unlinks existing files before writing to them

*/

#include "types.h"
#include "narrator.h"

typedef struct narrator_rolling {
  narrator base;

  int fd;           // file descriptor
  size_t written;
  uint16_t counter;
  int scanned;

  uint16_t max_files;
  uint32_t threshold;
  mode_t mode;
  char * directory;
  char * name_base;
} narrator_rolling;

extern struct narrator_vtable narrator_rolling_vtable;

/// narrator_rolling_create
//
// SUMMARY
//  allocate a rolling narrator
//
// PARAMETERS
//  n           - (returns) narrator
//  path_base   - base path for files
//  mode        - file mode creation bits (from fcntl.h)
//  threshold   - rollover after a file exceeds this size
//  max_files   - reset the file counter to zero after this many files
//
void narrator_rolling_create(
    narrator_rolling ** const restrict n
  , const char * const restrict path_base
  , mode_t mode
  , uint32_t threshold
  , uint16_t max_files
)
  __attribute__((nonnull));

void narrator_rolling_free(narrator_rolling *n);
void narrator_rolling_destroy(narrator_rolling *n)
  __attribute__((nonnull));

#endif
