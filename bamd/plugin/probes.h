/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

struct exec;
struct plugin;
struct iovec;

void plugin_exec_probe(const struct plugin * restrict plugin , const struct exec * restrict exec)
  __attribute__((nonnull));

void plugin_unready_probe(const struct plugin * restrict plugin)
  __attribute__((nonnull));

void plugin_ready_probe(const struct plugin * restrict plugin)
  __attribute__((nonnull));

void plugin_reap_probe(const struct plugin * restrict plugin)
  __attribute__((nonnull));

void plugin_tx_probe(const struct plugin * restrict plugin, const struct iovec * restrict iovs, int iovcnt)
  __attribute__((nonnull));

void plugin_rx_probe(const struct plugin * restrict plugin, const void *buf, size_t sz)
  __attribute__((nonnull));
