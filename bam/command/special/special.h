/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "command.h"

struct bacon_writer;

void special_describe_process(struct command * restrict cmd, struct bam_client * restrict client, struct bamipc_message * restrict msg)
  __attribute__((nonnull));

void special_describe_disconnected(struct command * restrict cmd)
  __attribute__((nonnull));

void special_describe_select(struct command * restrict cmd, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void special_describe_connected(struct command * restrict cmd, struct bam_client * restrict client)
  __attribute__((nonnull));

void special_list_process(struct command * restrict cmd, struct bam_client * restrict client, struct bamipc_message * restrict msg)
  __attribute__((nonnull));

void special_list_disconnected(struct command * restrict cmd)
  __attribute__((nonnull));

void special_list_connected(struct command * restrict cmd, struct bam_client * restrict client)
  __attribute__((nonnull));

void special_list_build(command * restrict _cmd, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));
