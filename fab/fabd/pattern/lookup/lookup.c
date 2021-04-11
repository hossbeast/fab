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

#include "moria/graph.h"
#include "moria/vertex.h"
#include "xlinux/xstdlib.h"

#include "lookup.h"
#include "fsent.h"
#include "pattern.h"
#include "render.h"
#include "selection.h"
#include "channel.h"

#include "zbuffer.h"

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

static bool candidate(void * ctxp, const moria_vertex * restrict v)
{
  lookup_context * ctx = ctxp;

  if((ctx->attrs & PATTERN_LOOKUP_DIR) == PATTERN_LOOKUP_DIR)
  {
    if(fsent_filetype_get(containerof(v, fsent, vertex)) != VERTEX_FILETYPE_DIR) {
      return false;
    }
  }
  if((ctx->attrs & PATTERN_LOOKUP_MODEL) == PATTERN_LOOKUP_MODEL)
  {
    if(!moria_vertex_downw(v, fsent_model_name, fsent_model_name_len)) {
      return false;
    }
  }
  else if((ctx->attrs & PATTERN_LOOKUP_MODULE) == PATTERN_LOOKUP_MODULE)
  {
    if(!moria_vertex_downw(v, fsent_module_name, fsent_module_name_len)) {
      return false;
    }
  }

  return true;
}

static xapi pattern_lookup_fragment(
    const char * restrict frag
  , uint16_t fragl
  , const char * restrict fname
  , const yyu_location * restrict loc
  , uint32_t attrs
  , selection * restrict nodes
  , channel * restrict chan
)
{
  enter;

  int r;
  lookup_context ctx = { 0 };
  moria_vertex * vertices[2];
  size_t sz;
  char *s;
  size_t z;
  fabipc_message *msg;

  ctx.attrs = attrs;
  ctx.path = frag;
  ctx.path_len = fragl;
  fatal(moria_graph_lookup, &g_graph, g_graph_ht, lookup_callback, candidate, &ctx, vertices, &r);

  if(r == 1)
  {
    fatal(selection_add_vertex, nodes, vertices[0], 0);
    goto XAPI_FINALIZE;
  }

  msg = channel_produce(chan);
  msg->id = chan->msgid;
  msg->type = FABIPC_MSG_RESULT;
  msg->code = EINVAL;

  z = 0;
  s = msg->text;
  sz = sizeof(msg->text);

  if(r == 0) {
    z += znloadf(s + z, sz - z, "unresolved ref %.*s", (int)fragl, frag);
  } else {
    z += znloadf(s + z, sz - z, "ambiguous ref %.*s", (int)fragl, frag);
  }
  z += znloadf(s + z, sz - z, " @ %s", fname);
  z += znloadf(s + z, sz - z, " [%d,%d - %d,%d]", loc->f_lin + 1, loc->f_col + 1, loc->l_lin + 1, loc->l_col + 1);

  if(r == 2) {
    z += znloads(s + z, sz - z, " ");
    z += fsent_path_znload(s + z, sz - z, containerof(vertices[0], fsent, vertex));
    z += znloads(s + z, sz - z, " and ");
    z += fsent_path_znload(s + z, sz - z, containerof(vertices[1], fsent, vertex));
  }
  z += znloads(s + z, sz - z, "\n");

  msg->size = z;
  channel_post(chan, msg);

  finally : coda;
}

xapi pattern_lookup(const pattern * restrict ref, uint32_t attrs, selection * restrict nodes, channel * restrict chan)
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
    fatal(pattern_lookup_fragment, fragment->text, fragment->len, ref->fname, &ref->loc, attrs, nodes, chan);
    fragment = pattern_render_fragment_next(fragment);
  }

finally:
  wfree(rendered);
coda;
}
