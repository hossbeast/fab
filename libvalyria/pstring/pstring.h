/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*
MODULE
 pstring

SUMMARY
 dynamically resizing strings
*/

#include <stdarg.h>

#include "types.h"

/* initialize a string */
void psinit(char ** restrict ps, size_t * restrict len, size_t * restrict a)
  __attribute__((nonnull));

/* initialize a string */
void psinitx(char ** restrict ps, size_t * restrict len, size_t * restrict a, size_t capacity)
  __attribute__((nonnull));

/* free a string */
void psdestroy(char ** restrict ps, size_t * restrict len, size_t * restrict a)
  __attribute__((nonnull));

/// psclear
//
// SUMMARY
//  set the internal length of the pstring to zero; the allocation remains intact
//
// PARAMETERS
//  p - pstring
//
void psclear(char ** restrict ps, size_t * restrict len, size_t * restrict a)
  __attribute__((nonnull));

/// psload
//
// SUMMARY
//  set the value of a pstring
//
// PARAMETERS
//  p     - pass/return pstring instance
//  [fmt] - printf-style format string
//  [va]  - va_args
//  [s]   - null-terminated string
//  [b]   - buffer
//  [l]   - buffer length
//  [c]   - single byte
//
// VARIANTS
//  psloadvf : load formatted output from va_list
//  psloadf : load formatted output from varargs
//  psloads : load a string
//  psloadw : load a buffer
//  psloadc : load a byte
//
void psloadvf(char ** restrict ps, size_t * restrict len, size_t * restrict a, const char * const restrict fmt, va_list va)
  __attribute__((nonnull));

void psloadf(char ** restrict ps, size_t * restrict len, size_t * restrict a, const char * const restrict fmt, ...)
  __attribute__((nonnull(1, 2)));

void psloads(char ** restrict ps, size_t * restrict len, size_t * restrict a, const char * const restrict s)
  __attribute__((nonnull));

void psloadw(char ** restrict ps, size_t * restrict len, size_t * restrict a, const char * const restrict s, size_t l)
  __attribute__((nonnull));

void psloadc(char ** restrict ps, size_t * restrict len, size_t * restrict a, int c)
  __attribute__((nonnull));

/// pscat
//
// SUMMARY
//  see psload, except pscat appends instead of overwriting
//
// PARAMETERS
//  [fmt] - printf-style format string
//  [va]  - va_args
//  [s]   - null-terminated string
//  [b]   - buffer
//  [l]   - buffer length
//  [c]   - single byte
//
void pscatvf(char ** restrict ps, size_t * restrict len, size_t * restrict a, const char * const restrict fmt, va_list va)
  __attribute__((nonnull));

void pscatf(char ** restrict ps, size_t * restrict len, size_t * restrict a, const char * const restrict fmt, ...)
  __attribute__((nonnull(1, 2)))
  __attribute__((format(printf, 4, 5)));

void pscats(char ** restrict ps, size_t * restrict len, size_t * restrict a, const char * const restrict s)
  __attribute__((nonnull));

void pscatw(char ** restrict ps, size_t * restrict len, size_t * restrict a, const char * const restrict b, size_t l)
  __attribute__((nonnull));

void pscatc(char ** restrict ps, size_t * restrict len, size_t * restrict a, int c)
  __attribute__((nonnull));
