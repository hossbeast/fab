/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include <stdarg.h>

#include "types.h"

/// mkdirp
//
// SUMMARY
//  ensure a directory path exists, iteratively creating each component of the path
//  if it does not already exist
//
// PARAMETERS
//  mode    - mode to create directories with
//  dirpath - path to directory to create
//
// PARAMETERS
//  s/w/f/vf - different ways of providing path
//
void mkdirps(mode_t mode, const char * const restrict path)
  __attribute__((nonnull));

void mkdirpw(mode_t mode, const char * const restrict path, size_t pathl)
  __attribute__((nonnull));

void mkdirpf(mode_t mode, const char * const restrict fmt, ...)
  __attribute__((nonnull));

void mkdirpvf(mode_t mode, const char * const restrict fmt, va_list va)
  __attribute__((nonnull));
