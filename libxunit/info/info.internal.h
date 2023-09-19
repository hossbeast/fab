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

#ifndef _XUNIT_INFO_INTERNAL_H
#define _XUNIT_INFO_INTERNAL_H

#include "types.h"
#include "info.h"

struct memblk;

typedef struct info
{
  char *  ks;   // key
  size_t  kl;
  size_t  ka;

  char *  vs;   // value
  size_t  vl;
  size_t  va;
} info;

// per-thread info staging
extern __thread info * xunit_infos_staging;
extern __thread size_t xunit_infos_stagingl;
extern __thread size_t xunit_infos_staginga;

/// info_teardown
//
// SUMMARY
//  cleanup staging infos
//
void info_teardown(void);

/// info_set
//
// SUMMARY
//  populate an info
//
// PARAMETERS
//  key  - pointer to key
//  keyl - nonzero key length
//
void info_setw(info * restrict i, const char * restrict key, size_t keyl, const char * restrict vbuf, size_t vlen)
  __attribute__((nonnull(1, 2)));

void info_setvf(info * restrict i, const char * restrict key, size_t keyl, const char * restrict vfmt, va_list va)
  __attribute__((nonnull(1, 2)));

/// info_freeze
//
// SUMMARY
//  see memblk_freeze
//
void info_freeze(struct memblk * const restrict mb, info * restrict i)
  __attribute__((nonnull));

/// info_freeze
//
// SUMMARY
//  see memblk_freeze
//
void info_unfreeze(struct memblk * const restrict mb, info * restrict i)
  __attribute__((nonnull));

/// info_freeze
//
// SUMMARY
//  see memblk_freeze
//
void info_thaw(char * const restrict mb, info * restrict i)
  __attribute__((nonnull));

#endif
