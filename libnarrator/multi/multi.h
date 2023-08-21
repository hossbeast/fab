/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*

MODULE
 narrator/multi

SUMMARY
 narrator that propagates writes to each of a set of underlying narrators

*/

#include "types.h"

#include "narrator.h"

typedef struct narrator_multi
{
  narrator base;
  narrator ** v;    // narrators
  size_t l;         // number in use
  size_t a;         // allocated size
} narrator_multi;

/// narrator_multi_create
//
// SUMMARY
//  allocate a multi narrator
//
// PARAMETERS
//  n - (returns) narrator
//
void narrator_multi_create(narrator_multi ** const restrict n)
  __attribute__((nonnull));

void narrator_multi_free(narrator_multi * restrict n);

void narrator_multi_init(narrator_multi * restrict n)
  __attribute__((nonnull));

void narrator_multi_destroy(narrator_multi * restrict n);

/// narrator_multi_add
//
// SUMMARY
//  add a narrator to the list of narrators underlying a multi narrator
//
// PARAMETERS
//  n  - multi narrator
//  np - narrator to add to the multi narrator
//
void narrator_multi_add(narrator_multi * const restrict n, narrator * const restrict np)
  __attribute__((nonnull));
