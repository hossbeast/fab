/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"
#include "valyria/llist.h"

struct rule_run_context;
struct config;

extern llist module_rebuild_list;   // modules and rules.bam files needing to be rebuilt

typedef struct rules_container {
  llist lln;
  uint32_t attrs;         // vertex.attrs

  /* rebuild state */
  bool disconnect;
  bool reparse;
  bool rebuild;
  bool reimport;
  bool prune;
} rules_container;

/*
 * reload invalidated modules
 */
void module_system_reconcile(struct rule_run_context * restrict rule_ctx)
  __attribute__((nonnull));
