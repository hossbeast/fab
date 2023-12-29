/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "narrator.h"
#include "xlinux/xstdlib.h"
#include "common/attrs.h"
#include "bacon/builder.h"

#include "qualifier.h"
#include "segment.h"
#include "pattern.internal.h"
#include "generate.internal.h"
#include "search.internal.h"
#include "match.internal.h"
#include "render.internal.h"

#include "fsent.h"
#include "vertex.h"

//
// static
//

static void qualifier_say(const pattern_segment * restrict fn, const pattern_segment * restrict next, narrator * restrict N)
{
  const pattern_qualifier * n = &fn->qualifier;

  switch(n->qualifier_type)
  {
    case PATTERN_QUALIFIER_TYPE_DIR:
      xsayc('/');
      break;
    case PATTERN_QUALIFIER_TYPE_DIR_NONEMPTY:
      xsayc('F');
      break;
    case PATTERN_QUALIFIER_TYPE_FILE:
      xsayc('.');
      break;
    case PATTERN_QUALIFIER_TYPE_SYMLINK:
      xsayc('@');
      break;
    case PATTERN_QUALIFIER_TYPE_ANCHOR:
      xsays("^A");
      break;
    case PATTERN_QUALIFIER_TYPE_MODULE:
      xsays("^M");
      break;
    default:
      RTA;
  }
}

static void qualifier_describe(const pattern_segment * restrict fn, bacon_writer * restrict builder_writer)
{
  const pattern_qualifier * qualifier = &fn->qualifier;

  { MAPPING
    STRING("qualifier-type")
    STRING(attrs16_name_byvalue(pattern_qualifier_type_attrs, qualifier->qualifier_type))
  }
}

static void qualifier_render(const pattern_segment * restrict fn, pattern_render_context * restrict ctx)
{
  qualifier_say(fn, 0, ctx->narrator);
  pattern_segment_render(ctx);
}

static void qualifier_destroy(pattern_segment * restrict n)
{
}

static void qualifier_generate(const pattern_segment * restrict pat, pattern_generate_context * restrict ctx)
{
  RTA;
}

static void qualifier_search(const pattern_segment * restrict segment, pattern_search_context * restrict ctx)
{
  const pattern_qualifier * qualifier = &segment->qualifier;
  bool satisfied = false;

  satisfied = pattern_qualifier_evaluate(qualifier->qualifier_type, &ctx->node->vertex);
  ctx->traversal->unsatisfied = !satisfied;
}

static int qualifier_cmp(const pattern_segment * A, const pattern_segment *B)
{
  int d;

  if((d = INTCMP(A->qualifier.qualifier_type, B->qualifier.qualifier_type))) {
    return d;
  }

  return 0;
}

static pattern_segment_vtable vtable = PATTERN_SEGMENT_VTABLE(qualifier, PATTERN_QUALIFIER, 0);

//
// public
//

bool pattern_qualifier_evaluate(pattern_qualifier_type qualifier_type, const moria_vertex * restrict v)
{
  bool satisfied = false;

  switch(qualifier_type)
  {
    case PATTERN_QUALIFIER_TYPE_DIR:
      satisfied =  vertex_kind_get(v) == VERTEX_KIND_FSENT
            && vertex_filetype_get(v) == VERTEX_FILETYPE_DIR
            ;
      break;
    case PATTERN_QUALIFIER_TYPE_DIR_NONEMPTY:
      satisfied =  vertex_kind_get(v) == VERTEX_KIND_FSENT
            && vertex_filetype_get(v) == VERTEX_FILETYPE_DIR
            && 1    // actually check child-count
            ;
      break;
    case PATTERN_QUALIFIER_TYPE_FILE:
      satisfied =  vertex_kind_get(v) == VERTEX_KIND_FSENT
            && vertex_filetype_get(v) == VERTEX_FILETYPE_REG
            ;
      break;
    case PATTERN_QUALIFIER_TYPE_SYMLINK:
      satisfied =  vertex_kind_get(v) == VERTEX_KIND_FSENT
            && vertex_filetype_get(v) == VERTEX_FILETYPE_LINK
            ;
      break;
    default:
      RTA;
  }

  return satisfied;
}

void pattern_qualifier_mk(pattern_segment ** restrict rv, const yyu_location * restrict loc, pattern_qualifier_type qualifier_type)
{
  pattern_segment * n = 0;

  xmalloc(&n, sizeof(*n));
  pattern_segment_init(n, &vtable, loc);

  n->qualifier.qualifier_type = qualifier_type;

  *rv = n;
}
