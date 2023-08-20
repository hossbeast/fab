/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

struct moria_edge;
struct moria_vertex;

extern unsigned short moria_explore_edge_semaphore;
extern unsigned short moria_explore_vertex_semaphore;

void explore_edge_probe(struct moria_edge * restrict e, int distance, bool travel, bool visit)
  __attribute__((nonnull));

void explore_vertex_probe(struct moria_vertex * restrict v, int distance, bool travel, bool visit)
  __attribute__((nonnull));
