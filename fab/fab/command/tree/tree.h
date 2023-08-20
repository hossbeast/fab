/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "fab/graph.h"

#include "command.h"
#include "args.h"

extern struct tree_args {
  bead target;            // identifies the node to start the tree from
  fab_graph graph;
  const char *direction;
  bool both;
  bool show_paths;
} tree_args;

extern struct command tree_command;
