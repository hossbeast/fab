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

#ifndef _FILESYSTEM_INTERNAL_H
#define _FILESYSTEM_INTERNAL_H

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

xapi fstree_say(struct narrator * restrict N)
  __attribute__((nonnull));

const fstree * fstree_down(fstree * restrict fst, const char * restrict name, uint16_t namel)
  __attribute__((nonnull));

/* trace */
size_t fstree_znload(void * restrict dst, size_t sz)
  __attribute__((nonnull));

#endif
