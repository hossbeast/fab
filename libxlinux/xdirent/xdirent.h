/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include <dirent.h>
#include <errno.h>

/// xopendir
//
// SUMMARY
//  proxy for opendir
//
DIR * xopendir(const char * name)
  __attribute__((nonnull));

/// uxopendir
//
// SUMMARY
//  proxy for opendir that fails only when errno != ENOENT
//
DIR *uxopendir(const char * name)
  __attribute__((nonnull));

/// xopendirat
//
// SUMMARY
//  like opendir, but relative to an O_PATH file descriptor (man 2 openat)
//
DIR *xopendirat(int dirfd, const char * const restrict path)
  __attribute__((nonnull));

/// xfdopendir
//
// SUMMARY
//  proxy for fdopendir
//
DIR *xfdopendir(int fd)
  __attribute__((nonnull));

/// xreaddir
//
// SUMMARY
//  proxy for readdir
//
struct dirent *xreaddir(DIR *dd)
  __attribute__((nonnull));

/// xclosedir
//
// SUMMARY
//  proxy for closedir
//
void xclosedir(DIR * dd);

void ixclosedir(DIR ** dd)
  __attribute__((nonnull));

/// xtelldir
//
// xapi proxy for telldir
//
long xtelldir(DIR * dirp)
  __attribute__((nonnull));
