/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "bacon/builder.h"
#include "common/attrs.h"
#include "moria.h"
#include "narrator.h"

#include "edge.h"
#include "vertex.h"

static void attrs32_describe(uint32_t attrs, const attrs32 * restrict list, bacon_writer * restrict builder_writer)
{
  char buf[128];
  char *s;
  size_t z;
  size_t sz;

  s = buf;
  sz = sizeof(buf);
  z = 0;
  z += attrs32_zcat(s + z, sz - z, list, attrs);
  BYTES(buf, z);
}

void edge_describe_up(const moria_edge * restrict e, bacon_writer * restrict builder_writer)
{
  int x;

  { SET
    LOADF("%p", e)
    { MAPPING
      STRING("up")
      { SET
        if(!(e->attrs & MORIA_EDGE_HYPER))
        {
          vertex_describe(e->A, builder_writer);
        }
        else
        {
          for(x = 0; x < e->Alen; x++)
          {
            vertex_describe(e->Alist[x].v, builder_writer);
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
        if(!(e->attrs & MORIA_EDGE_HYPER))
        {
          vertex_describe(e->B, builder_writer);
        }
        else
        {
          for(x = 0; x < e->Blen; x++)
          {
            vertex_describe(e->Blist[x].v, builder_writer);
          }
        }
      }
    }
  }
}

void edge_describe(const moria_edge * restrict e, bacon_writer * restrict builder_writer)
{
  int x;

  { SET
    HEX(e)
    { MAPPING
      STRING("up")
      if(!(e->attrs & MORIA_EDGE_HYPER))
      {
        vertex_describe(e->A, builder_writer);
      }
      else
      {
        LIST
        for(x = 0; x < e->Alen; x++)
        {
          vertex_describe(e->Alist[x].v, builder_writer);
        }
      }
    }
    { MAPPING
      STRING("attrs")
      attrs32_describe(e->attrs, graph_edge_attrs, builder_writer);
    }
    { MAPPING
      STRING("down")
      if(!(e->attrs & MORIA_EDGE_HYPER))
      {
        vertex_describe(e->B, builder_writer);
      }
      else
      {
        LIST
        for(x = 0; x < e->Blen; x++)
        {
          vertex_describe(e->Blist[x].v, builder_writer);
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
      if(!(e->attrs & MORIA_EDGE_HYPER))
      {
        vertex_identity_describe(e->A, builder_writer);
      }
      else
      {
        LIST
        for(x = 0; x < e->Alen; x++)
        {
          vertex_identity_describe(e->Alist[x].v, builder_writer);
        }
      }
    }
    { MAPPING
      STRING("attrs")
      attrs32_describe(e->attrs, graph_edge_attrs, builder_writer);
    }
    { MAPPING
      STRING("down")
      if(!(e->attrs & MORIA_EDGE_HYPER))
      {
        vertex_identity_describe(e->B, builder_writer);
      }
      else
      {
        LIST
        for(x = 0; x < e->Blen; x++)
        {
          vertex_identity_describe(e->Blist[x].v, builder_writer);
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
