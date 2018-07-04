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
#include "valyria/dictionary.h"
#include "valyria/map.h"

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

static const char * operation_type_names[] = {
#define GRAPH_OPERATION_DEF(x) [x] = #x,
  GRAPH_OPERATION_TABLE
};

struct operations_parser
{
  void * p;
};

//
// private
//

static const char * tokenname(int token)
{
  return operations_tokennames[token];
}

static const char * statename(int state)
{
  return state >= 0 ? operations_statenames[state] : "";
}

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
  fatal(graph_lookup, g, graph_lookup_identifier, &context, mm_tmp, vertices, &r);

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

  // attempt lookup
  fatal(identifier_resolve, g, id, mm_tmp, v);

  if(!*v)
  {
    graph_lookup_identifier_context context = { id : id };
    fatal(graph_lookup_identifier_attrs, &context, &label, &label_len, &attrs);
    if(label)
    {
      vertex * pv = 0;
      if(context.id)
        fatal(identifier_create, g, dispatch, context.id, mm_tmp, &pv);

      fatal(dispatch->vertex_create, v, g, attrs, label, label_len);
      if(pv)
        fatal(dispatch->connect, g, pv, *v, g->identity);
    }
  }

  finally : coda;
}

//
// public
//

void identifier_free(identifier * id)
{
  if(id)
    free(id->next);
  free(id);
}

void operation_free(operation * op)
{
  if(op)
  {
    identifier_free(op->A);
    identifier_free(op->B);
  }

  free(op);
}

const char * operation_type_name(operation_type type)
{
  if(type <= GRAPH_OPERATION_RANGE_BEFORE || type >= GRAPH_OPERATION_RANGE_AFTER)
    return NULL;

  return operation_type_names[type];
}

//
// API
//

API xapi operations_parser_create(operations_parser ** const parser)
{
  enter;

  fatal(xmalloc, parser, sizeof(**parser));
  tfatalize(perrtab_KERNEL, ENOMEM, operations_yylex_init, &(*parser)->p);

  finally : coda;
}

API xapi operations_parser_xfree(operations_parser* const p)
{
  enter;

  if(p)
    operations_yylex_destroy(p->p);

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

API xapi operations_parser_parse(
    operations_parser ** restrict parser
  , const char * const restrict text
  , size_t len
  , list ** restrict operations
)
{
  enter;

  list * lops = 0;
  operations_parser * lp = 0;
  void * lexer_state = 0;

  // parser
  if(!parser)
    parser = &lp;
  if(!*parser)
    fatal(operations_parser_create, parser);

  // storage
  if(!operations)
    operations = &lops;
  if(!*operations)
    fatal(list_createx, operations, operation_free, 0, 0);

  operations_xtra pp = {
      .tokname      = tokenname
    , .statename    = statename
#if DEBUG || DEVEL || XUNIT
    , .state_logs   = L_MORIA | L_OPERATIONS
    , .token_logs   = L_MORIA | L_OPERATIONS
#endif
  };

  // create state specific to this parse
  if((lexer_state = operations_yy_scan_bytes(text, len, (*parser)->p)) == 0)
    fail(KERNEL_ENOMEM);

  fatal(dictionary_create, &pp.definitions, sizeof(uint32_t));

  pp.scanner = (*parser)->p;
  pp.li = *operations;

  // make available to the lexer
  operations_yyset_extra(&pp, (*parser)->p);

  // invoke the appropriate parser, raise errors as needed
  fatal(yyu_reduce, operations_yyparse, &pp, MORIA_SYNTAX);

  // ownership transfer
  lops = 0;

finally:
  operations_yy_delete_buffer(lexer_state, (*parser)->p);
  yyu_extra_destroy(&pp.yyu);
  fatal(dictionary_xfree, pp.definitions);

  fatal(operations_parser_xfree, lp);
  fatal(list_ixfree, &lops);
coda;
}

API xapi operations_perform(graph * restrict g, const operations_dispatch * restrict dispatch, list * restrict ops)
{
  enter;

  vertex *A = 0, *B = 0;
  edge * e;
  operation *op;
  void * mm_tmp = 0;

  int x;
  for(x = 0; x < ops->l; x++)
  {
    op = list_get(ops, x);

    if(op->type == GRAPH_OPERATION_VERTEX)
    {
      fatal(identifier_create, g, dispatch, op->A, &mm_tmp, &A);
    }
    else if(op->type == GRAPH_OPERATION_CONNECT)
    {
      fatal(identifier_create, g, dispatch, op->A, &mm_tmp, &A);
      fatal(identifier_create, g, dispatch, op->B, &mm_tmp, &B);

      fatal(graph_connect, g, A, B, op->attrs);
    }
    else
    {
      fatal(identifier_resolve, g, op->A, &mm_tmp, &A);
      fatal(identifier_resolve, g, op->B, &mm_tmp, &B);
      e = edge_between(A, B);

      if(op->type == GRAPH_OPERATION_DISCONNECT)
      {
        fatal(graph_disconnect, g, A, B);
      }
      else if(op->type == GRAPH_OPERATION_DISINTEGRATE)
      {
        traversal_criteria criteria = {
            edge_travel: e->attrs
          , edge_visit: e->attrs
        };
        fatal(graph_disintegrate, g, e, 0, 0, &criteria, MORIA_TRAVERSE_POST | MORIA_TRAVERSE_DOWN, 0);
      }
    }
  }

finally:
  wfree(mm_tmp);
coda;
}

API xapi graph_connect(graph * const restrict g, vertex * const restrict A, vertex * const restrict B, uint32_t attrs)
{
  enter;

/*
  if inserting on identity
    if label is found  -> EXISTS (would violate the identity relation)
    if vertex is found -> EXISTS (cant convert from vertex edge to identity edge)

  if inserting on vertex
    if label is found  -> EXISTS (for this vertex? cant convert from identity edge to vertex edge)
    if vertex is found -> EXISTS (edge already exists)
*/

  int x;
  edge * e = 0;
  edge * tmp = 0;

  struct edge_key_compare_label_context label_ctx;
  struct edge_key_compare_vertex_context vertex_ctx;

  // search in the identity partition
  if(A->down_partition == 0)
  {
    label_ctx = (typeof(label_ctx)) { lc : -1 };
  }
  else
  {
    label_ctx = (typeof(label_ctx)) { B : B->label, len : B->label_len };
    e = list_search_range(A->down, 0, A->down_partition, &label_ctx, edge_key_compare_label);
  }

  // idempotency
  if(e && e->B == B && e->attrs == attrs)
    goto XAPI_FINALIZE;

  if(e && attrs == g->identity)
  {
    xapi_info_pushf("edge", "%s:0x%x:%s", e->A->label, e->attrs, e->B->label);
    fail(MORIA_LABELEXISTS);
  }

  // different attrs for this edge
  else if(e && e->B == B)
  {
    xapi_info_pushf("edge", "%s:0x%x:%s", e->A->label, e->attrs, e->B->label);
    fail(MORIA_VERTEXEXISTS);
  }

  // search in the non-identity partition
  e = 0;
  if(A->down->l == A->down_partition)
  {
    vertex_ctx = (typeof(vertex_ctx)) { lc : -1, lx : A->down_partition };
  }
  else
  {
    vertex_ctx = (typeof(vertex_ctx)) { B : B };
    e = list_search_range(A->down, A->down_partition, A->down->l - A->down_partition, &vertex_ctx, edge_key_compare_vertex);
  }

  // idempotency
  if(e && e->attrs == attrs)
    goto XAPI_FINALIZE;

  // different attrs for this edge
  if(e)
  {
    xapi_info_pushf("edge", "%s:0x%x:%s", e->A->label, e->attrs, e->B->label);
    fail(MORIA_VERTEXEXISTS);
  }

  // check for multiple up edges
  if(attrs == g->identity && B->up_partition)
  {
    e = list_get(B->up, 0);
    if(e->A != A)
    {
      xapi_info_pushf("edge", "%s:0x%x:%s", e->A->label, e->attrs, e->B->label);
      fail(MORIA_UPEXISTS);
    }
  }

  // create the edge
  fatal(xmalloc, &tmp, sizeof(*tmp));
  tmp->A = A;
  tmp->B = B;
  tmp->attrs = attrs;
  fatal(list_push, g->edges, tmp);
  e = tmp;
  tmp = 0;
  e->edges_index = g->edges->l - 1;

  // insert into A->down
  if(attrs == g->identity)
  {
    e->down_index = label_ctx.lx;
    if(label_ctx.lc > 0)
      e->down_index++;
  }
  else
  {
    e->down_index = vertex_ctx.lx;
    if(vertex_ctx.lc > 0)
      e->down_index++;
  }

  fatal(list_insert, A->down, e->down_index, e);
  for(x = e->down_index + 1; x < A->down->l; x++)
    ((edge*)list_get(A->down, x))->down_index++;

  // search for A in B->up
  if(attrs == g->identity)
  {
    if(B->up_partition == 0)
    {
      label_ctx = (typeof(label_ctx)) { lc : -1 };
    }
    else
    {
      label_ctx = (typeof(label_ctx)) { A : A->label, len : A->label_len };
      list_search_range(B->up, 0, B->up_partition, &label_ctx, edge_key_compare_label);
    }

    e->up_index = label_ctx.lx;
    if(label_ctx.lc > 0)
      e->up_index++;
  }
  else
  {
    if(B->up->l == B->up_partition)
    {
      vertex_ctx = (typeof(vertex_ctx)) { lc : -1, lx : B->up_partition };
    }
    else
    {
      vertex_ctx = (typeof(vertex_ctx)) { A : A };
      list_search_range(B->up, B->up_partition, B->up->l - B->up_partition, &vertex_ctx, edge_key_compare_vertex);
    }

    e->up_index = vertex_ctx.lx;
    if(vertex_ctx.lc > 0)
      e->up_index++;
  }

  fatal(list_insert, B->up, e->up_index, e);
  for(x = e->up_index + 1; x < B->up->l; x++)
    ((edge*)list_get(B->up, x))->up_index++;

  if(attrs == g->identity)
  {
    A->down_partition++;
    B->up_partition++;
  }

finally:
  wfree(tmp);
coda;
}

API xapi graph_disconnect(graph * const restrict g, vertex * const restrict A, vertex * const restrict B)
{
  enter;

  edge * e = edge_between(A, B);

  if(e)
    fatal(edge_disconnect, g, e);

  finally : coda;
}

xapi operations_disintegrate_visitor(edge * e, int distance, void * restrict context)
{
  enter;

  list * li = (list*)context;
  fatal(list_push, li, e);

  finally : coda;
}

API xapi graph_disintegrate(
    graph * const restrict g
  , edge * const restrict e
  , xapi (*visitor)(struct edge * restrict e, int distance, void * restrict ctx) __attribute__((nonnull))
  , int traversal_id
  , const traversal_criteria * restrict criteria
  , uint32_t attrs
  , void * ctx
)
{
  enter;

  list * li = 0;
  fatal(list_create, &li);

  fatal(graph_traverse_edges
    , g
    , e
    , visitor ?: operations_disintegrate_visitor
    , traversal_id
    , criteria
    , attrs
    , li
  );

  int x;
  for(x = 0; x < li->l; x++)
  {
    edge * e = list_get(li, x);
    fatal(edge_disconnect, g, e);
  }

finally:
  fatal(list_xfree, li);
coda;
}

APIDATA operations_dispatch * graph_operations_dispatch = (operations_dispatch[]) {{
    .connect = graph_connect
  , .disconnect = graph_disconnect
  , .vertex_create = vertex_createw
}};
