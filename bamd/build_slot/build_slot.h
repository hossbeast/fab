/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include <signal.h>

#include "types.h"
#include "bacon/parser.h"

#include "exec_builder.h"
#include "exec_render.h"

struct dependency;
struct module;
struct path_cache_entry;
struct bacon_value;
struct bacon_parser;

typedef struct build_slot {
  int32_t pid;
  int32_t status;                 // process exit status
  uint16_t stage;
  uint32_t stage_index;
  struct timespec forked_time;  // time when forked, monotonic clock
  struct timespec waited_time;  // time when waited, monotonic clock

  /* (not null) dependency edge with formula/sources/targets */
  /* this edge has 1+ targets, 0+ sources, and one EDGE_FML_BPE */
  struct buildplan_entity *bpe;

  const char *cwd;
  uint16_t cwd_len;

  struct bacon_value *sysvars;

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

  bacon_parser sysvar_parser;
  struct exec_builder exec_builder;
  struct exec_render_context exec_builder_context;
} build_slot;

/*
 * stream - stream to read by fd number, 1=stdout, 2=stderr, 1001=auxout
 */
void build_slot_read(struct build_slot * restrict bs, uint32_t stream)
  __attribute__((nonnull));

void build_slot_reap(struct build_slot * restrict bs)
  __attribute__((nonnull));

void build_slot_fork_and_exec(struct build_slot * restrict bs, struct buildplan_entity * restrict bpe, uint32_t stage_index)
  __attribute__((nonnull));
