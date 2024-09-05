/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include <sys/types.h>
#include <stdint.h>

#include "descriptor.h"

extern descriptor_type descriptor_bam_list_item;

/* response from the list api */
typedef struct bam_list_item {
  const char *type;
  uint16_t type_len;

  const char *state;
  uint16_t state_len;

  const char *label;
  uint16_t label_len;

  const char *path;
  uint16_t path_len;

  const char *ref;
  uint16_t ref_len;

  uint16_t distance;
} bam_list_item;
