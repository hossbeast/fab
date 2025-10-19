/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

/* https://github.com/git/git/blob/master/block-sha1/sha1.h */

/*
 * SHA1 routine optimized to do word accesses rather than byte accesses,
 * and to avoid unnecessary copies into the context array.
 *
 * This was initially based on the Mozilla SHA1 implementation, although
 * none of the original Mozilla code remains.
 */

typedef struct {
  unsigned long long size;
  unsigned int H[5];
  unsigned int W[16];
} sha1_ctx;

void sha1_init(sha1_ctx *ctx);
void sha1_update(sha1_ctx *ctx, const void *dataIn, size_t len);
void sha1_final(unsigned char hashout[20], sha1_ctx *ctx);
