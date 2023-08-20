/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*

MODULE
 narrator/nullity

SUMMARY
 narrator that discards writes

*/

#include "narrator.h"

typedef struct narrator_nullity {
  narrator base;
} narrator_nullity;


/// narrator_nullity_create
//
// SUMMARY
//  allocate a nullity narrator
//
// PARAMETERS
//  n - (returns) narrator
//
void narrator_nullity_create(narrator_nullity ** const restrict n)
  __attribute__((nonnull));

void narrator_nullity_free(narrator_nullity * restrict n);
