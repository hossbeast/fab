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

#include "xlinux/xstdlib.h"
#include "narrator.h"

#include "byte.h"
#include "pattern.internal.h"
#include "fsent.h"
#include "generate.internal.h"
#include "search.internal.h"
#include "match.internal.h"
#include "render.internal.h"

//
// static
//

static void say(const pattern_segment * restrict n, narrator * restrict N)
{
  byte_say(n->byte.code, N);
}

static void render(const pattern_segment * restrict fn, pattern_render_context * restrict ctx)
{
  const pattern_byte * n = &fn->byte;

  byte_say(n->code, ctx->narrator);
  pattern_segment_render(ctx);
}

static void match(const pattern_segment * restrict segment, pattern_match_context * restrict ctx)
{
  const pattern_byte * byte = &segment->byte;
  const char * restrict name;
  uint16_t namel;
  uint16_t name_offset;

  name = ctx->label;
  namel = ctx->label_len;
  name_offset = ctx->traversal->offset;

  if((namel > name_offset) && (name[name_offset] == byte->code))
  {
    ctx->traversal->offset += 1;
  }
}

static void search(const pattern_segment * restrict segment, pattern_search_context * restrict ctx)
{
  const pattern_byte * byte = &segment->byte;
  const char * restrict name; // = ctx->node->name.name;
  uint16_t namel; // = ctx->node->name.namel;
  uint16_t name_offset; // = ctx->traversal->offset;

  name = ctx->node->name.name;
  namel = ctx->node->name.namel;
  name_offset = ctx->traversal->offset;

  if((namel > name_offset) && (name[name_offset] == byte->code))
  {
    ctx->traversal->offset += 1;
  }
}

static void generate(const pattern_segment * restrict seg, pattern_generate_context * restrict context)
{
}

static void destroy(pattern_segment * restrict fn)
{
}

static int cmp(const pattern_segment * A, const pattern_segment *B)
{
  return INTCMP(A->byte.code, B->byte.code);
}

static pattern_segment_vtable vtable = {
    type : PATTERN_CHARACTER
  , say : say
  , search : search
  , match : match
  , generate : generate
  , destroy : destroy
  , render : render
  , cmp : cmp
};

//
// internal
//

void byte_say(uint8_t code, narrator * restrict N)
{
  if(code < 0x20 || code > 0x7e)
    xsayf("\\x%02hhx", code);
  else
    xsayc(code);
}

void pattern_byte_mk(pattern_segment ** restrict fn, const yyu_location * restrict loc, uint8_t code)
{
  pattern_segment * n = 0;

  xmalloc(&n, sizeof(*n));
  pattern_segment_init(n, &vtable, loc);

  n->byte.code = code;
  chain_init(n, chn);

  *fn = n;
}
