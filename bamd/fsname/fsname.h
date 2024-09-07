/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "moria.h"

struct index_edge;
struct graph_invalidation_context;
struct fsedge;
struct bacon_writer;

// VERTEX_FSNAME
typedef struct fsname {
  moria_vertex vertex;

  char name[255];
  uint16_t name_len;
  char id[40];
  uint16_t id_len;

  /* shadow link */
  struct fsedge *links[2];

  /* number of downwards edges */
  uint32_t down;
} fsname;

fsname *fsname_alloc(const char * restrict label, uint16_t label_len)
  __attribute__((nonnull));

void fsname_release(fsname * restrict n, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

void fsname_setup(void);
void fsname_cleanup(void);

size_t fsname_path_cat(void * restrict dst, size_t sz, const fsname * restrict name)
  __attribute__((nonnull));

void fsname_describe(const fsname * restrict formula, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void fsname_identity_say(const struct fsname * restrict fse, struct narrator * restrict N)
  __attribute__((nonnull));

void fsname_stats_build(fsname * restrict formula, bool reset, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));
