/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "xlinux/xstdlib.h"
#include "narrator.h"
#include "common/attrs.h"
#include "bacon/builder.h"

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

static void byte_describe(const pattern_segment * restrict fn, bacon_writer * restrict builder_writer)
{
  const pattern_byte *byte = &fn->byte;

  { MAPPING
    STRING("byte")
    LOADF("0x%02hhx", byte->code)
  }
}

static void byte_say(const pattern_segment * restrict n, const pattern_segment * restrict next, narrator * restrict N)
{
  byte_say_code(n->byte.code, N);
}

static void byte_render(const pattern_segment * restrict fn, pattern_render_context * restrict ctx)
{
  const pattern_byte * n = &fn->byte;

  byte_say_code(n->code, ctx->narrator);
  pattern_segment_render(ctx);
}

static void byte_search(const pattern_segment * restrict segment, pattern_search_context * restrict ctx)
{
  const pattern_byte * byte = &segment->byte;
  const char * restrict name;
  uint16_t namel;
  uint16_t name_offset;

  name = ctx->node->name.name;
  namel = ctx->node->name.namel;
  name_offset = ctx->traversal->offset;

  if((namel > name_offset) && (name[name_offset] == byte->code)) {
    ctx->traversal->offset += 1;
  } else {
    ctx->traversal->unsatisfied = true;
  }
}

static void byte_generate(const pattern_segment * restrict seg, pattern_generate_context * restrict context)
{
}

static void byte_destroy(pattern_segment * restrict fn)
{
}

static int byte_cmp(const pattern_segment * A, const pattern_segment *B)
{
  return INTCMP(A->byte.code, B->byte.code);
}

static pattern_segment_vtable vtable = PATTERN_SEGMENT_VTABLE(byte, PATTERN_CHARACTER, 0);

//
// internal
//

void byte_say_code(uint8_t code, narrator * restrict N)
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

  *fn = n;
}
