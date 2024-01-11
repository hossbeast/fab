/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include <sys/types.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>

/// xmalloc
//
// SUMMARY
//  does this: *target = calloc(size, 1);
//  recall that calloc zeroes out the memory it allocates
//
// RETURNS
//  returns 0 on success
//
void xmalloc(void* target, size_t size)
  __attribute__((nonnull));

/// xrealloc
//
// SUMMARY
//  does this: realloc(*target, size) AND ensures that any new portion of memory is zeroed out
//
// PARAMETERS
//  es  - element size
//  ec  - element count
//  oec - old element count
//
// RETURNS
//  returns 0 on success
//
void xrealloc(void* target, size_t es, size_t ec, size_t oec)
  __attribute__((nonnull));

/// ifree
//
// SUMMARY
//  does this: free(*target); *target = 0;
//
void ifree(void* target)
  __attribute__((nonnull));

/* readlink wrappers */
ssize_t xreadlinks(const char * pathname, char * buf, size_t bufsiz)
  __attribute__((nonnull(1, 2)));

ssize_t xreadlinkw(const char * pathname, uint16_t pathname_len, char * buf, size_t bufsiz)
  __attribute__((nonnull(1, 3)));

ssize_t xreadlinkf(const char * pathname_fmt, char * buf, size_t bufsiz, ...)
  __attribute__((nonnull(1, 2)));

ssize_t xreadlinkvf(const char * pathname_fmt, char * buf, size_t bufsiz, va_list va)
  __attribute__((nonnull(1, 2)));

ssize_t uxreadlinkw(const char * pathname, uint16_t pathname_len, char * buf, size_t bufsiz)
  __attribute__((nonnull(1, 3)));

ssize_t uxreadlinkf(const char * pathname_fmt, char * buf, size_t bufsiz, ...)
  __attribute__((nonnull(1, 2)));

ssize_t uxreadlinkvf(const char * pathname_fmt, char * buf, size_t bufsiz, va_list va)
  __attribute__((nonnull(1, 2)));

/* readlinkat wrappers */
ssize_t xreadlinkats(int dfd, const char * pathname, char * buf, size_t bufsiz)
  __attribute__((nonnull(2, 3)));

ssize_t xreadlinkatw(int dfd, const char * pathname, uint16_t pathname_len, char * buf, size_t bufsiz)
  __attribute__((nonnull(2, 4)));

ssize_t xreadlinkatf(int dfd, const char * pathname_fmt, char * buf, size_t bufsiz, ...)
  __attribute__((nonnull(2, 3)));

ssize_t xreadlinkatvf(int dfd, const char * pathname_fmt, char * buf, size_t bufsiz, va_list va)
  __attribute__((nonnull(2, 3)));

ssize_t uxreadlinkatw(int dfd, const char * pathname, uint16_t pathname_len, char * buf, size_t bufsiz)
  __attribute__((nonnull(2, 4)));

ssize_t uxreadlinkatf(int dfd, const char * pathname_fmt, char * buf, size_t bufsiz, ...)
  __attribute__((nonnull(2, 3)));

ssize_t uxreadlinkatvf(int dfd, const char * pathname_fmt, char * buf, size_t bufsiz, va_list va)
  __attribute__((nonnull(2, 3)));

/* realpath wrapper */
char * xrealpaths(char * restrict resolved_path, const char * restrict path)
  __attribute__((nonnull(2)));

char * xrealpathw(char * restrict resolved_path, const char * restrict path, uint16_t len)
  __attribute__((nonnull(2)));

char * xrealpathf(char * restrict resolved_path, const char * restrict path_fmt, ...)
  __attribute__((nonnull(2)))
  __attribute__((format(printf, 2, 3)));

char * xrealpathvf(char * restrict resolved_path, const char * restrict path_fmt, va_list va)
  __attribute__((nonnull(2)));

int xsystem(const char * restrict command)
  __attribute__((nonnull));
