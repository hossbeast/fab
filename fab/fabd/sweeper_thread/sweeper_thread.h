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

#ifndef _SWEEPER_THREAD_H
#define _SWEEPER_THREAD_H

#include "xapi.h"
#include "valyria/llist.h"
#include "valyria/rbtree.h"

struct node;

#define SWEEPER_EVENT_SELF    1
#define SWEEPER_EVENT_CHILD   2

typedef struct sweeper_event {
  llist lln;              // in freelist / event queue
  uint8_t kind;
  uint32_t era;
  uint32_t mask;
} sweeper_event;

typedef struct sweeper_child_event {
  sweeper_event sweep_event;
  rbnode rbn;            // in parent->pending_child
  struct node *parent;
  char name[128];
  uint8_t name_len;
} sweeper_child_event;

xapi sweeper_thread_setup(void);

xapi sweeper_thread_cleanup(void);

xapi sweeper_thread_launch(void);

xapi sweeper_thread_enqueue(struct node *n, uint32_t m, const char * restrict name, uint16_t namel)
  __attribute__((nonnull(1)));

#endif
