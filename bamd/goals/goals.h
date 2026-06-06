/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*

The goals describe the set of nodes which are to be brought up to date by building.

*/

#include "types.h"

struct request_handler;
struct narrator;
struct selector;
struct bacon_writer;
struct moria_vertex;

extern pid_t goals_pid;
extern uint64_t goals_token;
extern char goals_label[128];
extern uint16_t goals_label_len;
extern uint32_t goals_id;
extern struct timespec goals_time;
extern bool goals_reconcile;
extern bool goals_build;

void goals_set(
    pid_t pid
  , uint64_t token
  , const char *label
  , uint16_t label_len
  , bool reconcile
  , bool build
  , struct selector * restrict target_direct
  , struct selector * restrict target_transitive
);

void goals_kickoff(struct request_handler * restrict handler)
  __attribute__((nonnull));

void goals_setup(void);
void goals_cleanup(void);

void goals_sys_describe(const struct moria_vertex * restrict v, struct bacon_writer * restrict writer)
  __attribute__((nonnull));

void goals_create_buildplan(void);

void goals_set_event(struct request_handler * restrict handler);

void goals_sys_stats_build(const struct moria_vertex * restrict, bool reset, struct bacon_writer * restrict)
  __attribute__((nonnull));
