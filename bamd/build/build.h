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
struct moria_vertex;

typedef union stats_record_build {
  struct {
    /* cumulative */
    uint32_t fml_exec_ok;         /* slot built successfully */
    uint32_t fml_exec_err;        /* slot had an error */
    uint32_t slot_ok;             /* slot built successfully */
    uint32_t slot_err;            /* slot had an error */
    uint32_t build;               /* build started */
    uint32_t build_ok;            /* build ran to completion with no errors */
    uint32_t build_err;           /* build ran to completion ; one or more slots failed */

    /* most recent build */
    union {
      char _last;
      uint32_t last_build_ok;
    };
    uint32_t last_build_err;
    uint32_t last_fml_exec_ok;
    uint32_t last_fml_exec_err;
    uint32_t last_slot_ok;
    uint32_t last_slot_err;
    uint32_t last_slots;
    uint32_t last_stages;
  };

  stats_record record;
} stats_record_build;

extern uint32_t build_system_stdout_size;
extern uint32_t build_system_stderr_size;
extern uint32_t build_system_auxout_size;
extern uint32_t build_system_retention_max_entries;

extern union stats_record_build build_stats;
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
    bool autorun
  , pid_t pid
  , uint64_t token
  , const char * restrict label
  , uint16_t label_len
)
  __attribute__((nonnull));


void build_sys_describe(const struct moria_vertex * restrict, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void build_sys_stats_build(const struct moria_vertex * restrict, bool reset, struct bacon_writer * restrict)
  __attribute__((nonnull));

void build_sys_id(const struct moria_vertex * restrict, const char ** restrict, uint16_t * restrict)
  __attribute__((nonnull));
