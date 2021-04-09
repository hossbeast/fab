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

#ifndef _FABD_FSNAME_H
#define _FABD_FSNAME_H

#include "types.h"

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
typedef struct fsname
{
  char      name[255];
  uint16_t  namel;
  char *    ext;
  uint16_t  extl;
  char *    suffix;
  uint16_t  suffixl;
  char *    base;
  uint16_t  basel;
} fsname;

/// path_create
//
// SUMMARY
//  create path instance from a pre-normalized absolute or relative
//
// PARAMETERS
//  p  - (returns) path object
//
//xapi path_creates(path ** const restrict p, const char * const restrict s)
//  __attribute__((nonnull));

void fsname_init(fsname * const restrict p, const char * const restrict buf, size_t bufl)
  __attribute__((nonnull));

#endif
