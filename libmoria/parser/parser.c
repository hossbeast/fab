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

#include "xlinux/KERNEL.errtab.h"
#include "valyria/map.h"
#include "valyria/hashtable.h"
#include "valyria/dictionary.h"
#include "valyria/llist.h"

/* flex and bison do not agree on these names */
#define YYSTYPE struct graph_yystype
#define YYLTYPE yyu_location

/* the token enums shadow each other exclude all but one set */
#define OPERATIONS_YYTOKENTYPE

#include "parser.internal.h"
#include "MORIA.errtab.h"
#include "graph.tab.h"
#include "graph.tokens.h"
#include "graph.lex.h"
#include "graph.states.h"
#include "operations.tab.h"
#include "operations.tokens.h"
#include "operations.lex.h"
#include "operations.states.h"

#include "operations.h"
#include "logging.internal.h"
#include "edge.h"

#include "common/attrs.h"

static YYU_VTABLE(graph_vtable, graph, graph);
static YYU_VTABLE(operations_vtable, operations, operations);

//
// private
//

static xapi identifier_lookup(graph_parser * restrict parser, identifier * id, moria_vertex ** restrict rv)
{
  enter;

  moria_vertex *v = 0;
  identifier *idv;
  moria_vertex *p;

  *rv = 0;
  llist_foreach(parser->vertex_list, v, owner) {
    if(memncmp(v->label, v->label_len, id->label, id->label_len) != 0) {
      continue;
    }

    p = v;
//    if(!p->up_identity || !p->up_identity->A) {
//      break;
//    }
//    p = p->up_identity->A;

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
      fail(MORIA_AMBIGUOUS);
    }
    *rv = v;
  }

//printf("lookup ");
//for(idv = id; idv; idv = idv->next) {
//  printf("/%.*s", idv->label_len, idv->label);
//}
//if(*rv) {
//  printf(" -> %p %.*s\n", *rv, (int)(*rv)->label_len, (*rv)->label);
//} else {
//  printf(" -> \n");
//}

  finally : coda;
}

static xapi identifier_resolve(
    graph_parser * restrict parser
  , identifier * id
  , moria_vertex ** restrict rv
)
{
  enter;

  moria_vertex *v = 0;
  moria_vertex *pv = 0;

  v = 0;
  fatal(identifier_lookup, parser, id, &v);
  if(v)
  {
    *rv = v;
    goto XAPI_FINALLY;
  }

  if(id->next) {
    fatal(identifier_resolve, parser, id->next, &pv);
  }

  fatal(parser->dispatch->create_vertex, parser, &v, id->attrs, id->opattrs, id->label, id->label_len);
//printf("create %.*s -> %p %.*s\n", (int)id->label_len, id->label, v, (int)v->label_len, v->label);
  if(pv) {
    fatal(parser->dispatch->connect, parser, pv, v, MORIA_EDGE_IDENTITY, 0);
  }
  *rv = v;

  finally : coda;
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

xapi operation_vertex(graph_parser * restrict parser, identifier_list *A)
{
  enter;

  int y;
  moria_vertex *v;

  if(A->len == 1)
  {
//printf("create-vertex");
//identifier *id = A->v0;
//while(id) {
//  printf(" -> %.*s", (int)id->label_len, id->label);
//  id = id->next;
//}
//printf("\n");

    fatal(identifier_resolve, parser, A->v0, &v);
  }
  else
  {
    for(y = 0; y < A->len; y++) {
//printf("create-vertex");
//identifier *id = A->v[y];
//while(id) {
//  printf(" -> %.*s", (int)id->label_len, id->label);
//  id = id->next;
//}
//printf("\n");

      fatal(identifier_resolve, parser, A->v[y], &v);
    }
  }

finally:
  identifier_list_free(A);
coda;
}

xapi operation_refresh(graph_parser * restrict parser, identifier_list * restrict A)
{
  enter;

  moria_vertex *v;
  int y;

  if(A->len == 1)
    fatal(identifier_resolve, parser, A->v0, &v);
  else
  {
    for(y = 0; y < A->len; y++)
      fatal(identifier_resolve, parser, A->v[y], &v);
  }

  fatal(parser->dispatch->refresh_vertex, parser, v);

finally:
  identifier_list_free(A);
coda;
}

xapi operation_invalidate(graph_parser * restrict parser, identifier_list * restrict A)
{
  enter;

  moria_vertex *v;
  int y;

  if(A->len == 1)
    fatal(identifier_resolve, parser, A->v0, &v);
  else
  {
    for(y = 0; y < A->len; y++)
      fatal(identifier_resolve, parser, A->v[y], &v);
  }

  fatal(parser->dispatch->invalidate_vertex, parser, v);

finally:
  identifier_list_free(A);
coda;
}

xapi operation_connect(
    graph_parser * restrict parser
  , identifier_list * restrict A
  , identifier_list * restrict B
  , uint32_t attrs
)
{
  enter;

  moria_vertex *a;
  moria_vertex *b;
  moria_vertex **Alist = 0;
  moria_vertex **Blist = 0;
  int y;

  if(A && A->len == 1 && B && B->len == 1)
  {
    fatal(identifier_resolve, parser, A->v0, &a);
    fatal(identifier_resolve, parser, B->v0, &b);

    fatal(parser->dispatch->connect, parser, a, b, attrs, 0);
  }
  else
  {
    if(A && A->len > 0)
    {
      fatal(xrealloc, &Alist, sizeof(*Alist), A->len, 0);
      if(A->len == 1)
      {
        fatal(identifier_resolve, parser, A->v0, &Alist[0]);
      }
      else
      {
        for(y = 0; y < A->len; y++)
          fatal(identifier_resolve, parser, A->v[y], &Alist[y]);
      }
    }

    if(B && B->len > 0)
    {
      fatal(xrealloc, &Blist, sizeof(*Blist), B->len, 0);
      if(B->len == 1)
      {
        fatal(identifier_resolve, parser, B->v0, &Blist[0]);
      }
      else
      {
        for(y = 0; y < B->len; y++)
          fatal(identifier_resolve, parser, B->v[y], &Blist[y]);
      }
    }

    attrs |= MORIA_EDGE_HYPER;
    fatal(parser->dispatch->hyperconnect, parser, Alist, A ? A->len : 0, Blist, B ? B->len : 0, attrs, 0);
  }

finally:
  wfree(Alist);
  wfree(Blist);
  identifier_list_free(A);
  identifier_list_free(B);
coda;
}

xapi operation_disconnect(graph_parser * restrict parser, identifier_list * restrict A, identifier_list * restrict B)
{
  enter;

  moria_vertex *a;
  moria_vertex *b;
  moria_vertex **Alist = 0;
  moria_vertex **Blist = 0;
  moria_edge *e;
  int y;

  if(A && A->len == 1 && B && B->len == 1)
  {
    fatal(identifier_lookup, parser, A->v0, &a);
    fatal(identifier_lookup, parser, B->v0, &b);

    e = moria_edge_between(a, b);
  }
  else
  {
    if(A->len > 0)
    {
      fatal(xrealloc, &Alist, sizeof(*Alist), A->len, 0);
      if(A->len == 1)
      {
        fatal(identifier_lookup, parser, A->v0, &Alist[0]);
      }
      else
      {
        for(y = 0; y < A->len; y++)
          fatal(identifier_lookup, parser, A->v[y], &Alist[y]);
      }
    }

    if(B->len > 0)
    {
      fatal(xrealloc, &Blist, sizeof(*Blist), B->len, 0);
      if(B->len == 1)
      {
        fatal(identifier_lookup, parser, B->v0, &Blist[0]);
      }
      else
      {
        for(y = 0; y < B->len; y++)
          fatal(identifier_lookup, parser, B->v[y], &Blist[y]);
      }
    }

    e = moria_edge_of(Alist, A->len, Blist, B->len);
  }

  fatal(parser->dispatch->disconnect, parser, e);

finally:
  wfree(Alist);
  wfree(Blist);
  identifier_list_free(A);
  identifier_list_free(B);
coda;
}

//
// API
//

xapi API graph_parser_create(
    graph_parser ** const parser
  , moria_graph * restrict g
  , llist * restrict vertex_list
  , moria_operations_dispatch * restrict dispatch
  , const attrs32 * restrict vertex_defs
  , const attrs32 * restrict edge_defs
)
{
  enter;

  graph_parser *p;

  fatal(xmalloc, &p, sizeof(*p));

  // graph
  fatal(yyu_parser_init, &p->graph_yyu, &graph_vtable, MORIA_SYNTAX);
  fatal(yyu_parser_init_tokens, &p->graph_yyu, graph_token_table, graph_TOKEN_TABLE_SIZE);
  fatal(yyu_parser_init_states
    , &p->graph_yyu
    , graph_numstates
    , graph_statenumbers
    , graph_statenames
  );

  // operations
  fatal(yyu_parser_init, &p->operations_yyu, &operations_vtable, MORIA_SYNTAX);
  fatal(yyu_parser_init_tokens, &p->operations_yyu, operations_token_table, operations_TOKEN_TABLE_SIZE);
  fatal(yyu_parser_init_states
    , &p->operations_yyu
    , operations_numstates
    , operations_statenumbers
    , operations_statenames
  );

#if DEBUG || DEVEL || XUNIT
  p->graph_yyu.logs = L_MORIA | L_GRAPH;
  p->operations_yyu.logs = L_MORIA | L_GRAPH;
#endif

  p->g = g;
  p->dispatch = dispatch;
  fatal(map_create, &p->vertex_map);
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

finally:
  fatal(graph_parser_xfree, p);
coda;
}

xapi API graph_parser_xfree(graph_parser* const p)
{
  enter;

  moria_vertex* v;
  moria_edge* e;
  llist *T;

  if(p)
  {
    fatal(map_xfree, p->vertex_map);
//    fatal(hashtable_xfree, p->mm_owned);

    fatal(yyu_parser_xdestroy, &p->graph_yyu);
    fatal(yyu_parser_xdestroy, &p->operations_yyu);

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

  finally : coda;
}

xapi API graph_parser_ixfree(graph_parser ** const p)
{
  enter;

  fatal(graph_parser_xfree, *p);
  *p = 0;

  finally : coda;
}

xapi API graph_parser_parse(
    graph_parser * restrict parser
  , char * const restrict text
  , size_t len
)
{
  enter;

  fatal(map_recycle, parser->vertex_map);
  fatal(yyu_parse, &parser->graph_yyu, text, len, "-graph-", 0, 0, 0);

  finally : coda;
}

xapi API graph_parser_operations_parse(
    graph_parser * restrict parser
  , char * const restrict text
  , size_t len
)
{
  enter;

  fatal(yyu_parse, &parser->operations_yyu, text, len, "-ops-", 0, 0, 0);

  finally : coda;
}

xapi graph_parser_connect(
    graph_parser * restrict p
  , moria_vertex * restrict A
  , moria_vertex * restrict B
  , uint32_t attrs
  , moria_edge ** restrict er
)
{
  enter;

  moria_connect_context ctx;
  moria_edge *e;
  int r;

  r = moria_preconnect(&ctx, p->g, A, B, attrs, &e);
//printf("CONNECT %p '%s' : 0x%08x : %p '%s' : %d\n", A, A->label, attrs, B, B->label, r);
  if(r == MORIA_EDGEATTRS)
  {
    fail(MORIA_VERTEXEXISTS);
  }
  else if(r == MORIA_EDGEDOWN)
  {
    fail(MORIA_LABELEXISTS);
  }
  else if(r == MORIA_EDGEUP)
  {
    fail(MORIA_UPEXISTS);
  }
  else if(r == MORIA_NOEDGE)
  {
    fatal(graph_parser_create_edge, p, &e, 1, 1);
    moria_connect(&ctx, p->g, e, A, B, attrs);
  }
  if(er) {
    *er = e;
  }

  finally : coda;
}

xapi graph_parser_hyperconnect(
    graph_parser * restrict p
  , moria_vertex ** restrict Alist
  , uint16_t Alen
  , moria_vertex ** restrict Blist
  , uint16_t Blen
  , uint32_t attrs
  , moria_edge ** restrict er
)
{
  enter;

  moria_connect_context ctx;
  moria_edge *e;
  int r;

  if(Alen == 1 && Blen == 1) {
    fatal(graph_parser_connect, p, Alist[0], Blist[0], attrs, er);
    goto XAPI_FINALIZE;
  }

  r = moria_preconnect_hyper(&ctx, p->g, Alist, Alen, Blist, Blen, attrs, &e);

//  int x;
//printf("HYPERCONNECT");
//  for(x = 0; x < Alen; x++) {
//    printf(" %p '%s'", Alist[x], Alist[x]->label);
//  }
//printf(" : 0x%08x :", attrs);
//  for(x = 0; x < Blen; x++) {
//    printf(" %p '%s'", Blist[x], Blist[x]->label);
//  }
//printf(" : %d\n", r);

  if(r == MORIA_EDGEUP)
  {
    fail(MORIA_UPEXISTS);
  }
  else if(r == MORIA_EDGEATTRS)
  {
    fail(MORIA_VERTEXEXISTS);
  }
  else if(r == MORIA_NOEDGE)
  {
    fatal(graph_parser_create_edge, p, &e, Alen, Blen);
    moria_connect_hyper(&ctx, p->g, e, Alist, Alen, Blist, Blen, attrs);
  }
  if(er) {
    *er = e;
  }

  finally : coda;
}

static xapi graph_parser_edge_disconnect(
    graph_parser * restrict p
  , moria_edge * restrict e
)
{
  enter;

  fatal(moria_edge_disconnect, p->g, e);

  llist_delete(e, owner);
  llist_append(&p->edge_freelist, e, owner);

  finally : coda;
}

xapi graph_parser_create_vertex(
    graph_parser * restrict p
  , moria_vertex ** const restrict rv
  , uint32_t attrs
  , uint8_t opattrs
  , const char * const restrict label
  , uint16_t label_len
)
{
  enter;

  moria_vertex *v;
  char *s;

  if((v = llist_shift(&p->vertex_freelist, typeof(*v), owner)) == 0) {
    fatal(xmalloc, &v, sizeof(*v));
  }

  fatal(xmalloc, &s, label_len + 1);
  memcpy(s, label, label_len);

  moria_vertex_initw(v, p->g, attrs, s, label_len);
  llist_append(&p->vertices, v, owner);

  *rv = v;

  finally : coda;
}

xapi graph_parser_create_edge(
    graph_parser * restrict p
  , moria_edge ** const restrict er
  , uint16_t Alen
  , uint16_t Blen
)
{
  enter;

  moria_edge *e;

  if((e = llist_shift(&p->edge_freelist, typeof(*e), owner)) == 0) {
    fatal(xmalloc, &e, sizeof(*e));
  }

//printf("edge %p Alen %hu Blen %hu\n", e, Alen, Blen);

  if(Alen != 1 || Blen != 1)
  {
    fatal(xmalloc, &e->Alist, sizeof(*e->Alist) * Alen);
    fatal(xmalloc, &e->Blist, sizeof(*e->Blist) * Blen);
  }

  moria_edge_init(e, p->g);
  llist_append(&p->edges, e, owner);
  *er = e;

  finally : coda;
}

moria_operations_dispatch * API graph_operations_dispatch = (moria_operations_dispatch[]) {{
    .connect = graph_parser_connect
  , .hyperconnect = graph_parser_hyperconnect
  , .disconnect = graph_parser_edge_disconnect
  , .create_vertex = graph_parser_create_vertex
}};

#if 0
xapi API operations_parser_create(operations_parser ** const parser)
{
  enter;

  fatal(xmalloc, parser, sizeof(**parser));
#if DEBUG || DEVEL || XUNIT
  (*parser)->yyu.logs = L_MORIA | L_OPERATIONS;
#endif

  finally : coda;
}

xapi API operations_parser_xfree(operations_parser* const p)
{
  enter;

  if(p)
  {
    fatal(yyu_parser_xdestroy, &p->yyu);
  }

  wfree(p);

  finally : coda;
}

xapi API operations_parser_ixfree(operations_parser ** const p)
{
  enter;

  fatal(operations_parser_xfree, *p);
  *p = 0;

  finally : coda;
}

xapi API operations_parser_operations_create(list ** restrict operations)
{
  enter;

  fatal(list_createx, operations, 0, 0, operation_free, 0);

  finally : coda;
}

xapi API operations_perform(moria_graph * restrict g, multimap * restrict mm, operations_dispatch * restrict dispatch, list * restrict ops)
{
  enter;

  moria_vertex *A = 0;
  moria_vertex *B = 0;
  moria_vertex **Alist = 0;
  moria_vertex **Blist = 0;
  moria_edge *e;
  operation *op;
  void * mm_tmp = 0;
  int x;
  int y;

  if(dispatch->setup)
  {
    fatal(dispatch->setup, dispatch, g);
  }

  for(x = 0; x < ops->size; x++)
  {
    op = list_get(ops, x);

    if(op->type == GRAPH_OPERATION_VERTEX)
    {
      if(op->A->len == 1)
        fatal(identifier_resolve, parser, op->A->v0, &mm_tmp, &A);
      else
      {
        for(y = 0; y < op->A->len; y++)
          fatal(identifier_resolve, parser, op->A->v[y], &mm_tmp, &A);
      }
    }
    else if(op->type == GRAPH_OPERATION_REFRESH)
    {
      if(op->A->len == 1)
        fatal(identifier_resolve, parser, op->A->v0, &mm_tmp, &A);
      else
      {
        for(y = 0; y < op->A->len; y++)
          fatal(identifier_resolve, parser, op->A->v[y], &mm_tmp, &A);
      }

      fatal(dispatch->refresh_vertex, g, A);
    }
    else if(op->type == GRAPH_OPERATION_INVALIDATE)
    {
      if(op->A->len == 1)
        fatal(identifier_resolve, parser, op->A->v0, &mm_tmp, &A);
      else
      {
        for(y = 0; y < op->A->len; y++)
          fatal(identifier_resolve, parser, op->A->v[y], &mm_tmp, &A);
      }

      fatal(dispatch->invalidate_vertex, g, A);
    }
    else if(op->type == GRAPH_OPERATION_CONNECT)
    {
      if(op->A && op->A->len == 1 && op->B && op->B->len == 1)
      {
        fatal(identifier_resolve, parser, op->A->v0, &mm_tmp, &A);
        fatal(identifier_resolve, parser, op->B->v0, &mm_tmp, &B);

        fatal(dispatch->connect, g, A, B, op->attrs, 0, 0);
      }
      else
      {
        if(op->A && op->A->len > 0)
        {
          fatal(xrealloc, &Alist, sizeof(*Alist), op->A->len, 0);
          if(op->A->len == 1)
          {
            fatal(identifier_resolve, parser, op->A->v0, &mm_tmp, &Alist[0]);
          }
          else
          {
            for(y = 0; y < op->A->len; y++)
              fatal(identifier_resolve, parser, op->A->v[y], &mm_tmp, &Alist[y]);
          }
        }

        if(op->B && op->B->len > 0)
        {
          fatal(xrealloc, &Blist, sizeof(*Blist), op->B->len, 0);
          if(op->B->len == 1)
          {
            fatal(identifier_resolve, parser, op->B->v0, &mm_tmp, &Blist[0]);
          }
          else
          {
            for(y = 0; y < op->B->len; y++)
              fatal(identifier_resolve, parser, op->B->v[y], &mm_tmp, &Blist[y]);
          }
        }

        fatal(dispatch->hyperconnect, g, Alist, op->A ? op->A->len : 0, Blist, op->B ? op->B->len : 0, op->attrs, 0, 0);
      }
    }
    else if(op->type == GRAPH_OPERATION_DISCONNECT)
    {
      if(op->A && op->A->len == 1 && op->B && op->B->len == 1)
      {
        fatal(identifier_lookup, parser, op->A->v0, &A);
        fatal(identifier_lookup, parser, op->B->v0, &B);

        e = moria_edge_between(A, B);
      }
      else
      {
        if(op->A && op->A->len > 0)
        {
          fatal(xrealloc, &Alist, sizeof(*Alist), op->A->len, 0);
          if(op->A->len == 1)
          {
            fatal(identifier_lookup, parser, op->A->v0, &Alist[0]);
          }
          else
          {
            for(y = 0; y < op->A->len; y++)
              fatal(identifier_lookup, parser, op->A->v[y], &Alist[y]);
          }
        }

        if(op->B && op->B->len > 0)
        {
          fatal(xrealloc, &Blist, sizeof(*Blist), op->B->len, 0);
          if(op->B->len == 1)
          {
            fatal(identifier_lookup, parser, op->B->v0, &Blist[0]);
          }
          else
          {
            for(y = 0; y < op->B->len; y++)
              fatal(identifier_lookup, parser, op->B->v[y], &Blist[y]);
          }
        }

        e = moria_edge_of(Alist, op->A ? op->A->len : 0, Blist, op->B ? op->B->len : 0);
      }
      fatal(dispatch->disconnect, g, e);
    }
    else
    {
      RUNTIME_ABORT();
    }
  }

finally:
  wfree(mm_tmp);
  wfree(Alist);
  wfree(Blist);

  if(dispatch->cleanup)
  {
    fatal(dispatch->cleanup, dispatch, g);
  }
coda;
}

const char * operation_type_name(operation_type type)
{
  if(type <= GRAPH_OPERATION_RANGE_BEFORE || type >= GRAPH_OPERATION_RANGE_AFTER)
    return NULL;

  return operation_type_names[type];
}

//static void free_value(void * value)
//{
//  free(*(char**)value);
//}

xapi API graph_parser_graph_create(moria_graph ** restrict g, uint32_t identity)
{
  enter;

//  xproxy(graph_createx, g, identity, sizeof(void*), 0, free_value, 0, 0, 0);

  finally : coda;
}
#endif


#if 0

static xapi __attribute__((nonnull(1))) graph_lookup_identifier(void * restrict _context, const char ** restrict label, uint16_t * restrict label_len)
{
  enter;

  fatal(graph_lookup_identifier_attrs, _context, label, label_len, 0);

  finally : coda;
}


static xapi identifier_lookup(graph_parser * restrict parser, identifier * id, moria_vertex ** restrict v)
{
  enter;

  moria_vertex * vertices[2];
  int r;
  graph_lookup_identifier_context context;

  context.first = id;
  fatal(moria_graph_lookup, parser->g, parser->mm, graph_lookup_identifier, 0, &context, vertices, &r);

  *v = 0;
  if(r == 1)
  {
    *v = vertices[0];
  }
  else if(r == 2)
  {
    fail(MORIA_AMBIGUOUS);
  }

  finally : coda;
}
#endif

#if 0
typedef struct {
  identifier * id;
  identifier * first;
} graph_lookup_identifier_context;

static xapi __attribute__((nonnull(1))) graph_lookup_identifier_attrs(
    void * restrict _context
  , const char ** restrict label
  , uint16_t * restrict label_len
  , uint32_t * restrict attrs
  , uint8_t * restrict opattrs
)
{
  enter;

  graph_lookup_identifier_context * context = _context;

  if(label)
  {
    *label = 0;
    if(context->id)
    {
      *label = context->id->label;
      *label_len = context->id->label_len;
      //if(attrs)
      *attrs = context->id->attrs;
      *opattrs = context->id->opattrs;

      context->id = context->id->next;
    }
  }
  else
  {
    context->id = context->first;
  }

  finally : coda;
}
#endif
