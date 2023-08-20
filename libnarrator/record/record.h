/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*

MODULE
 narrator/record

SUMMARY
 narrator that accumulates writes and propagates records to an underlying narrator

*/

#include "types.h"
#include "narrator.h"

typedef struct narrator_record
{
  narrator base;
  char *  s;    // buffer
  size_t  l;    // position
  size_t  a;    // allocated size

  narrator * n;    // underlying narrator, (not owned)
} narrator_record;

extern struct narrator_vtable narrator_record_vtable;

/// narrator_record_create
//
// SUMMARY
//  allocate a record narrator
//
// PARAMETERS
//  n  - (returns) narrator
//  np - underlying narrator
//
void narrator_record_create(narrator_record ** const restrict n, narrator * const restrict np)
  __attribute__((nonnull(1)));

void narrator_record_free(narrator_record * restrict n);

void narrator_record_destroy(narrator_record * restrict n)
  __attribute__((nonnull));

/// narrator_record_reset
//
// SUMMARY
//  resets the underlying narrator
//
// PARAMETERS
//  n - record narrator
//
void narrator_record_reset(narrator_record * const restrict n)
  __attribute__((nonnull));

/// narrator_record_flush
//
// SUMMARY
//  propagate to the underlying narrator with a single write
//
void narrator_record_flush(narrator_record * const restrict n)
  __attribute__((nonnull));
