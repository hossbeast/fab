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

#ifndef _FILESYSTEM_H
#define _FILESYSTEM_H

#include <stdint.h>

#include "xapi.h"
#include "types.h"

struct value;
struct reconfigure_context;

// filesystem attributes
#define FILESYSTEM_TABLE                                                                         \
  FILESYSTEM(FILESYSTEM_INVALIDATE_STAT    , 1 , "stat")    /* stat hash (default) */            \
  FILESYSTEM(FILESYSTEM_INVALIDATE_CONTENT , 2 , "content") /* content hash */                   \
  FILESYSTEM(FILESYSTEM_INVALIDATE_NOTIFY  , 3 , "notify")  /* filesystem event subscription */  \
  FILESYSTEM(FILESYSTEM_INVALIDATE_ALWAYS  , 4 , "always")  /* always considered invalid */      \
  FILESYSTEM(FILESYSTEM_INVALIDATE_NEVER   , 5 , "never")   /* never considered invalid */

enum {
#define FILESYSTEM(a, b, c) a = UINT32_C(b),
FILESYSTEM_TABLE
#undef FILESYSTEM
};

typedef struct filesystem
{
  char *          path;   // path to the root of the filesystem, normalized and relative to the project dir
  uint32_t        attrs;  // attributes
  int             leaf;   // whether there do not exist more specific filesystems under this path
} filesystem;

/// filesystem_setup
//
// SUMMARY
//  setup
//
xapi filesystem_setup(void);

/// filesystem_cleanup
//
// SUMMARY
//  teardown
//
xapi filesystem_cleanup(void);

/// filesystem_reconfigure
//
// SUMMARY
//  rebuild the filesystem lookup structure from config
//
// PARAMETERS
/// ctx    - reconfiguration context
//  config - root of the config tree
//  dry    - whether to perform a dry-run
//
xapi filesystem_reconfigure(struct reconfigure_context * restrict ctx, const struct value * restrict config, uint32_t dry)
  __attribute__((nonnull));

/// filesystem_lookup
//
// SUMMARY
//  get the filesystem configuration for the specified path
//
// PARAMETERS
//  path - normalized absolute path
//  fs   - (returns) filesystem
//
filesystem * filesystem_lookup(const char * const restrict path, size_t pathl)
  __attribute__((nonnull));

#endif
