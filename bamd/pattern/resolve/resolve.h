/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

struct lookup_pattern;
struct path_pattern;

/* ref is either a lookup_pattern or a path_pattern, and it must
 * be a bound pattern */

typedef struct resolve_context {
  /* input */
  const struct pattern *ref;
  const struct module *mod;
  struct bacon_value *vars;
  const struct fsent *base;
  bool nofollow;
  bool hidden;

  /* state */
  struct moria_vertex *last;      // last vertex visited

  /* returns */
  int r;
  struct moria_vertex *vertices[2];
} resolve_context;

int pattern_resolve_path(struct resolve_context * restrict ctx, struct path_pattern * restrict pattern)
  __attribute__((nonnull));

int pattern_resolve_lookup(struct resolve_context * restrict ctx, struct lookup_pattern * restrict pattern)
  __attribute__((nonnull));
