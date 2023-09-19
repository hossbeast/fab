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

#ifndef FABD_EXEC_H
#define FABD_EXEC_H

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

#endif
