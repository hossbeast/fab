/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*

The goals describe the set of nodes which are to be brought up to date by building.

*/

#include "types.h"

struct handler_context;
struct narrator;
struct selector;

extern bool goals_reconcile;
extern bool goals_build;

void goals_set(
    uint64_t msg_id
  , bool reconcile
  , bool build
  , bool script
  , struct selector * restrict target_direct
  , struct selector * restrict target_transitive
);

void goals_kickoff(struct handler_context * restrict handler)
  __attribute__((nonnull));

void goals_setup(void);
void goals_cleanup(void);
void goals_say(struct narrator * restrict N)
  __attribute__((nonnull));

void goals_create_buildplan(void);
