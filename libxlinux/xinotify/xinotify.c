/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>

#include "types.h"
#include "macros.h"

#include "xinotify.h"

int API xinotify_init(int flags)
{
  int r;

  r = inotify_init1(flags);
  RUNTIME_ASSERT(r >= 0);

  return r;
}

/// inotify_add_watch
//
// SUMMARY
//  proxy for inotify_add_watch
//
int API xinotify_add_watch(int id, const char *path, uint32_t mask)
{
  int r;

  r = inotify_add_watch(id, path, mask);
  RUNTIME_ASSERT(r >= 0);

  return r;
}

void xinotify_rm_watch(int id, int wd)
{
  int r;

  r = inotify_rm_watch(id, wd);

  RUNTIME_ASSERT(r == 0);
}
