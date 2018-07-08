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
#include "narrator/fixed.h"

#include "ff_node_pattern.internal.h"
#include "ff_node.internal.h"

//
// static
//

static void __attribute__((nonnull)) ffn_bydir_ltr_setup(ffn_bydir_context * restrict ctx, const ff_node_pattern_part * restrict start)
{
  *ctx = (typeof(*ctx)){ state : start };
}

static void __attribute__((nonnull)) ffn_bydir_rtl_setup(ffn_bydir_context * restrict ctx, const ff_node_pattern_part * restrict start)
{
  typeof(start) last = start;
  while(last->next)
    last = (typeof(last))last->next;

  *ctx = (typeof(*ctx)){ state : last };
}

//
// public
//

xapi ffn_pattern_say_tree(const ff_node_pattern * restrict n, int level, narrator * restrict N)
{
  enter;

  xsayf("%*slist\n", level * 2, "");
  fatal(ffn_say_tree_level, n->chain, level + 1, N);

  finally : coda;
}

xapi ffn_pattern_say_normal(const ff_node_pattern * restrict n, narrator * restrict N)
{
  enter;

  fatal(ffn_say_normal_list, n->chain, N, 0);

  finally : coda;
}

xapi ffn_pattern_mknode(ff_node_pattern ** restrict n, va_list va)
{
  enter;

  fatal(xmalloc, n, sizeof(**n));

  (*n)->chain = va_arg(va, typeof((*n)->chain));

  finally : coda;
}

void ffn_pattern_destroy(ff_node_pattern * restrict n)
{
  ffn_free(n->chain);
}

//
// bydir api
//

void ffn_pattern_bydir_ltr_init(const ff_node_pattern_part * restrict start, ffn_bydir_context * restrict ctx)
{
  ffn_bydir_ltr_setup(ctx, start);
  ffn_pattern_bydir_ltr(ctx);
}

void ffn_pattern_bydir_ltr(ffn_bydir_context * restrict ctx)
{
  if((ctx->first = ctx->state) == 0)
  {
    ctx->stop = 0;
    return;
  }

  const ff_node_pattern_part * last = ctx->first;
  while(last->next && last->next->type != FFN_DIRSEP)
    last = (ff_node_pattern_part*)last->next;

  ctx->stop = (ff_node_pattern_part*)last->next;

  if(last->next)
    ctx->state = (ff_node_pattern_part*)last->next->next;
  else
    ctx->state = 0;
}

void ffn_pattern_bydir_rtl_init(const ff_node_pattern_part * restrict start, ffn_bydir_context * restrict ctx)
{
  ffn_bydir_rtl_setup(ctx, start);
  ffn_pattern_bydir_rtl(ctx);
}

void ffn_pattern_bydir_rtl(ffn_bydir_context * restrict ctx)
{
  const ff_node_pattern_part * last = ctx->state;
  if(last == 0)
  {
    ctx->first = 0;
    ctx->stop = 0;
    return;
  }

  ctx->stop = (ff_node_pattern_part*)last->next;
  while(last->prev && last->prev->type != FFN_DIRSEP)
    last = (ff_node_pattern_part*)last->prev;

  if(last->prev)
    ctx->state = (ff_node_pattern_part*)last->prev->prev;
  else
    ctx->state = 0;

  ctx->first = last;
}

xapi ffn_pattern_bydir_strings_rtl_init(const ff_node_pattern_part * restrict start, ffn_bydir_context * restrict ctx, char * restrict tmp, size_t tmpsz, const char ** restrict str)
{
  enter;

  ffn_bydir_rtl_setup(ctx, (ff_node_pattern_part*)start);
  fatal(ffn_pattern_bydir_strings_rtl, ctx, tmp, tmpsz, str);

  finally : coda;
}

xapi ffn_pattern_bydir_strings_rtl(ffn_bydir_context * restrict ctx, char * restrict tmp, size_t tmpsz, const char ** restrict str)
{
  enter;

  ffn_pattern_bydir_rtl(ctx);

  if(!ctx->first)
    *str = 0;

  else if(ctx->first == ctx->stop && ctx->first->type == FFN_WORD)
    *str = ((ff_node_word*)ctx->first)->text;

  else
  {
    char stor[NARRATOR_STATIC_SIZE];
    fatal(ffn_segment_say_normal, ctx->first, ctx->stop, narrator_fixed_init(stor, tmp, tmpsz));
    *str = tmp;
  }

  finally : coda;
}
