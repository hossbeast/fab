/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "command.h"
#include "args.h"

#define TREE_LS 1

extern struct tree_args {
  const char *targets[64]; // identifies the node to start the tree from
  uint16_t targets_lens[64];
  uint16_t targets_len;
  bool nofollow;          // only on target lookup
  bool hidden;
  const char *graph;
  uint16_t graph_len;
  const char *visit;
  uint16_t visit_len;
  const char *direction;  // up | down
  uint16_t direction_len;
  bool both;              // both up and down
  bool show_labels;
  bool show_paths;
  uint16_t min_distance;
  uint16_t max_distance;
  int special;
  bool primary;
  bool cross_module;
  bool cross_fstree;
} tree_args;

extern struct command tree_command;

void tree_init(uint32_t flags);
