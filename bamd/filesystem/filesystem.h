/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*
 * configuration of how to reconcile fsent state with actual filesystem state
 */

#include "types.h"
#include "valyria/llist.h"
#include "stats.h"
#include "descriptor.h"

struct config;
struct fsent;
struct bacon_writer;
struct moria_vertex;

extern llist filesystem_reconcile_list; // struct fsent

typedef union stats_filesystem {
  struct {
      uint32_t stathash;        /* file is stat-hashed */
      uint32_t contenthash;     /* file is content-hashed */
      uint32_t fstree_refresh;

      /* attempts to create invalidated directories */
      uint32_t mkdir_no;        /* mkdir not attempted, disallowed by fstree */
      uint32_t mkdir_ok;        /* mkdir() returned zero */
      uint32_t mkdir_eexist;    /* mkdir(), EEXIST */
      uint32_t mkdir_err;       /* mkdir(), other nonzero error */

      /* attempts to unlink filesystem entries */
      uint32_t unlink_no;           /* unlink not permitted by fstree */
      uint32_t unlink_ok;           /* unlink() returned zero */
      uint32_t unlink_enoent;       /* unlink(), ENOENT */
      uint32_t unlink_err;          /* unlink(), other nonzero error */
      uint32_t rmdir_no;            /* rmdir not permitted by fstree */
      uint32_t rmdir_ok;            /* rmdir() returned zero */
      uint32_t rmdir_enoent;        /* rmdir(), ENOENT */
      uint32_t rmdir_err;           /* rmdir(), other nonzero error */

      /* attempts to resolve symlinks via readlink */
      uint32_t readlink_no;         /* readlink not permitted by fstree */
      uint32_t readlink_ok;         /* readlink() succeeded, target resolved */
      uint32_t readlink_unresolved; /* readlink() succeeded, target did not resolve, fstree readlink : yes */
      uint32_t readlink_failed;     /* readlink() succeeded, target did not resolve, fstree readlink : required */
      uint32_t readlink_err;        /* readlink(), other nonzero error */
  };

  stats_record record;
} stats_filesystem;

extern union stats_filesystem filesystem_stats;
extern descriptor_type descriptor_stats_filesystem;

void filesystem_setup(void);

void filesystem_sys_stats_build(const struct moria_vertex * restrict, bool reset, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void filesystem_sys_describe(const struct moria_vertex * restrict, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void filesystem_sys_id(const struct moria_vertex * restrict, const char ** restrict, uint16_t * restrict)
  __attribute__((nonnull));
