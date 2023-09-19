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

#include <errno.h>

#include "types.h"

#include "moria/graph.h"
#include "moria/vertex.h"
#include "xlinux/xstdlib.h"

#include "lookup.h"
#include "fsent.h"
#include "pattern.h"
#include "render.h"
#include "selection.h"
#include "channel.h"
#include "events.h"
#include "system_state.h"

#include "zbuffer.h"

typedef struct lookup_context {
  const char * path;
  uint16_t path_len;
  uint32_t attrs;

  const char * segment;
} lookup_context;

static void __attribute__((nonnull(1))) lookup_callback(
    void * restrict _ctx
  , const char ** restrict label
  , uint16_t * restrict label_len
)
{
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
    if(!moria_vertex_downw(v, MMS(fsent_name_model))) {
      return false;
    }
  }
  else if((ctx->attrs & PATTERN_LOOKUP_MODULE) == PATTERN_LOOKUP_MODULE)
  {
    if(!moria_vertex_downw(v, MMS(fsent_name_module))) {
      return false;
    }
  }

  return true;
}

__attribute__((nonnull))
static void fragment_lookup_error(
    fabipc_message * restrict msg
  , int r
  , const char * restrict frag
  , uint16_t fragl
  , const char * restrict fname
  , const yyu_location * restrict loc
  , moria_vertex ** restrict vertices
)
{
  size_t z;
  size_t sz;
  char *s;

  z = 0;
  s = msg->text;
  sz = sizeof(msg->text);

  if(r == 0) {
    z += znloadf(s + z, sz - z, "unresolved ref %.*s", (int)fragl, frag);
    msg->code = ENOENT;
  } else {
    z += znloadf(s + z, sz - z, "ambiguous ref %.*s", (int)fragl, frag);
    msg->code = EINVAL;
  }
  z += znloadf(s + z, sz - z, " @ %s", fname);
  z += znloadf(s + z, sz - z, " [%d,%d - %d,%d]", loc->f_lin + 1, loc->f_col + 1, loc->l_lin + 1, loc->l_col + 1);

  if(r == 2) {
    z += znloads(s + z, sz - z, " ");
    z += fsent_path_znload(s + z, sz - z, containerof(vertices[0], fsent, vertex));
    z += znloads(s + z, sz - z, " and ");
    z += fsent_path_znload(s + z, sz - z, containerof(vertices[1], fsent, vertex));
  }

  msg->size = z;
}

static void pattern_lookup_fragment(
    const char * restrict frag
  , uint16_t fragl
  , const char * restrict fname
  , const yyu_location * restrict loc
  , uint32_t attrs
  , selection * restrict nodes
  , channel * restrict chan
)
{
  int r;
  lookup_context ctx = { 0 };
  moria_vertex * vertices[2];
  fabipc_message *msg;

  ctx.attrs = attrs;
  ctx.path = frag;
  ctx.path_len = fragl;
  moria_graph_lookup(&g_graph, g_graph_ht, lookup_callback, candidate, &ctx, vertices, &r);

  if(r == 1)
  {
    selection_add_vertex(nodes, vertices[0], 0);
    return;
  }

  if(chan)
  {
    msg = channel_produce(chan);
    msg->type = FABIPC_MSG_RESPONSE;
    fragment_lookup_error(msg, r, frag, fragl, fname, loc, vertices);
    channel_post(chan, msg);
  }
  else
  {
    system_error = true;
    if(events_would(FABIPC_EVENT_SYSTEM_STATE, &chan, &msg))
    {
      fragment_lookup_error(msg, r, frag, fragl, fname, loc, vertices);
      events_publish(chan, msg);
    }
  }
}

void pattern_lookup(const pattern * restrict ref, uint32_t attrs, selection * restrict nodes, channel * restrict chan)
{
  int x;

  pattern_render_result * rendered = 0;
  pattern_render_fragment * fragment;

  /* render the reference pattern to resolve classes and alternations */
  pattern_render(ref, &rendered);

  fragment = rendered->fragments;
  for(x = 0; x < rendered->size; x++)
  {
    pattern_lookup_fragment(fragment->text, fragment->len, ref->fname, &ref->loc, attrs, nodes, chan);
    fragment = pattern_render_fragment_next(fragment);
  }

  wfree(rendered);
}
