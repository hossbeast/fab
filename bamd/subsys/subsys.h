/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "moria.h"

enum vertex_type;
struct identity_ctx;

typedef struct subsys {
  moria_vertex vertex;

  char id[40];
  uint16_t id_len;
} subsys;

void subsys_init(subsys * restrict v, enum vertex_type type);

void subsys_id(const subsys * restrict v, const char ** restrict id, uint16_t * restrict idl)
  __attribute__((nonnull));

void subsys_compute_id(const struct subsys * restrict subsys, struct identity_ctx * restrict id)
  __attribute__((nonnull));
