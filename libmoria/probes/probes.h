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

#ifndef _PROBES_H
#define _PROBES_H

struct moria_edge;
struct moria_vertex;

extern unsigned short moria_explore_edge_semaphore;
extern unsigned short moria_explore_vertex_semaphore;

void explore_edge_probe(struct moria_edge * restrict e, int distance, bool travel, bool visit)
  __attribute__((nonnull));

void explore_vertex_probe(struct moria_vertex * restrict v, int distance, bool travel, bool visit)
  __attribute__((nonnull));

#endif
