/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "common/attrs.h"
#include "moria/edge.h"
#include "bacon/parser.h"
#include "valyria/hashtable.h"
#include "xlinux/xstdlib.h"

#include "narrator.h"

/* flex and bison do not agree on these names */
#define YYSTYPE RULESET_YYSTYPE
#define YYLTYPE yyu_location

#include "ruleset_parser.h"
#include "ruleset.tab.h"
#include "ruleset.tokens.h"
#include "ruleset.lex.h"
#include "ruleset.states.h"

#include "channel.h"
#include "fsedge.h"
#include "fsent.h"
#include "graph.h"
#include "lookup.h"
#include "pattern_parser.h"
#include "render.h"
#include "selection.h"
#include "shadow.h"
#include "anchor.h"
#include "pattern_parser.internal.h"

static YYU_VTABLE(vtable, ruleset, ruleset);

//
// public
//

void ruleset_parser_alloc(ruleset_parser ** rv)
{
  ruleset_parser * p = 0;

  xmalloc(&p, sizeof(*p));

  yyu_parser_init(&p->yyu, &vtable);
  yyu_parser_init_tokens(&p->yyu, ruleset_token_table, ruleset_TOKEN_TABLE_SIZE);
  yyu_parser_init_states(
      &p->yyu
    , ruleset_numstates
    , ruleset_statenumbers
    , ruleset_statenames
  );

  pattern_parser_create(&p->pattern_parser);
  entlist_init(&p->lookup_hits);
  p->g = &g_graph;

  bacon_parser_alloc(&p->bacon_parser);

  llist_init(&p->blocks);
  llist_init(&p->varsets);
  llist_init(&p->rulesets);

  *rv = p;
  p = 0;

  ruleset_parser_free(p);
}

void ruleset_parser_free(ruleset_parser* const parser)
{
  if(!parser) {
    return;
  }

  yyu_parser_destroy(&parser->yyu);
  pattern_parser_free(parser->pattern_parser);
  entlist_destroy(&parser->lookup_hits);

  block_release_bulk(parser->bacon_parser, &parser->blocks);
  pattern_list_free(&parser->varsets);
  pattern_list_free(&parser->rulesets);
  anchor_release(parser->anchor);

  bacon_parser_free(parser->bacon_parser);
  free(parser);
}

void ruleset_parser_ifree(ruleset_parser ** const p)
{
  ruleset_parser_free(*p);
  *p = 0;
}

static int block_validate(block *b, char * restrict error_str, size_t error_str_sz)
{
  rule *rule;
  block *sb;
  int r;

  llist_foreach(&b->rules, rule, lln) {
    if((r = rule_validate(rule, error_str, error_str_sz)) != 0) {
      return r;
    }
  }

  llist_foreach(&b->blocks, sb, lln) {
    if((r = block_validate(sb, error_str, error_str_sz)) != 0) {
      return r;
    }
  }

  return 0;
}

int ruleset_parser_parse(
    ruleset_parser * restrict parser
  , char * const restrict buf
  , size_t size
  , const char * relpath
  , uint16_t relpath_len
)
{
  int r;
  block *block;

  parser->yyu.fname = relpath;
  parser->bacon_parser->yyu = NULL;
  parser->pattern_parser->yyu = NULL;

  block_release_bulk(parser->bacon_parser, &parser->blocks);
  pattern_list_free(&parser->varsets);
  pattern_list_free(&parser->rulesets);

  anchor_release(parser->anchor);
  parser->anchor = 0;

  if((r = yyu_parse(&parser->yyu, buf, size, relpath, YYU_INPLACE, 0, 0)) != 0) {
    return r;
  }

  /* rules validation */
  llist_foreach(&parser->blocks, block, lln) {
    if((r = block_validate(block, parser->yyu.error_str, sizeof(parser->yyu.error_str))) != 0) {
      parser->yyu.error_len = r;
      return r;
    }
  }

  return 0;
}
