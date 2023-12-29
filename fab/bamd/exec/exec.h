/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*

exec - parameters needed to call exec

*/

#include "types.h"

#include "narrator/growing.h"

struct narrator;
struct path_cache_entry;

/* list of strings suitable for passing as the parameters to exec */
typedef struct exec {
  const struct path_cache_entry *file_pe; // file to execute
  char * path;           // null-terminated, pointer into text - absolute path of file to execute

  char ** args;          // sentinel-terminated, pointers into text
  uint16_t args_size;    // number of args

  char ** envs;          // name=value, sentinel-terminated, sorted by name, pointers into text
  uint16_t envs_size;    // number of envs

  char __attribute__((aligned(8))) text[];
} exec;

void exec_free(exec * e);
void exec_ifree(exec ** ep);

/* compare two name=value strings by name only */
int env_def_cmp(const char * restrict a, const char * restrict b)
  __attribute__((nonnull));
