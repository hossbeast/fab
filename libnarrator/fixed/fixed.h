/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*

MODULE
 narrator/fixed

SUMMARY
 narrator that writes to a fixed-size buffer, discarding overflow

REMARKS
 a fixed narrator requires no dynamic allocation, and passing a fixed narrator to the narrator free
 apis is a no-op

*/

#include "types.h"
#include "narrator.h"

typedef struct narrator_fixed {
  narrator base;
  char *  s;      // buffer
  size_t  l;      // position
  size_t  a;      // buffer size
  size_t  m;      // maximum position
} narrator_fixed;

/// narrator_fixed_init
//
// SUMMARY
//  initialize a fixed narrator
//
// PARAMETERS
//  fixed - storage for the fixed narrator
//
// RETURNS
//  fixed size narrator suitable for passing to other narrator apis
//
narrator * narrator_fixed_init(narrator_fixed * restrict n, char * s, size_t a)
  __attribute__((nonnull));

void narrator_fixed_destroy(narrator_fixed * restrict n)
  __attribute__((nonnull));
