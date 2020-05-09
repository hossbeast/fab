/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

   This file is part of fab.

   fab is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   fab is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with fab.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef _FABD_SELECTOR_H
#define _FABD_SELECTOR_H

#include "xapi.h"
#include "types.h"

#include "moria/traverse.h"
#include "valyria/llist.h"

#include "graph.h"

struct attrs32;
struct narrator;
struct node;
struct module;
struct pattern;
struct buildplan_entity;
struct value_writer;
struct selection;

#define SELECTOR_TYPE_OPT 0xf
#define SELECTOR_TYPE_TABLE                                              \
  /* aggregates */                                                       \
  SELECTOR_DEF(SELECTOR_SEQUENCE  , "sequence" , SELECTOR_TYPE_OPT, 0x2) \
  SELECTOR_DEF(SELECTOR_UNION     , "union"    , SELECTOR_TYPE_OPT, 0x3) \
  /* operations */                                                       \
  SELECTOR_DEF(SELECTOR_NODESET   , "nodeset"  , SELECTOR_TYPE_OPT, 0x4) \
  SELECTOR_DEF(SELECTOR_PATTERN   , "pattern"  , SELECTOR_TYPE_OPT, 0x5) \
  SELECTOR_DEF(SELECTOR_TRAVERSE  , "traverse" , SELECTOR_TYPE_OPT, 0x6) \

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

struct attrs32 * selector_type_attrs;

typedef enum selector_nodeset {
#define SELECTOR_DEF(x, s, r, y) x = UINT32_C(y),
SELECTOR_NODESET_TABLE
#undef SELECTOR_DEF
} selector_nodeset;

struct attrs32 * selector_nodeset_attrs;

struct selector;
typedef struct selector {
  selector_type type;

  union {
    selector_nodeset nodeset;                     // SELECTOR_NODESET
    struct pattern * pattern;                     // SELECTOR_PATTERN
    struct {                                      // SELECTOR_TRAVERSE
      uint8_t min_distance;
      uint8_t max_distance;
      traversal_direction direction;  // graph traverse direction
      traversal_criteria criteria;    // graph traverse criteria
      bool exhaustive;

      /* filters */
      bool module_only;
      char * extension;
      uint16_t extension_len;
      vertex_filetype filetype;
    };

    llist head;                                   // SELECTOR_AGGREGATE
  };

  llist lln;
} selector;

void selector_free(selector * restrict s);
void selector_ifree(selector ** restrict s)
  __attribute__((nonnull));

xapi selector_alloc(selector_type type, selector ** restrict s)
  __attribute__((nonnull));

typedef struct selector_context {
  /* inputs */
  const struct buildplan_entity * bpe;
  struct node * base;
  const struct module * mod;

  // mutable
  selector * sel;

  // results
  struct selection * selection; // the active selection
} selector_context;

xapi selector_context_xdestroy(selector_context *ctx)
  __attribute__((nonnull));

/// selector_exec
//
// SUMMARY
//  execute a selector against the node graph
//
// PARAMETERS
//  s       - selector
//  target  -
//  module  -
//  context -
//
xapi selector_exec(selector * restrict s, selector_context * restrict ctx)
  __attribute__((nonnull));

xapi selector_say(selector * restrict s, struct narrator * const restrict N)
  __attribute__((nonnull));

xapi selector_writer_write(selector * const restrict sel, struct value_writer * const restrict writer)
  __attribute__((nonnull));

#endif
