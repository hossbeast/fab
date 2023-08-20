/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include <stdarg.h>


/// symlinkp
//
// SUMMARY
//  rm linkpath (but dont fail if linkpath doesnt exist), then symlink(target, linkpath)
//
// PARAMETERS
//  target   - symlink value
//  linkpath - symlink location
//
// VARIANTS
//  s/f/vf - different ways of providing parameters
//
void symlinkps(const char * const restrict target, const char * const restrict linkpath)
  __attribute__((nonnull));

void symlinkpf(const char * const restrict target_fmt, const char * const restrict linkpath_fmt, ...)
  __attribute__((nonnull));

void symlinkpvf(const char * const restrict target_fmt, const char * const restrict linkpath_fmt, va_list va)
  __attribute__((nonnull));
