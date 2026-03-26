/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"
#include "moria.h"
#include "valyria/llist.h"
#include "vertex.h"

struct exec;
struct detail;
struct bacon_writer;
struct narrator;
struct fsedge;
struct graph_invalidation_context;
struct graph_disintegrate_context;
struct request_handler;
struct build_record;
struct buildplan_entity;

/*
 * VERTEX_FORMULA_RECORD - record of executing a formula
 */
typedef struct formula_record {
  moria_vertex vertex;

  stats_vertex stats;
  char id[40];
  uint16_t id_len;

  struct build_record *build_rec;
  struct buildplan_entity *bpe;

  /* shadow tree */
  struct fsent *shadow;                //formula-records/id/

  int64_t pid;          /* process id */
  int32_t status;       /* process exit status */
  uint16_t stage;
  uint32_t stage_index;
  uint32_t stage_count;

  /* parameters to the exec */
  struct exec *exec;

  struct timespec forked_time;  /* time when forked, monotonic clock */
  struct timespec waited_time;  /* time when waited, monotonic clock */

  /* stdout ; circular buffer ; trailing portion is retained for visibility */
  uint32_t stdout_tail;
  uint32_t stdout_head;
  uint32_t stdout_total;
  /* stderr ; circular buffer ; trailing portion is retained for visibility */
  uint32_t stderr_tail;
  uint32_t stderr_head;
  uint32_t stderr_total;
  /* auxout ; circular buffer ; trailing portion is retained for visibility */
  uint32_t auxout_tail;
  uint32_t auxout_head;
  uint32_t auxout_total;

  /* from here to the end -- retained across reuse */
  union {
    char *stdout_buf;
    char _retain;
  };
  uint32_t stdout_size;
  char *stderr_buf;
  uint32_t stderr_size;
  char *auxout_buf;
  uint32_t auxout_size;
} formula_record;

extern llist formula_record_list;

formula_record *formula_record_alloc(struct build_record * restrict build_rec, struct buildplan_entity * restrict bpe)
  __attribute__((nonnull));

void formula_record_setup(void);
void formula_record_cleanup(void);

void formula_record_describe(const formula_record * restrict rec, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void formula_record_identity_describe(const formula_record * restrict bpe, struct bacon_writer * restrict)
  __attribute__((nonnull));

size_t formula_record_path_cat(void * restrict dst, size_t sz, const formula_record * restrict bpe)
  __attribute__((nonnull));

void formula_record_stats_build(formula_record * restrict, bool reset, struct bacon_writer * restrict)
  __attribute__((nonnull));

void formula_record_release(formula_record * restrict rec)
  __attribute__((nonnull));

void formula_record_unlink(formula_record * restrict rec, struct graph_disintegrate_context * restrict ctx)
  __attribute__((nonnull));

void formula_record_detail_tell(const struct detail * restrict detail, struct request_handler * restrict handler)
  __attribute__((nonnull));

void formula_record_hash(const formula_record * restrict v, const void ** restrict hp, uint16_t * restrict hl)
  __attribute__((nonnull));

void formula_record_id(const formula_record * restrict v, const char ** restrict idp, uint16_t * restrict idl)
  __attribute__((nonnull));

void formula_record_compute_id(const formula_record * restrict formula_record, struct identity_ctx * restrict id)
  __attribute__((nonnull));

void formula_records_show(void);
