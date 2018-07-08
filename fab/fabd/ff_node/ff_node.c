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
#include "valyria/list.h"
#include "xapi.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xstring.h"

#include "ff_node.internal.h"
#include "artifact.h"
#include "extern.h"
#include "ff.h"
#include "ff_node_alternation.internal.h"
#include "ff_node_artifact.internal.h"
#include "ff_node_artifact_pattern.internal.h"
#include "ff_node_char.internal.h"
#include "ff_node_class.internal.h"
#include "ff_node_dirsep.internal.h"
#include "ff_node_pattern.internal.h"
#include "ff_node_patterns.internal.h"
#include "ff_node_range.internal.h"
#include "ff_node_require.internal.h"
#include "ff_node_rule.internal.h"
#include "ff_node_stem.internal.h"
#include "ff_node_string.internal.h"
#include "ff_node_strings.internal.h"
#include "ff_node_use.internal.h"
#include "ff_node_usrvar.internal.h"
#include "ff_node_vardef.internal.h"
#include "ff_node_variant.internal.h"
#include "ff_node_varname.internal.h"
#include "ff_node_word.internal.h"
#include "ff_parser.h"
#include "module.h"
#include "node.h"

#include "attrs16.h"
#include "zbuffer.h"

static attrs16 * ffn_attrs = (attrs16[]) {
#define FFN_ATTR_DEF(a, b) { value : (b), name : #a, namel : strlen(#a) },
FFN_ATTRS_TABLE
#undef FFN_ATTR_DEF
};

#define FFN_ATTR_DEF(a, b) + 1
static size_t ffn_attrs_len = 0 FFN_ATTRS_TABLE;
#undef FFN_ATTR_DEF

static void __attribute__((constructor)) attrs_init()
{
  qsort(ffn_attrs, ffn_attrs_len, sizeof(*ffn_attrs), attrs16_compare_byname);
}

static const char * ffn_type_names[] = {
#define FFN_TYPE_DEF(a, t) [a] = #a,
FFN_TYPE_TABLE
};

//
// static
//

static xapi say_normal(const ff_node * restrict n, narrator * restrict N)
{
  enter;

  // scalars
  if(n->type == FFN_DIRSEP)
    fatal(ffn_dirsep_say_normal, (ff_node_dirsep*)n, N);
  else if(n->type == FFN_VARIANT)
    fatal(ffn_variant_say_normal, (ff_node_variant*)n, N);
  else if(n->type == FFN_CHAR)
    fatal(ffn_char_say_normal, (ff_node_char*)n, N);
  else if(n->type == FFN_STEM)
    fatal(ffn_stem_say_normal, (ff_node_stem*)n, N);
  else if(n->type == FFN_USRVAR)
    fatal(ffn_usrvar_say_normal, (ff_node_usrvar*)n, N);
  else if(n->type == FFN_WORD)
    fatal(ffn_word_say_normal, (ff_node_word*)n, N);

  // aggregates
  else if(n->type == FFN_VARNAME)
    fatal(ffn_varname_say_normal, (ff_node_varname*)n, N);
  else if(n->type == FFN_ALTERNATION)
    fatal(ffn_alternation_say_normal, (ff_node_alternation*)n, N);
  else if(n->type == FFN_ARTIFACT)
    fatal(ffn_artifact_say_normal, (ff_node_artifact*)n, N);
  else if(n->type == FFN_ARTIFACT_PATTERN)
    fatal(ffn_artifact_pattern_say_normal, (ff_node_artifact_pattern*)n, N);
  else if(n->type == FFN_CLASS)
    fatal(ffn_class_say_normal, (ff_node_class*)n, N);
  else if(n->type == FFN_PATTERN)
    fatal(ffn_pattern_say_normal, (ff_node_pattern*)n, N);
  else if(n->type == FFN_PATTERNS)
    fatal(ffn_patterns_say_normal, (ff_node_patterns*)n, N);
  else if(n->type == FFN_RANGE)
    fatal(ffn_range_say_normal, (ff_node_range*)n, N);
  else if(n->type == FFN_REQUIRE)
    fatal(ffn_require_say_normal, (ff_node_require*)n, N);
  else if(n->type == FFN_RULE)
    fatal(ffn_rule_say_normal, (ff_node_rule*)n, N);
  else if(n->type == FFN_STRING)
    fatal(ffn_string_say_normal, (ff_node_string*)n, N);
  else if(n->type == FFN_STRINGS)
    fatal(ffn_strings_say_normal, (ff_node_strings*)n, N);
  else if(n->type == FFN_USE)
    fatal(ffn_use_say_normal, (ff_node_use*)n, N);
  else if(n->type == FFN_VARDEF)
    fatal(ffn_vardef_say_normal, (ff_node_vardef*)n, N);

  finally : coda;
}

static xapi say_tree(const ff_node * restrict n, int level, narrator * restrict N)
{
  enter;

  // scalars
  if(n->type == FFN_DIRSEP)
    fatal(ffn_dirsep_say_tree, (ff_node_dirsep*)n, N);
  else if(n->type == FFN_VARIANT)
    fatal(ffn_variant_say_tree, (ff_node_variant*)n, N);
  else if(n->type == FFN_CHAR)
    fatal(ffn_char_say_tree, (ff_node_char*)n, N);
  else if(n->type == FFN_STEM)
    fatal(ffn_stem_say_tree, (ff_node_stem*)n, N);
  else if(n->type == FFN_USRVAR)
    fatal(ffn_usrvar_say_tree, (ff_node_usrvar*)n, N);
  else if(n->type == FFN_WORD)
    fatal(ffn_word_say_tree, (ff_node_word*)n, N);

  // aggregates
  if(n->type == FFN_VARNAME)
    fatal(ffn_varname_say_tree, (ff_node_varname*)n, level, N);
  else if(n->type == FFN_ALTERNATION)
    fatal(ffn_alternation_say_tree, (ff_node_alternation*)n, level, N);
  else if(n->type == FFN_ARTIFACT)
    fatal(ffn_artifact_say_tree, (ff_node_artifact*)n, level, N);
  else if(n->type == FFN_ARTIFACT_PATTERN)
    fatal(ffn_artifact_pattern_say_tree, (ff_node_artifact_pattern*)n, level, N);
  else if(n->type == FFN_CLASS)
    fatal(ffn_class_say_tree, (ff_node_class*)n, level, N);
  else if(n->type == FFN_PATTERN)
    fatal(ffn_pattern_say_tree, (ff_node_pattern*)n, level, N);
  else if(n->type == FFN_PATTERNS)
    fatal(ffn_patterns_say_tree, (ff_node_patterns*)n, level, N);
  else if(n->type == FFN_RANGE)
    fatal(ffn_range_say_tree, (ff_node_range*)n, level, N);
  else if(n->type == FFN_REQUIRE)
    fatal(ffn_require_say_tree, (ff_node_require*)n, level, N);
  else if(n->type == FFN_RULE)
    fatal(ffn_rule_say_tree, (ff_node_rule*)n, level, N);
  else if(n->type == FFN_STRING)
    fatal(ffn_string_say_tree, (ff_node_string*)n, level, N);
  else if(n->type == FFN_STRINGS)
    fatal(ffn_strings_say_tree, (ff_node_strings*)n, level, N);
  else if(n->type == FFN_USE)
    fatal(ffn_use_say_tree, (ff_node_use*)n, level, N);
  else if(n->type == FFN_VARDEF)
    fatal(ffn_vardef_say_tree, (ff_node_vardef*)n, level, N);

  finally : coda;
}

static xapi say_normal_list(const ff_node * restrict n, narrator * restrict N, bool chain, const char * restrict sep, bool first)
{
enter;

if(!n)
  goto XAPI_FINALIZE;

if(chain && !first && sep)
  xsays(sep);

fatal(say_normal, n, N);

if(chain)
  fatal(say_normal_list, n->next, N, true, sep, false);

finally : coda;
}

//
// internal
//

xapi ffn_say_normal_list(const ff_node * restrict n, narrator * restrict N, const char * restrict sep)
{
  xproxy(say_normal_list, n, N, true, sep, true);
}

xapi ffn_say_tree_level(const ff_node * restrict n, int level, narrator * restrict N)
{
  enter;

  if(!n)
    goto XAPI_FINALIZE;

  xsayf("%*s%p %s @ [%3d,%3d - %3d,%3d] "
    , level * 2, ""
    , n
    , ffn_type_names[n->type]
    , n->loc.f_lin + 1
    , n->loc.f_col + 1
    , n->loc.l_lin + 1
    , n->loc.l_col + 1
  );

  if(n->type & FFN_TYPE_SCALAR)
    fatal(say_tree, n, level, N);

  xsayc('\n');

  if(n->type & FFN_TYPE_AGGREGATE)
    fatal(say_tree, n, level, N);

  fatal(ffn_say_tree_level, n->next, level, N);

  finally : coda;
}

xapi ffn_semantic_error(xapi error, const ff_node * restrict refnode)
{
  enter;

  char space[512];

  const ff_node * ref = (ff_node*)refnode;

  char stor[NARRATOR_STATIC_SIZE];
  narrator * N = narrator_fixed_init(stor, space, sizeof(space));
  fatal(ffn_say_normal, ref, N);

  xapi_info_pushs("ref", space);
  xapi_info_pushf("location", "%s line %d", ref->loc.fname, ref->loc.f_lin + 1);
  fail(error);

  finally : coda;
}

//
// public
//

const char *ffn_type_name(ffn_type type)
{
  if(type <= FFN_TYPE_RANGE_BEFORE || type >= FFN_TYPE_RANGE_AFTER)
    return NULL;

  return ffn_type_names[type];
}

uint16_t ffn_attr_byname(const char * name, size_t namel)
{
  return attrs16_lookup_byname(ffn_attrs, ffn_attrs_len, name, namel);
}

xapi ffn_mknode(ff_node ** restrict n, const yyu_location * restrict loc, ffn_type type, ...)
{
  enter;

	// pull nodetype-specific params off the stack
	va_list va;
	va_start(va, type);

  if(type == FFN_CHAR)
    fatal(ffn_char_mknode, (ff_node_char**)n, va);
  else if(type == FFN_STEM)
    fatal(ffn_stem_mknode, (ff_node_stem**)n, va);
  else if(type == FFN_USRVAR)
    fatal(ffn_usrvar_mknode, (ff_node_usrvar**)n, va);
  else if(type == FFN_WORD)
    fatal(ffn_word_mknode, (ff_node_word**)n, va);
  else if(type == FFN_ALTERNATION)
    fatal(ffn_alternation_mknode, (ff_node_alternation**)n, va);
  else if(type == FFN_ARTIFACT)
    fatal(ffn_artifact_mknode, (ff_node_artifact**)n, va);
  else if(type == FFN_ARTIFACT_PATTERN)
    fatal(ffn_artifact_pattern_mknode, (ff_node_artifact_pattern**)n, va);
  else if(type == FFN_CLASS)
    fatal(ffn_class_mknode, (ff_node_class**)n, va);
  else if(type == FFN_PATTERN)
    fatal(ffn_pattern_mknode, (ff_node_pattern**)n, va);
  else if(type == FFN_PATTERNS)
    fatal(ffn_patterns_mknode, (ff_node_patterns**)n, va);
  else if(type == FFN_RANGE)
    fatal(ffn_range_mknode, (ff_node_range**)n, va);
  else if(type == FFN_REQUIRE)
    fatal(ffn_require_mknode, (ff_node_require**)n, va);
  else if(type == FFN_RULE)
    fatal(ffn_rule_mknode, (ff_node_rule**)n, va);
  else if(type == FFN_STRING)
    fatal(ffn_string_mknode, (ff_node_string**)n, va);
  else if(type == FFN_STRINGS)
    fatal(ffn_strings_mknode, (ff_node_strings**)n, va);
  else if(type == FFN_DIRSEP)
    fatal(ffn_dirsep_mknode, (ff_node_dirsep**)n, va);
  else if(type == FFN_VARIANT)
    fatal(ffn_variant_mknode, (ff_node_variant**)n, va);
  else if(type == FFN_USE)
    fatal(ffn_use_mknode, (ff_node_use**)n, va);
  else if(type == FFN_VARDEF)
    fatal(ffn_vardef_mknode, (ff_node_vardef**)n, va);
  else if(type == FFN_VARNAME)
    fatal(ffn_varname_mknode, (ff_node_varname**)n, va);

	va_end(va);

  (*n)->type = type;
	memcpy(&(*n)->loc, loc, sizeof((*n)->loc));

	finally : coda;
}

ff_node* ffn_append(ff_node * n, ff_node * T)
{
  if(n->tail)
  {
    // discard subsequent slashes beyond the first
    if(n->tail->type == FFN_DIRSEP && T->type == FFN_DIRSEP)
    {
      ffn_free(T);
      return n;
    }

    n->tail->next = T;
    T->prev = n->tail;
  }
  else
  {
    n->next = T;
    T->prev = n;
  }

  n->tail = T;
  return n;
}

xapi ffn_attrs_say(uint16_t attrs, struct narrator * restrict N)
{
  enter;

  bool written = false;

  int x;
  for(x = 0; x < ffn_attrs_len; x++)
  {
    if(attrs & ffn_attrs[x].value)
    {
      if(written)
        xsayc(',');
      xsays(ffn_attrs[x].name);
      written = true;
    }
  }

  finally : coda;
}

void ffn_free(ff_node * n)
{
	if(n)
	{
    if(n->type == FFN_CHAR)
      ffn_char_destroy((ff_node_char*)n);
    else if(n->type == FFN_STEM)
      ffn_stem_destroy((ff_node_stem*)n);
    else if(n->type == FFN_USRVAR)
      ffn_usrvar_destroy((ff_node_usrvar*)n);
    else if(n->type == FFN_WORD)
      ffn_word_destroy((ff_node_word*)n);
    else if(n->type == FFN_ALTERNATION)
      ffn_alternation_destroy((ff_node_alternation*)n);
    else if(n->type == FFN_ARTIFACT)
      ffn_artifact_destroy((ff_node_artifact*)n);
    else if(n->type == FFN_ARTIFACT_PATTERN)
      ffn_artifact_pattern_destroy((ff_node_artifact_pattern*)n);
    else if(n->type == FFN_CLASS)
      ffn_class_destroy((ff_node_class*)n);
    else if(n->type == FFN_PATTERN)
      ffn_pattern_destroy((ff_node_pattern*)n);
    else if(n->type == FFN_PATTERNS)
      ffn_patterns_destroy((ff_node_patterns*)n);
    else if(n->type == FFN_RANGE)
      ffn_range_destroy((ff_node_range*)n);
    else if(n->type == FFN_REQUIRE)
      ffn_require_destroy((ff_node_require*)n);
    else if(n->type == FFN_RULE)
      ffn_rule_destroy((ff_node_rule*)n);
    else if(n->type == FFN_STRING)
      ffn_string_destroy((ff_node_string*)n);
    else if(n->type == FFN_STRINGS)
      ffn_strings_destroy((ff_node_strings*)n);
    else if(n->type == FFN_USE)
      ffn_use_destroy((ff_node_use*)n);
    else if(n->type == FFN_VARDEF)
      ffn_vardef_destroy((ff_node_vardef*)n);
    else if(n->type == FFN_VARNAME)
      ffn_varname_destroy((ff_node_varname*)n);

    ffn_free(n->next);
	}

	free(n);
}

void ffn_xfree(ff_node ** const n)
{
	ffn_free(*n);
	*n = 0;
}

xapi ffn_say_normal(const ff_node * restrict n, narrator * restrict N)
{
  xproxy(say_normal, n, N);
}

xapi ffn_say_tree(const ff_node * restrict n, narrator * restrict N)
{
  xproxy(ffn_say_tree_level, n, 0, N);
}

xapi ffn_segment_say_normal(const ff_node * first, const ff_node * end, narrator * restrict N)
{
  enter;

  typeof(first) n = first;
  while(n != end)
  {
    fatal(ffn_say_normal, n, N);
    n = (typeof(n))n->next;
  }

  finally : coda;
}
