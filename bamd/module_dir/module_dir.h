/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

struct fsent;
struct bacon_writer;

size_t module_dir_path_cat(void * restrict dst, size_t sz, const struct fsent * restrict dir)
  __attribute__((nonnull));
