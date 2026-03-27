/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "bacon/builder.h"
#include "common/attrs.h"
#include "moria.h"
#include "narrator.h"
#include "moria/foreach.h"

#include "edge.h"
#include "vertex.h"
#include "edges.h"
#include "vertices.h"

void edge_describe_up(const moria_edge * restrict e, bacon_writer * restrict builder_writer)
{
  int x;

  { SET
    LOADF("%p", e)
    { MAPPING
      STRING("up")
      { SET
        if(e->attrs & MORIA_EDGE_IDENTITY) {
          vertex_describe(e->A, builder_writer);
        } else if(!(e->attrs & MORIA_EDGE_HYPER)) {
          if(rbnode_attached(&e->rbn_up)) {
            vertex_describe(e->A, builder_writer);
          }
        } else {
          for(x = 0; x < e->Alen; x++) {
            if(rbnode_attached(&e->Alist[x].rbn)) {
              vertex_describe(e->Alist[x].v, builder_writer);
            }
          }
        }
      }
    }
    { MAPPING
      STRING("attrs")
      attrs32_describe(e->attrs, graph_edge_attrs, builder_writer);
    }
  }
}

void edge_describe_down(const moria_edge * restrict e, bacon_writer * restrict builder_writer)
{
  int x;

  { SET
    LOADF("%p", e)
    { MAPPING
      STRING("attrs")
      attrs32_describe(e->attrs, graph_edge_attrs, builder_writer);
    }
    { MAPPING
      STRING("down")
      { SET
        if(!(e->attrs & MORIA_EDGE_HYPER)) {
          if(rbnode_attached(&e->rbn_down)) {
            vertex_describe(e->B, builder_writer);
          }
        } else {
          for(x = 0; x < e->Blen; x++) {
            if(rbnode_attached(&e->Blist[x].rbn)) {
              vertex_describe(e->Blist[x].v, builder_writer);
            }
          }
        }
      }
    }
  }
}

void edge_describe(const moria_edge * restrict e, bacon_writer * restrict builder_writer)
{
  int x;

  { MAPPING
    HEX(e)
    { SET
      { MAPPING
        STRING("up")
        { SET
          if(e->attrs & MORIA_EDGE_IDENTITY) {
            vertex_describe(e->A, builder_writer);
          } else if(!(e->attrs & MORIA_EDGE_HYPER)) {
            if(rbnode_attached(&e->rbn_up)) {
              vertex_describe(e->A, builder_writer);
            }
          } else {
            for(x = 0; x < e->Alen; x++) {
              if(rbnode_attached(&e->Alist[x].rbn)) {
                vertex_describe(e->Alist[x].v, builder_writer);
              }
            }
          }
        }
      }
      { MAPPING
        STRING("attrs")
        attrs32_describe(e->attrs, graph_edge_attrs, builder_writer);
      }
      { MAPPING
        STRING("down")
        { SET
          if(!(e->attrs & MORIA_EDGE_HYPER)) {
            if(rbnode_attached(&e->rbn_down)) {
              vertex_describe(e->B, builder_writer);
            }
          } else {
            for(x = 0; x < e->Blen; x++) {
              if(rbnode_attached(&e->Blist[x].rbn)) {
                vertex_describe(e->Blist[x].v, builder_writer);
              }
            }
          }
        }
      }
    }
  }
}

void edge_identity_describe(const moria_edge * restrict e, bacon_writer * restrict builder_writer)
{
  int x;

  { SET
    HEX(e)
    { MAPPING
      STRING("up")
      if(e->attrs & MORIA_EDGE_IDENTITY) {
        SET
        vertex_identity_describe(e->A, builder_writer);
      } else if(!(e->attrs & MORIA_EDGE_HYPER)) {
        if(rbnode_attached(&e->rbn_up)) {
          SET
          vertex_identity_describe(e->A, builder_writer);
        }
      } else {
        LIST
        for(x = 0; x < e->Alen; x++) {
          if(rbnode_attached(&e->Alist[x].rbn)) {
            SET
            vertex_identity_describe(e->Alist[x].v, builder_writer);
          }
        }
      }
    }
    { MAPPING STRING("attrs")
      attrs32_describe(e->attrs, graph_edge_attrs, builder_writer);
    }
    { MAPPING
      STRING("down")
      if(!(e->attrs & MORIA_EDGE_HYPER))
      {
        if(rbnode_attached(&e->rbn_down)) {
          SET
          vertex_identity_describe(e->B, builder_writer);
        } else {
          STRING("_")
        }
      }
      else
      {
        LIST
        for(x = 0; x < e->Blen; x++)
        {
          if(rbnode_attached(&e->Blist[x].rbn)) {
            SET
            vertex_identity_describe(e->Blist[x].v, builder_writer);
          } else {
            STRING("_")
          }
        }
      }
    }
  }
}

void edge_say(const moria_edge * restrict e, narrator * restrict N)
{
  bacon_writer writer;

  bacon_writer_init(&writer);
  bacon_writer_open(&writer, N);
  edge_describe(e, &writer);
  bacon_writer_destroy(&writer);
}

void edge_identity_say(const moria_edge * restrict e, narrator * restrict N)
{
  bacon_writer writer;

  bacon_writer_init(&writer);
  bacon_writer_open(&writer, N);
  edge_identity_describe(e, &writer);
  bacon_writer_destroy(&writer);
}

void edge_show(const moria_edge * restrict e)
{
  edge_say(e, g_narrator_stdout);
  printf("\n");
}

void edge_identity_show(const moria_edge * restrict e)
{
  edge_identity_say(e, g_narrator_stdout);
  printf("\n");
}

bool edges_gather_down(const struct moria_vertex * restrict v, edge_type type, struct edges * restrict list)
{
  moria_foreach_state st;
  moria_edge *e;
  bool b;

  edges_init(list);

  b = false;
  moria_foreach_edge_down(&st, v, e) {
    if(edge_type_get(e) == type) {
      edges_push(list, e);
      b = true;
    }
  }

  return b;
}

bool edges_gather_up(const struct moria_vertex * restrict v, edge_type type, struct edges * restrict list)
{
  moria_foreach_state st;
  moria_edge *e;
  bool b;

  edges_init(list);

  b = false;
  moria_foreach_edge_up(&st, v, e) {
    if(edge_type_get(e) == type) {
      edges_push(list, e);
      b = true;
    }
  }

  return b;
}

moria_edge *edges_check_up(const struct moria_vertex * restrict v, edge_type type)
{
  moria_foreach_state st;
  moria_edge *e;

  moria_foreach_edge_up(&st, v, e) {
    if(edge_type_get(e) == type) {
      return e;
    }
  }

  return 0;
}

moria_edge *edges_check_down(const struct moria_vertex * restrict v, edge_type type)
{
  moria_foreach_state st;
  moria_edge *e;

  moria_foreach_edge_down(&st, v, e) {
    if(edge_type_get(e) == type) {
      return e;
    }
  }

  return 0;
}

uint32_t edges_count_up(const struct moria_vertex * restrict v, edge_type type)
{
  moria_foreach_state st;
  moria_edge *e;
  uint32_t count;

  count = 0;
  moria_foreach_edge_up(&st, v, e) {
    if(edge_type_get(e) == type) {
      count++;
    }
  }

  return count;
}

uint32_t edges_count_down(const struct moria_vertex * restrict v, edge_type type)
{
  moria_foreach_state st;
  moria_edge *e;
  uint32_t count;

  count = 0;
  moria_foreach_edge_down(&st, v, e) {
    if(edge_type_get(e) == type) {
      count++;
    }
  }

  return count;
}

moria_vertex *edge_follow_up(const struct moria_vertex * restrict v, edge_type type)
{
  moria_edge *e;

  if((e = edges_check_up(v, type)) == NULL) {
    return 0;
  }

  return e->A;
}

moria_vertex *edge_follow_down(const struct moria_vertex * restrict v, edge_type type)
{
  moria_edge *e;

  if((e = edges_check_down(v, type)) == NULL) {
    return 0;
  }

  return e->B;
}
