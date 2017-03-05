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

#ifndef _LORIEN_NFTWAT_H
#define _LORIEN_NFTWAT_H

#include <dirent.h>
#include <stdint.h>

#include "xapi.h"

#define restrict __restrict

#define FTWAT_F       1  // regular file
#define FTWAT_D       2  // a directory
#define FTWAT_PRE     3  // breadth-first
#define FTWAT_POST    4  // depth-first

typedef struct ftwinfo
{
  struct ftwinfo * parent;

  const char * path;
  uint16_t pathl;
  uint16_t name_off;
  uint8_t type;       // one of FTWAT_F, FTWAT_D
  void * udata;
} ftwinfo;

/// nftwat - file tree walk with AT dirfd
//
// SUMMARY
//  traverse a directory tree like nftw with FTW_DEPTH (children of a directory are visited before
//  the directory itself)
//
// PARAMETERS
//  dirfd   - file descriptor opened with O_PATH, man 2 openat
//  dirpath - path to the root of the tree to traverse
//  fn      - callback function for each directory entry
//  nopenfd - maximum number of file descriptors in use at once
//  udata   - opaque user data
//
// CALLBACK PARAMETERS
//  method - for FTWAT_D, one of FTWAT_PRE, FTWAT_POST, zero otherwise
//  info
//   parent   - info for the parent directory of this entry
//   path     - path to the entry, relative to (and including) dirpath
//   pathl    - size of path
//   name_off - offset from path to the name, that is, following the final directory separator
//   type     - filesystem type for the entry
//   udata    - opaque user data
//  udata  - opaque user data
//  stop   - (returns) for FTWAT_PRE, whether to not traverse this directory
//
// SEE ALSO
//  man 2 ftw
//  man 2 openat
//
xapi nftwat(
    int dirfd
  , const char * dirpath
  , xapi (*fn)(int method, ftwinfo * restrict info, void * udata, int * stop)
  , int nopenfd
  , void * udata
)
  __attribute__((nonnull(2, 3)));

#undef restrict
#endif
