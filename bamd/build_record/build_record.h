/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"
#include "moria.h"
#include "valyria/llist.h"
#include "vertex.h"

struct exec;
struct bacon_writer;
struct narrator;
struct fsedge;
struct graph_invalidation_context;
struct graph_disintegrate_context;

/*
 * VERTEX_BUILD_RECORD - record of a build attempt
 */
typedef struct build_record {
  moria_vertex vertex;
  llist order_lln;          // in byorder_list

  stats_vertex stats;
  char id[40];
  uint16_t id_len;
  uint32_t counter;

  char order[8];
  uint16_t order_len;

  /* info about the build this record was part of */
  bool autorun;
  int64_t pid;
  uint64_t token;
  char label[128];
  uint16_t label_len;
  struct timespec start_time;   // start event
  struct timespec end_time;     // end event
//  uint32_t slots;
//  uint32_t stages;

  uint32_t slot_pass;
  uint32_t slot_fail;
  uint32_t slot_prep_fail;
} build_record;

extern llist build_record_list;

build_record *build_record_alloc(void)
  __attribute__((nonnull));

void build_record_setup(void);
void build_record_cleanup(void);

void build_record_describe(const build_record * restrict rec, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void build_record_identity_describe(const build_record * restrict bpe, struct bacon_writer * restrict)
  __attribute__((nonnull));

size_t build_record_path_cat(void * restrict dst, size_t sz, const build_record * restrict bpe)
  __attribute__((nonnull));

void build_record_stats_build(build_record * restrict, bool reset, struct bacon_writer * restrict)
  __attribute__((nonnull));

void build_record_add(build_record * restrict rec)
  __attribute__((nonnull));

void build_record_release(build_record *rec)
  __attribute__((nonnull));

void build_record_unlink(build_record * restrict rec, struct graph_disintegrate_context * restrict ctx)
  __attribute__((nonnull));

void build_record_hash(const build_record * restrict v, const void ** restrict hp, uint16_t * restrict hl)
  __attribute__((nonnull));

void build_record_id(const build_record * restrict v, const char ** restrict idp, uint16_t * restrict idl)
  __attribute__((nonnull));

void build_record_compute_id(const build_record * restrict build_record, struct identity_ctx * restrict id)
  __attribute__((nonnull));

void build_records_show(void);
