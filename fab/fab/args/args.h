/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

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
struct trace_args;

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

  bool invalidate;        // trigger global invalidation
  bool kill;              // kill extant daemon if any
  bool help;
  bool version;
  int verbose;            // show request/response, and daemon stdout/stderr
  bool no_color;          // disable terminal color escapes

  uint64_t context_module_id;
  bool context_module;
  bool no_context_module;
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
void args_parse(int argc, char ** argv);

void args_cleanup(void);

void args_command_start(struct command * restrict cmd, uint32_t flags)
  __attribute__((nonnull));

void args_command_finish(void);

//void args_clear_module_path(void);
void args_set_module_path(const char * restrict path, uint16_t len);
void args_set_module_abspath(const char * restrict path, uint16_t len);
void args_set_no_module(void);
//void args_set_module_id(uint64_t id);//const char * restrict id, uint16_t len);
