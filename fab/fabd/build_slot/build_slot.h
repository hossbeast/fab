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

#ifndef _BUILD_SLOT_H
#define _BUILD_SLOT_H

#include <signal.h>

#include "xapi.h"
#include "types.h"

#include "exec_builder.h"
#include "exec_render.h"

struct dependency;
struct module;
struct path_cache_entry;
struct value;
struct variant;

typedef struct build_slot {
  int32_t pid;
  int32_t status;                 // process exit status
  uint16_t stage;
  uint32_t stage_index;

  /* (not null) dependency edge with formula/sources/targets */
  /* this edge has 1+ targets and 0+ sources */
  struct dependency * bpe;

  /* (not null) module the target nodes belong to, if all targets belong to the same module
   * otherwise the project module */
  struct module * mod;

  /* variant of the target nodes, if all targets are in the same variant */
  const struct variant * var;

  const struct value * vars;      // vars for the variant
  const struct exec * exec;       // pre-rendered env vars for the variant

  int file_fd;                      // descriptor of file to execute
  const char * file_path;           // or absolute path of file to execute
  const struct path_cache_entry *file_pe;  // path cache entry, if any

  char ** argv;     // sentinel-terminated args
  char ** envp;     // sentinel-terminated environment

  int stdout_pipe[2];
  uint32_t stdout_total;

  int stderr_pipe[2];
  uint32_t stderr_total;

  int auxout_pipe[2];
  uint32_t auxout_total;

  //
  // storage
  //

  // argv list
  char ** argv_stor;
  size_t argv_stor_a;

  // env list
  char ** env_stor;
  size_t env_stor_a;

  exec_builder exec_builder;
  exec_render_context exec_builder_context;
} build_slot;

/*
 * stream - stream to read by fd number, 1=stdout, 2=stderr, 1001=auxout
 */
xapi build_slot_read(struct build_slot * restrict bs, uint32_t stream)
  __attribute__((nonnull));

xapi build_slot_reap(struct build_slot * restrict bs, siginfo_t * info)
  __attribute__((nonnull));

xapi build_slot_prep(struct build_slot * restrict bs, struct dependency * restrict bpe, uint32_t stage_index)
  __attribute__((nonnull));

xapi build_slot_fork_and_exec(struct build_slot * restrict bs)
  __attribute__((nonnull));

#endif
