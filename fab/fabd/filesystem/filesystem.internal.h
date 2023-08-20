/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "valyria/llist.h"
#include "valyria/rbtree.h"

#include "filesystem.h"

struct filesystem;
struct narrator;

typedef struct fstree {
  char name[255];
  uint16_t namel;

  struct fstree * up;
  rbtree down;
  rbnode rbn; // in others down
  llist lln;  // freelist

  struct filesystem * fs; // if an fs is attached at this particular point
} fstree;

extern struct fstree fstree_root;
extern struct fstree fstree_shadow;

void fstree_say(struct narrator * restrict N)
  __attribute__((nonnull));

const fstree * fstree_down(fstree * restrict fst, const char * restrict name, uint16_t namel)
  __attribute__((nonnull));

/* trace */
size_t fstree_znload(void * restrict dst, size_t sz)
  __attribute__((nonnull));
