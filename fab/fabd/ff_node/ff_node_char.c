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

#include "xapi.h"
#include "types.h"

#include "xlinux/xstdlib.h"
#include "narrator.h"
#include "valyria/list.h"

#include "ff_node_char.internal.h"
#include "artifact.h"
#include "ff_node.internal.h"
#include "ff_node_pattern.internal.h"
#include "ff_parser.h"
#include "node.h"
#include "path.h"
#include "generate.internal.h"
#include "match.internal.h"

bool ffn_char_nonprintable(const ff_node_char * restrict n)
{
  return n->code < 0x20 || n->code > 0x7e;
}

xapi ffn_char_say_tree(const ff_node_char * restrict n, narrator * restrict N)
{
  enter;

  fatal(ffn_char_say_normal, n, N);

  finally : coda;
}

xapi ffn_char_say_normal(const ff_node_char * restrict n, narrator * restrict N)
{
  enter;

  char c = ff_cref_char(n->code);
  if(c)
    xsayf("\\%c", c);
  else if(ffn_char_nonprintable(n))
    xsayf("\\x%02hhx", n->code);
  else
    xsayc(n->code);

  finally : coda;
}

xapi ffn_char_mknode(ff_node_char ** restrict n, va_list va)
{
  enter;

  fatal(xmalloc, n, sizeof(**n));

  (*n)->code = (typeof((*n)->code))va_arg(va, int);

  finally : coda;
}

void ffn_char_destroy(ff_node_char * restrict n)
{

}

xapi ffn_char_match(pattern_match_context * restrict context, ffn_bydir_walk * restrict walk)
{
  enter;

  const char * name = context->node->name->name;
  int namel = context->node->name->namel;
  const ff_node_char * character = &walk->ffn->character;

  if(namel > context->name_offset && name[context->name_offset] == character->code)
    context->name_offset += 1;
  else
    context->node = 0;

  finally : coda;
}

xapi ffn_char_generate(
    pattern_generate_context * restrict context
  , ffn_bydir_walk * restrict walk
  , const artifact * restrict context_af
  , const char * restrict stem
  , uint16_t stem_len
  , list * restrict results
  , uint8_t opts
)
{
  enter;

  fatal(ffn_char_say_normal, &walk->ffn->character, context->segment_narrator);
  walk->ffn = (ff_node_pattern_part*)walk->ffn->next;
  fatal(pattern_segment_generate, context, walk, context_af, stem, stem_len, results, opts);

  finally : coda;
}
