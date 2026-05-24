/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

#define FTWAT_F       1  // regular file
#define FTWAT_LNK     2  // symlink
#define FTWAT_D       3  // a directory
#define FTWAT_PRE     4  // breadth-first
#define FTWAT_POST    5  // depth-first

#define FTWAT_SYMLINKS  1 // visit symlinks
#define FTWAT_FOLLOW    2 // follow symlinks

#define FTWAT_OPENDIR   1

typedef struct ftwinfo
{
  struct ftwinfo *parent;

  char *path;
  uint16_t pathl;
  uint16_t name_off;
  uint8_t type;       // one of FTWAT_F, FTWAT_D
  void *udata;
} ftwinfo;

/// nftwat - file tree walk with AT dirfd
//
// SUMMARY
//  traverse a directory tree like nftw with FTW_DEPTH (children of a directory are visited before
//  the directory itself, e.g. post-order)
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
void nftwat(
    int dirfd
  , const char * dirpath
  , void (*fn)(int method, ftwinfo * restrict info, void * udata, int * stop)
  , void (*err)(int method, ftwinfo * restrict info, void * udata, int error)
  , int nopenfd
  , uint32_t flags
  , void * udata
)
  __attribute__((nonnull(2, 3)));
