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

#include "xapi.h"

#include "valyria/list.h"

#include "internal.h"
#include "edge.internal.h"
#include "graph.internal.h"
#include "vertex.internal.h"

#include "macros.h"

//
// public
//

int edge_key_compare_label(void * _ctx, const void * _e, size_t idx)
{
  struct edge_key_compare_label_context * ctx = _ctx;
  const edge * e = _e;

  if(ctx->A)
    ctx->lc = memncmp(ctx->A, ctx->len, e->A->label, e->A->label_len);
  else
    ctx->lc = memncmp(ctx->B, ctx->len, e->B->label, e->B->label_len);

  ctx->lx = idx;
  return ctx->lc;
}

int edge_key_compare_vertex(void * _ctx, const void * _e, size_t idx)
{
  struct edge_key_compare_vertex_context * ctx = _ctx;
  const edge * e = _e;

  if(ctx->A)
    ctx->lc = INTCMP(ctx->A, e->A);
  else
    ctx->lc = INTCMP(ctx->B, e->B);

  ctx->lx = idx;
  return ctx->lc;
}

//
// api
//

API xapi edge_disconnect(graph * const restrict g, edge * const restrict e)
{
  enter;

  int x;

  fatal(list_delete, e->A->down, e->down_index);
  for(x = e->down_index; x < e->A->down->l; x++)
    ((edge*)list_get(e->A->down, x))->down_index--;

  fatal(list_delete, e->B->up, e->up_index);
  for(x = e->up_index; x < e->B->up->l; x++)
    ((edge*)list_get(e->B->up, x))->up_index--;

  size_t edges_index = e->edges_index;
  fatal(list_delete, g->edges, e->edges_index);
  for(x = edges_index; x < g->edges->l; x++)
    ((edge*)list_get(g->edges, x))->edges_index--;

  finally : coda;
}

API edge * edge_between(const vertex * restrict A, const vertex * restrict B)
{
  struct edge_key_compare_label_context label_ctx;
  struct edge_key_compare_vertex_context vertex_ctx;
  edge * e;

  label_ctx = (typeof(label_ctx)) { B : B->label, len : B->label_len };
  e = list_search_range(A->down, 0, A->down_partition, &label_ctx, edge_key_compare_label);

  if(!e)
  {
    vertex_ctx = (typeof(vertex_ctx)) { B : B };
    e = list_search_range(A->down, A->down_partition, A->down->l - A->down_partition, &vertex_ctx, edge_key_compare_vertex);
  }

  return e;
}
