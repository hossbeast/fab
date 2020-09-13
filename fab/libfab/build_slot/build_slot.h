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

#ifndef _FAB_BUILD_SLOT_H
#define _FAB_BUILD_SLOT_H

#include <sys/types.h>
#include <stdint.h>

#include "descriptor.h"

extern descriptor_type descriptor_fab_build_slot;

typedef struct yarn {
  uint16_t len[0];
  const char s[0];
} yarn;

typedef struct fab_build_slot {
  int32_t pid;            // process id
  int32_t status;         // process exit status
  uint16_t stage;         // slots in the same stage can execute concurrently

  /* exec params */
  const char *path;       // absolute path of file to execute
  uint16_t path_len;

  const char *pwd;        // working directory
  uint16_t pwd_len;

  const yarn *args;       // list of command args
  uint16_t args_len;

//  const char **args;      // command args
//  uint16_t args_len;
//  uint16_t *args_lens;

  const yarn *envs;       // list of command envs
  uint16_t envs_len;

//  const char **envs;      // command envs
//  uint16_t envs_len;
//  uint16_t *envs_lens;

  /* metadata */
  const yarn *sources;
  uint16_t sources_len;
//  const char **sources;
//  uint16_t *sources_lens;
//  uint16_t sources_len;

  const yarn *targets;
  uint16_t targets_len;
//  const char **targets;
//  uint16_t *targets_lens;
//  uint16_t targets_len;

  /* results */
  uint32_t stdout_total;
  uint32_t stderr_total;
  uint32_t auxout_total;
} fab_build_slot;

#endif
