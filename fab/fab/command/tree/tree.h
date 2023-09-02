/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "fab/graph.h"

#include "command.h"
#include "args.h"

#define TREE_LS 1

extern struct tree_args {
  bead target;            // identifies the node to start the tree from
  bool nofollow;          // on target lookup
  fab_graph graph;
  const char *direction;  // up | down
  bool both;              // both up and down
  bool show_labels;
  bool show_paths;
  uint16_t min_distance;
  uint16_t max_distance;
} tree_args;

extern struct command tree_command;
