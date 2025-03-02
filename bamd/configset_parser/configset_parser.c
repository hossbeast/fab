/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "xlinux/xstdlib.h"
#include "bacon/parser.h"
#include "bacon/make.h"
#include "bacon/location.h"
#include "common/attrs.h"

#include "narrator.h"

/* flex and bison do not agree on these names */
#define YYSTYPE CONFIGSET_YYSTYPE
#define YYLTYPE yyu_location

#include "configset_parser/glue.h"
#include "configset_parser.h"
#include "configset.tab.h"
#include "configset.tokens.h"
#include "configset.lex.h"
#include "configset.states.h"

#include "pattern_parser.h"
#include "config.h"
#include "zcat.h"

static YYU_VTABLE(vtable, configset, configset);

//
// public
//

void configset_parser_create(configset_parser ** rv)
{
  configset_parser * p = 0;

  xmalloc(&p, sizeof(*p));

  yyu_parser_init(&p->yyu, &vtable);
  yyu_parser_init_tokens(&p->yyu, configset_token_table, configset_TOKEN_TABLE_SIZE);
  yyu_parser_init_states(
      &p->yyu
    , configset_numstates
    , configset_statenumbers
    , configset_statenames
  );

  pattern_parser_create(&p->pattern_parser);
  bacon_parser_alloc(&p->bacon_parser);

  *rv = p;
}

void configset_parser_free(configset_parser* const p)
{
  if(!p) {
    return;
  }

  pattern_parser_free(p->pattern_parser);
  bacon_parser_free(p->bacon_parser);

  yyu_parser_destroy(&p->yyu);

  free(p);
}

void configset_parser_ifree(configset_parser ** const p)
{
  configset_parser_free(*p);
  *p = 0;
}

int configset_parser_parse(
    configset_parser * restrict parser
  , char * const restrict buf
  , size_t size
  , const char * restrict path
  , yyu_location * restrict init_loc
  , config * restrict cfg
)
{
  int r;

  parser->cfg = cfg;
  r = yyu_parse(&parser->yyu, buf, size, path, YYU_INPLACE, init_loc, 0);

  return r;
}

static int config_fstree_mapping_key_cmp(void * restrict _key, const rbnode * restrict rbn)
{
  const struct config_fstree_mapping *key;
  const struct config_fstree_mapping *fsm;

  key = _key;
  fsm = containerof(rbn, typeof(*fsm), rbn);

  return pattern_cmp(key->key.v, fsm->key.v);
}

void configset_parser_fsm_alloc(configset_parser * restrict parser, struct pattern * restrict key)
{
  struct config_fstree_mapping *fsm;
  rbtree_search_context ctx;
  rbnode *rbn;

  xmalloc(&fsm, sizeof(*fsm));
  fsm->key.v = key;

  if((rbn = rbtree_search(&parser->cfg->filesystem.fstrees.set, &ctx, fsm, config_fstree_mapping_key_cmp))) {
    free(fsm);
    pattern_free(key);
    parser->fsm = containerof(rbn, typeof(*fsm), rbn);
    return;
  }
  parser->fsm = fsm;
  parser->fsm->key.v = key;
  rbtree_insert(&parser->cfg->filesystem.fstrees.set, &ctx, &fsm->rbn);
}

config_plugin_entry *configset_parser_plugin_alloc(configset_parser * restrict parser)
{
  config_plugin_entry *pe;

  xmalloc(&pe, sizeof(*pe));

  return pe;
}

void configset_parser_plugin_put(configset_parser * restrict parser, struct config_plugin_entry * restrict pe)
{
  llist_push(&parser->cfg->plugins.list, &pe->lln);
}

void configset_parser_plugin_arg_put(configset_parser * restrict parser, bacon_value * restrict string)
{
  yyu_location loc;

  loc.fname = string->loc->fname;
  loc.f_lin = string->loc->f_lin;
  loc.f_col = string->loc->f_col;
  loc.l_lin = string->loc->l_lin;
  loc.l_col = string->loc->l_col;
  loc.s = string->loc->s;
  loc.e = string->loc->e;
  loc.l = string->loc->l;

  bacon_list_mkv(parser->bacon_parser, &loc, parser->pe->args.list, &parser->pe->args.list, string);
}

static int config_special_key_cmp(void * restrict _key, const rbnode * restrict rbn)
{
  const struct bacon_value *key;
  const struct config_special_entry *wse;

  key = _key;
  wse = containerof(rbn, typeof(*wse), rbn);

  return memncmp(key->s, key->len, wse->s, wse->len);
}

void configset_parser_wse_alloc(configset_parser * restrict parser, struct bacon_value *string)
{
  struct config_special_entry *wse;
  rbtree_search_context ctx;
  rbnode *rbn;

  if((rbn = rbtree_search(&parser->cfg->filesystem.special.set, &ctx, string, config_special_key_cmp))) {
    wse = containerof(rbn, typeof(*wse), rbn);
    bacon_value_free(parser->bacon_parser, string);
    return;
  }
  xmalloc(&wse, sizeof(*wse));
  llist_init(&wse->lln);
  rbnode_init(&wse->rbn);
  rbtree_insert(&parser->cfg->filesystem.special.set, &ctx, &wse->rbn);

  wse->v = string;
  wse->s = string->s;
  wse->len = string->len;
}

static int config_formula_path_dirs_key_cmp(void * restrict _key, const rbnode * restrict rbn)
{
  const struct bacon_value *key;
  const struct config_formula_path_dirs_entry *fpde;

  key = _key;
  fpde = containerof(rbn, typeof(*fpde), rbn);

  return memncmp(key->s, key->len, fpde->s, fpde->len);
}

void configset_parser_fpde_alloc(configset_parser * restrict parser, struct bacon_value *string)
{
  struct config_formula_path_dirs_entry *fpde;
  rbtree_search_context ctx;
  rbnode *rbn;

  if((rbn = rbtree_search(&parser->cfg->formula.path.dirs.set, &ctx, string, config_formula_path_dirs_key_cmp))) {
    fpde = containerof(rbn, typeof(*fpde), rbn);
    bacon_value_free(parser->bacon_parser, string);
    return;
  }
  xmalloc(&fpde, sizeof(*fpde));
  llist_init(&fpde->lln);
  rbnode_init(&fpde->rbn);
  rbtree_insert(&parser->cfg->formula.path.dirs.set, &ctx, &fpde->rbn);

  fpde->v = string;
  fpde->s = string->s;
  fpde->len = string->len;
}

static int config_plugin_path_dirs_key_cmp(void * restrict _key, const rbnode * restrict rbn)
{
  const struct bacon_value *key;
  const struct config_plugin_path_dirs_entry *fpde;

  key = _key;
  fpde = containerof(rbn, typeof(*fpde), rbn);

  return memncmp(key->s, key->len, fpde->s, fpde->len);
}

void configset_parser_ppde_alloc(configset_parser * restrict parser, struct bacon_value *string)
{
  struct config_plugin_path_dirs_entry *fpde;
  rbtree_search_context ctx;
  rbnode *rbn;

  if((rbn = rbtree_search(&parser->cfg->plugins.path.dirs.set, &ctx, string, config_plugin_path_dirs_key_cmp))) {
    fpde = containerof(rbn, typeof(*fpde), rbn);
    bacon_value_free(parser->bacon_parser, string);
    return;
  }
  xmalloc(&fpde, sizeof(*fpde));
  llist_init(&fpde->lln);
  rbnode_init(&fpde->rbn);
  rbtree_insert(&parser->cfg->plugins.path.dirs.set, &ctx, &fpde->rbn);

  fpde->v = string;
  fpde->s = string->s;
  fpde->len = string->len;
}

static int config_pattern_key_cmp(void * restrict _key, const rbnode * restrict rbn)
{
  const struct pattern *key;
  const struct pattern *pat;

  key = _key;
  pat = containerof(rbn, typeof(*pat), rbn);

  return pattern_cmp(key, pat);
}

void configset_parser_we_put(configset_parser * restrict parser, pattern * restrict pat)
{
  rbtree_search_context ctx;
  rbnode *rbn;

  if((rbn = rbtree_search(&parser->cfg->filesystem.exclude.set, &ctx, pat, config_pattern_key_cmp))) {
    rbtree_replace(&parser->cfg->filesystem.exclude.set, rbn, &pat->rbn);
    pat = containerof(rbn, typeof(*pat), rbn);
    pattern_free(pat);
  } else {
    rbtree_insert(&parser->cfg->filesystem.exclude.set, &ctx, &pat->rbn);
  }
}
