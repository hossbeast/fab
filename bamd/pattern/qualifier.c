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
    default:
      RTA;
  }
}

static void qualifier_describe(const pattern_segment * restrict fn, bacon_writer * restrict builder_writer)
{
  const pattern_qualifier * qualifier = &fn->qualifier;

  { MAPPING STRING("qualifier-type")
    STRING(attrs16_name_byvalue(pattern_qualifier_type_attrs, qualifier->qualifier_type))
  }
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

  satisfied = pattern_qualifier_match(qualifier->qualifier_type, &ctx->node->vertex);
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

#define qualifier_cat 0
static pattern_segment_vtable vtable = PATTERN_SEGMENT_VTABLE(qualifier, PATTERN_QUALIFIER, 0);

//
// public
//

bool pattern_qualifier_check(pattern_qualifier_type qualifier_type, vertex_type type)
{
  bool satisfied;
  vertex_kind kind;
  vertex_filetype filetype;

  kind = type;
  kind &= VERTEX_KIND_OPT;

  filetype = type;
  filetype &= VERTEX_FILETYPE_OPT;

  satisfied = false;
  switch(qualifier_type)
  {
    case PATTERN_QUALIFIER_TYPE_DIR:
      satisfied =  kind == VERTEX_KIND_FSENT
            && filetype == VERTEX_FILETYPE_DIR
            ;
      break;
    case PATTERN_QUALIFIER_TYPE_DIR_NONEMPTY:
      satisfied =  kind == VERTEX_KIND_FSENT
            && filetype == VERTEX_FILETYPE_DIR
            && 1    // actually check child-count
            ;
      break;
    case PATTERN_QUALIFIER_TYPE_FILE:
      satisfied =  kind == VERTEX_KIND_FSENT
            && filetype == VERTEX_FILETYPE_REG
            ;
      break;
    case PATTERN_QUALIFIER_TYPE_SYMLINK:
      satisfied =  kind == VERTEX_KIND_FSENT
            && filetype == VERTEX_FILETYPE_SYMLINK
            ;
      break;
    default:
      RTA;
  }

  return satisfied;
}

bool pattern_qualifier_match(pattern_qualifier_type qualifier_type, const moria_vertex * restrict v)
{
  /* N.B. symlinks are resolved before rules are run */
  switch(qualifier_type)
  {
    case PATTERN_QUALIFIER_TYPE_DIR:
    case PATTERN_QUALIFIER_TYPE_DIR_NONEMPTY:
    case PATTERN_QUALIFIER_TYPE_FILE:
    case PATTERN_QUALIFIER_TYPE_SYMLINK:
      v = moria_vertex_follow(v);
      return pattern_qualifier_check(qualifier_type, vertex_type_get(v));
    default:
  }

  RTA;
}

void pattern_qualifier_mk(pattern_segment ** restrict rv, const yyu_location * restrict loc, pattern_qualifier_type qualifier_type)
{
  pattern_segment * n = 0;

  xmalloc(&n, sizeof(*n));
  pattern_segment_init(n, &vtable, loc);

  n->qualifier.qualifier_type = qualifier_type;

  *rv = n;
}
