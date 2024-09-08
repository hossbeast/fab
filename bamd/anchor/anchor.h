/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

#include "valyria/llist.h"

struct narrator;
struct pattern;

typedef struct anchor {
  llist owner;

  /* anchor lookup */
  struct pattern * lookup;
} anchor;

void anchor_mk(anchor ** restrict anchorp, struct pattern * lookup)
  __attribute__((nonnull(1)));

void anchor_dispose(anchor * restrict)
  __attribute__((nonnull));

void anchor_release(anchor * restrict);

void anchor_dispose_bulk(llist * restrict list)
  __attribute__((nonnull));

void anchor_release_bulk(llist * restrict list)
  __attribute__((nonnull));

void anchor_say(const anchor * restrict r, struct narrator * restrict N)
  __attribute__((nonnull));
