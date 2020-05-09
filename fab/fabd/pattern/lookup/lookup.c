/* Copyright (c) 2012-2017 Todd Freed <todd.freed@gmail.com>

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

#include "types.h"
#include "xapi.h"

#include "xlinux/xstdlib.h"
#include "yyutil/parser.h"
#include "moria/vertex.h"
#include "moria/graph.h"
#include "valyria/rbtree.h"

#include "lookup.internal.h"
#include "pattern.h"
#include "node.h"
#include "MODULE.errtab.h"
#include "shadow.h"
#include "module.h"
#include "selection.h"
#include "render.h"

typedef struct lookup_context {
  const char * path;
  uint16_t path_len;
  uint32_t attrs;

  const char * segment;
} lookup_context;

static xapi __attribute__((nonnull(1))) lookup_callback(
    void * restrict _ctx
  , const char ** restrict label
  , uint16_t * restrict label_len
)
{
  enter;

  lookup_context * ctx = _ctx;
  uint16_t seg_len;

  if(label && !ctx->segment)
  {
    *label = 0;
  }
  else if(label)
  {
    seg_len = 0;
    if(*ctx->segment == '/')
    {
      ctx->segment--;
      seg_len++;
    }
    while(ctx->segment != ctx->path && *ctx->segment != '/')
    {
      ctx->segment--;
      seg_len++;
    }

    if(ctx->segment == ctx->path)
    {
      *label = ctx->segment;
      *label_len = seg_len;
      ctx->segment = 0;
    }
    else
    {
      *label = ctx->segment + 1;
      *label_len = seg_len - 1;
    }
  }

  // rewind
  else
  {
    ctx->segment = ctx->path + ctx->path_len;
  }

  finally : coda;
}

static xapi lookup_relative(
    const node * restrict base
  , const module * restrict mod
  , const char * restrict text
  , uint16_t len
  , uint32_t attrs
  , vertex * restrict vertices[2]
  , int * restrict r
)
{
  enter;

  uint16_t label_len;
  vertex *v;

  v = vertex_containerof(base);
  while(v && len)
  {
    label_len = 0;
    while(1)
    {
      if(text[label_len] == '/' || label_len == len)
        break;

      label_len++;
    }

    if(memncmp(text, label_len, ".", 1) == 0)
    {

    }
    else if(memncmp(text, label_len, "..", 2) == 0)
    {
      if((v = vertex_up(v)) == 0)
        break;
    }
    else
    {
      if((v = vertex_downw(v, text, label_len)) == 0)
        break;

      if(node_shadowtype_get(vertex_value(v)) == VERTEX_SHADOWTYPE_MODULE)
        v = vertex_containerof(mod->shadow);
    }

    if(text[label_len] == '/')
      label_len++;

    text += label_len;
    len -= label_len;
  }

  *r = 0;
  if(len == 0 && v != NULL)
  {
    *r = 1;
    vertices[0] = v;
  }

  finally : coda;
}

static bool candidate(void * ctxp, const vertex * restrict v)
{
  lookup_context * ctx = ctxp;

  if((ctx->attrs & PATTERN_LOOKUP_DIR) == PATTERN_LOOKUP_DIR)
  {
    if(node_filetype_get(vertex_value(v)) != VERTEX_FILETYPE_DIR)
      return false;
  }
  if((ctx->attrs & PATTERN_LOOKUP_MODEL) == PATTERN_LOOKUP_MODEL)
  {
    if(!vertex_downs(v, NODE_MODEL_BAM))
      return false;
  }
  else if((ctx->attrs & PATTERN_LOOKUP_MODULE) == PATTERN_LOOKUP_MODULE)
  {
    if(!vertex_downs(v, NODE_MODULE_BAM))
      return false;
  }

  return true;
}

/* this function is only called from unit test code */
xapi pattern_lookup_fragment(
    const char * restrict frag
  , uint16_t fragl
  , const char * restrict fname
  , const yyu_location * restrict loc
  , const module * restrict mod
  , const node * restrict base
  , uint32_t attrs
  , selection * restrict nodes
  , node ** restrict np
)
{
  enter;

  int r;
  lookup_context ctx = { 0 };
  vertex * vertices[2];
  void * mm_tmp = 0;
  char path1[512];
  char path2[512];
  size_t sz;

  ctx.attrs = attrs;

  if(fragl > 2 && memncmp(frag, 2, "//", 2) == 0)
  {
    fatal(lookup_relative, g_shadow, mod, frag + 2, fragl - 2, attrs, vertices, &r);
  }
  else if(
       (fragl == 1 && memncmp(frag, 1, ".", 1) == 0)
    || (fragl == 2 && memncmp(frag, 2, "..", 2) == 0)
    || (fragl > 2 && memncmp(frag, 2, "./", 2) == 0)
    || (fragl > 3 && memncmp(frag, 3, "../", 3) == 0))
  {
    fatal(lookup_relative, base, mod, frag, fragl, attrs, vertices, &r);
  }
  else
  {
    ctx.path = frag;
    ctx.path_len = fragl;
    fatal(graph_lookup, g_graph, lookup_callback, candidate, &ctx, &mm_tmp, vertices, &r);
  }

  if(r == 1)
  {
    if(np)
      *np = vertex_value(vertices[0]);
    if(nodes)
      fatal(selection_add_node, nodes, vertex_value(vertices[0]), 0);

    goto XAPI_FINALIZE;
  }

  xapi_info_pushw("ref", frag, fragl);
  if(fname && loc) {
    xapi_info_pushf("location", "%s line %d", fname, loc->f_lin + 1);
  }

  if(r == 0)
  {
    fail(MODULE_NOREF);
  }
  else if(r == 2)
  {
    sz = node_get_path(vertex_value(vertices[0]), path1, sizeof(path1));
    xapi_info_pushw("one", path1, sz);

    sz = node_get_path(vertex_value(vertices[1]), path2, sizeof(path2));
    xapi_info_pushw("two", path2, sz);

    fail(MODULE_AMBIGREF);
  }

finally:
  free(mm_tmp);
coda;
}

xapi pattern_lookup(
    const pattern * restrict ref
  , const module * restrict mod
  , const node * restrict base
  , uint32_t attrs
  , selection * restrict nodes
  , node ** restrict np
)
{
  enter;

  int x;

  pattern_render_result * rendered = 0;
  pattern_render_fragment * fragment;

  /* render the reference pattern to resolve classes and alternations */
  fatal(pattern_render, ref, &rendered);

  fragment = rendered->fragments;
  for(x = 0; x < rendered->size; x++)
  {
    fatal(pattern_lookup_fragment, fragment->text, fragment->len, ref->fname, &ref->loc, mod, base, attrs, nodes, np);
    fragment = pattern_render_fragment_next(fragment);
  }

finally:
  free(rendered);
coda;
}
