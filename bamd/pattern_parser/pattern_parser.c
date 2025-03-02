/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "common/attrs.h"
#include "narrator.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xstring.h"

/* flex and bison do not agree on these names */
#define YYSTYPE struct pattern_yystype
#define YYLTYPE yyu_location

/* the token enums shadow each other - only include one set */
#define SEARCH_PATTERN_YYTOKENTYPE
#define REFERENCE_PATTERN_YYTOKENTYPE
#define LOOKUP_PATTERN_YYTOKENTYPE
#define INCLUDE_PATTERN_YYTOKENTYPE
#define MATCH_PATTERN_YYTOKENTYPE

#include "pattern_parser.internal.h"
#include "pattern.lex.h"
#include "pattern.states.h"
#include "search_pattern.tab.h"
#include "search_pattern.tokens.h"
#include "generate_pattern.tab.h"
#include "generate_pattern.tokens.h"
#include "reference_pattern.tab.h"
#include "reference_pattern.tokens.h"
#include "lookup_pattern.tab.h"
#include "lookup_pattern.tokens.h"
#include "include_pattern.tab.h"
#include "include_pattern.tokens.h"
#include "match_pattern.tab.h"
#include "match_pattern.tokens.h"


static YYU_VTABLE(search_vtable, pattern, search_pattern);
static YYU_VTABLE(generate_vtable, pattern, generate_pattern);
static YYU_VTABLE(reference_vtable, pattern, reference_pattern);
static YYU_VTABLE(lookup_vtable, pattern, lookup_pattern);
static YYU_VTABLE(include_vtable, pattern, include_pattern);
static YYU_VTABLE(match_vtable, pattern, match_pattern);

//
// public
//

void pattern_parser_create(pattern_parser ** rv)
{
  pattern_parser * p = 0;

  xmalloc(&p, sizeof(*p));

  // search
  yyu_parser_init(&p->search_yyu, &search_vtable);
  yyu_parser_init_tokens(&p->search_yyu, search_pattern_token_table, search_pattern_TOKEN_TABLE_SIZE);
  yyu_parser_init_states(
      &p->search_yyu
    , pattern_numstates
    , pattern_statenumbers
    , pattern_statenames
  );

  // generate
  yyu_parser_init(&p->generate_yyu, &generate_vtable);
  yyu_parser_init_tokens(&p->generate_yyu, generate_pattern_token_table, generate_pattern_TOKEN_TABLE_SIZE);
  yyu_parser_init_states(
      &p->generate_yyu
    , pattern_numstates
    , pattern_statenumbers
    , pattern_statenames
  );

  // reference (formula)
  yyu_parser_init(&p->reference_yyu, &reference_vtable);
  yyu_parser_init_tokens(&p->reference_yyu, reference_pattern_token_table, reference_pattern_TOKEN_TABLE_SIZE);
  yyu_parser_init_states(
      &p->reference_yyu
    , pattern_numstates
    , pattern_statenumbers
    , pattern_statenames
  );

  // lookup
  yyu_parser_init(&p->lookup_yyu, &lookup_vtable);
  yyu_parser_init_tokens(&p->lookup_yyu, lookup_pattern_token_table, lookup_pattern_TOKEN_TABLE_SIZE);
  yyu_parser_init_states(
      &p->lookup_yyu
    , pattern_numstates
    , pattern_statenumbers
    , pattern_statenames
  );

  // include
  yyu_parser_init(&p->include_yyu, &include_vtable);
  yyu_parser_init_tokens(&p->include_yyu, include_pattern_token_table, include_pattern_TOKEN_TABLE_SIZE);
  yyu_parser_init_states(
      &p->include_yyu
    , pattern_numstates
    , pattern_statenumbers
    , pattern_statenames
  );

  // match
  yyu_parser_init(&p->match_yyu, &match_vtable);
  yyu_parser_init_tokens(&p->match_yyu, match_pattern_token_table, match_pattern_TOKEN_TABLE_SIZE);
  yyu_parser_init_states(
      &p->match_yyu
    , pattern_numstates
    , pattern_statenumbers
    , pattern_statenames
  );

  *rv = p;
}

void pattern_parser_free(pattern_parser* const p)
{
  if(p)
  {
    yyu_parser_destroy(&p->search_yyu);
    yyu_parser_destroy(&p->generate_yyu);
    yyu_parser_destroy(&p->reference_yyu);
    yyu_parser_destroy(&p->lookup_yyu);
    yyu_parser_destroy(&p->include_yyu);
    yyu_parser_destroy(&p->match_yyu);
  }

  free(p);
}

void pattern_parser_ifree(pattern_parser ** const p)
{
  pattern_parser_free(*p);
  *p = 0;
}

static int parse_partial(
    pattern_parser * restrict parser
  , yyu_parser *yyu
  , char * const restrict buf
  , size_t size
  , const char * restrict fname
  , yyu_location * init_loc
  , yyu_location * used_loc
  , pattern ** restrict rv
)
{
  int r;

  parser->group_counter = 0;
  parser->capture_group_counter = 0;
  parser->yyu = yyu;

  r = yyu_parse(parser->yyu, buf, size, fname, YYU_PARTIAL | YYU_INPLACE, init_loc, used_loc);
  if(r) {
  } else if(rv) {
    if(fname && parser->pattern) {
      ixstrdup(&parser->pattern->fname, fname);
    }
    parser->pattern->groups_count = parser->group_counter;
    parser->pattern->capture_groups_count = parser->capture_group_counter;
    *rv = parser->pattern;
    parser->pattern = 0;
  }

  pattern_free(parser->pattern);

  return r;
}

int search_pattern_parse_partial(
    pattern_parser * restrict parser
  , char * const restrict buf
  , size_t size
  , const char * restrict fname
  , yyu_location * init_loc
  , yyu_location * used_loc
  , pattern ** restrict rv
)
{
  return parse_partial(parser, &parser->search_yyu, buf, size, fname, init_loc, used_loc, rv);
}

int generate_pattern_parse_partial(
    pattern_parser * restrict parser
  , char * const restrict buf
  , size_t size
  , const char * restrict fname
  , yyu_location * init_loc
  , yyu_location * used_loc
  , pattern ** restrict rv
)
{
  return parse_partial(parser, &parser->generate_yyu, buf, size, fname, init_loc, used_loc, rv);
}

int reference_pattern_parse_partial(
    pattern_parser * restrict parser
  , char * const restrict buf
  , size_t size
  , const char * restrict fname
  , yyu_location * init_loc
  , yyu_location * used_loc
  , pattern ** restrict rv
)
{
  return parse_partial(parser, &parser->reference_yyu, buf, size, fname, init_loc, used_loc, rv);
}

int lookup_pattern_parse_partial(
    pattern_parser * restrict parser
  , char * const restrict buf
  , size_t size
  , const char * restrict fname
  , yyu_location * init_loc
  , yyu_location * used_loc
  , pattern ** restrict rv
)
{
  return parse_partial(parser, &parser->lookup_yyu, buf, size, fname, init_loc, used_loc, rv);
}

int include_pattern_parse_partial(
    pattern_parser * restrict parser
  , char * const restrict buf
  , size_t size
  , const char * restrict fname
  , yyu_location * init_loc
  , yyu_location * used_loc
  , pattern ** restrict rv
)
{
  return parse_partial(parser, &parser->include_yyu, buf, size, fname, init_loc, used_loc, rv);
}

int match_pattern_parse_partial(
    pattern_parser * restrict parser
  , char * const restrict buf
  , size_t size
  , const char * restrict fname
  , yyu_location * init_loc
  , yyu_location * used_loc
  , pattern ** restrict rv
)
{
  return parse_partial(parser, &parser->match_yyu, buf, size, fname, init_loc, used_loc, rv);
}

int lookup_pattern_parse(
    pattern_parser * restrict parser
  , char * const restrict buf
  , size_t size
  , const char * restrict fname
  , pattern ** restrict rv
)
{
  int r;

  parser->group_counter = 0;
  parser->capture_group_counter = 0;
  parser->yyu = &parser->lookup_yyu;

  if((r = yyu_parse(parser->yyu, buf, size, fname, YYU_INPLACE, 0, 0))) {
  } else if(rv) {
    if(fname && parser->pattern) {
      ixstrdup(&parser->pattern->fname, fname);
    }
    parser->pattern->groups_count = parser->group_counter;
    parser->pattern->capture_groups_count = parser->capture_group_counter;
    *rv = parser->pattern;
    parser->pattern = 0;
  }

  pattern_free(parser->pattern);

  return r;
}
