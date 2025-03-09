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
#include "disintegrate.h"
#include "resolve.h"
#include "pattern_parser.h"
#include "selection.h"
#include "shadow.h"
#include "pattern_parser.internal.h"
#include "ruleset.h"

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
  vertices_init(&p->lookup_hits);
  p->g = &g_graph;

  bacon_parser_alloc(&p->bacon_parser);

  llist_init(&p->blocks);
  llist_init(&p->varsets_names);
  llist_init(&p->rulesets_names);
  p->managed = false;

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
  vertices_destroy(&parser->lookup_hits);

  //block_release_bulk(parser->bacon_parser, &parser->blocks);
  ruleset_special_name_list_free(&parser->varsets_names);
  ruleset_special_name_list_free(&parser->rulesets_names);
  pattern_free(&parser->anchor->pattern);

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

static void ruleset_parser_clean(ruleset_parser * restrict parser)
{
  struct edges edges;
  struct vertices unlinking;
  struct vertices releasing;
  graph_invalidation_context invalidation = { };

  graph_invalidation_begin(&invalidation);
  edges_init(&edges);
  vertices_init(&unlinking);
  vertices_init(&releasing);

  blocks_gather_rules(&unlinking, &parser->blocks);

  graph_disintegrate(&edges, &unlinking, &releasing, &invalidation);

  graph_invalidation_end(&invalidation);

  block_release_bulk(parser->bacon_parser, &parser->blocks);
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

  //block_release_bulk(parser->bacon_parser, &parser->blocks);
  parser->varsets_set = false;
  ruleset_special_name_list_free(&parser->varsets_names);
  parser->rulesets_set = false;
  ruleset_special_name_list_free(&parser->rulesets_names);
  parser->managed = false;

  pattern_free(&parser->anchor->pattern);
  parser->anchor = 0;

  r = yyu_parse(&parser->yyu, buf, size, relpath, YYU_INPLACE, 0, 0);

  /* error aggregation */
  if(r == 0) {
    parser->error = 0;
    parser->error_len = 0;
  } else if(parser->bacon_parser->error) {
    parser->error = parser->bacon_parser->error;
    parser->error_len = parser->bacon_parser->error_len;
  } else if(parser->pattern_parser->error) {
    parser->error = parser->pattern_parser->error;
    parser->error_len = parser->pattern_parser->error_len;
  } else {
    parser->error = parser->yyu.error_str;
    parser->error_len = parser->yyu.error_len;
  }

  if(r != 0) {
    ruleset_parser_clean(parser);
    return r;
  }

  /* rules validation */
  llist_foreach(&parser->blocks, block, lln) {
    if((r = block_validate(block, parser->yyu.error_str, sizeof(parser->yyu.error_str))) != 0) {
      parser->error = parser->yyu.error_str;
      parser->error_len = r;
      ruleset_parser_clean(parser);
      return r;
    }
  }

  return 0;
}

void ruleset_parser_varsets_append(ruleset_parser * restrict parser, const char *s, uint16_t len)
{
  ruleset_special_name *name;

  xmalloc(&name, sizeof(*name));

  llist_init(&name->lln);
  name->len = len;
  name->name = s;

  llist_push(&parser->varsets_names, &name->lln);
}

void ruleset_parser_rulesets_append(ruleset_parser * restrict parser, const char *s, uint16_t len)
{
  ruleset_special_name *name;

  xmalloc(&name, sizeof(*name));

  llist_init(&name->lln);
  name->len = len;
  name->name = s;

  llist_push(&parser->rulesets_names, &name->lln);
}

void ruleset_special_name_list_free(llist * restrict list)
{
  ruleset_special_name *name;

  while((name = llist_shift(list, typeof(*name), lln))) {
    free((void*)name->name);
    free(name);
  }
}
