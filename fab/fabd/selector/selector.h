/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

#include "moria/traverse.h"
#include "valyria/llist.h"

#include "graph.h"
#include "selection.h"

struct attrs32;
struct channel;
struct dependency;
struct module;
struct narrator;
struct pattern;
struct value_writer;

#define SELECTOR_TRAVERSE_DEFAULT_MODE          MORIA_TRAVERSE_PRE
#define SELECTOR_TRAVERSE_DEFAULT_DIRECTION     MORIA_TRAVERSE_DOWN
#define SELECTOR_TRAVERSE_DEFAULT_CROSS_MODULE  true
#define SELECTOR_TRAVERSE_DEFAULT_CRITERIA (moria_traversal_criteria) {  \
      vertex_travel : VERTEX_TYPE_FSENT                                  \
    , vertex_visit : VERTEX_TYPE_FSENT                                   \
    , edge_travel : EDGE_FSTREE                                          \
    , edge_visit : 0                                                     \
    , min_depth : 1               /* dont visit self */                  \
    , max_depth : UINT16_MAX                                             \
  }

#define SELECTOR_TYPE_OPT 0xf
#define SELECTOR_TYPE_TABLE                                              \
  /* aggregates */                                                       \
  SELECTOR_DEF(SELECTOR_SEQUENCE  , "sequence" , SELECTOR_TYPE_OPT, 0x2) \
  SELECTOR_DEF(SELECTOR_UNION     , "union"    , SELECTOR_TYPE_OPT, 0x3) \
  /* operations */                                                       \
  SELECTOR_DEF(SELECTOR_NODESET   , "nodeset"  , SELECTOR_TYPE_OPT, 0x4) \
  SELECTOR_DEF(SELECTOR_PATTERN   , "pattern"  , SELECTOR_TYPE_OPT, 0x5) \
  SELECTOR_DEF(SELECTOR_PATH      , "path"     , SELECTOR_TYPE_OPT, 0x6) \
  SELECTOR_DEF(SELECTOR_TRAVERSE  , "traverse" , SELECTOR_TYPE_OPT, 0x7) \

#define SELECTOR_NODESET_OPT 0xf
#define SELECTOR_NODESET_TABLE                                                   \
  SELECTOR_DEF(SELECTOR_NODESET_TARGET  , "target"  , SELECTOR_NODESET_OPT, 0x1) \
  SELECTOR_DEF(SELECTOR_NODESET_TARGETS , "targets" , SELECTOR_NODESET_OPT, 0x2) \
  SELECTOR_DEF(SELECTOR_NODESET_SOURCE  , "source"  , SELECTOR_NODESET_OPT, 0x3) \
  SELECTOR_DEF(SELECTOR_NODESET_SOURCES , "sources" , SELECTOR_NODESET_OPT, 0x4) \
  SELECTOR_DEF(SELECTOR_NODESET_MODULE  , "module"  , SELECTOR_NODESET_OPT, 0x5) \
  SELECTOR_DEF(SELECTOR_NODESET_MODULES , "modules" , SELECTOR_NODESET_OPT, 0x6) \
  SELECTOR_DEF(SELECTOR_NODESET_ALL     , "all"     , SELECTOR_NODESET_OPT, 0x7) \
  SELECTOR_DEF(SELECTOR_NODESET_NONE    , "none"    , SELECTOR_NODESET_OPT, 0x8) \

typedef enum selector_type {
#define SELECTOR_DEF(x, s, r, y) x = UINT32_C(y),
SELECTOR_TYPE_TABLE
#undef SELECTOR_DEF
} selector_type;

extern struct attrs32 * selector_type_attrs;

typedef enum selector_nodeset {
#define SELECTOR_DEF(x, s, r, y) x = UINT32_C(y),
SELECTOR_NODESET_TABLE
#undef SELECTOR_DEF
} selector_nodeset;

extern struct attrs32 * selector_nodeset_attrs;

struct selector;
typedef struct selector {
  selector_type type;

  union {
    selector_nodeset nodeset;                     // SELECTOR_NODESET
    struct pattern * pattern;                     // SELECTOR_PATTERN
    struct {
      char * path;                                // SELECTOR_PATH
      uint16_t path_len;
    };
    struct {                                      // SELECTOR_TRAVERSE
      moria_traversal_direction direction;
      moria_traversal_mode mode;
      moria_traversal_criteria criteria;
      bool exhaustive;
      bool cross_module;

      /* filters */
      char * extension;
      uint16_t extension_len;
    };

    llist head;                                   // SELECTOR_AGGREGATE
  };

  llist lln;
} selector;

void selector_free(selector * restrict s);
void selector_ifree(selector ** restrict s)
  __attribute__((nonnull));

void selector_alloc(selector_type type, selector ** restrict s)
  __attribute__((nonnull));

typedef struct selector_context {
  /* inputs */
  struct dependency * bpe;      // set when run in the context of a build slot
  struct module * mod;
  selection_iteration_type iteration_type;

  // mutable
  selector * sel;

  // results
  struct selection * selection; // the active selection
  struct channel * chan;
} selector_context;

void selector_context_xdestroy(selector_context *ctx)
  __attribute__((nonnull));

/*
 * execute a selector against the graph
 */
void selector_exec(selector * restrict s, selector_context * restrict ctx, selection_iteration_type iteration_type)
  __attribute__((nonnull));

void selector_say(selector * restrict s, struct narrator * const restrict N)
  __attribute__((nonnull));

void selector_writer_write(selector * const restrict sel, struct value_writer * const restrict writer)
  __attribute__((nonnull));
