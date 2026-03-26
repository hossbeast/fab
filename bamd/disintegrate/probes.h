/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

struct graph_disintegrate_context;

void disintegrate_probe(struct graph_disintegrate_context * restrict)
  __attribute__((nonnull));

void disintegrate_round_probe(struct graph_disintegrate_context * restrict)
  __attribute__((nonnull));

void disintegrate_reap_probe(const moria_vertex * restrict v)
  __attribute__((nonnull));

void disintegrate_keep_unlinked_probe(const moria_vertex * restrict v)
  __attribute__((nonnull));

void disintegrate_keep_exists_probe(const moria_vertex * restrict v)
  __attribute__((nonnull));

void disintegrate_keep_has_up_probe(const moria_vertex * restrict v)
  __attribute__((nonnull));

void disintegrate_keep_has_down_probe(const moria_vertex * restrict v)
  __attribute__((nonnull));
