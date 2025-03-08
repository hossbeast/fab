/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*

exec - parameters needed to call exec

*/

#include "types.h"

#include "narrator/growing.h"

struct narrator;
struct fsent;
struct bacon_writer;

/* list of strings suitable for passing as the parameters to exec */
typedef struct exec {
  char *cwd;
  uint16_t cwd_len;

  char *file_path;
  uint16_t file_path_len;

  char **args;           // sentinel-terminated, pointers into text
  uint16_t args_len;     // number of args

  char **envs;           // name=value, sentinel-terminated, sorted by name, pointers into text
  uint16_t envs_len;     // number of envs

  char __attribute__((aligned(8))) text[];
} exec;

void exec_free(exec * e);
void exec_ifree(exec ** ep);

/* compare two name=value strings by name only */
int env_def_cmp(const char * restrict a, const char * restrict b)
  __attribute__((nonnull));

void exec_show(const struct exec * restrict exec)
  __attribute__((nonnull));

void exec_describe(const struct exec * restrict exec, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));
