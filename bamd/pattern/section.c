/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>

#include "common/attrs.h"
#include "narrator.h"
#include "xlinux/xstdlib.h"
#include "moria.h"
#include "bacon/value.h"
#include "bacon/set.h"
#include "bacon/builder.h"

#include "section.h"
#include "pattern.internal.h"
#include "segment.h"
#include "system.h"
#include "events/events.h"
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
    STRING("type-qualifiers-mask")
    HEX(n->type_qualifiers_mask)
  }
  if(!llist_empty(&n->qualifiers)) {
    MAPPING
    STRING("segments-list")
    pattern_segments_list_describe(&n->qualifiers, builder_writer);
  }
}

void pattern_section_show(const pattern_section * restrict section)
{
  bacon_writer writer, *builder_writer;

  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, g_narrator_stdout);

  pattern_section_describe(section, builder_writer);

  bacon_writer_close(&writer);
  bacon_writer_destroy(&writer);
  printf("\n");
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
  , uint32_t type_qualifiers_mask
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
  section->type_qualifiers_mask = type_qualifiers_mask;

  *rv = section;
}

void pattern_section_say(const pattern_section * restrict n, bool first, narrator * restrict N)
{
  bool qualnot = !llist_empty(&n->qualifiers) && llist_first(&n->qualifiers, pattern_segments, lln) ->qualifier_type == PATTERN_QUALIFIER_TYPE_NOT;
  bool only = false;

  if(n->nodeset == PATTERN_NODESET_ANCHOR_REL)
  {
    xsays("$^A");
  }
  else if(n->nodeset == PATTERN_NODESET_BUILD_REL)
  {
    xsays("$^D");
  }
  else if(n->nodeset == PATTERN_NODESET_ANCHOR)
  {
    xsays("$bm_anchor");
  }
  else if(n->nodeset == PATTERN_NODESET_BUILD)
  {
    xsays("$bm_build");
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
  else if(n->nodeset == PATTERN_NODESET_BASEDIR)
  {
    xsays("$bm_basedir");
  }
  else if(n->nodeset == PATTERN_NODESET_HOMEDIR)
  {
    xsays("$bm_homedir");
  }
  else if(n->nodeset == PATTERN_NODESET_IPCDIR)
  {
    xsays("$bm_ipcdir");
  }
  else if(n->nodeset)
  {
    RTA;
  }
  else if(n->graph == PATTERN_GRAPH_FS && n->axis == PATTERN_AXIS_DOWN)
  {
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

size_t pattern_section_cat(void * restrict dst, size_t sz, const struct pattern_section *n, bool first)
{
  bool qualnot;
  bool only = 0;
  size_t z;

  qualnot = !llist_empty(&n->qualifiers) && llist_first(&n->qualifiers, pattern_segments, lln) ->qualifier_type == PATTERN_QUALIFIER_TYPE_NOT;
  z = 0;

  if(n->nodeset == PATTERN_NODESET_ANCHOR_REL)
  {
    z += zcats(dst + z, sz - z, "$^A");
  }
  else if(n->nodeset == PATTERN_NODESET_BUILD_REL)
  {
    z += zcats(dst + z, sz - z, "$^D");
  }
  else if(n->nodeset == PATTERN_NODESET_ANCHOR)
  {
    z += zcats(dst + z, sz - z, "$bm_anchor");
  }
  else if(n->nodeset == PATTERN_NODESET_BUILD)
  {
    z += zcats(dst + z, sz - z, "$bm_build");
  }
  else if(n->nodeset == PATTERN_NODESET_SELF)
  {
    z += zcats(dst + z, sz - z, ".");
  }
  else if(n->graph == PATTERN_GRAPH_FS && n->axis == PATTERN_AXIS_UP)
  {
    z += zcats(dst + z, sz - z, "..");
  }
  else if(n->nodeset == PATTERN_NODESET_SHADOW)
  {
    z += zcats(dst + z, sz - z, "/");
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
    //if(!first) {
    //  xsayc('/');
    //}

    if(llist_empty(&n->qualifiers) || qualnot)
    {
      z += zcats(dst + z, sz - z, "*");
    }
    else
    {
      only = true;
    }
  }
  else if(n->graph == PATTERN_GRAPH_DIRS && n->axis == PATTERN_AXIS_SELF_OR_BELOW)
  {
    z += zcats(dst + z, sz - z, "**");
    only = false;
  }
  else if(n->graph && !n->axis)
  {
    z += zcats(dst + z, sz - z, "//");
    z += zcats(dst + z, sz - z, attrs16_name_byvalue(pattern_graph_attrs, PATTERN_GRAPH_OPT & n->graph));
  }
  else if(!n->graph && n->axis)
  {
    z += zcats(dst + z, sz - z, "//");
    z += zcats(dst + z, sz - z, attrs16_name_byvalue(pattern_axis_attrs, PATTERN_AXIS_OPT & n->axis));
  }
  else
  {
    z += zcats(dst + z, sz - z, "//");
    z += zcats(dst + z, sz - z, attrs16_name_byvalue(pattern_graph_attrs, PATTERN_GRAPH_OPT & n->graph));
    z += zcatc(dst + z, sz - z, ':');
    z += zcats(dst + z, sz - z, attrs16_name_byvalue(pattern_axis_attrs, PATTERN_AXIS_OPT & n->axis));
  }

  if(!llist_empty(&n->qualifiers)) {
    z += pattern_segments_list_cat(dst + z, sz - z, NULL, &n->qualifiers, only);
  }

  return z;
}

bool pattern_section_evaluate_type_qualifiers(const pattern_section * restrict section, const moria_vertex * restrict v)
{
  pattern_segments *segments;

  llist_foreach(&section->qualifiers, segments, lln) {
    if(!segments->type_qualifiers_mask) {
      continue;
    }

    if(!pattern_segments_evaluate_type_qualifiers(segments, v)) {
      return false;
    }
  }

  return true;
}
