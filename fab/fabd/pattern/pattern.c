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

#include <string.h>

#include "moria/vertex.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "value/writer.h"
#include "valyria/chain.h"
#include "xapi.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xstring.h"

#include "pattern.internal.h"
#include "section.internal.h"
#include "segment.internal.h"
#include "extern.h"
#include "module.h"
#include "node.h"

#include "common/attrs.h"
#include "zbuffer.h"

attrs16 * pattern_segment_type_attrs = (attrs16[]){{
#undef ATTR_DEF
#define ATTR_DEF(x, y) + 1
    num : 0 PATTERN_SEGMENT_TYPE_TABLE
  , members : (member16[]) {
#undef ATTR_DEF
#define ATTR_DEF(x, y) { name : #x, value : UINT16_C(y), range : PATTERN_SEGMENT_TYPE_OPT },
PATTERN_SEGMENT_TYPE_TABLE
  }
}};

attrs16 * pattern_graph_attrs = (attrs16[]){{
#undef ATTR_NAME_DEF
#define ATTR_NAME_DEF(x, s, y) + 1
    num : 0 PATTERN_GRAPH_TABLE
  , members : (member16[]) {
#undef ATTR_NAME_DEF
#define ATTR_NAME_DEF(x, s, y) { name : s, value : UINT16_C(y), range : PATTERN_GRAPH_OPT },
PATTERN_GRAPH_TABLE
  }
}};

attrs16 * pattern_nodeset_attrs = (attrs16[]){{
#undef ATTR_NAME_DEF
#define ATTR_NAME_DEF(x, s, y) + 1
    num : 0 PATTERN_NODESET_TABLE
  , members : (member16[]) {
#undef ATTR_NAME_DEF
#define ATTR_NAME_DEF(x, s, y) { name : s, value : UINT16_C(y), range : PATTERN_NODESET_OPT },
PATTERN_NODESET_TABLE
  }
}};

attrs16 * pattern_axis_attrs = (attrs16[]){{
#undef ATTR_NAME_DEF
#define ATTR_NAME_DEF(x, s, y) + 1
    num : 0 PATTERN_AXIS_TABLE
  , members : (member16[]) {
#undef ATTR_NAME_DEF
#define ATTR_NAME_DEF(x, s, y) { name : s, value : UINT16_C(y), range : PATTERN_AXIS_OPT },
PATTERN_AXIS_TABLE
  }
}};

attrs16 * pattern_qualifier_type_attrs = (attrs16[]){{
#undef ATTR_DEF
#define ATTR_DEF(x, y) + 1
    num : 0 PATTERN_QUALIFIER_TYPE_TABLE
  , members : (member16[]) {
#undef ATTR_DEF
#define ATTR_DEF(x, y) { name : #x, value : UINT16_C(y), range : PATTERN_QUALIFIER_TYPE_OPT },
PATTERN_QUALIFIER_TYPE_TABLE
  }
}};

attrs16 * pattern_replacement_type_attrs = (attrs16[]){{
#undef ATTR_DEF
#define ATTR_DEF(x, y) + 1
    num : 0 PATTERN_REPLACEMENT_TYPE_TABLE
  , members : (member16[]) {
#undef ATTR_DEF
#define ATTR_DEF(x, y) { name : #x, value : UINT16_C(y), range : PATTERN_REPLACEMENT_TYPE_OPT },
PATTERN_REPLACEMENT_TYPE_TABLE
  }
}};

//
// static
//

static void __attribute__((constructor)) init()
{
  attrs16_init(pattern_segment_type_attrs);
  attrs16_init(pattern_axis_attrs);
  attrs16_init(pattern_graph_attrs);
  attrs16_init(pattern_qualifier_type_attrs);
  attrs16_init(pattern_replacement_type_attrs);
}

//
// internal
//

xapi pattern_segment_init(pattern_segment * restrict n, const pattern_segment_vtable * restrict vtab, const yyu_location * restrict loc)
{
  enter;

  n->vtab = vtab;
  n->type = vtab->type;
	memcpy(&n->loc, loc, sizeof(n->loc));

  finally : coda;
}

xapi pattern_segments_say(const pattern_segments * segment_list, bool only, narrator * restrict N)
{
  enter;

  const pattern_segments * segments;
  const chain *T;

  chain_foreach(T, segments, chn, segment_list) {
    if(segments->qualifier_type == PATTERN_QUALIFIER_TYPE_NOT) {
      xsays("~");
    } else if(segments->qualifier_type == PATTERN_QUALIFIER_TYPE_AND && !only) {
      xsays("+");
    }

    fatal(pattern_segment_chain_say, segments->segment_head, N);
    only = false;
  }

  finally : coda;
}

xapi pattern_segment_chain_say(const pattern_segment * head, narrator * restrict N)
{
  enter;

  const pattern_segment * segment;
  const chain *T;

  chain_foreach(T, segment, chn, head) {
    fatal(segment->vtab->say, segment, N);
  }

  finally : coda;
}

//
// public
//

xapi pattern_mk(
    pattern ** restrict rv
  , const struct yyu_location * restrict loc
  , pattern_section * restrict section_head
)
{
  enter;

  pattern * p = 0;

  fatal(xmalloc, &p, sizeof(*p));
  memcpy(&p->loc, loc, sizeof(p->loc));

  p->section_head = section_head;

  *rv = p;
  p = 0;

finally:
  pattern_free(p);
coda;
}

xapi pattern_segments_mk(
    pattern_segments ** restrict rv
  , const struct yyu_location * restrict loc
  , pattern_qualifier_type qualifier_type
  , pattern_segment * restrict segment_head
)
{
  enter;

  pattern_segments * p = 0;

  fatal(xmalloc, &p, sizeof(*p));
  memcpy(&p->loc, loc, sizeof(p->loc));

  p->qualifier_type = qualifier_type;
  p->segment_head = segment_head;
  chain_init(p, chn);

  *rv = p;

  finally : coda;
}

void pattern_free(pattern * restrict pat)
{
  pattern_section * section;
  const chain *T[2];

  if(!pat)
    return;

  chain_foreach_safe(T, section, chn, pat->section_head) {
    pattern_section_free(section);
  }

  wfree(pat->fname);
	wfree(pat);
}

void pattern_segments_list_free(pattern_segments * restrict segments_list)
{
  pattern_segments * segments;
  const chain *T[2];

  if(!segments_list)
    return;

  chain_foreach_safe(T, segments, chn, segments_list) {
    pattern_segments_free(segments);
  }
}

void pattern_segments_free(pattern_segments * restrict segments)
{
  pattern_segment_list_free(segments->segment_head);
  wfree(segments);
}

void pattern_segment_list_free(pattern_segment * restrict segment_list)
{
  pattern_segment *segment;
  const chain *T[2];

  chain_foreach_safe(T, segment, chn, segment_list) {
    pattern_segment_free(segment);
	}
}

void pattern_segment_free(pattern_segment * restrict segment)
{
  segment->vtab->destroy(segment);
  wfree(segment);
}

xapi pattern_say(const pattern * restrict pattern, narrator * restrict N)
{
  enter;

  pattern_section * section;
  const chain *T;

  int x = 0;
  chain_foreach(T, section, chn, pattern->section_head) {
    fatal(pattern_section_say, section, x++ == 0, N);
  }

  finally : coda;
}

int pattern_cmp(const pattern * A, const pattern * B)
{
  if(!A && !B)
    return 0;

  if(!A)
    return -1;

  if(!B)
    return -1;

  return pattern_sections_cmp(A->section_head, B->section_head);
}
