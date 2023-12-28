/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include <sys/types.h>
#include <stdint.h>

#include "descriptor.h"
#include "vertex.h"

extern descriptor_type descriptor_bam_describe_item;

/* result from the describe api */
typedef union bam_describe_item {
  struct {
    uint16_t distance;
    enum bam_vertex_type type;
    enum bam_vertex_state state;

    /* node label */
    const char *label;
    uint16_t label_len;

    /* node full path */
    const char *path;
    uint16_t path_len;

    /* unique id for the node path */
    const char *id;
    uint16_t id_len;

    /* unique id for the node path and node lifetime */
    const char *token;
    uint16_t token_len;

    const char *ref;
    uint16_t ref_len;

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
} bam_describe_item;
