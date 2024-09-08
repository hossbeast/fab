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

struct fsent;
struct formula_value;
struct moria_graph;
struct channel;
struct rule_module_association;
struct bacon_writer;

extern llist formula_list;

/* BAM_NODE_TYPE_FORMULA_FILE - cc.bam file - referenced as formula from a rule */
typedef union stats_formula {
  struct {
    /* event counters */
    uint16_t parse;           /* parse attempted */
    uint16_t parse_ok;        /* parsed successfully */
    uint32_t fml_exec;        /* formula execution attempted */
    uint32_t fml_exec_ok;     /* formula executed successfully */
  };

  stats_record record;
} stats_formula;

extern descriptor_type descriptor_stats_formula;

/* VERTEX_TYPE_FML */
typedef struct formula {
  moria_vertex vertex;

  /* runtime state */
  char id[40];
  uint16_t id_len;

  stats_vertex stats;
  stats_formula formula_stats;
  int fd;
  llist invalid_lln;

  /* regarding the formula node itself */
  char abspath[PATH_MAX];
  uint16_t abspath_len;
  struct fsent * self_node;

  /* shadow link */
  struct fsedge *link;

  /* parsed from the embedded bacon block */
  struct formula_value *file;   // primitive
  struct formula_value *envs;   // set of mappings
  struct formula_value *args;   // list
} formula;

void formula_create(formula ** restrict fml, struct moria_graph * restrict g)
  __attribute__((nonnull));

void formula_reparse(formula * restrict fml)
  __attribute__((nonnull));

void formula_system_reconcile(struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

void formula_setup(void);
void formula_cleanup(void);

void formula_release(formula * restrict fml, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

/*
 * Write a formulas stats to a buffer
 *
 * reset - true to reset the stats while reading them
 */
void formula_stats_collate(formula * restrict fml, bool reset)
  __attribute__((nonnull));

void formula_invalidated(formula * restrict fml)
  __attribute__((nonnull));

void formula_describe(const formula * restrict formula, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void formula_file_describe(const struct fsent * restrict fse, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

size_t formula_path_cat(void * restrict dst, size_t sz, const struct formula * restrict formula)
  __attribute__((nonnull));

void formula_identity_say(const struct formula * restrict formula, struct narrator * restrict N)
  __attribute__((nonnull));

void formula_file_identity_say(const struct fsent * restrict fse, struct narrator * restrict N)
  __attribute__((nonnull));

void formula_stats_build(formula * restrict, bool reset, struct bacon_writer * restrict)
  __attribute__((nonnull));

void formula_file_stats_build(struct fsent * restrict, bool reset, struct bacon_writer * restrict)
  __attribute__((nonnull));
