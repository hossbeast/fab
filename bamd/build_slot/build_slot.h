/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include <signal.h>

#include "types.h"
#include "bacon/parser.h"
#include "valyria/llist.h"

#include "exec_builder.h"
#include "exec_render.h"
#include "adaptor.h"

struct dependency;
struct module;
struct bacon_value;
struct bacon_parser;
struct formula_record;

typedef struct build_slot {
  llist lln;

  /* (not null) recording of executing this slot */
  struct formula_record *rec;

  /* (not null) dependency edge with formula/sources/targets */
  /* this edge has 1+ targets, 0+ sources, and one EDGE_FML_BPE */
  struct buildplan_entity *bpe;

  int stdout_pipe[2];
  adaptor stdout_adaptor;

  int stderr_pipe[2];
  adaptor stderr_adaptor;

  int auxout_pipe[2];
  adaptor auxout_adaptor;

  //
  // storage
  //

  bacon_parser sysvar_parser;
  struct exec_builder exec_builder;
  struct exec_render_context exec_builder_context;
} build_slot;

/*
 * stream - stream to read by fd number, 1=stdout, 2=stderr, 1001=auxout
 */
size_t build_slot_read(struct build_slot * restrict bs, uint32_t stream)
  __attribute__((nonnull));

void build_slot_reap(struct build_slot * restrict bs)
  __attribute__((nonnull));

void build_slot_fork_and_exec(struct build_slot * restrict bs)
  __attribute__((nonnull));


  /* or absolute path of file to execute */
  //const char *file_path;

  //char **argv;     // sentinel-terminated args
  //char **envp;     // sentinel-terminated environment

  //// argv list
  //char **argv_stor;
  //size_t argv_stor_a;

  //// env list
  //char **env_stor;
  //size_t env_stor_a;

  //const char *cwd;
  //uint16_t cwd_len;

  //int64_t pid;
  //int32_t status;                 // process exit status
  //uint16_t stage;
  //uint32_t stage_index;
  //struct timespec forked_time;  // time when forked, monotonic clock
  //struct timespec waited_time;  // time when waited, monotonic clock
  //uint32_t stdout_total;
  //uint32_t stderr_total;
  //uint32_t auxout_total;
  //struct bacon_value *sysvars;
