/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "types.h"

#include "narrator.h"
#include "moria/dispatch.h"
#include "moria/graph.h"
#include "moria/vertex.h"
#include "moria/edge.h"
#include "moria/parser.h"
#include "moria/linear_search.h"
#include "common/attrs.h"

#include "node_operations_test.h"
#include "fsent.h"
#include "vertex.h"
#include "shadow.h"
#include "fsedge.h"
#include "ref_edge.h"
#include "dependency.h"
#include "filesystem.h"
#include "graph.h"
#include "selection.h"
#include "dependency.h"
#include "disintegrate.h"
#include "bpe.h"
#include "edge.h"
#include "symlink.h"
#include "edges.h"
#include "vertices.h"

/* used in unit tests */
static graph_invalidation_context invalidation;

static void node_operations_setup(moria_parser * restrict parser)
{
  graph_invalidation_begin(&invalidation);
}

static void node_operations_cleanup(moria_parser * restrict parser)
{
  graph_invalidation_end(&invalidation);
}

static void node_operations_connect(
    moria_parser * const restrict parser
  , moria_vertex * const restrict B
  , moria_vertex * const restrict A
  , uint32_t attrs
  , moria_edge ** restrict e
)
{
  fsent *An;
  fsent *Bn;

  if(attrs == MORIA_EDGE_IDENTITY) {
    attrs = EDGE_FS;
  }

  if((attrs & EDGE_TYPE_OPT) == EDGE_FS)
  {
    An = containerof(A, fsent, vertex);
    Bn = containerof(B, fsent, vertex);
    fsedge_connect(Bn, An, &invalidation);
  }
  else if((attrs & EDGE_DEPENDS))
  {
    dependency_connect(B, A, attrs);
  }
  else if(attrs == EDGE_REF_SYMLINK)
  {
    An = containerof(A, fsent, vertex);
    Bn = containerof(B, fsent, vertex);
    An->vertex.attrs = VERTEX_SYMLINK;
    fsent_symlink_connect(Bn, An);
  }
  else
  {
    RTA;
  }
}

static void node_operations_hyperconnect(
    moria_parser * restrict p
  , moria_vertex ** restrict Blist
  , uint16_t Blen
  , moria_vertex ** restrict Alist
  , uint16_t Alen
  , uint32_t attrs
  , moria_edge ** restrict er
)
{
  if(Alen == 1 && Blen == 1) {
    attrs &= ~MORIA_EDGE_HYPER;
    node_operations_connect(p, Blist[0], Alist[0], attrs, er);
    return;
  }

  RTA;
}

static void node_operations_refresh_vertex(moria_parser * restrict parser, moria_vertex * const restrict v)
{
  vertex_mark_fresh(v);
}

static void node_operations_invalidate_vertex(moria_parser * restrict parser, moria_vertex * const restrict v)
{
  vertex_invalidate(v, &invalidation);
}

static void node_operations_disconnect(moria_parser * restrict parser, moria_edge * restrict e)
{
  edges edges;
  vertices unlinking;
  vertices releasing;

  edges_init(&edges);
  vertices_init(&unlinking);
  vertices_init(&releasing);

  if(edge_type_get(e) == EDGE_FS) {
    vertices_push(&unlinking, e->B);
  } else {
    edges_push(&edges, e);
  }

  graph_disintegrate(&edges, &unlinking, &releasing, &invalidation);
}

static void create_vertex(
    moria_parser * restrict parser
  , moria_vertex ** const restrict rv
  , uint32_t attrs
  , uint8_t opattrs
  , const char * const restrict label
  , uint16_t label_len
)
{
  fsent *n;
  fsent *b;
  buildplan_entity *bpe;
  bool shadow = false;
  bool root = false;

  /* first segment in the identifier, and starting with double-slash */
  if(opattrs & MORIA_OPATTRS_INIT_SLASH)
  {
    shadow = true;
  }
  /* first segment in the identifier */
  else if(opattrs & MORIA_OPATTRS_INIT)
  {
    root = true;
  }

  n = 0;
  if(attrs == VERTEX_BPE) {
    buildplan_entity_alloc(&bpe);
    bpe->vertex.label = label;
    bpe->vertex.label_len = label_len;
    *rv = &bpe->vertex;
  } else {
    fsent_create(&n, VERTEX_FILETYPE_REG, attrs | VERTEX_OK, label, label_len);
    *rv = &n->vertex;
  }

  b = 0;
  if(n && root && g_root)
  {
    b = g_root;
  }
  else if(n && shadow && g_shadow)
  {
    b = g_shadow;
  }

  if(b)
  {
    node_operations_connect(parser, &n->vertex, &b->vertex, EDGE_FS, 0);
  }
}

fsent *resolve_fragment(const char *frag, uint16_t fragl)
{
  /* skip shadow nodes */
  uint32_t masks[] = { VERTEX_SHADOW_BIT };
  uint32_t bits[] = { 0 };
  moria_vertex *v;

  moria_graph_linear_search(&fsent_list, frag, fragl, masks, bits, sizeof(masks) / sizeof(*masks), &v);
  RUNTIME_ASSERT(v);

  return containerof(v, fsent, vertex);
}

void graph_say_lists(narrator * restrict N, llist ** vertex_lists, uint16_t vertex_lists_len, llist ** edge_lists, uint16_t edge_lists_len)
{
  /* skip shadow nodes */
  uint32_t masks[] = { VERTEX_SHADOW_BIT };
  uint32_t bits[] = { 0 };

  moria_graph_say(
      &g_graph
    , vertex_lists
    , vertex_lists_len
    , edge_lists
    , edge_lists_len
    , graph_vertex_attrs
    , graph_edge_attrs
    , masks
    , bits
    , sizeof(masks) / sizeof(*masks)
    , N
  );
}

void graph_say(narrator * restrict N)
{
  llist *vertex_lists[2];
  llist *edge_lists[3];

  vertex_lists[0] = &fsent_list;
  vertex_lists[1] = &bpe_list;
  edge_lists[0] = &fsedge_list;
  edge_lists[1] = &dependency_list;
  edge_lists[2] = &ref_edge_list;

  graph_say_lists(N, vertex_lists, sizeof(vertex_lists) / sizeof(*vertex_lists), edge_lists, sizeof(edge_lists) / sizeof(*edge_lists));
}

void graph_show()
{
  graph_say(g_narrator_stdout);
  printf("\n");
}

moria_operations_vtable *node_operations_test_dispatch = (moria_operations_vtable[]) {{
    .create_vertex = create_vertex
  , .connect = node_operations_connect
  , .hyperconnect = node_operations_hyperconnect
  , .disconnect = node_operations_disconnect
  , .refresh_vertex = node_operations_refresh_vertex
  , .invalidate_vertex = node_operations_invalidate_vertex
  , .setup = node_operations_setup
  , .cleanup = node_operations_cleanup
}};
