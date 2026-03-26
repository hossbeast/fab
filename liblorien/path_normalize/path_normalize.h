/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

typedef struct path_normalize_context {
  const char *base;  // for . sections, can be absolute or relative
  uint16_t base_len;
  const char *home;  // for ~ and $HOME sections, can be absolute or relative
  uint16_t home_len;
} path_normalize_context;

/*
 * writes a normalized path with redundant sections removed
 * does not resolve or follow symlinks
 * the default base is . (relative)
 *
 * relative output begins with either . or .. and
 *  if input is absolute, output is absolute
 *  if input is relative and base is absolute, output is absolute
 *  if input is relative and base is relative, output is relative
 *
 * PARAMETERS
 *  dst   - location to write the normalized path to
 *  sz    - size of dst >= len + 2
 *  path  - path to normalize
 *  len   - length of path
 *
 * RETURNS
 *  number of bytes written to dst, excluding the terminating null byte
 */
size_t path_normalize(path_normalize_context * restrict ctx, char * restrict dst, size_t sz, const char * restrict path, uint16_t len)
  __attribute__((nonnull));
