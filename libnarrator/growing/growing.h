/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*

MODULE
 narrator/growing

SUMMARY
 narrator that accumulates all writes, growing without bound

*/

#include "types.h"

#include "narrator.h"

typedef struct narrator_growing
{
  narrator base;
  char *  s;    // the buffer
  size_t  l;    // position
  size_t  a;    // allocated size
  size_t  m;    // maximum position
} narrator_growing;

/// narrator_growing_create
//
// SUMMARY
//  allocate a growing narrator
//
// PARAMETERS
//  n - (returns) narrator
//
void narrator_growing_create(narrator_growing ** const restrict n)
  __attribute__((nonnull));

void narrator_growing_free(narrator_growing * restrict n);

/// narrator_growing_init
//
// SUMMARY
//  initialize a growing narrator
//
narrator * narrator_growing_init(narrator_growing * restrict n)
  __attribute__((nonnull));

narrator * narrator_growing_init_from(narrator_growing * restrict n, char * buf, size_t bufa)
  __attribute__((nonnull(1)));

void narrator_growing_destroy(narrator_growing * restrict n)
  __attribute__((nonnull));

/// narrator_growing_claim_buffer
//
// SUMMARY
//  claim the underlying buffer and reset the narrator
//
void narrator_growing_claim_buffer(narrator_growing * restrict n, void * bufp, size_t * allocp)
  __attribute__((nonnull(1)));

void narrator_growing_allocate(narrator_growing * restrict n, size_t size)
  __attribute__((nonnull));
