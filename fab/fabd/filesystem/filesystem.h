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

#ifndef FABD_FILESYSTEM_H
#define FABD_FILESYSTEM_H

#include <stdint.h>

#include "xapi.h"
#include "types.h"

#include "valyria/rbtree.h"

struct config;
struct attrs16;
struct fstree;

// filesystem attributes
#define INVALIDATE_OPT 0xf
#define INVALIDATE_TYPE_TABLE                                                                              \
  INVALIDATE_DEF(INVALIDATE_STAT    , "stat"    , INVALIDATE_OPT, 0x1) /* stat hash (default) */           \
  INVALIDATE_DEF(INVALIDATE_CONTENT , "content" , INVALIDATE_OPT, 0x2) /* content hash */                  \
  INVALIDATE_DEF(INVALIDATE_NOTIFY  , "notify"  , INVALIDATE_OPT, 0x3) /* filesystem event subscription */ \
  INVALIDATE_DEF(INVALIDATE_ALWAYS  , "always"  , INVALIDATE_OPT, 0x4) /* always considered invalid */     \
  INVALIDATE_DEF(INVALIDATE_NEVER   , "never"   , INVALIDATE_OPT, 0x5) /* never considered invalid */      \

typedef enum invalidate_type {
#define INVALIDATE_DEF(x, n, r, y) x = UINT16_C(y),
INVALIDATE_TYPE_TABLE
#undef INVALIDATE_DEF
} invalidate_type;

struct attrs16 * invalidate_attrs;

typedef struct filesystem {
  union {
    invalidate_type invalidate;
    uint16_t attrs;
  };

  struct fstree *fst; // filesystem attachment point

  llist lln;  // freelist
} filesystem;

extern struct filesystem filesystem_root;
extern struct filesystem filesystem_shadow;

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
xapi filesystem_reconfigure(struct config * restrict cfg, bool dry)
  __attribute__((nonnull));

size_t filesystem_get_absolute_path(const filesystem * restrict fs, void * restrict dst, size_t sz)
  __attribute__((nonnull));

#endif
