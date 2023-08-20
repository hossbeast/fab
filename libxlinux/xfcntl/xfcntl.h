/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>


/// xopen
//
// SUMMARY
//  proxy for open
//
int xopens(int flags, const char * path)
  __attribute__((nonnull(2)));

int xopenf(int flags, const char * path_fmt, ...)
  __attribute__((nonnull(2)))
  __attribute__((format(printf, 2, 3)));

int xopenvf(int flags, const char * path_fmt, va_list va)
  __attribute__((nonnull(2)));

/// xopenat
//
// SUMMARY
//  proxy for opent
//
int xopenats(int flags, int dirfd, const char * path)
  __attribute__((nonnull(3)));

int xopenatf(int flags, int dirfd, const char * path_fmt, ...)
  __attribute__((nonnull(3)))
  __attribute__((format(printf, 3, 4)));

int xopenatvf(int flags, int dirfd, const char * path_fmt, va_list va)
  __attribute__((nonnull(3)));

/// xopen_mode
//
// SUMMARY
//  proxy for open
//
int xopen_modes(int flags, mode_t mode, const char * path)
  __attribute__((nonnull(3)));

int xopen_modef(int flags, mode_t mode, const char * path_fmt, ...)
  __attribute__((nonnull(3)))
  __attribute__((format(printf, 3, 4)));

int xopen_modevf(int flags, mode_t mode, const char * path_fmt, va_list va)
  __attribute__((nonnull(3)));

/// xopenat_mode
//
// SUMMARY
//  proxy for open
//
int xopenat_modes(int flags, mode_t mode, int dirfd, const char * path)
  __attribute__((nonnull(4)));

int xopenat_modef(int flags, mode_t mode, int dirfd, const char * path_fmt, ...)
  __attribute__((nonnull(4)))
  __attribute__((format(printf, 4, 5)));

int xopenat_modevf(int flags, mode_t mode, int dirfd, const char * path_fmt, va_list va)
  __attribute__((nonnull(4)));

/// uxopen
//
// SUMMARY
//  proxy for open that only fails when errno not in { ENOENT, EEXIST }
//
int uxopens(int flags, const char * path)
  __attribute__((nonnull(2)));

int uxopenf(int flags, const char * path_fmt, ...)
  __attribute__((nonnull(2)))
  __attribute__((format(printf, 2, 3)));

int uxopenvf(int flags, const char * path_fmt, va_list va)
  __attribute__((nonnull(2)));

/// uxopenat
//
// SUMMARY
//  proxy for openat that only fails when errno not in { ENOENT, EEXIST }
//
int uxopenats(int flags, int dirfd, const char * path)
  __attribute__((nonnull(3)));

int uxopenatf(int flags, int dirfd, const char * path_fmt, ...)
  __attribute__((nonnull(3)))
  __attribute__((format(printf, 3, 4)));

int uxopenatvf(int flags, int dirfd, const char * path_fmt, va_list va)
  __attribute__((nonnull(3)));

/// xopen_mode
//
// SUMMARY
//  proxy for open that only fails when errno not in { ENOENT, EEXIST }
//
int uxopen_modes(int flags, mode_t mode, const char * path)
  __attribute__((nonnull(3)));

int uxopen_modef(int flags, mode_t mode, const char * path_fmt, ...)
  __attribute__((nonnull(3)))
  __attribute__((format(printf, 3, 4)));

int uxopen_modevf(int flags, mode_t mode, const char * path_fmt, va_list va)
  __attribute__((nonnull(3)));

/// xopenat_mode
//
// SUMMARY
//  proxy for open that only fails when errno not in { ENOENT, EEXIST }
//
int uxopenat_modes(int flags, mode_t mode, int dirfd, const char * path)
  __attribute__((nonnull(4)));

int uxopenat_modef(int flags, mode_t mode, int dirfd, const char * path_fmt, ...)
  __attribute__((nonnull(4)))
  __attribute__((format(printf, 4, 5)));

int uxopenat_modevf(int flags, mode_t mode, int dirfd, const char * path_fmt, va_list va)
  __attribute__((nonnull(4)));

/// xsplice
//
// SUMMARY
//  proxy for splice
//
ssize_t xsplice(int fd_in, loff_t *off_in, int fd_out, loff_t *off_out, size_t len, unsigned int flags);
