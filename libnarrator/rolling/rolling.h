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

#include "xapi.h"
#include "types.h"

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
xapi narrator_rolling_create(
    narrator ** const restrict n
  , const char * const restrict path_base
  , mode_t mode
  , uint32_t threshold
  , uint16_t max_files
)
  __attribute__((nonnull));

#endif
