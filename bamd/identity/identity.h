/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"
#include "common/sha1.h"

enum vertex_type;

typedef struct identity_ctx {
  sha1_ctx sha1;
} identity_ctx;

/* initialize an identity context */
void identity_init(identity_ctx * restrict ctx);

/* add a vertex type and label to an identity context */
void identity_name(identity_ctx * restrict id, enum vertex_type type, const char * restrict label, uint16_t label_len);

/* add a vertex type to an identity context */
void identity_type(identity_ctx * restrict id, enum vertex_type type);

/* add text to an identity context */
void identity_add(identity_ctx * restrict ctx, const void * restrict data, uint16_t len);

/* write an id string from a completed identity context */
uint16_t identity_mk(identity_ctx * restrict ctx, char * restrict id, size_t id_size);
