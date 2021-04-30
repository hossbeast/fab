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

#include "types.h"
#include "xapi.h"

#include "narrator.h"
#include "value/writer.h"
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

static xapi say(const pattern_segment * restrict fn, narrator * restrict N)
{
  enter;

  const pattern_word * n = &fn->word;
  xsayw(n->text, n->len);

  finally : coda;
}

static xapi render(const pattern_segment * restrict fn, pattern_render_context * restrict ctx)
{
  enter;

  const pattern_word * n = &fn->word;

  fatal(narrator_xsayw, ctx->narrator, n->text, n->len);
  fatal(pattern_segment_render, ctx);

  finally : coda;
}

static void destroy(pattern_segment * restrict fn)
{
  pattern_word * n = &fn->word;
  wfree(n->text);
}

static xapi search(const pattern_segment * restrict segment, pattern_search_context * restrict ctx)
{
  enter;

  const pattern_word * word;
  const char * restrict name;
  uint16_t namel;

  word = &segment->word;
  name = ctx->node->name.name;
  namel = ctx->node->name.namel;

printf("%s:%d %.*s <=> %.*s\n", __FUNCTION__, __LINE__, namel - ctx->traversal->offset, name + ctx->traversal->offset, word->len, word->text);

  if(     (namel - ctx->traversal->offset) >= word->len
       && strncmp(name + ctx->traversal->offset, word->text, word->len) == 0)
  {
    ctx->traversal->offset += word->len;
  }

  finally : coda;
}

static xapi generate(const pattern_segment * restrict segment, pattern_generate_context * restrict ctx)
{
  enter;

  const pattern_word * word = &segment->word;

  fatal(narrator_xsayw, ctx->section_narrator, word->text, word->len);
  fatal(pattern_segment_generate, ctx);

  finally : coda;
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
  , generate : generate
  , cmp : cmp
};

xapi pattern_word_mk(pattern_segment ** restrict rv, const yyu_location * restrict loc, const char * restrict s, uint16_t len)
{
  enter;

  pattern_segment * n = 0;

  fatal(xmalloc, &n, sizeof(*n));
  fatal(pattern_segment_init, n, &vtable, loc);

  n->word.len = len;
  fatal(ixstrndup, &n->word.text, s, len);

  chain_init(n, chn);

  *rv = n;

  finally : coda;
}
