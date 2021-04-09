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

#ifndef EXEC_BUILDER_H
#define EXEC_BUILDER_H

/*
exec_builder - build up the parameters to an exec call, namely
 * file to execute
 * arguments to the program (argv)
 * program environment (envp)
*/

#include "types.h"
#include "xapi.h"

#include "narrator/growing.h"
#include "fsent.h"

struct exec;
struct formula_value;
struct fsent;
struct narrator;
struct path_cache_entry;
struct value;

/* intermediate state for building an exec */
typedef struct exec_builder {
  struct exec * exec;       // points into Nexec

  /* narrator into which the exec strings are accumulated */
  struct narrator_growing Nexec_growing;
  struct narrator * Nexec;

  const struct path_cache_entry *file_pe;
  uint32_t file;            // offset into the backing buffer, 0 = uninitialized
  uint32_t *args;           // offsets into the backing buffer
  uint16_t args_len;
  size_t args_alloc;
  uint32_t *envs;           // offsets into the backing buffer
  uint16_t envs_len;
  size_t envs_alloc;

  /* allocations state */
  char * file_stor;
  char ** args_stor;
  size_t args_stor_alloc;
  char ** envs_stor;
  size_t envs_stor_alloc;
} exec_builder;

/*
 * build an exec from the builder
 *
 * [envp] - (returns) exec stored in the builder
 */
xapi exec_builder_build(exec_builder * restrict builder, struct exec ** restrict envp)
  __attribute__((nonnull(1)));

/*
 * claim the built exec stored in the builder
 *
 * envp - (returns) exec stored in the builder - its now owned by the caller
 */
void exec_builder_take(exec_builder * restrict builder, struct exec ** restrict envp)
  __attribute__((nonnull));

/* initialize an exec builder */
xapi exec_builder_xinit(exec_builder * restrict builder)
  __attribute__((nonnull));

/* reset an exec builder */
xapi exec_builder_xreset(exec_builder * restrict builder)
  __attribute__((nonnull));

/* destroy an exec builder */
xapi exec_builder_xdestroy(exec_builder * restrict builder)
  __attribute__((nonnull));

enum builder_add_mode {
  BUILDER_PREPEND = 1,    /* if the item already exists, prepend to it */
  BUILDER_APPEND          /* if the item already exists, append to it */
};

enum builder_add_item {
  BUILDER_FILE = 1,       /* the file to execute */
  BUILDER_ARGS,           /* the arguments list */
  BUILDER_ENVS            /* the environment name=value set */
};

enum builder_render_function {
    BUILDER_STRING = 1
  , BUILDER_FORMULA_VALUE
  , BUILDER_VALUE
  , BUILDER_PATH_CACHE_ENTRY
  , BUILDER_PROPERTY
  , BUILDER_FMT
};

union builder_render_function_args {
  const char *s;
  const struct formula_value * f;
  const struct value * v;
  const struct path_cache_entry *pe;
  struct {
    const struct fsent * n;
    fsent_property prop;
    fsent_property_context pctx;
  };
  struct {
    const char *fmt;
    va_list *va;
  };
};

typedef struct exec_builder_args {
  enum builder_add_item item;
  enum builder_add_mode mode;

  /* args / envs */
  int32_t position;   // for argv/envp, index of the item in the collection to modify, -1 to add to the collection
  const char * name;  // for a new envp, its name
  uint16_t name_len;

  /* value */
  enum builder_render_function render_val;
  union builder_render_function_args val;

  /* separator, in case of append/prepend */
  enum builder_render_function render_sep;
  union builder_render_function_args sep;
} exec_builder_args;

/* add an item to an exec builder */
xapi exec_builder_add(exec_builder * restrict builder, const exec_builder_args *args)
  __attribute__((nonnull));

/*
 * add an envp to an exec builder
 *
 * name_fmt - format string for the envp name
 * val_fmt  - format string for the envp value
 */
xapi exec_builder_env_addf(exec_builder * restrict builder, const char * restrict name_fmt, const char * restrict val_fmt, ...)
  __attribute__((nonnull))
  __attribute__((format(printf, 3, 4)));

#endif
