/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "xlinux/xstdlib.h"
#include "bacon/parser.h"
#include "common/attrs.h"

#include "narrator.h"

/* flex and bison do not agree on these names */
#define YYSTYPE CONFIG_YYSTYPE
#define YYLTYPE yyu_location

#include "config_parser/glue.h"
#include "config_parser.h"
#include "config.tab.h"
#include "config.tokens.h"
#include "config.lex.h"
#include "config.states.h"

#include "pattern_parser.h"
#include "configuration.h"

static YYU_VTABLE(vtable, config, config);

//
// public
//

void config_parser_create(config_parser ** rv)
{
  config_parser * p = 0;

  xmalloc(&p, sizeof(*p));

  yyu_parser_init(&p->yyu, &vtable);
  yyu_parser_init_tokens(&p->yyu, config_token_table, config_TOKEN_TABLE_SIZE);
  yyu_parser_init_states(
      &p->yyu
    , config_numstates
    , config_statenumbers
    , config_statenames
  );

  pattern_parser_create(&p->pattern_parser);
  bacon_parser_alloc(&p->bacon_parser);

  *rv = p;
}

void config_parser_free(config_parser* const p)
{
  if(!p) {
    return;
  }

  pattern_parser_free(p->pattern_parser);
  bacon_parser_free(p->bacon_parser);

  free(p->fm);
  yyu_parser_xdestroy(&p->yyu);

  free(p);
}

void config_parser_ifree(config_parser ** const p)
{
  config_parser_free(*p);
  *p = 0;
}

int config_parser_parse(
    config_parser * restrict parser
  , char * const restrict buf
  , size_t size
  , const char * restrict path
  , yyu_location * restrict init_loc
  , configuration * restrict cfg
)
{
  int r;

  parser->cfg = cfg;
  r = yyu_parse(&parser->yyu, buf, size, path, YYU_INPLACE, init_loc, 0);

  return r;
}

static int configuration_filesystems_mapping_key_cmp(void * restrict _key, const rbnode * restrict rbn)
{
  const struct configuration_filesystems_mapping *key;
  const struct configuration_filesystems_mapping *fm;

  key = _key;
  fm = containerof(rbn, typeof(*fm), rbn);

  return memncmp(key->key.path, key->key.path_len, fm->key.path, fm->key.path_len);
}

void config_parser_fm_alloc(config_parser * restrict parser, struct bacon_value *key)
{
  struct configuration_filesystems_mapping *fm;
  rbtree_search_context ctx;
  rbnode *rbn;

  xmalloc(&fm, sizeof(*fm));
  fm->key.path_len = path_normalize(fm->key.path, sizeof(fm->key.path), key->s, key->len);

  if((rbn = rbtree_search(&parser->cfg->filesystems.set, &ctx, fm, configuration_filesystems_mapping_key_cmp))) {
    parser->fm = containerof(rbn, typeof(*fm), rbn);
    free(fm);
    return;
  }
  parser->fm = fm;
  rbtree_insert(&parser->cfg->filesystems.set, &ctx, &fm->rbn);
}

static int configuration_walker_special_key_cmp(void * restrict _key, const rbnode * restrict rbn)
{
  const struct bacon_value *key;
  const struct configuration_walker_special_entry *wse;

  key = _key;
  wse = containerof(rbn, typeof(*wse), rbn);

  return memncmp(key->s, key->len, wse->s, wse->len);
}

void config_parser_wse_alloc(config_parser * restrict parser, struct bacon_value *string)
{
  struct configuration_walker_special_entry *wse;
  rbtree_search_context ctx;
  rbnode *rbn;

  if((rbn = rbtree_search(&parser->cfg->walker.special.set, &ctx, string, configuration_walker_special_key_cmp))) {
    wse = containerof(rbn, typeof(*wse), rbn);
  } else {
    xmalloc(&wse, sizeof(*wse));
    llist_init(&wse->lln);
    rbnode_init(&wse->rbn);
    rbtree_insert(&parser->cfg->walker.special.set, &ctx, &wse->rbn);
  }

  wse->v = string;
  wse->s = string->s;
  wse->len = string->len;
}

static int configuration_formula_path_dirs_key_cmp(void * restrict _key, const rbnode * restrict rbn)
{
  const struct bacon_value *key;
  const struct configuration_formula_path_dirs_entry *fpde;

  key = _key;
  fpde = containerof(rbn, typeof(*fpde), rbn);

  return memncmp(key->s, key->len, fpde->s, fpde->len);
}

void config_parser_fpde_alloc(config_parser * restrict parser, struct bacon_value *string)
{
  struct configuration_formula_path_dirs_entry *fpde;
  rbtree_search_context ctx;
  rbnode *rbn;

  if((rbn = rbtree_search(&parser->cfg->formula.path.dirs.set, &ctx, string, configuration_formula_path_dirs_key_cmp))) {
    fpde = containerof(rbn, typeof(*fpde), rbn);
  } else {
    xmalloc(&fpde, sizeof(*fpde));
    llist_init(&fpde->lln);
    rbnode_init(&fpde->rbn);
    rbtree_insert(&parser->cfg->formula.path.dirs.set, &ctx, &fpde->rbn);
  }

  fpde->v = string;
  fpde->s = string->s;
  fpde->len = string->len;
}

static int configuration_pattern_key_cmp(void * restrict _key, const rbnode * restrict rbn)
{
  const struct pattern *key;
  const struct pattern *pat;

  key = _key;
  pat = containerof(rbn, typeof(*pat), rbn);

  return pattern_cmp(key, pat);
}

void config_parser_we_put(config_parser * restrict parser, pattern * restrict pat)
{
  rbtree_search_context ctx;
  rbnode *rbn;

  if((rbn = rbtree_search(&parser->cfg->walker.exclude.set, &ctx, pat, configuration_pattern_key_cmp))) {
    rbtree_replace(&parser->cfg->walker.exclude.set, rbn, &pat->rbn);
    pat = containerof(rbn, typeof(*pat), rbn);
    pattern_free(pat);
  } else {
    rbtree_insert(&parser->cfg->walker.exclude.set, &ctx, &pat->rbn);
  }
}

void config_parser_wi_put(config_parser * restrict parser, pattern * restrict pat)
{
  rbtree_search_context ctx;
  rbnode *rbn;

  if((rbn = rbtree_search(&parser->cfg->walker.include.set, &ctx, pat, configuration_pattern_key_cmp))) {
    rbtree_replace(&parser->cfg->walker.include.set, rbn, &pat->rbn);
    pat = containerof(rbn, typeof(*pat), rbn);
    pattern_free(pat);
  } else {
    rbtree_insert(&parser->cfg->walker.include.set, &ctx, &pat->rbn);
  }
}

//static int configuration_filesystems_mapping_rbn_cmp(const rbnode * restrict A, const rbnode * restrict B)
//{
//  return 0;
//}
