/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

#include "valyria/llist.h"
#include "valyria/hashtable.h"
#include "moria.h"

#include "graph.h"

struct map;
struct fsent;
struct bacon_value;
struct graph_invalidation_context;
struct fsedge;
struct rules_parser;
struct module;
struct narrator;
struct rules;
struct moria_vertex;
enum edge_type;
struct entlist;

extern llist module_edge_list; // active list

/*
 * edge connecting two module vertices
 *  EDGE_RULES_MOD
 *  EDGE_REQUIRES
 *
 * edge connecting a module vertex A -> a dirnode vertex B
 *  EDGE_IMPORTS
 */
typedef struct module_edge {
  struct moria_edge edge;
  uint16_t reconciliation_epoch;

  /* related shadow edge, e.g. //module/requires, //module/uses */
  struct fsedge *related;
} module_edge;

void module_module_cleanup(void);

void module_edge_disconnecting(
    module_edge * restrict me
  , struct entlist * restrict edges
  , struct entlist * restrict vertices
  , struct graph_invalidation_context * restrict invalidation
)
  __attribute__((nonnull));

bool module_edge_connect(struct module * restrict A, struct moria_vertex * restrict B, enum edge_type type, struct module_edge ** restrict mmep)
  __attribute__((nonnull));

void module_edge_release(module_edge * restrict me)
  __attribute__((nonnull));
