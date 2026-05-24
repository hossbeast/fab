/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include <sys/inotify.h>


/// xinotify_init
//
// SUMMARY
//  proxy for inotify_init
//
int xinotify_init(int flags)
  __attribute__((nonnull));

/// inotify_add_watch
//
// SUMMARY
//  proxy for inotify_add_watch
//
int xinotify_add_watch(int id, const char *path, uint32_t mask)
  __attribute__((nonnull));

void xinotify_rm_watch(int id, int wd);
