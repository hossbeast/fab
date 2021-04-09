/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

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

#include "common/attrs.h"
#include "narrator.h"
#include "xlinux/xstdlib.h"

#include "section.h"
#include "pattern.internal.h"
#include "segment.h"

//
// internal
//

xapi pattern_section_say(const pattern_section * restrict n, bool first, narrator * restrict N)
{
  enter;

  bool qualnot = n->qualifiers_head && n->qualifiers_head->qualifier_type == PATTERN_QUALIFIER_TYPE_NOT;
  bool only = false;

  if(n->nodeset == PATTERN_NODESET_MATCHDIR)
  {
    xsays("$^D");
  }
  else if(n->graph == PATTERN_GRAPH_FS && n->nodeset == PATTERN_NODESET_SELF && !n->qualifiers_head)
  {
    xsays(".");
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
    xsays("//");
    xsays(attrs16_name_byvalue(pattern_nodeset_attrs, PATTERN_NODESET_OPT & n->nodeset));
  }
  else if(n->graph == PATTERN_GRAPH_FS && n->axis == PATTERN_AXIS_DOWN)
  {
    if(!first) {
      xsayc('/');
    }

    if(!n->qualifiers_head || qualnot)
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

  if(n->qualifiers_head)
    fatal(pattern_segments_say, n->qualifiers_head, only, N);

  finally : coda;
}

void pattern_section_free(pattern_section * restrict n)
{
  pattern_segments_list_free(n->qualifiers_head);
	wfree(n);
}

int pattern_section_cmp(const pattern_section * A, const pattern_section * B)
{
  int d;

  if(!A && !B)
    return 0;

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

  return pattern_segments_cmp(A->qualifiers_head, B->qualifiers_head);
}

int pattern_sections_cmp(const pattern_section * A, const pattern_section * B)
{
  const pattern_section * a = A;
  const pattern_section * b = B;
  int d;

  while(1)
  {
    if(!a && !b)
      return 0;

    if(!a)
      return -1;

    if(!b)
      return 1;

    if((d = pattern_section_cmp(a, b)))
      return d;

    a = containerof(a->chn.next, typeof(*a), chn);
    if(a == A)
      a = NULL;

    b = containerof(b->chn.next, typeof(*b), chn);
    if(b == B)
      b = NULL;
  }
}

//
// public
//

xapi pattern_section_mk(
    pattern_section ** restrict rv
  , const yyu_location * restrict loc
  , pattern_nodeset nodeset
  , pattern_graph graph
  , pattern_axis axis
  , pattern_segments * restrict qualifiers_head
)
{
  enter;

  pattern_section * p;

  fatal(xmalloc, &p, sizeof(*p));
	memcpy(&p->loc, loc, sizeof(p->loc));

  p->nodeset = nodeset;
  p->graph = graph;
  p->axis = axis;
  p->qualifiers_head = qualifiers_head;

  chain_init(p, chn);

  *rv = p;

  finally : coda;
}
