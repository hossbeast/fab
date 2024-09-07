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

#include "edge.h"
#include "connect.h"
#include "dispatch.h"

static void moria_parser_edge_disconnect(
    moria_parser * restrict p
  , moria_edge * restrict e
)
{
  moria_edge_disconnect(p->g, e);

  llist_delete(&e->owner);
  llist_append(&p->edge_freelist, &e->owner);
}

static void create_edge(
    moria_parser * restrict p
  , moria_edge ** const restrict er
  , uint16_t Blen
  , uint16_t Alen
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
  llist_append(&p->edges, &e->owner);
  *er = e;
}

void moria_parser_setup(moria_parser * restrict p) { }
void moria_parser_cleanup(moria_parser * restrict p) { }

void moria_parser_connect(
    moria_parser * restrict p
  , moria_vertex * restrict B
  , moria_vertex * restrict A
  , uint32_t attrs
  , moria_edge ** restrict er
)
{
  moria_connect_context ctx;
  moria_edge *e;
  int r;

  r = moria_preconnect(&ctx, p->g, B, A, attrs, &e);
  if(r == MORIA_EDGEATTRS)
  {
    RTA; // fail(MORIA_VERTEXEXISTS);
  }
  else if(r == MORIA_EDGEDOWN)
  {
    RTA; // fail(MORIA_LABELEXISTS);
  }
  else if(r == MORIA_EDGEUP)
  {
    RTA; // fail(MORIA_UPEXISTS);
  }
  else if(r == MORIA_NOEDGE)
  {
    create_edge(p, &e, 1, 1);
    moria_connect(&ctx, p->g, e, B, A, attrs);
  }
  if(er) {
    *er = e;
  }
}

void moria_parser_hyperconnect(
    moria_parser * restrict p
  , moria_vertex ** restrict Blist
  , uint16_t Blen
  , moria_vertex ** restrict Alist
  , uint16_t Alen
  , uint32_t attrs
  , moria_edge ** restrict er
)
{
  moria_connect_context ctx;
  moria_edge *e;
  int r;

  if(Alen == 1 && Blen == 1) {
    attrs &= ~MORIA_EDGE_HYPER;
    moria_parser_connect(p, Blist[0], Alist[0], attrs, er);
    return;
  }

  r = moria_preconnect_hyper(&ctx, p->g, Blist, Blen, Alist, Alen, attrs, &e);
  if(r == MORIA_EDGEUP)
  {
    RTA;  // test-only abort
  }
  else if(r == MORIA_EDGEATTRS)
  {
    RTA;  // test-only abort
  }
  else if(r == MORIA_NOEDGE)
  {
    create_edge(p, &e, Blen, Alen);
    moria_connect_hyper(&ctx, p->g, e, Blist, Blen, Alist, Alen, attrs);
  }
  if(er) {
    *er = e;
  }
}

void moria_parser_create_vertex(
    moria_parser * restrict p
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
  llist_append(&p->vertices, &v->owner);

  *rv = v;
}

moria_operations_vtable * API moria_operations_dispatch = (moria_operations_vtable[]) {{
    .setup = moria_parser_setup
  , .cleanup = moria_parser_cleanup
  , .connect = moria_parser_connect
  , .hyperconnect = moria_parser_hyperconnect
  , .disconnect = moria_parser_edge_disconnect
  , .create_vertex = moria_parser_create_vertex
}};
