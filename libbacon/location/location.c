/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "valyria/hashtable.h"
#include "valyria/pstring.h"
#include "valyria/hashtable.h"
#include "valyria/llist.h"
#include "xlinux/xstdlib.h"
#include "yyutil/grammar.h"

#include "location.h"
#include "parser.h"

#include "common/attrs.h"

bacon_location * bacon_location_alloc(bacon_parser * restrict parser)
{
  bacon_location * loc = 0;

  if(!(loc = llist_shift(&parser->location_freelist, typeof(*loc), lln))) {
    xmalloc(&loc, sizeof(*loc));
    llist_init(&loc->lln);
  }

  return loc;
}

void bacon_location_clone(
    bacon_location ** const restrict dst
  , const yyu_location * const restrict src
  , bacon_parser * restrict parser
)
{
  bacon_location *loc;

  if(*dst) {
    loc = *dst;
  } else {
    loc = bacon_location_alloc(parser);
  }

  loc->f_lin = src->f_lin;
  loc->f_col = src->f_col;
  loc->l_lin = src->l_lin;
  loc->l_col = src->l_col;
  loc->s = src->s;
  loc->e = src->e;
  loc->l = src->l;

  *dst = loc;
}

void bacon_location_free(bacon_parser * restrict parser, struct bacon_location * restrict loc)
{
  if(!loc) {
    return;
  }

  llist_push(&parser->location_freelist, &loc->lln);
}
