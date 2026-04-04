/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

struct moria_vertex;

void vertex_alloc_probe(const struct moria_vertex * restrict v)
  __attribute__((nonnull));

void vertex_unlink_probe(const struct moria_vertex * restrict v)
  __attribute__((nonnull));

void vertex_free_probe(const struct moria_vertex * restrict v)
  __attribute__((nonnull));

void vertex_state_probe(const struct moria_vertex * restrict v, uint32_t before)
  __attribute__((nonnull));
