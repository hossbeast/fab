/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "xlinux/xstdlib.h"

#include "narrator.h"

/* flex and bison do not agree on these names */
#define YYSTYPE SELECTOR_YYSTYPE
#define YYLTYPE yyu_location

#include "selector_parser.h"
#include "selector.tab.h"
#include "selector.tokens.h"
#include "selector.lex.h"
#include "selector.states.h"

#include "pattern_parser.internal.h"

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

  llist_init(&p->selector_stack);

  *rv = p;
}

void selector_parser_free(selector_parser* const p)
{
  selector * sel;
  llist *T;

  if(p)
  {
    pattern_parser_free(p->pattern_parser);

    llist_foreach_safe(&p->selector_stack, sel, lln, T) {
      llist_delete(&sel->lln);
      selector_free(sel);
    }

    yyu_parser_destroy(&p->yyu);
  }

  free(p);
}

void selector_parser_ifree(selector_parser ** const p)
{
  selector_parser_free(*p);
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
  selector *sel;
  int r;

  parser->pattern_parser->yyu = 0;

  llist_init(&parser->selector_stack);
  r = yyu_parse(&parser->yyu, buf, size, fname, YYU_PARTIAL | YYU_INPLACE, init_loc, used_loc);

  /* error aggregation */
  if(r == 0) {
    parser->error = 0;
    parser->error_len = 0;
  } else if(parser->pattern_parser->error) {
    parser->error = parser->pattern_parser->error;
    parser->error_len = parser->pattern_parser->error_len;
  } else {
    parser->error = parser->yyu.error_str;
    parser->error_len = parser->yyu.error_len;
  }

  *rv = 0;
  if(r == 0) {
    *rv = llist_shift(&parser->selector_stack, selector, lln);
  }

  while((sel = llist_shift(&parser->selector_stack, typeof(*sel), lln))) {
    selector_free(sel);
  }

  return r;
}
