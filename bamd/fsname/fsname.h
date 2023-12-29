/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

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
//void path_creates(path ** const restrict p, const char * const restrict s)
//  __attribute__((nonnull));

void fsname_init(fsname * const restrict p, const char * const restrict buf, size_t bufl)
  __attribute__((nonnull));
