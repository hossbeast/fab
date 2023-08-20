/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

struct fab_client;
struct fabipc_message;
struct narrator;

typedef struct command {
  char *name;

  void (*usage)(struct command *cmd);
  void __attribute__((nonnull)) (*connected)(struct command *cmd, struct fab_client * restrict client);
  void __attribute__((nonnull)) (*process)(
      struct command *cmd
    , struct fab_client * restrict client
    , struct fabipc_message * restrict msg
  );
} command;

void client_post(struct fab_client * restrict client, struct fabipc_message * restrict msg)
  __attribute__((nonnull));
