/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "bacon/value.h"
#include "moria.h"
#include "moria/graph.h"
#include "moria/vertex.h"
#include "resolve.h"
#include "fsent.h"
#include "vertex.h"
#include "pattern.h"
#include "shadow.h"
#include "segments.h"
#include "module.h"

static const moria_vertex *follow(const moria_vertex * restrict v, const module * restrict mod, bool nofollow)
{
  const moria_vertex *next;

  next = v;
  do {
    v = next;
    switch(vertex_type_get(v)) {
      case VERTEX_CONTEXT_MODULE:
        if(mod) {
          next = &mod->shadow->vertex;
        }
        break;
      case VERTEX_SYMLINK:
        if(!v->ref) {
          break;
        }
      case VERTEX_SHADOW_LINK:
        /* this particular link is always followed */
        if(vertex_type_get(v->ref) == VERTEX_CONTEXT_MODULE) {
          next = v->ref;
        } else if(!nofollow) {
          next = v->ref;
        }
      default:
        break;
    }
  } while(next != v);

  return v;
}

static int pattern_resolve_bound(resolve_context * restrict ctx)
{
  const pattern_section *section;
  const pattern_segments *segments;
  const fsent *fse = 0;
  const moria_vertex *v = 0;
  char label[256];
  uint16_t label_len;

  RUNTIME_ASSERT(ctx->ref->bound);

  if(ctx->base) {
    v = &ctx->base->vertex;
  }

  /* left-to-right */
  llist_foreach(&ctx->ref->sections, section, lln) {
    ctx->last = (moria_vertex *)v;

#define CASE3(x, y, z) ((x << 16) | (y << 8) | z)
    switch(CASE3(section->nodeset, section->graph, section->axis)) {
      case CASE3(PATTERN_NODESET_SELF, 0, 0):
        break;
      case CASE3(PATTERN_NODESET_ROOT, 0, 0):
        RUNTIME_ASSERT(g_root);
        v = &g_root->vertex;
        break;
      case CASE3(PATTERN_NODESET_SHADOW, 0, 0):
        RUNTIME_ASSERT(g_shadow);
        v = &g_shadow->vertex;
        break;
      case CASE3(0, PATTERN_GRAPH_FS, PATTERN_AXIS_UP):
        if(!v) {
          return ctx->r = 0;
        }
        fse = containerof(v, typeof(*fse), vertex);
        fse = fsent_parent(fse);
        v = &fse->vertex;
        v = follow(v, ctx->mod, ctx->nofollow);
        break;
      case CASE3(0, PATTERN_GRAPH_FS, PATTERN_AXIS_DOWN):
        if(!v) {
          break;
        }
        /* the section must have a single qualifiers-list
         * and it may not contain any type qualifiers. this is enforced in the parser */
        segments = llist_first(&section->qualifiers, typeof(*segments), lln);
        RUNTIME_ASSERT(segments->qualifier_type == PATTERN_QUALIFIER_TYPE_AND);
        RUNTIME_ASSERT(segments->type_qualifiers_mask == 0);

        /* TODO : should add a fast-path for single-segment word section */
        label_len = pattern_segments_cat(label, sizeof(label), ctx->vars, segments);
        if((v = moria_vertex_downw(v, label, label_len))) {
          v = follow(v, ctx->mod, ctx->nofollow);
        }
        break;
      default:
        RTA;
    }
  }

  if(v && !ctx->hidden && !vertex_exists_get(v)) {
    v = 0;
  }

  if((ctx->vertices[0] = (moria_vertex *)v)) {
    return ctx->r = 1;
  }

  return ctx->r = 0;
}

int pattern_resolve_path(resolve_context * restrict ctx, path_pattern * restrict pattern)
{
  ctx->ref = &pattern->pattern;
  return pattern_resolve_bound(ctx);
}

int pattern_resolve_lookup(resolve_context * restrict ctx, lookup_pattern * restrict pattern)
{
  ctx->ref = &pattern->pattern;
  return pattern_resolve_bound(ctx);
}
