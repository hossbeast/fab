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

/* flex and bison do not agree on these names */
#define YYSTYPE SELECTOR_YYSTYPE
#define YYLTYPE yyu_location

#include "selector_parser.internal.h"
#include "selector.tab.h"
#include "selector.tokens.h"
#include "selector.lex.h"
#include "selector.states.h"

#include "pattern_parser.h"

static YYU_VTABLE(vtable, selector, selector);

//
// public
//

void selector_parser_create(selector_parser ** rv)
{
  selector_parser * p = 0;

  xmalloc(&p, sizeof(*p));

  yyu_parser_init(&p->yyu, &vtable);
  yyu_parser_init_tokens(&p->yyu, selector_token_table, selector_TOKEN_TABLE_SIZE);
  yyu_parser_init_states(
      &p->yyu
    , selector_numstates
    , selector_statenumbers
    , selector_statenames
  );

  pattern_parser_create(&p->pattern_parser);

  llist_init_node(&p->selector_stack);

  *rv = p;
}

void selector_parser_xfree(selector_parser* const p)
{
  selector * sel;
  llist *T;

  if(p)
  {
    pattern_parser_xfree(p->pattern_parser);

    llist_foreach_safe(&p->selector_stack, sel, lln, T) {
      llist_delete(sel, lln);
      selector_free(sel);
    }

    yyu_parser_xdestroy(&p->yyu);
  }

  wfree(p);
}

void selector_parser_ixfree(selector_parser ** const p)
{
  selector_parser_xfree(*p);
  *p = 0;
}

int selector_parser_parse_partial(
    selector_parser * restrict parser
  , char * const restrict buf
  , size_t size
  , const char * restrict fname
  , yyu_location * init_loc
  , yyu_location * used_loc
  , selector ** restrict rv
)
{
  selector * sel;
  llist *T;
  int r;

  llist_init_node(&parser->selector_stack);
  r = yyu_parse(&parser->yyu, buf, size, fname, YYU_PARTIAL | YYU_INPLACE, init_loc, used_loc);
  if(rv) {
    *rv = llist_shift(&parser->selector_stack, selector, lln);
  }

  llist_foreach_safe(&parser->selector_stack, sel, lln, T) {
    llist_delete(sel, lln);
    selector_free(sel);
  }

  return r;
}
