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

#ifndef FABD_BUILDPLAN_ENTITY_H
#define FABD_BUILDPLAN_ENTITY_H

#include "xapi.h"
#include "types.h"

struct narrator;
struct node;

#define BPE_NODE        0xbeef
#define BPE_NODE_EDGE   0xdead

/* either a node, or a node_edge */
typedef struct buildplan_entity {
  uint16_t id;        // build plan id
  uint16_t depth;     // max depth
  uint16_t typemark;

  const struct node * fml;
} buildplan_entity;

xapi bpe_say_sources(buildplan_entity * restrict bpe, struct narrator * restrict N)
  __attribute__((nonnull));

xapi bpe_say_targets(buildplan_entity * restrict bpe, struct narrator * restrict N)
  __attribute__((nonnull));

#endif
