/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "command.h"
#include "args.h"

struct bamipc_message;
struct bam_client;

typedef struct command_show {
  command cmd;
} command_show;

extern struct command show_command;

void command_show_connected(command * restrict cmd, struct bam_client * restrict client)
  __attribute__((nonnull));

void command_show_process(command * restrict cmd, struct bam_client * restrict client, struct bamipc_message * restrict msg)
  __attribute__((nonnull));

void command_show_disconnected(command * restrict cmd)
  __attribute__((nonnull));
