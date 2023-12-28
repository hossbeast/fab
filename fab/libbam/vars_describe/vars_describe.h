/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include <sys/types.h>
#include <stdint.h>

#include "descriptor.h"

extern descriptor_type descriptor_bam_vars_describe_string;
extern descriptor_type descriptor_bam_vars_describe_result;

typedef struct bam_vars_describe_string {
  uint16_t text_len;
  const char *text;
} bam_vars_describe_string;

/* results from vars-describe */
typedef struct bam_vars_describe_result {
  const char *dir_abspath;
  uint16_t dir_abspath_len;

  const char *mod_abspath;
  uint16_t mod_abspath_len;

  const char *mod_id;
  uint16_t mod_id_len;

  /* struct bam_vars_string */
  void *file_list;
  uint32_t file_list_size;     // size of the file_list blob
  uint16_t file_list_len;      // items in file_list
} bam_vars_describe_result;
