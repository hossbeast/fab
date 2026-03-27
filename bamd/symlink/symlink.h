/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

struct fsent;
struct ref_edge;

void symlink_readlink_push(struct fsent * restrict fse)
  __attribute__((nonnull));

void symlink_system_reconcile(void);

void symlink_readlink_cancel(struct fsent * restrict fse)
  __attribute__((nonnull));

struct ref_edge *fsent_symlink_connect(struct fsent * restrict ref, struct fsent * restrict link)
  __attribute__((nonnull));
