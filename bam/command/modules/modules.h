/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "command.h"
#include "special.h"
#include "args.h"

struct bamipc_message;
struct bam_client;

extern struct {
  command cmd;

  /* arguments */
  char *target;           // path to resolve the module of
  uint16_t target_len;
  char module_id[40];
  uint16_t module_id_len;

  /* state */
  uint64_t module_identify_id;
  uint64_t module_describe_id;
  bool any;
} e;

extern struct command module_command;
extern struct command modules_command;

void module_command_identify_dot(struct command * restrict cmd, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));
void module_command_identify_id(struct command * restrict cmd, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));
void module_command_identify_target(struct command * restrict cmd, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void module_command_connected(struct command * restrict cmd, struct bam_client * restrict client)
  __attribute__((nonnull));

/*
 * process a message for the module identify phase posted by module_command_connected
 * (identify_id_0 / identify_id_1). identify by id is tried first where the target looks
 * like an id, falling back to identify by path. returns true once the module has been
 * identified; on lookup failure, reports the failure and shuts down the command.
 */
bool module_identify_msg(struct command * restrict cmd, struct bam_client * restrict client, struct bamipc_message * restrict msg)
  __attribute__((nonnull));
