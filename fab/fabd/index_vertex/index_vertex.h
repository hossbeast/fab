/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "moria.h"
#include "bam/stats.h"
#include "fsname.h"

struct index_edge;
struct graph_invalidation_context;

// VERTEX_INDEX
typedef struct index_vertex {
  moria_vertex vertex;

  fsname name;
  uint64_t hash;

  char id[40];
  uint16_t id_len;

  char token[40];
  uint16_t token_len;

  bam_vertex_stats stats;

  /* number of downwards edges */
  uint32_t down;
} index_vertex;

extern struct index_vertex *g_index;

void index_vertex_setup(void);
void index_vertex_cleanup(void);

index_vertex *index_vertex_lookup(const char * restrict label, uint16_t label_len)
  __attribute__((nonnull));

index_vertex *index_vertex_create(const char * restrict label, uint16_t label_len)
  __attribute__((nonnull));

void index_vertex_release(index_vertex * restrict n, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));
