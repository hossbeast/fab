/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "descriptor.h"
#include "stats.h"
#include "types.h"

struct fsent;
struct graph_disintegrate_context;
struct moria_vertex;

extern descriptor_type descriptor_stats_fsent_symlink;

/* VERTEX_SYMLINK - symlink node */
typedef union stats_fsent_symlink {
  struct {
    /* event counters */
    uint32_t readlink_no;         /* readlink not permitted by fstree */
    uint32_t readlink_ok;         /* readlink() succeeded, target resolved */
    uint32_t readlink_unresolved; /* readlink() succeeded, target did not resolve, fstree readlink : yes */
    uint32_t readlink_failed;     /* readlink() succeeded, target did not resolve, fstree readlink : required */
    uint32_t readlink_err;        /* readlink(), other nonzero error */

    /* instantaneous counts */
    uint8_t  readlink_pending;
  };

  stats_record record;
} stats_fsent_symlink;

bool fsent_symlink_reap(struct fsent * restrict, struct graph_disintegrate_context * restrict ctx)
  __attribute__((nonnull));

void fsent_symlink_unlink(struct fsent * restrict fse, struct graph_disintegrate_context * restrict ctx)
  __attribute__((nonnull));

struct moria_vertex *fsent_symlink_follow(const struct fsent * restrict fse)
  __attribute__((nonnull));
