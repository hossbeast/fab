/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"
#include "valyria/hashtable.h"

struct build_slot;
struct configuration;
struct rcu_thread;

extern struct build_slot * build_slots;
extern struct hashtable build_slots_bypid;
extern int build_slots_bypid_lock;
extern bool build_stage_failure;
extern int build_devnull_fd;

void build_setup(void);
void build_cleanup(void);

/*
 * reapply configuration to the build subsystem
 *
 * config - root of the config tree
 * dry    - whether to perform a dry-run
 */
void build_reconfigure(struct configuration * restrict cfg, bool dryrun)
  __attribute__((nonnull));

/*
 * carry out the build
 */
void build(struct rcu_thread *self)
  __attribute__((nonnull));
