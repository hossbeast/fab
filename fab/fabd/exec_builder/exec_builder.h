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

SUMMARY

*/

#include "types.h"
#include "xapi.h"

#include "narrator.h"
#include "selector.h"

struct buildplan_entity;
struct module;
struct narrator;
struct value;
struct formula_value;

/* set of key/value pairs, suitable for passing to exec as envp */
typedef struct exec {
  char * path;      // null-terminated, pointer into text
  char * filename;  // pointer into path
  char ** args;     // sentinel-terminated, pointer into text
  char ** envs;     // name=value, sentinel-terminated, sorted by name, pointers into text
  struct {
    uint16_t args_size;    // number of args
    uint16_t envs_size;    // number of envs
  } __attribute__((aligned(8)));
  char text[];
} exec;

typedef struct exec_builder {
  exec * exec;    // points into Nexec
  size_t exec_size;

  /* intermediate state for builder_build */
  struct narrator * Nexec;
  char Nexec_stor[NARRATOR_STATIC_SIZE];

  uint32_t path;            // offset into the backing buffer, 0 = uninitialized
  uint32_t *args;           // offsets into the buffer backing Nexec
  uint16_t args_len;
  size_t args_alloc;
  uint32_t *envs;           // offsets into the buffer backing Nexec
  uint16_t envs_len;
  size_t envs_alloc;

  /* allocations state */
  char * path_stor;
  char ** args_stor;
  size_t args_stor_alloc;
  char ** envs_stor;
  size_t envs_stor_alloc;
} exec_builder;

void exec_free(exec * e);
void exec_ifree(exec ** ep);

int env_def_cmp(const char * restrict a, const char * restrict b)
  __attribute__((nonnull));

xapi exec_builder_build(exec_builder * restrict builder, exec ** restrict envp)
  __attribute__((nonnull(1)));

void exec_builder_take(exec_builder * restrict builder, exec ** restrict envp)
  __attribute__((nonnull));

xapi exec_builder_xinit(exec_builder * restrict builder)
  __attribute__((nonnull));

xapi exec_builder_xreset(exec_builder * restrict builder)
  __attribute__((nonnull));

xapi exec_builder_xdestroy(exec_builder * restrict builder)
  __attribute__((nonnull));

#endif
