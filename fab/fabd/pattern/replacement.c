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

#include "narrator.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xstring.h"

#include "replacement.h"
#include "generate.internal.h"
#include "match.internal.h"
#include "pattern.internal.h"
#include "variant.h"

#include "zbuffer.h"

//
// static
//

static xapi say(const pattern_segment * restrict fn, narrator * restrict N)
{
  enter;

  const pattern_replacement * n = &fn->replacement;
  uint8_t x;
  uint8_t len;

  xsayc('$');

  if(n->replacement_type == PATTERN_REPLACEMENT_TYPE_NAME)
    xsayw(n->name, n->name_len);
  else if(n->replacement_type == PATTERN_REPLACEMENT_TYPE_NUM)
    xsayf("%hhu", n->num);
  else if(n->replacement_type == PATTERN_REPLACEMENT_TYPE_VARIANT)
  {
    if(n->tags.len)
    {
      xsayc('{');
      xsayc('?');
      for(x = 0; x < n->tags.len; x++)
      {
        len = n->tags.text[x];
        xsayw(&n->tags.text[x + 1], len + 1);
        x += len + 1;
      }
      xsayc('}');
    }
    else
    {
      xsayc('?');
    }
  }

  finally : coda;
}

static void destroy(pattern_segment * restrict fn)
{
  pattern_replacement * n = &fn->replacement;

  if(n->replacement_type == PATTERN_REPLACEMENT_TYPE_NAME)
    wfree(n->name);
}

static xapi generate(const pattern_segment * restrict seg, pattern_generate_context * restrict ctx)
{
  enter;

  char space[256];
  uint16_t len = 0;
  uint16_t tag_len;
  int x;
  int y;

  const pattern_replacement * n = &seg->replacement;
  const variant *v;

  if(n->replacement_type == PATTERN_REPLACEMENT_TYPE_VARIANT)
  {
    if((v = ctx->match->variant))
    {
      if(n->tags.len == 0)
      {
        ctx->section_traversal.variant_replacement = v;
      }
      else
      {
        // apply variant tags except those with a "-" tag in the replacement
        for(x = 0; x < v->tags_len; x++)
        {
          for(y = 0; y < n->tags.len; y++)
          {
            if(n->tags.text[y + 1] != '-')
              continue;
            tag_len = n->tags.text[y];
            if(memncmp(v->tags[x].text, v->tags[x].len, &n->tags.text[y + 2], tag_len) == 0) {
              break;
            }
            y += tag_len + 1;
          }

          if(y == n->tags.len)
          {
            if(len)
              len += znloadc(space + len, sizeof(space) - len, '.');
            len += znloadw(space + len, sizeof(space) - len, v->tags[x].text, v->tags[x].len);
          }
        }

        // apply "+" tags from the replacement
        for(x = 0; x < n->tags.len; x++)
        {
          if(n->tags.text[x + 1] != '+')
            continue;
          tag_len = n->tags.text[x];
          if(len)
            len += znloadc(space + len, sizeof(space) - len, '.');
          len += znloadw(space + len, sizeof(space) - len, &n->tags.text[x + 2], tag_len);
          x += tag_len + 1;
        }

        fatal(variant_get, space, len, (variant **)&v);
      }

      fatal(narrator_xsays, ctx->section_narrator, v->norm);
    }
  }
  else if(n->replacement_type == PATTERN_REPLACEMENT_TYPE_NAME)
  {
    /* not implemented */
    RUNTIME_ABORT();
  }
  else if(n->replacement_type == PATTERN_REPLACEMENT_TYPE_NUM)
  {
    if(ctx->match && ctx->match->group_max >= n->num)
    {
        fatal(narrator_xsayw, ctx->section_narrator
          , ctx->match->groups[n->num].start
          , ctx->match->groups[n->num].len
        );
    }
  }
  else
  {
    RUNTIME_ABORT();
  }

  fatal(pattern_segment_generate, ctx);

  finally : coda;
}

static int cmp(const pattern_segment * A, const pattern_segment *B)
{
  int d;

  if((d = INTCMP(A->replacement.num, B->replacement.num)))
    return d;

  return memncmp(A->replacement.name, A->replacement.name_len, B->replacement.name, B->replacement.name_len);
}

static pattern_segment_vtable vtable = {
    type : PATTERN_WORD
  , say : say
  , destroy : destroy
  , generate : generate
  , cmp : cmp
};

xapi pattern_replacement_mk(
    pattern_segment ** restrict rv
  , const yyu_location * restrict loc
  , pattern_replacement_type replacement_type
  , uint16_t num
  , const char * restrict name
  , uint16_t name_len
  , const char * tag_text
  , uint8_t tag_text_len
)
{
  enter;

  pattern_segment * n = 0;

  fatal(xmalloc, &n, sizeof(*n));
  fatal(pattern_segment_init, n, &vtable, loc);

  n->replacement.replacement_type = replacement_type;
  if(n->replacement.replacement_type == PATTERN_REPLACEMENT_TYPE_NUM)
  {
    n->replacement.num = num;
  }
  else if(replacement_type == PATTERN_REPLACEMENT_TYPE_NAME)
  {
    fatal(ixstrndup, &n->replacement.name, name, name_len);
    n->replacement.name_len = name_len;
  }
  else if(replacement_type == PATTERN_REPLACEMENT_TYPE_VARIANT)
  {
    if(tag_text)
    {
      memcpy(n->replacement.tags.text, tag_text, tag_text_len);
      n->replacement.tags.len = tag_text_len;
    }
  }
  else
  {
    RUNTIME_ABORT();
  }

  chain_init(n, chn);

  *rv = n;

  finally : coda;
}
