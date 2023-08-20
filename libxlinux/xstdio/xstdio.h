/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include <stdarg.h>

#include "types.h"

/// xvdprintf
//
// SUMMARY
//  proxy for vdprintf
//
// PARAMETERS
//  as for vdprintf
//
void xvdprintf(int fd, const char * const restrict fmt, va_list va)
  __attribute__((nonnull(2)));

/// xrename
//
// SUMMARY
//  void proxy for rename
//
// VARIANTS
//  s/f/vf - different ways to pass path
//
void xrenames(const char * const restrict old, const char * const restrict new)
  __attribute__((nonnull));

void xrenamef(const char * const restrict oldfmt, const char * const restrict newfmt, ...)
  __attribute__((nonnull(1, 2)));

void xrenamevf(const char * const restrict oldfmt, const char * const restrict newfmt, va_list va)
  __attribute__((nonnull(1, 2)));

/// uxrename
//
// SUMMARY
//  proxy for rename that only fails when errno not in { ENOENT, ENOTDIR }
//

void uxrenames(const char * const restrict old, const char * const restrict new)
  __attribute__((nonnull));

void uxrenamef(const char * const restrict oldfmt, const char * const restrict newfmt, ...)
  __attribute__((nonnull(1, 2)));

void uxrenamevf(const char * const restrict oldfmt, const char * const restrict newfmt, va_list va)
  __attribute__((nonnull(1, 2)));
