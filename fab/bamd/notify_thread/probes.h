/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

struct inotify_event;
struct fsent;

void notify_thread_probe(const struct inotify_event * restrict ev, const struct fsent * restrict fse)
  __attribute__((nonnull));
