/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*
 * configuration of how to reconcile fsent state with actual filesystem state
 */

#include "types.h"
#include "valyria/llist.h"

struct configblob;
struct attrs16;
struct fstree;

#define FILESYSTEM_INVALIDATE_DEFAULT INVALIDATE_STAT

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

extern struct attrs16 * invalidate_attrs;

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

void filesystem_setup(void);
void filesystem_cleanup(void);

/*
 * filesystem_reconfigure - rebuild the filesystem lookup structure from config
 *
 * ctx    - reconfiguration context
 * config - root of the config tree
 * dry    - whether to perform a dry-run
 */
int filesystem_reconfigure(struct configblob * restrict cfg, char * restrict err, uint16_t err_sz);

size_t filesystem_absolute_path_znload(void * restrict dst, size_t sz, const filesystem * restrict fs)
  __attribute__((nonnull));
