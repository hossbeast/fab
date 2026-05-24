/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include <sys/stat.h>
#include <stdarg.h>

/// xfstatat
//
// SUMMARY
//  proxy for fstatat
//
// VARIANTS
//  s/f/vf - different ways to pass path
//
void xfstatats(int dirfd, int flags, struct stat * restrict buf, const char * restrict path)
  __attribute__((nonnull(3, 4)));

void xfstatatf(int dirfd, int flags, struct stat * restrict buf, const char * restrict path_fmt, ...)
  __attribute__((nonnull(3, 4)))
  __attribute__((format(printf, 4, 5)));

void xfstatatvf(int dirfd, int flags, struct stat * restrict buf, const char * restrict path_fmt, va_list va)
  __attribute__((nonnull(3, 4)));

/// uxfstatat
//
// SUMMARY
//  proxy for fstatat which only fails when errno != ENOENT and errno != ENOTDIR
//  if fstatat fails but uxfstatat does not, buf is zeroed
//
// VARIANTS
//  s/f/vf - different ways to pass path
//
int uxfstatats(int dirfd, int flags, struct stat * restrict buf, const char * restrict path)
  __attribute__((nonnull(3, 4)));

int uxfstatatf(int dirfd, int flags, struct stat * restrict buf, const char * restrict path_fmt, ...)
  __attribute__((nonnull(3, 4)))
  __attribute__((format(printf, 4, 5)));

int uxfstatatvf(int dirfd, int flags, struct stat * restrict buf, const char * restrict path_fmt, va_list va)
  __attribute__((nonnull(3, 4)));


/// xfutimens
//
// SUMMARY
//  proxy for futimens
//
void xfutimens(int fd, const struct timespec times[2]);

/// xutimensat
//
// SUMMARY
//  proxy for utimensat
//
void xutimensats(int dirfd, const struct timespec times[2], int flags, const char * const restrict path)
  __attribute__((nonnull));

/// xutimensat
//
// SUMMARY
//  proxy for utimensat that only fails when errno != ENOENT
//
int uxutimensats(int dirfd, const struct timespec times[2], int flags, const char * const restrict path)
  __attribute__((nonnull(4)));

/// xmkdir
//
// SUMMARY
//  proxy for mkdir
//
void xmkdirs(mode_t mode, const char * path)
  __attribute__((nonnull));

void xmkdirf(mode_t mode, const char * path_fmt, ...)
  __attribute__((nonnull));

void xmkdirvf(mode_t mode, const char * path_fmt, va_list va)
  __attribute__((nonnull));

/// uxmkdir
//
// SUMMARY
//  proxy for mkdir which only fails when errno != ENOENT
//
int uxmkdirs(mode_t mode, const char * path)
  __attribute__((nonnull));

int uxmkdirf(mode_t mode, const char * path_fmt, ...)
  __attribute__((nonnull));

int uxmkdirvf(mode_t mode, const char * path_fmt, va_list va)
  __attribute__((nonnull));

/// xfchmod
//
// SUMMARY
//  proxy for fchmod
//
void xfchmod(int fd, mode_t mode);
