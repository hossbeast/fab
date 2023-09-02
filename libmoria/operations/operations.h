/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"
#include "parser.h"
#include "valyria/llist.h"

struct attrs32;
struct moria_edge;
struct moria_graph;
struct moria_operations_dispatch;
struct moria_parser;
struct moria_vertex;

/* label in an identifier */
typedef struct label {
  llist lln;                  // in identifier->labels
  uint32_t attrs;
  uint8_t opattrs;
  const char *s;
  uint16_t len;
} label;

/* identifies a vertex by a sequence of labels in the identity tree */
typedef struct identifier {
  llist lln;                  // in identifier_list->identifiers
  llist labels;
} identifier;

/* list of identifiers */
typedef struct identifier_list {
  llist identifiers;          // identifier
} identifier_list;

// label
label *label_alloc(const char *, uint16_t len)
  __attribute__((nonnull));

void label_free(label * restrict label)
  __attribute__((nonnull));

// identifier
identifier *identifier_alloc(struct label * restrict label, uint8_t opattrs)
  __attribute__((nonnull));

void identifier_free(identifier *);

// identifier list
identifier_list *identifier_list_alloc(void);
void identifier_list_free(identifier_list *list);

/* create some vertices */
void operations_declare(struct moria_parser * restrict parser, identifier_list *A)
  __attribute__((nonnull));

/* perform the refresh operation on some vertices */
void operations_refresh(struct moria_parser * restrict parser, identifier_list * restrict A)
  __attribute__((nonnull));

/* perform the invalidate operations on some vertices */
void operations_invalidate(struct moria_parser * restrict parser, identifier_list * restrict A)
  __attribute__((nonnull));

/* create an edge */
void operations_connect(
    struct moria_parser * restrict parser
  , identifier_list * restrict B
  , identifier_list * restrict A
  , uint32_t attrs
);

/* remove an edge */
void operations_disconnect(
    struct moria_parser * restrict parser
  , identifier_list * restrict B
  , identifier_list * restrict A
);

llist *parser_llist_alloc(struct moria_parser * restrict parser)
  __attribute__((nonnull));
