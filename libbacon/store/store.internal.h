/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "store.h"

struct list;    // valyria/list
struct value_parser;
struct value;

typedef struct value_store {
  struct list * values;
  struct list * strings;
  struct list * lists;
  struct list * sets;
} value_store;

/* create a value_store */
void value_store_create(value_store ** const restrict store)
  __attribute__((nonnull));

/* free a value_store */
void value_store_xfree(value_store * const restrict store);

/* free a value_store, zero its reference */
void value_store_ixfree(value_store ** const store)
  __attribute__((nonnull));

void value_store_recycle(value_store * const restrict store)
  __attribute__((nonnull));

void store_string(struct value_parser * const restrict parser, struct value ** rv)
  __attribute__((nonnull));

void store_float(struct value_parser * const restrict parser, struct value ** rv)
  __attribute__((nonnull));

void store_boolean(struct value_parser * const restrict parser, struct value ** rv)
  __attribute__((nonnull));

void store_posint(struct value_parser * const restrict parser, struct value ** rv)
  __attribute__((nonnull));

void store_negint(struct value_parser * const restrict parser, struct value ** rv)
  __attribute__((nonnull));

void store_variable(struct value_parser * const restrict parser, struct value ** rv)
  __attribute__((nonnull));

void store_mapping(struct value_parser * const restrict parser, struct value ** rv)
  __attribute__((nonnull));

void store_list(struct value_parser * const restrict parser, struct value ** rv)
  __attribute__((nonnull));

void store_set(struct value_parser * const restrict parser, struct value ** rv)
  __attribute__((nonnull));
