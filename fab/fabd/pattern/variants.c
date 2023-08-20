/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

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

static void say(const pattern_segment * restrict n, narrator * restrict N)
{
  xsayc('?');
}

static void destroy(pattern_segment * restrict n)
{

}

static void search(const pattern_segment * restrict segment, pattern_search_context * restrict ctx)
{
  int x;
  const char * restrict name;
  uint16_t namel;
  struct search_segments_traversal *traversal;
  const char *var;
  uint16_t var_len;
  int16_t locked_variant_index;
  uint16_t start;

/*

Different variants segments should never bind to different variant instances at the same time, e.g.

a?b?c - with variants { x, y } - produces these combinations

 axbxc
 aybyc

AND NOT these

 axbyc
 aybxc

*/

  traversal = ctx->traversal;
  name = ctx->node->name.name;
  namel = ctx->node->name.namel;
  start = ctx->traversal->offset;

  /* if set, another variants segment is active and has selected this index */
  locked_variant_index = ctx->variant_index;

  variant *v;
  for(x = 0; x < ctx->variants->table_size; x++)
  {
    if(!(v = set_table_get(ctx->variants, x))) {
      continue;
    }
    if(locked_variant_index != -1 && x != locked_variant_index) {
      continue;
    }

    var = v->norm;
    var_len = v->norm_len;

    if((namel - start) < var_len) {
      continue;
    }
    if(strncmp(name + start, var, var_len)) {
      continue;
    }

    ctx->variant_index = x;
    ctx->traversal->offset = start + var_len;

    pattern_segments_search(ctx);
    if(ctx->matched) {
      break;
    }

    ctx->traversal = traversal;
  }
  /* N.B. ctx->variant_index is restored in search_visit and not here so that the index can be
   * recorded in the match */
}

static void generate(const pattern_segment * restrict seg, pattern_generate_context * restrict ctx)
{
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
  narrator_seek(ctx->section_narrator, 0, NARRATOR_SEEK_CUR, &saved_section_narrator_pos);
  narrator_seek(ctx->section_narrator, 0, NARRATOR_SEEK_SET, 0);
  narrator_read(ctx->section_narrator, saved_section_text, saved_section_narrator_pos, 0);

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
    narrator_sayw(ctx->section_narrator, v->norm, v->norm_len);
    pattern_segment_generate(ctx);

    ctx->node = saved_context_node;
    ctx->section_traversal = saved_section_traversal;
    narrator_seek(ctx->section_narrator, 0, NARRATOR_SEEK_SET, 0);
    narrator_sayw(ctx->section_narrator, saved_section_text, saved_section_narrator_pos);
  }

  if(ctx->variants->size == 0)
  {
    pattern_segment_generate(ctx);
  }
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

void pattern_variants_mk(pattern_segment ** restrict rv, const yyu_location * restrict loc)
{
  pattern_segment * n = 0;

  xmalloc(&n, sizeof(*n));
  pattern_segment_init(n, &vtable, loc);

  chain_init(n, chn);

  *rv = n;
}
