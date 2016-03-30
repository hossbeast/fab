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

#ifndef _NARRATE_FILE_H
#define _NARRATE_FILE_H

/*

MODULE
 narrator/file

SUMMARY
 narrator that passes all writes to a file descriptor

*/

#define restrict __restrict

/// narrator_file_create
//
// SUMMARY
//  allocate a file narrator
//
// PARAMETERS
//  n  - (returns) narrator
//  fd - file descriptor
//
xapi narrator_file_create(narrator ** const restrict n, int fd)
  __attribute__((nonnull));

#undef restrict
#endif
