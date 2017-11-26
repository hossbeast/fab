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
#include "valyria/list.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xstring.h"

#include "ff_node_word.internal.h"
#include "artifact.h"
#include "ff_node.internal.h"
#include "ff_node_pattern.internal.h"
#include "node.h"
#include "path.h"
#include "pattern/generate.internal.h"
#include "pattern/match.internal.h"

xapi ffn_word_say_tree(const ff_node_word * restrict n, narrator * restrict N)
{
  enter;

  xsayw(n->text, n->len);

  finally : coda;
}

xapi ffn_word_say_normal(const ff_node_word * restrict n, narrator * restrict N)
{
  enter;

  xsayw(n->text, n->len);

  finally : coda;
}

xapi ffn_word_mknode(ff_node_word ** restrict n, va_list va)
{
  enter;

  fatal(xmalloc, n, sizeof(**n));

  size_t len = va_arg(va, size_t);
  char * s = va_arg(va, char*);
  uint8_t ref = (uint8_t)va_arg(va, int);

  if(ref)
  {
    (*n)->len = 1;
    fatal(xmalloc, &(*n)->text, 2);
    (*n)->text[0] = ref;
  }
  else
  {
    (*n)->len = len;
    fatal(ixstrndup, &(*n)->text, s, (*n)->len);
  }

  finally : coda;
}

void ffn_word_destroy(ff_node_word * restrict n)
{
  wfree(n->text);
}

xapi ffn_word_match(pattern_match_context * restrict context, ffn_bydir_walk * restrict walk)
{
  enter;

  const char * name = context->node->name->name;
  int namel = context->node->name->namel;
  const ff_node_word * restrict word = &walk->ffn->word;

  if(     (namel - context->name_offset) >= word->len
       && strncmp(name + context->name_offset, word->text, word->len) == 0)
  {
    context->name_offset += word->len;
  }
  else
  {
    context->node = 0;
  }

  finally : coda;
}

xapi ffn_word_generate(
    pattern_generate_context * restrict context
  , ffn_bydir_walk * restrict walk
  , const artifact * restrict context_af
  , const char * restrict stem
  , uint16_t stem_len
  , list * restrict results
  , bool generating_artifact
)
{
  enter;

  fatal(narrator_xsayw, context->segment_narrator, walk->ffn->word.text, walk->ffn->word.len);
  walk->ffn = (ff_node_pattern_part*)walk->ffn->next;
  fatal(pattern_segment_generate, context, walk, context_af, stem, stem_len, results, generating_artifact);

  finally : coda;
}
