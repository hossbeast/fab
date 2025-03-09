/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "moria.h"
#include "vertex.h"

struct plugin;
struct module;
struct bacon_writer;
struct request_handler;
struct graph_disintegrate_context;

/*
VERTEX_PLUGIN_STDIN
VERTEX_PLUGIN_STDOUT
VERTEX_PLUGIN_STDERR
VERTEX_PLUGIN_AUXOUT
VERTEX_FORMULA_STDIN
VERTEX_FORMULA_STDOUT
VERTEX_FORMULA_STDERR
VERTEX_FORMULA_AUXOUT
VERTEX_FORMULA_ARGS
VERTEX_FORMULA_ENVS
VERTEX_FORMULA_FILE
VERTEX_FORMULA_CWD
*/
typedef struct detail {
  moria_vertex vertex;

  stats_vertex stats;
  uint64_t hash;
  char id[40];
  uint16_t id_len;

  struct moria_vertex *subject;
} detail;

extern llist detail_list;

detail *detail_alloc(struct moria_vertex *v, vertex_type type)
  __attribute__((nonnull));

size_t detail_path_cat(void * restrict dst, size_t sz, const detail * restrict envs)
  __attribute__((nonnull));

void detail_describe(const detail * restrict envs, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void detail_tell(const detail * restrict envs, struct request_handler * restrict handler)
  __attribute__((nonnull));

void detail_cleanup(void);

bool detail_reap(struct detail * restrict fsn, struct graph_disintegrate_context * restrict ctx);
void detail_release(struct detail * restrict fsn)
  __attribute__((nonnull));

void detail_hash(const detail * restrict v, const void ** restrict hp, uint16_t * restrict hl)
  __attribute__((nonnull));

void detail_id(const detail * restrict v, const char ** restrict idp, uint16_t * restrict idl)
  __attribute__((nonnull));

void detail_compute_id(const detail * restrict v, struct identity_ctx * restrict id)
  __attribute__((nonnull));

void detail_show(void);
