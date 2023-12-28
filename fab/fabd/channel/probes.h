/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

struct channel;
struct bamipc_message;

void channel_rx_probe(const struct channel * restrict chan, struct bamipc_message * restrict msg)
  __attribute__((nonnull));

void channel_tx_probe(const struct channel * restrict chan, struct bamipc_message * restrict msg)
  __attribute__((nonnull));
