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

#ifndef COMMAND_BUILD_H
#define COMMAND_BUILD_H

#include "xapi.h"
#include "types.h"

#include "command.h"

struct narrator;
struct fab_client;
struct fabipc_message;

extern struct build_args {
  char mode;
  struct build_target {
    char *s;
    uint16_t sl;
    char mode;    // t = transitive | x = direct
  } targets[64];
  uint16_t targets_len;

  struct show_settings {
    bool show_path;
    bool show_arguments;
    bool show_command;
    bool show_cwd;
    bool show_sources;
    bool show_targets;
    bool show_environment;
    bool show_status;

    bool show_stdout;
    int16_t show_stdout_limit_lines;
    int16_t show_stdout_limit_bytes;

    bool show_stderr;
    int16_t show_stderr_limit_lines;
    int16_t show_stderr_limit_bytes;

    bool show_auxout;
    int16_t show_auxout_limit_lines;
    int16_t show_auxout_limit_bytes;
  } error;

  struct show_settings success;
} build_args;

extern struct command build_command;
extern bool build_was_failed_slot;

xapi build_command_setup(void);
xapi build_command_cleanup(void);

void build_command_usage(struct command * restrict cmd);

xapi build_command_collate_goals(struct narrator * restrict N, bool reconcile)
  __attribute__((nonnull));

xapi build_command_collate_invalidations(struct narrator * restrict N)
  __attribute__((nonnull));

xapi build_command_process_event(struct fab_client * restrict client, struct fabipc_message * restrict msg);

#endif
