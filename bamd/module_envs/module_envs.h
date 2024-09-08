/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "moria.h"
#include "vertex.h"

struct module;
struct bacon_writer;
struct channel;

/* VERTEX_MODULE_ENVS */
typedef struct module_envs {
  moria_vertex vertex;

  stats_vertex stats;
  char id[40];
  uint16_t id_len;

  /* shadow link */
  struct module *mod;
} module_envs;

module_envs *module_envs_create(struct module * restrict mod)
  __attribute__((nonnull));

size_t module_envs_path_cat(void * restrict dst, size_t sz, const module_envs * restrict envs)
  __attribute__((nonnull));

void module_envs_describe(const module_envs * restrict envs, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void module_envs_tell(const module_envs * restrict envs, struct channel * restrict channel)
  __attribute__((nonnull));
