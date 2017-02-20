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

struct value;

// filesystme attributes
#define FILESYSTEM_TABLE(x, y)                                                                  \
  FILESYSTEM(INVALIDATE_STAT    , 1 , "stat"    , x , y)  /* stat hash (default) */             \
  FILESYSTEM(INVALIDATE_CONTENT , 2 , "content" , x , y)  /* content hash */                    \
  FILESYSTEM(INVALIDATE_NOTIFY  , 3 , "notify"  , x , y)  /* filesystem event subscription */   \
  FILESYSTEM(INVALIDATE_ALWAYS  , 4 , "always"  , x , y)  /* always considered invalid */       \
  FILESYSTEM(INVALIDATE_NEVER   , 5 , "never"   , x , y)  /* never considered invalid */

#define FILESYSTEM_INVALIDATE_MIN 1
#define FILESYSTEM_INVALIDATE_MAX 5

enum {
#define FILESYSTEM(a, b, c, x, y) FILESYSTEM_ ## a = UINT32_C(b),
FILESYSTEM_TABLE(0, 0)
#undef FILESYSTEM
};

typedef struct filesystem
{
  char *          path;   // canonical path to the root of the filesystem
  uint32_t        attrs;  // attributes
} filesystem;

#define restrict __restrict

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
//  config - root of the config tree
//  dry    - whether to perform a dry-run
//
xapi filesystem_reconfigure(const struct value * restrict config, uint32_t dry)
  __attribute__((nonnull));

/// filesystem_lookup
//
// SUMMARY
//  get the filesystem configuration for the specified path
//
// PARAMETERS
//  path - canonical path
//  fs   - (returns) filesystem
//
xapi filesystem_lookup(char * const restrict path, filesystem ** const restrict fs)
  __attribute__((nonnull));

#undef restrict
#endif
