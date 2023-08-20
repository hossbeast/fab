/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include <sys/types.h>
#include <stdint.h>

#include "descriptor.h"
#include "fsent.h"

extern descriptor_type descriptor_fab_describe_item;

/* result from the describe api */
typedef union fab_describe_item {
  struct {
    uint16_t distance;
    enum fab_fsent_type type;
    enum fab_fsent_state state;

    const char *label;
    uint16_t label_len;

    const char *path;
    uint16_t path_len;

    const char *ref;
    uint16_t ref_len;

    const char *variant;
    uint16_t variant_len;

    const char *ext;
    uint16_t ext_len;

    const char *suffix;
    uint16_t suffix_len;

    const char *base;
    uint16_t base_len;

    const char *fsroot;
    uint16_t fsroot_len;

    const char *invalidation;
    uint16_t invalidation_len;

    const char *formula;
    uint16_t formula_len;
  };

  const char *s[0];
  uint16_t u16[0];
} fab_describe_item;
