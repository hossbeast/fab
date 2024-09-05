/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include <sys/types.h>
#include <stdint.h>

#include "descriptor.h"

extern descriptor_type descriptor_bam_envs_tell_item;
extern descriptor_type descriptor_bam_envs_tell_result;

typedef struct bam_envs_tell_item {
  const char *id;       /* file id */
  uint16_t id_len;
  const char *abspath;  /* file path */
  uint16_t abspath_len;
} bam_envs_tell_item;

/* results from envs-describe */
typedef struct bam_envs_tell_result {
  const char *module_id;       /* owning module id */
  uint16_t module_id_len;

  const char *module_abspath;  /* owning module directory absolute path */
  uint16_t module_abspath_len;

  /* struct bam_envs_tell_item */
  void *file_list;             /* struct bam_envs_tell_item */
  uint32_t file_list_size;     // size of the file_list blob
  uint16_t file_list_len;      // items in file_list
} bam_envs_tell_result;
