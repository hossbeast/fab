/* cOPYRIGHT (C) 2012-2015 Todd Freed <todd.freed@gmail.com>

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
#include "types.h"

#include "xlinux/KERNEL.errtab.h"
#include "xapi/SYS.errtab.h"
#include "valyria/dictionary.h"
#include "valyria/map.h"

/* flex and bison do not agree on these names */
#define YYSTYPE OPERATIONS_YYSTYPE
#define YYLTYPE yyu_location

#include "internal.h"
#include "operations.internal.h"
#include "operations.tab.h"
#include "operations.tokens.h"
#include "operations.lex.h"
#include "operations.states.h"
#include "MORIA.errtab.h"
#include "edge.internal.h"
#include "graph.internal.h"
#include "logging.internal.h"
#include "vertex.internal.h"
#include "edge.internal.h"

#include "macros.h"
#include "attrs.h"

static const char * operation_type_names[] = {
#undef DEF
#define DEF(x) [x] = #x,
  GRAPH_OPERATION_TABLE
};

//
// private
//

typedef struct {
  identifier * id;
  identifier * first;
} graph_lookup_identifier_context;

static xapi __attribute__((nonnull(1))) graph_lookup_identifier_attrs(
    void * restrict _context
  , const char ** restrict label
  , uint16_t * restrict label_len
  , uint32_t * restrict attrs
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
      if(attrs)
        *attrs = context->id->attrs;

      context->id = context->id->next;
    }
  }
  else
  {
    context->id = context->first;
  }

  finally : coda;
}

static xapi __attribute__((nonnull(1))) graph_lookup_identifier(void * restrict _context, const char ** restrict label, uint16_t * restrict label_len)
{
  enter;

  fatal(graph_lookup_identifier_attrs, _context, label, label_len, 0);

  finally : coda;
}

static xapi identifier_resolve(graph * restrict g, identifier * id, void * mm_tmp, vertex ** restrict v)
{
  enter;

  vertex * vertices[2];
  int r;
  graph_lookup_identifier_context context;

  context.first = id;
  fatal(graph_lookup, g, graph_lookup_identifier, 0, &context, mm_tmp, vertices, &r);

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

static xapi identifier_create(
    graph * restrict g
  , const operations_dispatch * restrict dispatch
  , identifier * id
  , void * mm_tmp
  , vertex ** restrict v
)
{
  enter;

  const char * label = 0;
  uint16_t label_len = 0;
  uint32_t attrs = 0;
  vertex * pv = 0;

  // attempt lookup
  fatal(identifier_resolve, g, id, mm_tmp, v);

  if(!*v)
  {
    graph_lookup_identifier_context context = { id : id };
    fatal(graph_lookup_identifier_attrs, &context, &label, &label_len, &attrs);
    if(label)
    {
      if(context.id)
        fatal(identifier_create, g, dispatch, context.id, mm_tmp, &pv);

      fatal(dispatch->create_vertex, v, g, attrs, id->op_attrs, label, label_len);
      if(pv)
        fatal(dispatch->connect, g, pv, *v, g->identity, 0, 0);
    }
  }

  finally : coda;
}

//
// public
//

void identifier_list_free(identifier_list * id)
{
  int x;

  if(!id)
    return;

  if(id->len == 1)
  {
    identifier_free(id->v0);
    wfree(id);
    return;
  }

  for(x = 0; x < id->len; x++)
    identifier_free(id->v[x]);
  wfree(id->v);
  wfree(id);
}

void identifier_free(identifier * id)
{
  if(id)
    identifier_free(id->next);

  wfree(id);
}

void operation_free(operation * op)
{
  if(op)
  {
    identifier_list_free(op->A);
    identifier_list_free(op->B);
  }

  free(op);
}

const char * operation_type_name(operation_type type)
{
  if(type <= GRAPH_OPERATION_RANGE_BEFORE || type >= GRAPH_OPERATION_RANGE_AFTER)
    return NULL;

  return operation_type_names[type];
}

static yyu_vtable vtable = {
    yy_scan_bytes : operations_yy_scan_bytes
  , yy_delete_buffer : operations_yy_delete_buffer
  , yyset_extra : operations_yyset_extra
  , yyparse : operations_yyparse
  , yylex_init : operations_yylex_init
  , yylex_destroy : operations_yylex_destroy
};

//
// API
//

API xapi operations_parser_create(operations_parser ** const parser)
{
  enter;

  fatal(xmalloc, parser, sizeof(**parser));
#if DEBUG || DEVEL || XUNIT
  (*parser)->yyu.logs = L_MORIA | L_OPERATIONS;
#endif

  fatal(yyu_parser_init, &(*parser)->yyu, &vtable, MORIA_SYNTAX);

  fatal(yyu_parser_init_tokens
    , &(*parser)->yyu
    , operations_numtokens
    , operations_mintoken
    , operations_maxtoken
    , operations_tokenindexes
    , operations_tokennumbers
    , operations_tokennames
    , operations_tokenstrings
    , operations_tokenstring_tokens
  );

  fatal(yyu_parser_init_states
    , &(*parser)->yyu
    , operations_numstates
    , operations_statenumbers
    , operations_statenames
  );

  finally : coda;
}

API xapi operations_parser_xfree(operations_parser* const p)
{
  enter;

  if(p)
  {
    fatal(yyu_parser_xdestroy, &p->yyu);
  }

  wfree(p);

  finally : coda;
}

API xapi operations_parser_ixfree(operations_parser ** const p)
{
  enter;

  fatal(operations_parser_xfree, *p);
  *p = 0;

  finally : coda;
}

API xapi operations_parser_operations_create(list ** restrict operations)
{
  enter;

  fatal(list_createx, operations, 0, 0, operation_free, 0);

  finally : coda;
}

API xapi operations_parser_parse(
    operations_parser * restrict parser
  , graph * restrict g
  , char * const restrict text
  , size_t len
  , list * restrict operations
)
{
  enter;

  fatal(list_recycle, operations);

  parser->li = operations;
  parser->vertex_defs = g->vertex_defs;
  parser->edge_defs = g->edge_defs;

  fatal(yyu_parse, &parser->yyu, text, len, "-ops-", 0, 0, 0);

  finally : coda;
}

API xapi operations_perform(graph * restrict g, operations_dispatch * restrict dispatch, list * restrict ops)
{
  enter;

  vertex *A = 0;
  vertex *B = 0;
  vertex **Alist = 0;
  vertex **Blist = 0;
  edge *e;
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
        fatal(identifier_create, g, dispatch, op->A->v0, &mm_tmp, &A);
      else
      {
        for(y = 0; y < op->A->len; y++)
          fatal(identifier_create, g, dispatch, op->A->v[y], &mm_tmp, &A);
      }
    }
    else if(op->type == GRAPH_OPERATION_REFRESH)
    {
      if(op->A->len == 1)
        fatal(identifier_create, g, dispatch, op->A->v0, &mm_tmp, &A);
      else
      {
        for(y = 0; y < op->A->len; y++)
          fatal(identifier_create, g, dispatch, op->A->v[y], &mm_tmp, &A);
      }

      fatal(dispatch->refresh_vertex, g, A);
    }
    else if(op->type == GRAPH_OPERATION_INVALIDATE)
    {
      if(op->A->len == 1)
        fatal(identifier_create, g, dispatch, op->A->v0, &mm_tmp, &A);
      else
      {
        for(y = 0; y < op->A->len; y++)
          fatal(identifier_create, g, dispatch, op->A->v[y], &mm_tmp, &A);
      }

      fatal(dispatch->invalidate_vertex, g, A);
    }
    else if(op->type == GRAPH_OPERATION_CONNECT)
    {
      if(op->A && op->A->len == 1 && op->B && op->B->len == 1)
      {
        fatal(identifier_create, g, dispatch, op->A->v0, &mm_tmp, &A);
        fatal(identifier_create, g, dispatch, op->B->v0, &mm_tmp, &B);

        fatal(dispatch->connect, g, A, B, op->attrs, 0, 0);
      }
      else
      {
        if(op->A && op->A->len > 0)
        {
          fatal(xrealloc, &Alist, sizeof(*Alist), op->A->len, 0);
          if(op->A->len == 1)
          {
            fatal(identifier_create, g, dispatch, op->A->v0, &mm_tmp, &Alist[0]);
          }
          else
          {
            for(y = 0; y < op->A->len; y++)
              fatal(identifier_create, g, dispatch, op->A->v[y], &mm_tmp, &Alist[y]);
          }
        }

        if(op->B && op->B->len > 0)
        {
          fatal(xrealloc, &Blist, sizeof(*Blist), op->B->len, 0);
          if(op->B->len == 1)
          {
            fatal(identifier_create, g, dispatch, op->B->v0, &mm_tmp, &Blist[0]);
          }
          else
          {
            for(y = 0; y < op->B->len; y++)
              fatal(identifier_create, g, dispatch, op->B->v[y], &mm_tmp, &Blist[y]);
          }
        }

        fatal(dispatch->hyperconnect, g, Alist, op->A ? op->A->len : 0, Blist, op->B ? op->B->len : 0, op->attrs, 0, 0);
      }
    }
    else if(op->type == GRAPH_OPERATION_DISCONNECT)
    {
      if(op->A && op->A->len == 1 && op->B && op->B->len == 1)
      {
        fatal(identifier_resolve, g, op->A->v0, &mm_tmp, &A);
        fatal(identifier_resolve, g, op->B->v0, &mm_tmp, &B);

        e = edge_between(A, B);
      }
      else
      {
        if(op->A && op->A->len > 0)
        {
          fatal(xrealloc, &Alist, sizeof(*Alist), op->A->len, 0);
          if(op->A->len == 1)
          {
            fatal(identifier_resolve, g, op->A->v0, &mm_tmp, &Alist[0]);
          }
          else
          {
            for(y = 0; y < op->A->len; y++)
              fatal(identifier_resolve, g, op->A->v[y], &mm_tmp, &Alist[y]);
          }
        }

        if(op->B && op->B->len > 0)
        {
          fatal(xrealloc, &Blist, sizeof(*Blist), op->B->len, 0);
          if(op->B->len == 1)
          {
            fatal(identifier_resolve, g, op->B->v0, &mm_tmp, &Blist[0]);
          }
          else
          {
            for(y = 0; y < op->B->len; y++)
              fatal(identifier_resolve, g, op->B->v[y], &mm_tmp, &Blist[y]);
          }
        }

        e = edge_of(Alist, op->A ? op->A->len : 0, Blist, op->B ? op->B->len : 0);
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

API xapi graph_connect(graph * const restrict g, vertex * const restrict Ax, vertex * const restrict Bx, uint32_t attrs, edge ** restrict er, bool * restrict r)
{
  enter;

  fatal(graph_connect_replace, g, Ax, Bx, attrs, er, r, 0);

  finally : coda;
}

API xapi graph_connect_replace(
    struct graph * const restrict g
  , struct vertex * Ax
  , struct vertex * Bx
  , uint32_t attrs
  , struct edge ** restrict er
  , bool * restrict r
  , struct vertex ** oldBx
)
{
  enter;

  const vertex * X;
  const vertex * Y;

/*
  if inserting on identity
    if label is found  -> EXISTS (would violate the identity relation)
    if vertex is found -> EXISTS (cant convert from vertex edge to identity edge)

  if inserting on vertex
    if label is found  -> EXISTS (for this vertex? cant convert from identity edge to vertex edge)
    if vertex is found -> EXISTS (edge already exists)
*/

  vertex_t * A = containerof(Ax, vertex_t, vx);
  vertex_t * B = containerof(Bx, vertex_t, vx);

  edge_t * e = 0;
  edge_t * existing_edge = 0;
  edge_key key;
  rbnode * rbn;

  // search by label (identity)
  key = (typeof(key)) {
      attrs : MORIA_EDGE_IDENTITY
    , Blist : (vertex*[]) { &B->vx }
    , Blen : 1
  };
  if((rbn = rbtree_lookup_node(&A->down, &key, edge_cmp_key_down))) {
    e = containerof(rbn, edge_t, rbn_down);
  }

  // idempotency
  if(e && (e->attrs & ~(MORIA_EDGE_IDENTITY | MORIA_EDGE_HYPER)) == attrs)
  {
    X = e->B;
    while(X->attrs & MORIA_VERTEX_LINK) {
      X = X->ref;
    }

    Y = &B->vx;
    while(Y->attrs & MORIA_VERTEX_LINK) {
      Y = Y->ref;
    }

    if(X == Y)
    {
      if(er) *er = &e->ex;
      if(r) *r = false;
      goto XAPI_FINALIZE;
    }
  }

  if(e && g->identity && attrs == g->identity)
  {
    if(oldBx) // permit replacement of B
    {
      existing_edge = e;
    }
    else
    {
      xapi_info_pushf("edge", "%s:0x%x:%s", e->A->label, e->attrs, e->B->label);
      fail(MORIA_LABELEXISTS);
    }
  }

  // different attrs for this edge
  else if(e && e->B == &B->vx)
  {
    xapi_info_pushf("edge", "%s:0x%x:%s", e->A->label, e->attrs, e->B->label);
    fail(MORIA_VERTEXEXISTS);
  }

  // search by vertex (non-identity)
  key.attrs &= ~MORIA_EDGE_IDENTITY;
  e = 0;
  if((rbn = rbtree_lookup_node(&A->down, &key, edge_cmp_key_down))) {
    e = containerof(rbn, edge_t, rbn_down);
  }

  // idempotency
  if(e && (e->attrs & ~(MORIA_EDGE_IDENTITY | MORIA_EDGE_HYPER)) == attrs)
  {
    if(er) *er = &e->ex;
    if(r) *r = false;
    goto XAPI_FINALIZE;
  }

  // different attrs for this edge
  if(e)
  {
    xapi_info_pushf("edge", "%s:0x%x:%s", e->A->label, e->attrs, e->B->label);
    fail(MORIA_VERTEXEXISTS);
  }

  // check for multiple up edges
  if(g->identity && attrs == g->identity && B->up_identity)
  {
    if(B->up_identity->A != &A->vx)
    {
      xapi_info_pushf("edge", "%s:0x%x:%s", B->up_identity->A->label, B->up_identity->attrs, B->label);
      fail(MORIA_UPEXISTS);
    }
  }

  if(existing_edge)
  {
    /* just replace the B node on the existing edge and return the previous B node */
    e = existing_edge;
    *oldBx = e->B;
    e->B->up_identity = 0;
    e->B = &B->vx;
    B->up_identity = &e->ex;
  }
  else
  {
    fatal(edge_alloc, g, &e);
    e->attrs = attrs;
    e->A = &A->vx;
    e->B = &B->vx;

    if(g->identity && (attrs & g->identity) == g->identity)
    {
      e->attrs |= MORIA_EDGE_IDENTITY;
      B->up_identity = &e->ex;
    }
    else
    {
      rbtree_put(&B->up, e, rbn_up, edge_cmp_rbn_up);
    }

    rbtree_put(&A->down, e, rbn_down, edge_cmp_rbn_down);
  }

  if(er) *er = &e->ex;
  if(r) *r = true;

  finally : coda;
}

API xapi graph_hyperconnect(
    graph * const restrict g
  , vertex ** const restrict Axlist
  , uint16_t Axlen
  , vertex ** const restrict Bxlist
  , uint16_t Bxlen
  , uint32_t attrs
  , edge ** restrict er
  , bool * restrict r
)
{
  enter;

  edge_t * e = 0;
  rbnode * rbn;
  edge_key key;
  int x;

  /* prefer non-hyperedge when possible */
  if(Axlen == 1 && Bxlen == 1)
  {
    fatal(graph_connect, g, Axlist[0], Bxlist[0], attrs, er, r);
    er = 0;
    goto XAPI_FINALIZE;
  }

  /* if inserting on identity -> ERROR (hyperedges do not participate in the identity relation) */
  if(g->identity && attrs == g->identity)
  {
    fail(MORIA_ILLIDENTITY);
  }

  if(Axlist)
    qsort(Axlist, Axlen, sizeof(*Axlist), ptrcmp);
  if(Bxlist)
    qsort(Bxlist, Bxlen, sizeof(*Bxlist), ptrcmp);

  // search by vertex-set (non-identity)
  key = (typeof(key)) {
      attrs : MORIA_EDGE_HYPER
    , Alist : Axlist
    , Alen : Axlen
    , Blist : Bxlist
    , Blen : Bxlen
  };
  if(Axlen)
  {
    if((rbn = rbtree_lookup_node(&Axlist[0]->down, &key, edge_cmp_key_down))) {
      e = rbn->ud.p;
    }
  }
  else
  {
    if((rbn = rbtree_lookup_node(&Bxlist[0]->up, &key, edge_cmp_key_up))) {
      e = rbn->ud.p;
    }
  }

  // idempotency
  if(e && (e->attrs & ~(MORIA_EDGE_IDENTITY | MORIA_EDGE_HYPER)) == attrs)
  {
    if(er) *er = &e->ex;
    if(r) *r = false;
    goto XAPI_FINALIZE;
  }

  // different attrs for this edge
  if(e)
  {
    fail(MORIA_VERTEXEXISTS);
  }

  fatal(edge_alloc, g, &e);
  e->attrs = attrs | MORIA_EDGE_HYPER;

  fatal(xmalloc, &e->Alist, sizeof(*e->Alist) * Axlen);
  for(x = 0; x < Axlen; x++)
  {
    e->Alist[x].v = Axlist[x];
    e->Alist[x].rbn.ud.p = e;
  }
  e->Alen = Axlen;

  fatal(xmalloc, &e->Blist, sizeof(*e->Blist) * Bxlen);
  for(x = 0; x < Bxlen; x++)
  {
    e->Blist[x].v = Bxlist[x];
    e->Blist[x].rbn.ud.p = e;
  }
  e->Blen = Bxlen;

  // insert into down tree of each A vertex
  for(x = 0; x < Axlen; x++)
    rbtree_put(&Axlist[x]->down, e, Alist[x].rbn, edge_cmp_rbn_down);

  // insert into up tree of each B vertex
  for(x = 0; x < Bxlen; x++)
    rbtree_put(&Bxlist[x]->up, e, Blist[x].rbn, edge_cmp_rbn_up);

  if(er) *er = &e->ex;
  if(r) *r = true;

  finally : coda;
}

API xapi graph_edge_disconnect(graph * const restrict g, edge * const restrict ex)
{
  enter;

  edge_t *e;
  int x;

  e = containerof(ex, edge_t, ex);

  if((e->attrs & MORIA_EDGE_HYPER))
  {
    for(x = 0; x < e->Alen; x++)
    {
      rbtree_delete(&e->Alist[x].v->down, &e->Alist[x], rbn);
    }

    for(x = 0; x < e->Blen; x++)
    {
      rbtree_delete(&e->Blist[x].v->up, &e->Blist[x], rbn);
    }
  }
  else
  {
    rbtree_delete(&e->A->down, e, rbn_down);

    if(e->attrs & MORIA_EDGE_IDENTITY)
    {
      e->B->up_identity = 0;
    }
    else
    {
      rbtree_delete(&e->B->up, e, rbn_up);
    }
  }

  llist_delete(e, graph_lln);
  llist_append(&g->edge_freelist, e, graph_lln);

  finally : coda;
}

API xapi graph_disconnect(graph * const restrict g, vertex * const restrict A, vertex * const restrict B)
{
  enter;

  edge * e = edge_between(A, B);

  if(e)
    fatal(graph_edge_disconnect, g, e);

  finally : coda;
}

API xapi graph_hyperdisconnect(graph * restrict g, vertex ** restrict Alist, uint16_t Alen, vertex ** restrict Blist, uint16_t Blen)
{
  enter;

  edge * e = edge_of(Alist, Alen, Blist, Blen);

  if(e)
    fatal(graph_edge_disconnect, g, e);

  finally : coda;
}

API xapi graph_vertex_delete(struct graph * restrict g, struct vertex * restrict vx)
{
  enter;

  vertex_t * v = containerof(vx, vertex_t, vx);

  fatal(vertex_delete, v, g);

  finally : coda;
}

static xapi operations_create_vertex(vertex ** const restrict rv, graph * const restrict g, uint32_t attrs, uint8_t op_attrs, const char * const restrict label, uint16_t label_len)
{
  enter;

  fatal(vertex_createw, rv, g, attrs, label, label_len);

  finally : coda;
}

APIDATA operations_dispatch * graph_operations_dispatch = (operations_dispatch[]) {{
    .connect = graph_connect
  , .hyperconnect = graph_hyperconnect
  , .disconnect = graph_edge_disconnect
  , .create_vertex = operations_create_vertex
}};
