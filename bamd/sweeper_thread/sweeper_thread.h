/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "valyria/llist.h"
#include "valyria/rbtree.h"

struct fsent;
struct config;

#define SWEEPER_EVENT_SELF    1
#define SWEEPER_EVENT_CHILD   2

typedef struct sweeper_event {
  llist lln;              // in freelist / event queue
  uint8_t kind;
  uint32_t time;          // relative to sweeper_thread_clock
  uint32_t mask;
} sweeper_event;

typedef struct sweeper_child_event {
  sweeper_event sweep_event;
  rbnode rbn;            // in parent->pending_child
  struct fsent *parent;
  char name[128];
  uint8_t name_len;
} sweeper_child_event;

extern uint64_t sweeper_thread_interval;
extern uint32_t sweeper_thread_clock;

void sweeper_thread_cleanup(void);
void sweeper_thread_launch(void);
void sweeper_thread_reconfigure(struct config * restrict cfg, bool dryrun);

void sweeper_thread_enqueue(struct fsent *n, uint32_t m, const char * restrict name, uint16_t namel)
  __attribute__((nonnull(1)));

void sweeper_thread_cancel(struct fsent *n)
  __attribute__((nonnull));
