/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"
#include "valyria/hashtable.h"
#include "stats.h"
#include "descriptor.h"

struct build_slot;
struct config;
struct rcu_thread;

typedef union stats_record_build {
  struct {
      uint32_t fml_exec;                /* formula execution attempted */
      uint32_t fml_exec_ok;             /* formula executed successfully */
      uint32_t build;                   /* build started */
      uint32_t build_ok;                /* build ran to completion with no errors */
      uint32_t build_error;             /* build ran to completion ; one or more formulas failed */
  };

  stats_record record;
} stats_record_build;

extern union stats_record_build build_stats;
extern struct moria_vertex *g_build;
extern descriptor_type descriptor_stats_record_build;

extern struct build_slot *build_slots;
extern struct hashtable build_slots_bypid;
extern int build_slots_bypid_lock;
extern int build_devnull_fd;

void build_setup(void);
void build_cleanup(void);

/*
 * reapply config to the build subsystem
 *
 * config - root of the config tree
 * dry    - whether to perform a dry-run
 */
void build_reconfigure(struct config * restrict cfg, bool dryrun)
  __attribute__((nonnull));

/*
 * carry out the build
 */
void build(
    struct rcu_thread * restrict rcu_self
  , bool autorun
  , pid_t pid
  , uint64_t token
  , const char * restrict label
  , uint16_t label_len
)
  __attribute__((nonnull));


void build_describe(struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void build_stats_build(bool reset, struct bacon_writer * restrict)
  __attribute__((nonnull));

size_t build_id_cat(char *dst, size_t sz)
  __attribute__((nonnull));
