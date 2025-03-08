/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/* A formula is a program which can be run to update the targets of a dependency edge. */

#include "types.h"

#include "moria.h"
#include "valyria/llist.h"

#include "graph.h"
#include "vertex.h"
#include "descriptor.h"
#include "stats.h"
#include "types.h"

struct bacon_writer;
struct channel;
struct exec_value;
struct fsent;
struct moria_graph;
struct rule_module_association;
struct rule_run_context;
struct graph_disintegrate_context;
struct identity_ctx;
struct lookup_pattern;

extern llist formula_list;

/* BAM_NODE_TYPE_FORMULA_FILE - cc.bam file - referenced as formula from a rule */
typedef union stats_formula {
  struct {
    /* event counters */
    uint16_t parse_ok;        /* parsed successfully */
    uint16_t parse_err;       /* parse failed */
    uint32_t fml_exec_ok;     /* formula executed successfully */
    uint32_t fml_exec_err;    /* formula execution failed */
  };

  stats_record record;
} stats_formula;

extern descriptor_type descriptor_stats_formula;

/* VERTEX_TYPE_FML */
typedef struct formula {
  moria_vertex vertex;
  llist unref_lln;

  /* runtime state */
  char id[40];
  uint16_t id_len;

  stats_vertex stats;
  stats_formula formula_stats;
  int fd;

  /* regarding the formula node itself */
  char abspath[PATH_MAX];
  uint16_t abspath_len;
  struct fsent *self_node;    // EDGE_FSENT_FML

  /* parsed from the embedded bacon block */
  struct bacon_value *file;   // primitive
  struct bacon_value *envs;   // set of mappings
  struct bacon_value *args;   // list
} formula;

void formula_create(formula ** restrict fml, struct moria_graph * restrict g)
  __attribute__((nonnull));

void formula_reparse(formula * restrict fml)
  __attribute__((nonnull));

void formula_setup(void);
void formula_cleanup(void);

void formula_unlink(formula * restrict fml, struct graph_disintegrate_context * restrict ctx)
  __attribute__((nonnull));

void formula_release(formula * restrict fml)
  __attribute__((nonnull));

/*
 * Write a formulas stats to a buffer
 *
 * reset - true to reset the stats while reading them
 */
void formula_stats_collate(formula * restrict fml, bool reset)
  __attribute__((nonnull));

void formula_describe(const formula * restrict formula, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void formula_file_describe(const struct fsent * restrict fse, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

size_t formula_path_cat(void * restrict dst, size_t sz, const struct formula * restrict formula)
  __attribute__((nonnull));

void formula_identity_describe(const struct formula * restrict formula, struct bacon_writer * restrict)
  __attribute__((nonnull));

void formula_stats_build(formula * restrict, bool reset, struct bacon_writer * restrict)
  __attribute__((nonnull));

void formula_file_stats_build(struct fsent * restrict, bool reset, struct bacon_writer * restrict)
  __attribute__((nonnull));

void formula_bacon_describe(const formula * restrict formula, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

formula *formula_resolve(struct lookup_pattern * restrict ref, struct rule_run_context * restrict ctx)
  __attribute__((nonnull));

void formula_rma_unref(formula * restrict fml)
  __attribute__((nonnull));

void formulas_show(void);

bool formula_reap(formula * restrict fml, struct graph_disintegrate_context * restrict ctx)
  __attribute__((nonnull));

void formula_hash(const formula * restrict v, const void ** restrict hp, uint16_t * restrict hl)
  __attribute__((nonnull));

void formula_id(const formula * restrict v, const char ** restrict idp, uint16_t * restrict idl)
  __attribute__((nonnull));

void formula_compute_id(const formula * restrict fml, struct identity_ctx * restrict id)
  __attribute__((nonnull));

void formulas_show(void);
