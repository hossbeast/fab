/* Copyright (c) 2012-2019 Todd Freed <todd.freed@gmail.com>

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

#ifndef EXEC_BUILDER_INTERNAL_H
#define EXEC_BUILDER_INTERNAL_H

/*

SUMMARY

*/

#include "types.h"
#include "xapi.h"

#include "exec_builder.h"
#include "node.h"

struct node;
struct narrator;
struct formula_value;
struct value;
struct buildplan_entity;
struct path_cache_entry;

xapi exec_builder_arg_add(exec_builder * restrict builder, xapi (*render_val)(const void * ud, struct narrator * restrict N), const void * ud)
  __attribute__((nonnull));

xapi exec_builder_arg_addf(exec_builder * restrict builder, const char * restrict val_fmt, ...)
  __attribute__((nonnull))
  __attribute__((format(printf, 2, 3)));

xapi exec_builder_env_add(exec_builder * restrict builder, const char * restrict name_fmt, xapi (*render_val)(const void * ud, struct narrator * restrict N), const void * ud, ...)
  __attribute__((nonnull))
  __attribute__((format(printf, 2, 5)));

xapi exec_builder_env_addf(exec_builder * restrict builder, const char * restrict name_fmt, const char * restrict val_fmt, ...)
  __attribute__((nonnull))
  __attribute__((format(printf, 3, 4)));

enum builder_add_mode {
  BUILDER_PREPEND = 1,
  BUILDER_APPEND
};

enum builder_add_item {
  BUILDER_FILE = 1,
  BUILDER_ARGS,
  BUILDER_ENVS
};

enum builder_render_function {
    RENDER_PROPERTY = 1
  , RENDER_PATH_CACHE_ENTRY
  , RENDER_STRING
  , RENDER_FORMULA_VALUE
  , RENDER_VALUE
  , RENDER_FMT
};

typedef struct builder_add_args {
  enum builder_add_mode mode;
  enum builder_add_item item;

  /* args / envs */
  int32_t position;
  const char * name;
  uint16_t name_len;

  /* value */
  enum builder_render_function render_val;
  union builder_render_function_args {
    const char * s;
    const struct formula_value * f;
    const struct value * v;
    const struct path_cache_entry *pe;
    struct {
      const struct node * n;
      node_property prop;
      node_property_context pctx;
    };
    struct {
      const char * fmt;
      va_list *va;
    };
  } val;

  /* separator */
  enum builder_render_function render_sep;
  union builder_render_function_args sep;
} builder_add_args;

xapi builder_add(exec_builder * restrict builder, const builder_add_args *args)
  __attribute__((nonnull));

xapi builder_add_args_say(const exec_builder * restrict builder, const builder_add_args * restrict args, narrator * restrict N)
  __attribute__((nonnull));

#endif
