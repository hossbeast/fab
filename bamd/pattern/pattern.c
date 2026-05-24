/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <string.h>

#include "narrator/fixed.h"
#include "xlinux/xstdlib.h"
#include "bacon/builder.h"

#include "pattern.internal.h"
#include "section.h"

#include "common/attrs.h"
#include "zcat.h"

attrs16 * pattern_segment_type_attrs = (attrs16[]){{
#undef DEF
#define DEF(x, y) + 1
    num : 0 PATTERN_SEGMENT_TYPE_TABLE
  , members : (member16[]) {
#undef DEF
#define DEF(x, y) { name : #x, value : UINT16_C(y), range : PATTERN_SEGMENT_TYPE_OPT },
PATTERN_SEGMENT_TYPE_TABLE
  }
}};

attrs16 * pattern_graph_attrs = (attrs16[]){{
#undef DEF
#define DEF(x, s, y) + 1
    num : 0 PATTERN_GRAPH_TABLE
  , members : (member16[]) {
#undef DEF
#define DEF(x, s, y) { name : s, value : UINT16_C(y), range : PATTERN_GRAPH_OPT },
PATTERN_GRAPH_TABLE
  }
}};

attrs16 * pattern_nodeset_attrs = (attrs16[]){{
#undef DEF
#define DEF(x, s, y) + 1
    num : 0 PATTERN_NODESET_TABLE
  , members : (member16[]) {
#undef DEF
#define DEF(x, s, y) { name : s, value : UINT16_C(y), range : PATTERN_NODESET_OPT },
PATTERN_NODESET_TABLE
  }
}};

attrs16 * pattern_axis_attrs = (attrs16[]){{
#undef DEF
#define DEF(x, s, y) + 1
    num : 0 PATTERN_AXIS_TABLE
  , members : (member16[]) {
#undef DEF
#define DEF(x, s, y) { name : s, value : UINT16_C(y), range : PATTERN_AXIS_OPT },
PATTERN_AXIS_TABLE
  }
}};

attrs16 * pattern_qualifier_type_attrs = (attrs16[]){{
#undef DEF
#define DEF(x, y) + 1
    num : 0 PATTERN_QUALIFIER_TYPE_TABLE
  , members : (member16[]) {
#undef DEF
#define DEF(x, y) { name : #x, value : UINT16_C(y), range : PATTERN_QUALIFIER_TYPE_OPT },
PATTERN_QUALIFIER_TYPE_TABLE
  }
}};

attrs16 * pattern_variable_type_attrs = (attrs16[]){{
#undef DEF
#define DEF(x, y) + 1
    num : 0 PATTERN_VARIABLE_TYPE_TABLE
  , members : (member16[]) {
#undef DEF
#define DEF(x, y) { name : #x, value : UINT16_C(y), range : PATTERN_VARIABLE_TYPE_OPT },
PATTERN_VARIABLE_TYPE_TABLE
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
  attrs16_init(pattern_variable_type_attrs);
}

//
// internal
//

//
// public
//

void pattern_mk(
    pattern ** restrict rv
  , const struct yyu_location * restrict loc
  , llist * restrict list
)
{
  pattern *p = 0;

  xmalloc(&p, sizeof(*p));
  memcpy(&p->loc, loc, sizeof(p->loc));

  llist_init(&p->sections);
  llist_splice_head(&p->sections, list);

  llist_init(&p->lln);
  rbnode_init(&p->rbn);

  *rv = p;
}

void pattern_free(pattern * restrict pat)
{
  pattern_section *section;
  llist *T;

  if(!pat) {
    return;
  }

  llist_foreach_safe(&pat->sections, section, lln, T) {
    pattern_section_free(section);
  }

  free(pat->fname);
	free(pat);
}

void pattern_say(const struct pattern * restrict pattern, narrator * restrict N)
{
  pattern_section *section;
  int x = 0;

  /* special case */
  if((section = llist_first(&pattern->sections, typeof(*section), lln))) {
    if(llist_count(&pattern->sections) == 1 && section->nodeset == PATTERN_NODESET_ROOT) {
      xsayc('/');
    }
  }

  llist_foreach(&pattern->sections, section, lln) {
    if(x) {
      xsayc('/');
    }
    pattern_section_say(section, x++ == 0, N);
  }
}

size_t pattern_cat(void * restrict dst, size_t sz, const struct pattern * restrict pattern)
{
  pattern_section *section;
  int x = 0;
  size_t z;

  z = 0;

  /* special case */
  if((section = llist_first(&pattern->sections, typeof(*section), lln))) {
    if(llist_count(&pattern->sections) == 1 && section->nodeset == PATTERN_NODESET_ROOT) {
      z += zcatc(dst + z, sz - z, '/');
    }
  }

  llist_foreach(&pattern->sections, section, lln) {
    if(x) {
      z += zcatc(dst + z, sz - z, '/');
    }
    z += pattern_section_cat(dst + z, sz - z, section, x++ == 0);
  }

  return z;
}

int pattern_cmp(const pattern * A, const pattern * B)
{
  if(!A && !B) {
    return 0;
  }

  if(!A) {
    return -1;
  }

  if(!B) {
    return 1;
  }

  return pattern_sections_cmp(&A->sections, &B->sections);
}

void pattern_describe(const pattern * restrict pattern, struct bacon_writer * restrict builder_writer)
{
  pattern_section *section;

  { SET
    { MAPPING STRING("bound") BOOL(pattern->bound) }
    { MAPPING
      STRING("sections")
      { LIST
        llist_foreach(&pattern->sections, section, lln) {
          MAPPING
          LOADF("section %p", section)
          pattern_section_describe(section, builder_writer);
        }
      }
    }
  }
}

void pattern_show(const pattern * restrict pattern)
{
  bacon_writer writer, *builder_writer;

  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, g_narrator_stdout);

  pattern_describe(pattern, builder_writer);

  bacon_writer_close(&writer);
  bacon_writer_destroy(&writer);
  printf("\n");
}

void pattern_list_free(llist * restrict list)
{
  pattern *pat;
  llist *T;

  llist_foreach_safe(list, pat, lln, T) {
    pattern_free(pat);
  }

  llist_init(list);
}
