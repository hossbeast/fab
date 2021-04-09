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

extern descriptor_type descriptor_fab_build_string;
extern descriptor_type descriptor_fab_build_slot_results;
extern descriptor_type descriptor_fab_build_slot_info;

/* FABIPC_EVENT_FORMULA_EXEC_FORKED */
typedef struct fab_build_slot_info {
  int32_t pid;            // process id
  uint16_t stage;         // slots in the same stage can execute concurrently

  /* exec params */
  char *path;             // absolute path of file to execute
  uint16_t path_len;

  char *pwd;              // working directory
  uint16_t pwd_len;

  /* struct fab_build_string */
  void *arg_list;
  uint32_t arg_list_size;
  uint16_t arg_list_len;

  /* struct fab_build_string */
  void *env_list;
  uint32_t env_list_size;
  uint16_t env_list_len;

  /* struct fab_build_string */
  void *source_list;
  uint32_t source_list_size;
  uint16_t source_list_len;

  /* struct fab_build_string */
  void *target_list;
  uint32_t target_list_size;
  uint16_t target_list_len;
} fab_build_slot_info;

/* fab_build_slot_info strings */
typedef struct fab_build_string {
  uint16_t text_len;
  const char *text;
} fab_build_string;

/* FABIPC_EVENT_FORMULA_EXEC_WAITED */
typedef struct fab_build_slot_results {
  int32_t status;         // process exit status
  uint32_t stdout_total;
  uint32_t stderr_total;
  uint32_t auxout_total;
} fab_build_slot_results;

#endif
