/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*
 * configuration of how to reconcile fsent state with actual filesystem state
 */

#include "types.h"
#include "valyria/llist.h"

struct configuration;
struct attrs16;
struct fstree;
struct fsent;
struct bacon_writer;

#define FILESYSTEM_INVALIDATE_DEFAULT INVALIDATE_STAT
#define FILESYSTEM_INDEXED_DEFAULT true

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
  invalidate_type invalidate;
  bool mkdirs;
  bool readlinks;
  bool indexed;

  struct fstree *fst; // filesystem attachment point

  llist lln;  // freelist
} filesystem;

extern struct filesystem filesystem_root;
extern struct filesystem filesystem_shadow;

extern llist filesystem_reconcile_list;

void filesystem_setup(void);
void filesystem_cleanup(void);

/*
 * filesystem_reconfigure - rebuild the filesystem lookup structure from config
 *
 * ctx    - reconfiguration context
 * config - root of the config tree
 * dry    - whether to perform a dry-run
 */
void filesystem_reconfigure(struct configuration * restrict cfg, bool dryrun)
  __attribute__((nonnull));

size_t filesystem_absolute_path_cat(void * restrict dst, size_t sz, const filesystem * restrict fs)
  __attribute__((nonnull));

void filesystem_reconcile(void);

void fsent_symlink_resolve(struct fsent *fse)
  __attribute__((nonnull));

void filesystem_mkdir_push(struct fsent * restrict fse)
  __attribute__((nonnull));

void filesystem_mkdir_unshift(struct fsent * restrict fse)
  __attribute__((nonnull));

void fstree_describe(struct bacon_writer * restrict writer)
  __attribute__((nonnull));

void fstree_show(void);

void filesystem_describe(struct bacon_writer * restrict writer)
  __attribute__((nonnull));

void filesystem_show(void);
