/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"
#include "command.h"

struct bacon_writer;
struct bam_client;
struct bamipc_message;

extern struct build_args {
  char mode;
  struct build_target {
    char *s;
    uint16_t sl;
    char mode;    // t = transitive | x = direct
  } targets[64];
  uint16_t targets_len;

  struct show_settings {
    bool show_nodes;
    bool show_sources;
    bool show_targets;
    bool show_path;
    bool show_arguments;
    bool show_command;
    bool show_cwd;
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

void build_command_setup(void);
void build_command_cleanup(void);

void build_command_usage(struct command * restrict cmd);

void build_command_collate_goals(const char * restrict label, uint16_t label_len, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void build_command_collate_invalidations(struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void build_command_process_event(struct bam_client * restrict client, struct bamipc_message * restrict msg);
