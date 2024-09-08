/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

#include "moria.h"
#include "varset.h"
#include "descriptor.h"
#include "stats.h"
#include "vertex.h"

struct fsent;
struct bacon_value;
struct moria_graph;
struct graph_invalidation_context;

extern descriptor_type descriptor_stats_varset;

/* BAM_FSENT_TYPE_VARSET_FILE - var.bam file */
typedef union stats_varset {
  struct {
    /* event counters */
    uint16_t parse;          /* parse attempted */
    uint16_t parse_ok;       /* parsed successfully */
  };

  stats_record record;
} stats_varset;

/* VERTEX_VARSET */
typedef struct varset {
  moria_vertex vertex;

  /* runtime state */
  stats_vertex stats;
  stats_varset varset_stats;
  struct bacon_value *val;
  char id[40];
  uint16_t id_len;

  /* shadow link */
  struct fsedge *link;

  /* self_node */
  struct fsent *self_node;    // VERTEX_VARSET_FILE var.bam
  char self_node_abspath[PATH_MAX];
  uint16_t self_node_abspath_len;
} varset;

void varset_setup(void);
void varset_cleanup(void);

extern llist varset_list;    // active

/*
 * Write a vars stats to a buffer
 *
 * reset - true to reset the stats while reading them
 * zp    - (returns) number of bytes written to dst
 */
void varset_stats_collate(struct varset * restrict var, bool reset)
  __attribute__((nonnull));

/* allocate a new var in the invalid state */
void varset_alloc(varset ** restrict vp, struct moria_graph * restrict g)
  __attribute__((nonnull));

/* parse the var if invalid */
void varset_reparse(varset * restrict var)
  __attribute__((nonnull));

void varset_release(varset * restrict var, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

void varset_system_reconcile(struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

size_t varset_path_cat(void * restrict dst, size_t sz, const varset * restrict config)
  __attribute__((nonnull));

void varset_describe(const struct varset * restrict varset, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void varset_file_describe(const struct fsent * restrict fse, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void varset_identity_say(const struct varset * restrict varset, struct narrator * restrict N)
  __attribute__((nonnull));

void varset_file_identity_say(const struct fsent * restrict fse, struct narrator * restrict N)
  __attribute__((nonnull));

void varset_stats_build(varset * restrict, bool reset, struct bacon_writer * restrict)
  __attribute__((nonnull));

void varset_file_stats_build(struct fsent * restrict, bool reset, struct bacon_writer * restrict)
  __attribute__((nonnull));
