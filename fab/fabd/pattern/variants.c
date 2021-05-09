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

#include "narrator.h"
#include "valyria/set.h"
#include "xlinux/xstdlib.h"

#include "variants.h"
#include "fsent.h"
#include "generate.internal.h"
#include "search.internal.h"
#include "pattern.internal.h"
#include "variant.h"

//
// static
//

static xapi say(const pattern_segment * restrict n, narrator * restrict N)
{
  enter;

  xsayc('?');

  finally : coda;
}

static void destroy(pattern_segment * restrict n)
{

}

static xapi search(const pattern_segment * restrict segment, pattern_search_context * restrict ctx)
{
  enter;

  int x;
  const char * restrict name;
  uint16_t namel;
  struct search_segments_traversal *traversal;
  const char *var;
  uint16_t var_len;

/*

Different variants segments should never bind to different variant instances at the same time, e.g.

a?b?c - with variants { x, y } - should produce these combinations

axbxc
aybyc

AND NOT

axbyc
aybxc

*/

  traversal = ctx->traversal;
  //name = ctx->node->name.name;
  //namel = ctx->node->name.namel;
  name = ctx->label;
  namel = ctx->label_len;

  variant *v;
  for(x = 0; x < ctx->variants->table_size; x++)
  {
    if(!(v = set_table_get(ctx->variants, x))) {
      continue;
    }

    if(ctx->variant_index != -1 && x != ctx->variant_index) {
      continue;
    }

    var = v->norm;
    var_len = v->norm_len;

    if((namel - ctx->traversal->offset) < var_len) {
      continue;
    }
    if(strncmp(name + ctx->traversal->offset, var, var_len)) {
      continue;
    }

    ctx->variant_index = x;
    ctx->traversal->offset += var_len;

    fatal(ctx->segments_process, ctx);
    if(ctx->matched)
      break;

    ctx->traversal = traversal;
  }

  finally : coda;
}

static xapi generate(const pattern_segment * restrict seg, pattern_generate_context * restrict ctx)
{
  enter;

  int x;
  variant *v;

  fsent * saved_context_node;
  char saved_section_text[256];
  off_t saved_section_narrator_pos;

  // section traversal
  struct section_traversal saved_section_traversal;
  struct segment_traversal *saved_segment_traversal;

  saved_context_node = ctx->node;
  saved_section_traversal = ctx->section_traversal;
  saved_segment_traversal = ctx->segment_traversal;
  fatal(narrator_xseek, ctx->section_narrator, 0, NARRATOR_SEEK_CUR, &saved_section_narrator_pos);
  fatal(narrator_xseek, ctx->section_narrator, 0, NARRATOR_SEEK_SET, 0);
  fatal(narrator_xread, ctx->section_narrator, saved_section_text, saved_section_narrator_pos, 0);

  for(x = 0; x < ctx->variants->table_size; x++)
  {
    if(!(v = set_table_get(ctx->variants, x))) {
      continue;
    }

    if(ctx->section_traversal.variant_index != -1 && x != ctx->section_traversal.variant_index) {
      continue;
    }

    ctx->section_traversal.variant_index = x;

    ctx->segment_traversal = saved_segment_traversal;
    fatal(narrator_xsayw, ctx->section_narrator, v->norm, v->norm_len);
    fatal(pattern_segment_generate, ctx);

    ctx->node = saved_context_node;
    ctx->section_traversal = saved_section_traversal;
    fatal(narrator_xseek, ctx->section_narrator, 0, NARRATOR_SEEK_SET, 0);
    fatal(narrator_xsayw, ctx->section_narrator, saved_section_text, saved_section_narrator_pos);
  }

  if(ctx->variants->size == 0)
  {
    fatal(pattern_segment_generate, ctx);
  }

  finally : coda;
}

static int cmp(const pattern_segment * A, const pattern_segment *B)
{
  return 0;
}

static pattern_segment_vtable vtable = {
    type : PATTERN_VARIANTS
  , say : say
  , search : search
  , generate : generate
  , destroy : destroy
  , cmp : cmp
};

//
// public
//

xapi pattern_variants_mk(pattern_segment ** restrict rv, const yyu_location * restrict loc)
{
  enter;

  pattern_segment * n = 0;

  fatal(xmalloc, &n, sizeof(*n));
  fatal(pattern_segment_init, n, &vtable, loc);

  chain_init(n, chn);

  *rv = n;

  finally : coda;
}
