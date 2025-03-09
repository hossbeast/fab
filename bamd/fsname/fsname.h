/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "stats.h"
#include "descriptor.h"
#include "moria.h"
#include "vertex.h"

struct index_edge;
struct graph_invalidation_context;
struct fsedge;
struct bacon_writer;
struct namespace;
struct graph_disintegrate_context;

extern llist fsname_list;

typedef union stats_fsname {
  struct {
      uint32_t count;   // number of indexed directories
      uint32_t put;     // counter ; directory added to the index
      uint32_t del;     // counter ; directory removed from the index
  };

  stats_record record;
} stats_fsname;

extern descriptor_type descriptor_stats_fsname;

// VERTEX_FSNAME
typedef struct fsname {
  moria_vertex vertex;

  char id[40];
  uint16_t id_len;
  char name[255];
  uint16_t name_len;

  stats_vertex stats;
  stats_fsname fsname_stats;

  struct namespace *ns;
} fsname;

void fsname_release(fsname * restrict n)//, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

void fsname_setup(void);
void fsname_cleanup(void);

size_t fsname_path_cat(void * restrict dst, size_t sz, const fsname * restrict name)
  __attribute__((nonnull));

void fsname_describe(const fsname * restrict formula, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void fsname_identity_describe(const struct fsname * restrict fse, struct bacon_writer * restrict)
  __attribute__((nonnull));

void fsname_stats_build(const fsname * restrict formula, bool reset, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

fsname *fsname_create(struct namespace * restrict ns, const char * restrict label, uint16_t label_len)
  __attribute__((nonnull));

bool fsname_reap(struct fsname * restrict fsname, struct graph_disintegrate_context * restrict ctx)
  __attribute__((nonnull));

void fsname_unlink(struct fsname * restrict fsname, struct graph_disintegrate_context * restrict ctx)
  __attribute__((nonnull));

void fsname_hash(const fsname * restrict v, const void ** restrict hp, uint16_t * restrict hl)
  __attribute__((nonnull));

void fsname_id(const fsname * restrict v, const char ** restrict idp, uint16_t * restrict idl)
  __attribute__((nonnull));

void fsname_compute_id(const struct fsname * restrict fsn, struct identity_ctx * restrict id)
  __attribute__((nonnull));

void fsnames_show(void);
