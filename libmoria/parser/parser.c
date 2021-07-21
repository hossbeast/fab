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

#include "xlinux/xstdlib.h"

/* flex and bison do not agree on these names */
#define YYSTYPE struct graph_yystype
#define YYLTYPE yyu_location

/* the token enums shadow each other exclude all but one set */
#define OPERATIONS_YYTOKENTYPE

#include "parser.internal.h"
#include "moria.h"
#include "graph.tab.h"
#include "graph.tokens.h"
#include "graph.lex.h"
#include "graph.states.h"
#include "operations.tab.h"
#include "operations.tokens.h"
#include "operations.lex.h"
#include "operations.states.h"

#include "operations.h"
#include "edge.h"

static YYU_VTABLE(graph_vtable, graph, graph);
static YYU_VTABLE(operations_vtable, operations, operations);

//
// private
//

static void identifier_lookup(graph_parser * restrict parser, identifier * id, moria_vertex ** restrict rv)
{
  moria_vertex *v = 0;
  identifier *idv;
  moria_vertex *p;

  *rv = 0;
  llist_foreach(parser->vertex_list, v, owner) {
    if(memncmp(v->label, v->label_len, id->label, id->label_len) != 0) {
      continue;
    }

    p = v;
    for(idv = id->next; idv; idv = idv->next)
    {
      if(!p->up_identity || !p->up_identity->A) {
        break;
      }
      p = p->up_identity->A;

      if(memncmp(p->label, p->label_len, idv->label, idv->label_len) != 0) {
        break;
      }
    }
    if(idv) {
      continue;
    }

    if(*rv) {
      RUNTIME_ABORT();
    }
    *rv = v;
  }
}

static void identifier_resolve(
    graph_parser * restrict parser
  , identifier * id
  , moria_vertex ** restrict rv
)
{
  moria_vertex *v = 0;
  moria_vertex *pv = 0;

  v = 0;
  identifier_lookup(parser, id, &v);
  if(v)
  {
    *rv = v;
    return;
  }

  if(id->next) {
    identifier_resolve(parser, id->next, &pv);
  }

  parser->dispatch->create_vertex(parser, &v, id->attrs, id->opattrs, id->label, id->label_len);
  if(pv) {
    parser->dispatch->connect(parser, pv, v, MORIA_EDGE_IDENTITY, 0);
  }
  *rv = v;
}

//
// public
//

void identifier_list_free(identifier_list * id)
{
  int x;

  if(id)
  {
    if(id->len == 1)
    {
      identifier_free(id->v0);
    }
    else
    {
      for(x = 0; x < id->len; x++)
      {
        identifier_free(id->v[x]);
      }

      wfree(id->v);
    }
  }

  wfree(id);
}

void identifier_free(identifier * id)
{
  if(id)
    identifier_free(id->next);

  wfree(id);
}

void operation_vertex(graph_parser * restrict parser, identifier_list *A)
{
  int y;
  moria_vertex *v;

  if(A->len == 1)
  {
    identifier_resolve(parser, A->v0, &v);
  }
  else
  {
    for(y = 0; y < A->len; y++) {
      identifier_resolve(parser, A->v[y], &v);
    }
  }

  identifier_list_free(A);
}

void operation_refresh(graph_parser * restrict parser, identifier_list * restrict A)
{
  moria_vertex *v = 0;
  int y;

  if(A->len == 1)
    identifier_resolve(parser, A->v0, &v);
  else
  {
    for(y = 0; y < A->len; y++)
      identifier_resolve(parser, A->v[y], &v);
  }

  parser->dispatch->refresh_vertex(parser, v);

  identifier_list_free(A);
}

void operation_invalidate(graph_parser * restrict parser, identifier_list * restrict A)
{
  moria_vertex *v = 0;
  int y;

  if(A->len == 1)
    identifier_resolve(parser, A->v0, &v);
  else
  {
    for(y = 0; y < A->len; y++)
      identifier_resolve(parser, A->v[y], &v);
  }

  parser->dispatch->invalidate_vertex(parser, v);

  identifier_list_free(A);
}

void operation_connect(
    graph_parser * restrict parser
  , identifier_list * restrict A
  , identifier_list * restrict B
  , uint32_t attrs
)
{
  moria_vertex *a;
  moria_vertex *b;
  moria_vertex **Alist = 0;
  moria_vertex **Blist = 0;
  int y;

  if(A && A->len == 1 && B && B->len == 1)
  {
    identifier_resolve(parser, A->v0, &a);
    identifier_resolve(parser, B->v0, &b);

    parser->dispatch->connect(parser, a, b, attrs, 0);
  }
  else
  {
    if(A && A->len > 0)
    {
      xrealloc(&Alist, sizeof(*Alist), A->len, 0);
      if(A->len == 1)
      {
        identifier_resolve(parser, A->v0, &Alist[0]);
      }
      else
      {
        for(y = 0; y < A->len; y++)
          identifier_resolve(parser, A->v[y], &Alist[y]);
      }
    }

    if(B && B->len > 0)
    {
      xrealloc(&Blist, sizeof(*Blist), B->len, 0);
      if(B->len == 1)
      {
        identifier_resolve(parser, B->v0, &Blist[0]);
      }
      else
      {
        for(y = 0; y < B->len; y++)
          identifier_resolve(parser, B->v[y], &Blist[y]);
      }
    }

    attrs |= MORIA_EDGE_HYPER;
    parser->dispatch->hyperconnect(parser, Alist, A ? A->len : 0, Blist, B ? B->len : 0, attrs, 0);
  }

  wfree(Alist);
  wfree(Blist);
  identifier_list_free(A);
  identifier_list_free(B);
}

void operation_disconnect(graph_parser * restrict parser, identifier_list * restrict A, identifier_list * restrict B)
{
  moria_vertex *a;
  moria_vertex *b;
  moria_vertex **Alist = 0;
  moria_vertex **Blist = 0;
  moria_edge *e;
  int y;

  if(A && A->len == 1 && B && B->len == 1)
  {
    identifier_lookup(parser, A->v0, &a);
    identifier_lookup(parser, B->v0, &b);

    e = moria_edge_between(a, b);
  }
  else
  {
    if(A->len > 0)
    {
      xrealloc(&Alist, sizeof(*Alist), A->len, 0);
      if(A->len == 1)
      {
        identifier_lookup(parser, A->v0, &Alist[0]);
      }
      else
      {
        for(y = 0; y < A->len; y++)
          identifier_lookup(parser, A->v[y], &Alist[y]);
      }
    }

    if(B->len > 0)
    {
      xrealloc(&Blist, sizeof(*Blist), B->len, 0);
      if(B->len == 1)
      {
        identifier_lookup(parser, B->v0, &Blist[0]);
      }
      else
      {
        for(y = 0; y < B->len; y++)
          identifier_lookup(parser, B->v[y], &Blist[y]);
      }
    }

    e = moria_edge_of(Alist, A->len, Blist, B->len);
  }

  parser->dispatch->disconnect(parser, e);

  wfree(Alist);
  wfree(Blist);
  identifier_list_free(A);
  identifier_list_free(B);
}

//
// API
//

void API graph_parser_create(
    graph_parser ** const parser
  , moria_graph * restrict g
  , llist * restrict vertex_list
  , moria_operations_dispatch * restrict dispatch
  , const attrs32 * restrict vertex_defs
  , const attrs32 * restrict edge_defs
)
{
  graph_parser *p;

  xmalloc(&p, sizeof(*p));

  // graph
  yyu_parser_init(&p->graph_yyu, &graph_vtable);
  yyu_parser_init_tokens(&p->graph_yyu, graph_token_table, graph_TOKEN_TABLE_SIZE);
  yyu_parser_init_states(
      &p->graph_yyu
    , graph_numstates
    , graph_statenumbers
    , graph_statenames
  );

  // operations
  yyu_parser_init(&p->operations_yyu, &operations_vtable);
  yyu_parser_init_tokens(&p->operations_yyu, operations_token_table, operations_TOKEN_TABLE_SIZE);
  yyu_parser_init_states(
      &p->operations_yyu
    , operations_numstates
    , operations_statenumbers
    , operations_statenames
  );

  p->g = g;
  p->dispatch = dispatch;
  map_create(&p->vertex_map);
  if(vertex_list)
  {
    p->vertex_list = vertex_list;
  }
  else
  {
    p->vertex_list = &p->vertices;
  }
  p->vertex_defs = vertex_defs;
  p->edge_defs = edge_defs;
  llist_init_node(&p->vertices);
  llist_init_node(&p->vertex_freelist);
  llist_init_node(&p->edges);
  llist_init_node(&p->edge_freelist);

  *parser = p;
  p = 0;

  graph_parser_xfree(p);
}

void API graph_parser_xfree(graph_parser* const p)
{
  moria_vertex* v;
  moria_edge* e;
  llist *T;

  if(p)
  {
    map_xfree(p->vertex_map);
    yyu_parser_xdestroy(&p->graph_yyu);
    yyu_parser_xdestroy(&p->operations_yyu);

    llist_foreach_safe(&p->vertices, v, owner, T) {
      wfree((void*)v->label);
      wfree(v);
    }
    llist_foreach_safe(&p->vertex_freelist, v, owner, T) {
      wfree((void*)v->label);
      wfree(v);
    }
    llist_foreach_safe(&p->edges, e, owner, T) {
      if((e->attrs & MORIA_EDGE_HYPER)) {
        wfree(e->Alist);
        wfree(e->Blist);
      }
      wfree(e);
    }
    llist_foreach_safe(&p->edge_freelist, e, owner, T) {
      if((e->attrs & MORIA_EDGE_HYPER)) {
        wfree(e->Alist);
        wfree(e->Blist);
      }
      wfree(e);
    }
  }

  wfree(p);
}

void API graph_parser_ixfree(graph_parser ** const p)
{
  graph_parser_xfree(*p);
  *p = 0;
}

int API graph_parser_parse(
    graph_parser * restrict parser
  , char * const restrict text
  , size_t len
)
{
  map_recycle(parser->vertex_map);
  return yyu_parse(&parser->graph_yyu, text, len, "-graph-", 0, 0, 0);
}

int API graph_parser_operations_parse(
    graph_parser * restrict parser
  , char * const restrict text
  , size_t len
)
{
  return yyu_parse(&parser->operations_yyu, text, len, "-ops-", 0, 0, 0);
}

void graph_parser_connect(
    graph_parser * restrict p
  , moria_vertex * restrict A
  , moria_vertex * restrict B
  , uint32_t attrs
  , moria_edge ** restrict er
)
{
  moria_connect_context ctx;
  moria_edge *e;
  int r;

  r = moria_preconnect(&ctx, p->g, A, B, attrs, &e);
  if(r == MORIA_EDGEATTRS)
  {
    RUNTIME_ABORT(); // fail(MORIA_VERTEXEXISTS);
  }
  else if(r == MORIA_EDGEDOWN)
  {
    RUNTIME_ABORT(); // fail(MORIA_LABELEXISTS);
  }
  else if(r == MORIA_EDGEUP)
  {
    RUNTIME_ABORT(); // fail(MORIA_UPEXISTS);
  }
  else if(r == MORIA_NOEDGE)
  {
    graph_parser_create_edge(p, &e, 1, 1);
    moria_connect(&ctx, p->g, e, A, B, attrs);
  }
  if(er) {
    *er = e;
  }
}

void graph_parser_hyperconnect(
    graph_parser * restrict p
  , moria_vertex ** restrict Alist
  , uint16_t Alen
  , moria_vertex ** restrict Blist
  , uint16_t Blen
  , uint32_t attrs
  , moria_edge ** restrict er
)
{
  moria_connect_context ctx;
  moria_edge *e;
  int r;

  if(Alen == 1 && Blen == 1) {
    graph_parser_connect(p, Alist[0], Blist[0], attrs, er);
    return;
  }

  r = moria_preconnect_hyper(&ctx, p->g, Alist, Alen, Blist, Blen, attrs, &e);
  if(r == MORIA_EDGEUP)
  {
    RUNTIME_ABORT(); // fail(MORIA_UPEXISTS);
  }
  else if(r == MORIA_EDGEATTRS)
  {
    RUNTIME_ABORT(); // fail(MORIA_VERTEXEXISTS);
  }
  else if(r == MORIA_NOEDGE)
  {
    graph_parser_create_edge(p, &e, Alen, Blen);
    moria_connect_hyper(&ctx, p->g, e, Alist, Alen, Blist, Blen, attrs);
  }
  if(er) {
    *er = e;
  }
}

static void graph_parser_edge_disconnect(
    graph_parser * restrict p
  , moria_edge * restrict e
)
{
  moria_edge_disconnect(p->g, e);

  llist_delete(e, owner);
  llist_append(&p->edge_freelist, e, owner);
}

void graph_parser_create_vertex(
    graph_parser * restrict p
  , moria_vertex ** const restrict rv
  , uint32_t attrs
  , uint8_t opattrs
  , const char * const restrict label
  , uint16_t label_len
)
{
  moria_vertex *v;
  char *s;

  if((v = llist_shift(&p->vertex_freelist, typeof(*v), owner)) == 0) {
    xmalloc(&v, sizeof(*v));
  }

  xmalloc(&s, label_len + 1);
  memcpy(s, label, label_len);

  moria_vertex_initw(v, p->g, attrs, s, label_len);
  llist_append(&p->vertices, v, owner);

  *rv = v;
}

void graph_parser_create_edge(
    graph_parser * restrict p
  , moria_edge ** const restrict er
  , uint16_t Alen
  , uint16_t Blen
)
{
  moria_edge *e;

  if((e = llist_shift(&p->edge_freelist, typeof(*e), owner)) == 0) {
    xmalloc(&e, sizeof(*e));
  }

  if(Alen != 1 || Blen != 1)
  {
    xmalloc(&e->Alist, sizeof(*e->Alist) * Alen);
    xmalloc(&e->Blist, sizeof(*e->Blist) * Blen);
  }

  moria_edge_init(e, p->g);
  llist_append(&p->edges, e, owner);
  *er = e;
}

moria_operations_dispatch * API graph_operations_dispatch = (moria_operations_dispatch[]) {{
    .connect = graph_parser_connect
  , .hyperconnect = graph_parser_hyperconnect
  , .disconnect = graph_parser_edge_disconnect
  , .create_vertex = graph_parser_create_vertex
}};
