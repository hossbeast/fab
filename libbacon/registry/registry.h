/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "valyria/rbtree.h"

struct bacon_parser;
struct bacon_writer;
struct bacon_value;

/* bacon object */
typedef struct bacon_object {
  rbnode rbn;

  /* unique integer not in the enum bacon_type */
  uint32_t type;

  /* called from bacon_value_free to destroy an object before it is reclaimed
   *
   * parser - the parser
   * v      - the value
   */
  void (*destroy)(struct bacon_parser * restrict parser, struct bacon_value * restrict v)
    __attribute__((nonnull));

  /* called from bacon_value_free to reclaim an object after it has been destroyed
   *
   * parser - the parser
   * v      - the value
   */
  void (*reclaim)(struct bacon_value * restrict v) __attribute__((nonnull));

  /* called from bacon_value_cmp to compare two values. A is of the type declared by this
   * object, B can be a value of any type
   *
   * A - an object of the type declared by this bacon_object
   * B - a bacon_object of any type
   *
   * returns an integer less than, equal to, or greater than zero indicating the relationship between A and B
   */
  int (*cmp)(const struct bacon_value * restrict A, const struct bacon_value * restrict B);

  /* called from bacon_value_write to describe an object of this type
   *
   * writer - writer
   * N      - the value
   */
  void (*write)(struct bacon_writer * restrict writer, const struct bacon_value * restrict v)
    __attribute__((nonnull));
} bacon_object;

/* register a new type of bacon value */
void bacon_registry_register(struct bacon_object * restrict object)
  __attribute__((nonnull));
