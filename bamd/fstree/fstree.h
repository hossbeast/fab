/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "moria.h"
#include "valyria/llist.h"
#include "valyria/rbtree.h"
#include "descriptor.h"
#include "stats.h"
#include "vertex.h"

struct config;
struct moria_vertex;
struct bacon_writer;
struct filesystem;
struct narrator;
struct fsedge;
struct fsent;
struct vertices;
struct graph_disintegrate_context;
struct graph_invalidation_context;

#define FILESYSTEM_INVALIDATION_DEFAULT INVALIDATION_STAT
#define FILESYSTEM_INDEX_DEFAULT true
#define FILESYSTEM_READLINK_DEFAULT READLINK_YES

// filesystem attributes
#define INVALIDATION_OPT 0xf
#define INVALIDATION_TYPE_TABLE                                                                     \
  DEF(INVALIDATION_STAT    , "stat"    , INVALIDATION_OPT, 0x1) /* stat hash (default) */           \
  DEF(INVALIDATION_CONTENT , "content" , INVALIDATION_OPT, 0x2) /* content hash */                  \
  DEF(INVALIDATION_NOTIFY  , "notify"  , INVALIDATION_OPT, 0x3) /* filesystem event subscription */ \
  DEF(INVALIDATION_ALWAYS  , "always"  , INVALIDATION_OPT, 0x4) /* always considered invalid */     \
  DEF(INVALIDATION_NEVER   , "never"   , INVALIDATION_OPT, 0x5) /* never considered invalid */      \

typedef enum invalidation_type {
#undef DEF
#define DEF(x, n, r, y) x = UINT16_C(y),
INVALIDATION_TYPE_TABLE
#undef DEF
} invalidation_type;

extern struct attrs16 *invalidation_attrs;

#define READLINK_TYPE_TABLE                                                                        \
  DEF(READLINK_YES       , "yes"       ,   1)    /* readlink symlinks */                           \
  DEF(READLINK_NO        , "no"        ,   2)    /* dont readlink symlinks */                      \
  DEF(READLINK_REQUIRED  , "required"  ,   3)    /* readlink and require that symlinks resolve */

typedef enum readlink_type {
#undef DEF
#define DEF(x, s, v) x = v,
READLINK_TYPE_TABLE
#undef DEF
} readlink_type;

extern struct attrs16 *readlink_attrs;

extern descriptor_type descriptor_stats_fstree;

typedef union stats_fstree {
  struct {
    /* event counters */
    uint32_t stathash;     /* file is stat-hashed */
    uint32_t contenthash;  /* file is content-hashed */

    /* attempts to create invalidated directories */
    uint32_t mkdir_no;        /* mkdir not attempted, disallowed by fstree */
    uint32_t mkdir_ok;        /* mkdir() returned zero */
    uint32_t mkdir_eexist;    /* mkdir(), EEXIST */
    uint32_t mkdir_err;       /* mkdir(), other nonzero error */

    /* attempts to unlink filesystem entries */
    uint32_t unlink_no;     /* unlink not permitted by fstree */
    uint32_t unlink_ok;     /* unlink() returned zero */
    uint32_t unlink_enoent; /* unlink(), ENOENT */
    uint32_t unlink_err;    /* unlink(), other nonzero error */
    uint32_t rmdir_no;      /* rmdir not permitted by fstree */
    uint32_t rmdir_ok;      /* rmdir() returned zero */
    uint32_t rmdir_enoent;  /* rmdir(), ENOENT */
    uint32_t rmdir_err;     /* rmdir(), other nonzero error */

    uint32_t readlink_no;         /* readlink not permitted by fstree */
    uint32_t readlink_ok;         /* readlink() succeeded, target resolved */
    uint32_t readlink_unresolved; /* readlink() succeeded, target did not resolve, fstree readlink : yes */
    uint32_t readlink_failed;     /* readlink() succeeded, target did not resolve, fstree readlink : required */
    uint32_t readlink_err;        /* readlink(), other nonzero error */
  };

  stats_record record;
} stats_fstree;

/* VERTEX_FSTREE */
typedef struct fstree {
  moria_vertex vertex;

  stats_vertex stats;
  stats_fstree fstree_stats;
  char id[40];
  uint16_t id_len;

  struct fsent *base;
  char base_path[PATH_MAX];
  uint16_t base_path_len;

  /* configuration */
  bool mount;                     // whether to descend
  bool mkdir;                     // whether to create directories
  bool unlink;                    // whether to unlink entries
  readlink_type readlink;         // symlink strategy
  invalidation_type invalidation; // invalidation strategy
} fstree;

extern struct fstree *fstree_root;
extern struct fstree *fstree_shadow;
extern llist fstree_list;

void fstree_describe(const fstree *fst, struct bacon_writer * restrict writer)
  __attribute__((nonnull));

void fstree_identity_describe(const struct fstree * restrict fstree, struct bacon_writer * restrict)
  __attribute__((nonnull));

void fstree_show(const fstree *fst)
  __attribute__((nonnull));

/* trace */
size_t fstree_cat(void * restrict dst, size_t sz)
  __attribute__((nonnull));

void fstree_setup(void);
void fstree_root_setup(void);
void fstree_cleanup(void);

/*
 * filesystem_reconfigure - rebuild the filesystem lookup structure from config
 *
 * ctx    - reconfiguration context
 * config - root of the config tree
 * dry    - whether to perform a dry-run
 */
void fstree_reconfigure(struct config * restrict cfg, bool dryrun)
  __attribute__((nonnull));

size_t fstree_path_cat(void * restrict dst, size_t sz, const fstree * restrict fst)
  __attribute__((nonnull));

void fstree_stats_build(fstree * restrict, bool reset, struct bacon_writer * restrict)
  __attribute__((nonnull));

/* returns true if the node is in a mounted fsent */
bool fstree_in_ancestry(const struct fsent * restrict)
  __attribute__((nonnull));

void fstree_unlink(fstree * restrict fst, struct graph_disintegrate_context * restrict)
  __attribute__((nonnull));

void fstree_release(fstree * restrict fst)
  __attribute__((nonnull));

void fstree_hash(const fstree * restrict v, const void ** restrict hp, uint16_t * restrict hl)
  __attribute__((nonnull));

void fstree_id(const fstree * restrict v, const char ** restrict idp, uint16_t * restrict idl)
  __attribute__((nonnull));

void fstree_compute_id(const struct fstree * restrict fsts, struct identity_ctx * restrict id)
  __attribute__((nonnull));

void fstrees_show(void);
