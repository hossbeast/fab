/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "descriptor.h"
#include "stats.h"
#include "types.h"

struct fsent;
struct graph_disintegrate_context;

extern descriptor_type descriptor_stats_fsent_dir;

/* BAM_FSENT_TYPE_DIR - directory node */
typedef union stats_fsent_dir {
  struct {
    /* directory nodes only */
    uint32_t rules;         /* number of rma references to this directory */
    uint16_t dentries;      /* children in the fstree graph */
    uint8_t indexed_lookup; /* whether this node is indexed for lookup in any namespace */

    /* event counters */
    /* attempts to create invalidated directories */
    uint32_t mkdir_no;        /* mkdir not attempted, disallowed by fstree */
    uint32_t mkdir_ok;        /* mkdir() returned zero */
    uint32_t mkdir_eexist;    /* mkdir(), EEXIST */
    uint32_t mkdir_err;       /* mkdir(), other nonzero error */

    /* attempts to delete fs dirent entries */
    uint32_t unlink_no;     /* unlink not permitted by fstree */
    uint32_t unlink_ok;     /* unlink() returned zero */
    uint32_t unlink_enoent; /* unlink(), ENOENT */
    uint32_t unlink_err;    /* unlink(), other nonzero error */
    uint32_t rmdir_no;      /* rmdir not permitted by fstree */
    uint32_t rmdir_ok;      /* rmdir() returned zero */
    uint32_t rmdir_enoent;  /* rmdir(), ENOENT */
    uint32_t rmdir_err;     /* rmdir(), other nonzero error */
  };

  stats_record record;
} stats_fsent_dir;

void fsent_dir_reconcile(void);
void fsent_dir_mkdir_push(struct fsent * restrict fse)
  __attribute__((nonnull));
void fsent_dir_mkdir_unshift(struct fsent * restrict fse)
  __attribute__((nonnull));
void fsent_dir_mkdir_cancel(struct fsent * restrict fse)
  __attribute__((nonnull));

bool fsent_dir_reap(struct fsent * restrict, struct graph_disintegrate_context * restrict ctx)
  __attribute__((nonnull));
