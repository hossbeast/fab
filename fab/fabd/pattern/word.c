/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <string.h>

#include "types.h"

#include "narrator.h"
#include "bacon/writer.h"
#include "valyria/list.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xstring.h"

#include "pattern.internal.h"
#include "word.h"
#include "generate.internal.h"
#include "render.internal.h"
#include "search.internal.h"
#include "match.internal.h"
#include "fsent.h"

#include "common/attrs.h"

//
// static
//

static void say(const pattern_segment * restrict fn, narrator * restrict N)
{
  const pattern_word * n = &fn->word;
  xsayw(n->text, n->len);
}

static void render(const pattern_segment * restrict fn, pattern_render_context * restrict ctx)
{
  const pattern_word * n = &fn->word;

  narrator_sayw(ctx->narrator, n->text, n->len);
  pattern_segment_render(ctx);
}

static void destroy(pattern_segment * restrict fn)
{
  pattern_word * n = &fn->word;
  free(n->text);
}

static void match(const pattern_segment * restrict segment, pattern_match_context * restrict ctx)
{
  const pattern_word * word;
  const char * restrict name;
  uint16_t namel;

  word = &segment->word;
  name = ctx->label;
  namel = ctx->label_len;

  if(     (namel - ctx->traversal->offset) >= word->len
       && strncmp(name + ctx->traversal->offset, word->text, word->len) == 0)
  {
    ctx->traversal->offset += word->len;
  }
}

static void search(const pattern_segment * restrict segment, pattern_search_context * restrict ctx)
{
  const pattern_word * word;
  const char * restrict name;
  uint16_t namel;

  word = &segment->word;
  name = ctx->node->name.name;
  namel = ctx->node->name.namel;

  if(     (namel - ctx->traversal->offset) >= word->len
       && strncmp(name + ctx->traversal->offset, word->text, word->len) == 0)
  {
    ctx->traversal->offset += word->len;
  }
}

static void generate(const pattern_segment * restrict segment, pattern_generate_context * restrict ctx)
{
  const pattern_word * word = &segment->word;

  narrator_sayw(ctx->section_narrator, word->text, word->len);
  pattern_segment_generate(ctx);
}

static int cmp(const pattern_segment * A, const pattern_segment *B)
{
  return memncmp(A->word.text, A->word.len, B->word.text, B->word.len);
}

static pattern_segment_vtable vtable = {
    type : PATTERN_WORD
  , say : say
  , destroy : destroy
  , render : render
  , search : search
  , match : match
  , generate : generate
  , cmp : cmp
};

void pattern_word_mk(pattern_segment ** restrict rv, const yyu_location * restrict loc, const char * restrict s, uint16_t len)
{
  pattern_segment * n = 0;

  xmalloc(&n, sizeof(*n));
  pattern_segment_init(n, &vtable, loc);

  n->word.len = len;
  ixstrndup(&n->word.text, s, len);

  chain_init(n, chn);

  *rv = n;
}
