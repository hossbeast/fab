/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

/* flex and bison do not agree on these names */
#define YYSTYPE struct graph_yystype
#define YYLTYPE yyu_location

/* the token enums shadow each other exclude all but one set */
#define OPERATIONS_YYTOKENTYPE


#include "graph.tab.h"
#include "graph.tokens.h"
#include "graph.lex.h"
#include "graph.states.h"
#include "operations.tab.h"
#include "operations.tokens.h"
#include "operations.lex.h"
#include "operations.states.h"


static YYU_VTABLE(graph_vtable, graph, graph);
static YYU_VTABLE(operations_vtable, operations, operations);

llist *parser_llist_alloc(struct moria_parser * restrict parser)
{
  llist *list;

  if(!llist_empty(&parser->lists)) {
    list = parser->lists.next;
    llist_delete(list);
  } else {
    xmalloc(&list, sizeof(*list));
    llist_init(list);
  }

  return list;
}

//
// API
//

void moria_parser_create(
    moria_parser ** const parser
  , moria_graph * restrict g
  , llist ** restrict vertex_lists
  , uint16_t vertex_lists_len
  , moria_operations_vtable * restrict dispatch
  , const attrs32 * restrict vertex_defs
  , const attrs32 * restrict edge_defs
)
{
  moria_parser *p;

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
  map_create(&p->vertex_by_id);
  if(vertex_lists_len)
  {
    RUNTIME_ASSERT(vertex_lists_len <= (sizeof(p->vertex_lists) / sizeof(*p->vertex_lists)));
    memcpy(p->vertex_lists, vertex_lists, sizeof(*p->vertex_lists) * vertex_lists_len);
    p->vertex_lists_len = vertex_lists_len;
  }
  else
  {
    p->vertex_lists[0] = &p->vertices;
    p->vertex_lists_len = 1;
  }
  p->vertex_defs = vertex_defs;
  p->edge_defs = edge_defs;
  llist_init(&p->vertices);
  llist_init(&p->vertex_freelist);
  llist_init(&p->edges);
  llist_init(&p->edge_freelist);
  llist_init(&p->lists);

  *parser = p;
}

void moria_parser_free(moria_parser* const p)
{
  moria_vertex* v;
  moria_edge* e;
  llist *T;

  if(!p) {
    return;
  }

  map_free(p->vertex_by_id);
  yyu_parser_destroy(&p->graph_yyu);
  yyu_parser_destroy(&p->operations_yyu);

  llist_foreach_safe(&p->vertices, v, owner, T) {
    free((void*)v->label);
    free(v);
  }
  llist_foreach_safe(&p->vertex_freelist, v, owner, T) {
    free((void*)v->label);
    free(v);
  }
  llist_foreach_safe(&p->edges, e, owner, T) {
    if((e->attrs & MORIA_EDGE_HYPER)) {
      free(e->Alist);
      free(e->Blist);
    }
    free(e);
  }
  llist_foreach_safe(&p->edge_freelist, e, owner, T) {
    if((e->attrs & MORIA_EDGE_HYPER)) {
      free(e->Alist);
      free(e->Blist);
    }
    free(e);
  }

  free(p);
}

void moria_parser_ifree(moria_parser ** const p)
{
  moria_parser_free(*p);
  *p = 0;
}

int moria_parser_graph_parse(
    moria_parser * restrict parser
  , char * const restrict text
  , size_t len
)
{
  map_recycle(parser->vertex_by_id);
  return yyu_parse(&parser->graph_yyu, text, len, "-graph-", 0, 0, 0);
}

int moria_parser_operations_parse(
    moria_parser * restrict parser
  , char * const restrict text
  , size_t len
)
{
  int r;

  parser->dispatch->setup(parser);
  r = yyu_parse(&parser->operations_yyu, text, len, "-ops-", 0, 0, 0);
  parser->dispatch->cleanup(parser);

  return r;
}
