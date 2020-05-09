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

#include "types.h"
#include "xapi.h"

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
  char *    dir;
  uint16_t  dirl;
  char *    name;
  uint16_t  namel;
  char *    ext;
  uint16_t  extl;
  char *    suffix;
  uint16_t  suffixl;
  char *    base;
  uint16_t  basel;
  char *    stem;
  uint16_t  steml;

  uint16_t  pathl;
  char      path[];
} path;

/// path_create
//
// SUMMARY
//  create path instance from a pre-normalized absolute or relative
//
// PARAMETERS
//  p  - (returns) path object
//
xapi path_creates(path ** const restrict p, const char * const restrict s)
  __attribute__((nonnull));

xapi path_createw(path ** const restrict p, const char * const restrict buf, size_t bufl)
  __attribute__((nonnull));

xapi path_createf(path ** const restrict p, const char * const restrict fmt, ...)
  __attribute__((nonnull))
  __attribute__((format(printf, 2, 3)));

xapi path_createvf(path ** const restrict p, const char * const restrict fmt, va_list va)
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

#endif
