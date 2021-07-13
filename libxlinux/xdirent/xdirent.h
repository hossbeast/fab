/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

   This file is part of fab.

   fab is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   fab is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with fab.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef _XDIRENT_H
#define _XDIRENT_H

#include <sys/types.h>
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

#endif
