/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

struct bam_client;
struct bamipc_message;
struct narrator;

#define COMMAND_OPT               0xffff
#define COMMAND_ATTRS_TABLE               \
  DEF(COMMAND_CONTEXT_MODULE,     0x1)    \

typedef enum command_opt {
#undef DEF
#define DEF(x, y) x = y,
COMMAND_ATTRS_TABLE
} command_opt;

extern struct attrs32 command_attrs;

typedef struct command {
  char *name;
  uint32_t attrs;
  void (*usage)(struct command *cmd);
  void __attribute__((nonnull)) (*init)(struct command *cmd, uint32_t flags);
  void __attribute__((nonnull)) (*attached)(struct command *cmd, struct bam_client * restrict client);
  void __attribute__((nonnull)) (*connected)(struct command *cmd, struct bam_client * restrict client);
  void __attribute__((nonnull)) (*process)(
      struct command *cmd
    , struct bam_client * restrict client
    , struct bamipc_message * restrict msg
  );
  void __attribute__((nonnull)) (*disconnected)(struct command *cmd);
} command;

void client_post(struct bam_client * restrict client, struct bamipc_message * restrict msg)
  __attribute__((nonnull));
