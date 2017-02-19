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

#ifndef _FABD_PATH_H
#define _FABD_PATH_H

#include <stdint.h>

#define restrict __restrict

/*

/foo/bar/baz.qux.tez
^------------------^  path
^------^              dir
         ^---------^  name
             ^-----^  ext
                 ^-^  suffix
         ^-^          base
^----------^          stem
*/

// immutable
typedef struct path
{
  char *  dir;
  size_t  dirl;
  char *  name;
  size_t  namel;
  char *  ext;
  size_t  extl;
  char *  suffix;
  size_t  suffixl;
  size_t  pathl;
  char    path[];
} path;

/// path_create
//
// SUMMARY
//  create path instance from a relative path representation
//
// PARAMETERS
//  p        - result path object
//  in_base  - input path is relative to this base
//  cwd      - canonicalized cwd
//  fmt      - input path
//
xapi path_createf(path ** const restrict p, const char * const restrict fmt, ...)
  __attribute__((nonnull));

/// path_free
//
// SUMMARY
//  free a path with free semantics
//
void path_free(path * const restrict);

/// path_xfree
//
// SUMMARY
//  free a path with wfree semantics
//
void path_xfree(path ** const restrict)
  __attribute__((nonnull));

#undef restrict
#endif
