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
#include "pattern_parser.internal.h"
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

  parser->pattern_parser->yyu = 0;

  parser->cfg = cfg;
  r = yyu_parse(&parser->yyu, buf, size, path, YYU_INPLACE, init_loc, 0);

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

  return r;
}

static int config_fstree_mapping_key_cmp(void * restrict _key, const rbnode * restrict rbn)
{
  const struct config_fstree_mapping *key;
  const struct config_fstree_mapping *fsm;

  key = _key;
  fsm = containerof(rbn, typeof(*fsm), rbn);

  return pattern_cmp(&key->key.v->pattern, &fsm->key.v->pattern);
}

void configset_parser_fsm_alloc(configset_parser * restrict parser, struct include_pattern * restrict key)
{
  struct config_fstree_mapping *fsm;
  rbtree_search_context ctx;
  rbnode *rbn;

  xmalloc(&fsm, sizeof(*fsm));
  fsm->key.v = key;

  if((rbn = rbtree_search(&parser->cfg->filesystem.fstrees.set, &ctx, fsm, config_fstree_mapping_key_cmp))) {
    free(fsm);
    pattern_free(&key->pattern);
    parser->fsm = containerof(rbn, typeof(*fsm), rbn);
    return;
  }
  parser->fsm = fsm;
  parser->fsm->key.v = key;
  rbtree_insert(&parser->cfg->filesystem.fstrees.set, &ctx, &fsm->rbn);
}

static int config_namespace_key_cmp(void * restrict _key, const rbnode * restrict rbn)
{
  const struct config_namespace_entry *key;
  const struct config_namespace_entry *nse;

  key = _key;
  nse = containerof(rbn, typeof(*nse), rbn);

  return bacon_value_cmp(key->name, nse->name);
}

void configset_parser_namespace_alloc(configset_parser * restrict parser, bacon_value * restrict name)
{
  struct config_namespace_entry *nse;
  rbtree_search_context ctx;
  rbnode *rbn;

  xmalloc(&nse, sizeof(*nse));
  nse->name = name;

  if((rbn = rbtree_search(&parser->cfg->namespaces.set, &ctx, nse, config_namespace_key_cmp))) {
    free(nse);
    bacon_value_free(parser->bacon_parser, name);
    return;
  }
  nse->name = name;
  rbtree_insert(&parser->cfg->namespaces.set, &ctx, &nse->rbn);
}

static int config_specials_key_cmp(void * restrict _key, const rbnode * restrict rbn)
{
  const struct config_specials_entry *key;
  const struct config_specials_entry *nse;

  key = _key;
  nse = containerof(rbn, typeof(*nse), rbn);

  return bacon_value_cmp(key->name, nse->name);
}

void configset_parser_varsets_alloc(configset_parser * restrict parser, bacon_value * restrict name)
{
  struct config_specials_entry *entry;
  rbtree_search_context ctx;
  rbnode *rbn;

  xmalloc(&entry, sizeof(*entry));
  entry->name = name;

  if((rbn = rbtree_search(&parser->cfg->modules.varsets.set, &ctx, entry, config_specials_key_cmp))) {
    free(entry);
    bacon_value_free(parser->bacon_parser, name);
    return;
  }
  entry->name = name;
  rbtree_insert(&parser->cfg->modules.varsets.set, &ctx, &entry->rbn);
}

void configset_parser_rulesets_alloc(configset_parser * restrict parser, bacon_value * restrict name)
{
  struct config_specials_entry *entry;
  rbtree_search_context ctx;
  rbnode *rbn;

  xmalloc(&entry, sizeof(*entry));
  entry->name = name;

  if((rbn = rbtree_search(&parser->cfg->modules.rulesets.set, &ctx, entry, config_specials_key_cmp))) {
    free(entry);
    bacon_value_free(parser->bacon_parser, name);
    return;
  }
  entry->name = name;
  rbtree_insert(&parser->cfg->modules.rulesets.set, &ctx, &entry->rbn);
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

void configset_parser_wse_put(configset_parser * restrict parser, match_pattern * restrict pat)
{
  struct config_specials_entry *entry;
  rbtree_search_context ctx;
  rbnode *rbn;
  char text[512];
  uint16_t text_len;
  bacon_value *name;

  text_len = pattern_cat(text, sizeof(text), &pat->pattern);
  bacon_string_mkw(parser->bacon_parser, &pat->pattern.loc, 0, &name, text, text_len);
  pattern_free(&pat->pattern);

  xmalloc(&entry, sizeof(*entry));
  entry->name = name;

  if((rbn = rbtree_search(&parser->cfg->filesystem.special.set, &ctx, entry, config_specials_key_cmp))) {
    free(entry);
    bacon_value_free(parser->bacon_parser, name);
    return;
  }
  entry->name = name;
  rbtree_insert(&parser->cfg->filesystem.special.set, &ctx, &entry->rbn);
}

static int config_pattern_key_cmp(void * restrict _key, const rbnode * restrict rbn)
{
  const struct pattern *key;
  const struct pattern *pat;

  key = _key;
  pat = containerof(rbn, typeof(*pat), rbn);

  return pattern_cmp(key, pat);
}

void configset_parser_we_put(configset_parser * restrict parser, match_pattern * restrict pat)
{
  rbtree_search_context ctx;
  rbnode *rbn;

  if((rbn = rbtree_search(&parser->cfg->filesystem.exclude.set, &ctx, pat, config_pattern_key_cmp))) {
    rbtree_replace(&parser->cfg->filesystem.exclude.set, rbn, &pat->pattern.rbn);
    pat = containerof(rbn, typeof(*pat), pattern.rbn);
    pattern_free(&pat->pattern);
  } else {
    rbtree_insert(&parser->cfg->filesystem.exclude.set, &ctx, &pat->pattern.rbn);
  }
}
