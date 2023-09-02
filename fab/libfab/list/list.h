/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include <sys/types.h>
#include <stdint.h>

#include "descriptor.h"
#include "vertex.h"

extern descriptor_type descriptor_fab_list_item;

/* response from the list api */
typedef struct fab_list_item {
  enum fab_vertex_type type;
  enum fab_vertex_state state;

  const char *label;
  uint16_t label_len;

  const char *path;
  uint16_t path_len;

  const char *ref;
  uint16_t ref_len;

  uint16_t distance;
} fab_list_item;
