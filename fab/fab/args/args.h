/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

   This file is part of fab.

   fab is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   fab is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with fab.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef _ARGS_H
#define _ARGS_H

#include "xapi.h"
#include "types.h"

#include "fab/graph.h"

struct command;

struct build_args;
struct describe_args;
struct events_args;
struct ls_args;
struct stats_args;
struct touch_args;
struct tree_args;

typedef struct bead {
  const char *s;
  uint16_t len;
} bead;

extern struct {
  /* passthrough to fabd */
  bead system_config_path;
  bead user_config_path;
  bead project_config_path;
  bead default_filesystem_invalidate;
  bead sweeper_period_nsec;
  bool no_launch;

  bool invalidate;    // trigger global invalidation
  bool kill;          // kill extant daemon if any
  bool help;
  bool version;
  int verbose;        // show request/response, and daemon stdout/stderr

  struct stats_args *stats;
  struct touch_args *touch;
  struct build_args *build;
  struct tree_args *tree;
  struct ls_args *ls;
  struct describe_args *describe;
  struct adhoc_args *adhoc;
  struct events_args *events;
} g_args;

extern struct command *g_cmd;

/// args_parse
//
// SUMMARY
//  parses command-line options, populating g_args.
//
// REMARKS
//  prints the usage statement and terminates the program if options are
//  invalid, for example required options are not present, or invalid
//  parameters are given to an option
//
// PARAMETERS
//  cmd - (returns) subcommand dispatch
//
xapi args_parse(void);

#endif
