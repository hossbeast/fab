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

#ifndef _XSTAT_H
#define _XSTAT_H

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "xapi.h"

#define restrict __restrict

/// xstat
//
// SUMMARY
//  proxy for stat
//
xapi xstat(const char * restrict path, struct stat * restrict buf)
	__attribute__((nonnull(1, 2)));

/// uxstat
//
// SUMMARY
//  proxy for stat which only fails when errno != ENOENT and errno != ENOTDIR
//  if stat fails but uxstat does not, buf is zeroed
//
xapi uxstat(const char * restrict path, struct stat * restrict buf, int * restrict r)
	__attribute__((nonnull(1,2)));

/// xlstat
//
// SUMMARY
//  proxy for lstat
//
xapi xlstat(const char * restrict path, struct stat * restrict buf, int * restrict r)
	__attribute__((nonnull(1,2)));

/// uxlstat
//
// SUMMARY
//  proxy for lstat which only fails when errno != ENOENT
//  if lstat fails but uxlstat does not, buf is zeroed
//
xapi uxlstat(const char * restrict path, struct stat * restrict buf, int * restrict r)
	__attribute__((nonnull(1,2)));

/// xfstat
//
// SUMMARY
//  proxy for fstat
//
xapi xfstat(int fd, struct stat * restrict buf)
	__attribute__((nonnull));

/// uxfstat
//
// SUMMARY
//  proxy for stat which only fails when errno != ENOENT
//  if stat fails and errno == ENOENT, buf is zeroed
//
xapi uxfstat(int fd, struct stat * restrict buf)
	__attribute__((nonnull));

/// xfutimens
//
// SUMMARY
//  proxy for futimens
//
xapi xfutimens(int fd, const struct timespec times[2]);

/// xutimensat
//
// SUMMARY
//  proxy for utimensat
//
xapi xutimensat(int dirfd, const char * const restrict pathname, const struct timespec times[2], int flags);

/// xutimensat
//
// SUMMARY
//  proxy for utimensat that only fails when errno != ENOENT
//
xapi uxutimensat(int dirfd, const char * const restrict pathname, const struct timespec times[2], int flags, int * restrict r);

/// xmkdir
//
// SUMMARY
//  proxy for mkdir
//
xapi xmkdir(const char * restrict pathname, mode_t mode)
	__attribute__((nonnull));

/// uxmkdir
//
// SUMMARY
//  proxy for mkdir which only fails when errno != ENOENT
//
xapi uxmkdir(const char * restrict pathname, mode_t mode)
	__attribute__((nonnull));

/// xfchmod
//
// SUMMARY
//  proxy for fchmod
//
xapi xfchmod(int fd, mode_t mode);

#undef restrict
#endif
