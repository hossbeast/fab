/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>

#include "common/attrs.h"
#include "narrator.h"
#include "xlinux/xstdlib.h"
#include "bacon/value.h"
#include "bacon/set.h"
#include "bacon/builder.h"

#include "section.h"
#include "pattern.internal.h"
#include "segment.h"
#include "system.h"
#include "events.h"
#include "channel.h"
#include "segments.h"

#include "zcat.h"

//
// internal
//

void pattern_section_describe(const pattern_section * restrict n, bacon_writer * restrict builder_writer)
{
  SET
  { MAPPING
    STRING("nodeset")
    STRING(attrs16_name_byvalue(pattern_nodeset_attrs, n->nodeset))
  }
  { MAPPING
    STRING("graph")
    STRING(attrs16_name_byvalue(pattern_graph_attrs, n->graph))
  }
  { MAPPING
    STRING("axis")
    STRING(attrs16_name_byvalue(pattern_axis_attrs, n->axis))
  }
  { MAPPING
    STRING("has-type-qualifiers")
    BOOL(n->has_type_qualifiers)
  }
  if(!llist_empty(&n->qualifiers)) {
    MAPPING
    STRING("segments-list")
    pattern_segments_list_describe(&n->qualifiers, builder_writer);
  }
}

void pattern_section_free(pattern_section * restrict n)
{
  if(!n) {
    return;
  }

  pattern_segments_list_free(&n->qualifiers);
	free(n);
}

int pattern_section_cmp(const pattern_section * A, const pattern_section * B)
{
  int d;

  if(!A)
    return -1;

  if(!B)
    return 1;

  if((d = INTCMP(A->nodeset, B->nodeset)))
    return d;

  if((d = INTCMP(A->graph, B->graph)))
    return d;

  if((d = INTCMP(A->axis, B->axis)))
    return d;

  return pattern_segments_list_cmp(&A->qualifiers, &B->qualifiers);
}

int pattern_sections_cmp(const llist * restrict A, const llist * restrict B)
{
  const pattern_section * a;
  const pattern_section * b;
  int d;

  a = llist_first(A, typeof(*a), lln);
  b = llist_first(B, typeof(*a), lln);

  while(a || b)
  {
    if((d = pattern_section_cmp(a, b))) {
      return d;
    }

    a = llist_next(A, a, lln);
    b = llist_next(B, b, lln);
  }

  return 0;
}

//
// public
//

void pattern_section_mk(
    pattern_section ** restrict rv
  , const yyu_location * restrict loc
  , pattern_nodeset nodeset
  , pattern_graph graph
  , pattern_axis axis
  , llist * restrict qualifiers_list
  , bool has_type_qualifiers
)
{
  pattern_section *section;

  xmalloc(&section, sizeof(*section));
	memcpy(&section->loc, loc, sizeof(section->loc));

  section->nodeset = nodeset;
  section->graph = graph;
  section->axis = axis;

  llist_init(&section->qualifiers);
  if(qualifiers_list && qualifiers_list->next) {
    llist_splice_head(&section->qualifiers, qualifiers_list);
  }
  section->has_type_qualifiers = has_type_qualifiers;

  *rv = section;
}

size_t pattern_section_cat(
    char * restrict dst
  , size_t sz
  , const bacon_value * restrict vars
  , const struct pattern_section * restrict section
)
{
  const pattern_segments *segs;
  size_t z;

  z = 0;
  llist_foreach(&section->qualifiers, segs, lln) {
    z += pattern_segments_cat(dst + z, sz - z, vars, segs);
  }

  return z;
}

void pattern_section_say(const pattern_section * restrict n, bool first, narrator * restrict N)
{
  bool qualnot = !llist_empty(&n->qualifiers) && llist_first(&n->qualifiers, pattern_segments, lln) ->qualifier_type == PATTERN_QUALIFIER_TYPE_NOT;
  bool only = false;

  if(n->nodeset == PATTERN_NODESET_ANCHOR)
  {
    xsays("$^A");
  }
  else if(n->nodeset == PATTERN_NODESET_MODULE)
  {
    xsays("$^D");
  }
  else if(n->nodeset == PATTERN_NODESET_SELF)
  {
    xsays(".");
  }
  else if(n->graph == PATTERN_GRAPH_FS && n->axis == PATTERN_AXIS_UP)
  {
    xsays("..");
  }
  else if(n->nodeset == PATTERN_NODESET_SHADOW)
  {
    xsays("/");
  }
  else if(n->nodeset == PATTERN_NODESET_ROOT)
  {

  }
  else if(n->nodeset)
  {
    RTA;
  }
  else if(n->graph == PATTERN_GRAPH_FS && n->axis == PATTERN_AXIS_DOWN)
  {
    if(!first) {
      xsayc('/');
    }

    if(llist_empty(&n->qualifiers) || qualnot)
    {
      xsays("*");
    }
    else
    {
      only = true;
    }
  }
  else if(n->graph == PATTERN_GRAPH_DIRS && n->axis == PATTERN_AXIS_SELF_OR_BELOW)
  {
    if(!first) {
      xsayc('/');
    }

    xsays("**");
    only = false;
  }
  else if(n->graph && !n->axis)
  {
    xsays("//");
    xsays(attrs16_name_byvalue(pattern_graph_attrs, PATTERN_GRAPH_OPT & n->graph));
  }
  else if(!n->graph && n->axis)
  {
    xsays("//");
    xsays(attrs16_name_byvalue(pattern_axis_attrs, PATTERN_AXIS_OPT & n->axis));
  }
  else
  {
    xsays("//");
    xsays(attrs16_name_byvalue(pattern_graph_attrs, PATTERN_GRAPH_OPT & n->graph));
    xsayc(':');
    xsays(attrs16_name_byvalue(pattern_axis_attrs, PATTERN_AXIS_OPT & n->axis));
  }

  if(!llist_empty(&n->qualifiers)) {
    pattern_segments_list_say(&n->qualifiers, only, N);
  }
}
