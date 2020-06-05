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

#include <string.h>

#include "types.h"
#include "xapi.h"

#include "xapi/trace.h"
#include "xapi/calltree.h"
#include "moria/edge.h"
#include "moria/graph.h"
#include "moria/graph.h"
#include "moria/vertex.h"
#include "moria/operations.h"
#include "narrator.h"
#include "valyria/dictionary.h"
#include "valyria/list.h"
#include "valyria/map.h"
#include "valyria/set.h"
#include "valyria/llist.h"
#include "xapi/info.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xstring.h"
#include "xlinux/xunistd.h"
#include "xlinux/xfcntl.h"
#include "value.h"
#include "value/merge.h"
#include "value/parser.h"

#include "module.internal.h"
#include "extern.h"
#include "module_parser.h"
#include "logging.h"
#include "node.h"
#include "shadow.h"
#include "node_operations.h"
#include "pattern.h"
#include "generate.h"
#include "render.h"
#include "params.h"
#include "path.h"
#include "rule.h"
#include "formula.h"
#include "exec_builder.h"
#include "variant.h"
#include "var.h"
#include "exec_builder.h"
#include "exec_render.h"
#include "stats.h"
#include "MODULE.errtab.h"

#include "zbuffer.h"
#include "common/assure.h"
#include "common/snarf.h"
#include "common/attrs.h"
#include "common/hash.h"
#include "usage.h"

llist g_modules;                            // all modules
static llist modules_invalidated;           // module files with pending invalidation
llist parsers = LLIST_INITIALIZER(parsers); // struct module_parser
static exec_builder local_exec_builder;
static exec_render_context local_exec_render;

//
// static
//

static xapi __attribute__((nonnull(1, 3)))
build_block_variant_envs(module * restrict mod, statement_block * restrict block, value * restrict vars)
{
  enter;

  variant *v;
  value *bag = 0;
  value *bagg;
  exec * e = 0;
  int x;

  if(block == 0 || block->variants->size == 0)
  {
    fatal(var_denormalize, mod->value_parser, 0, vars, &bag);
    mod->novariant_var = bag;
    bag = 0;

    fatal(exec_builder_xreset, &local_exec_builder);
    fatal(exec_render_context_xreset, &local_exec_render);
    exec_render_context_configure(&local_exec_render, &local_exec_builder, mod, mod->novariant_var, 0);
    fatal(exec_render_env_vars, &local_exec_render);
    fatal(exec_builder_build, &local_exec_builder, 0);
    exec_builder_take(&local_exec_builder, &e);
    mod->novariant_envs = e;
    e = 0;
  }

  // denormalize module vars for all variants in the block
  for(x = 0; block && x < block->variants->table_size; x++)
  {
    if((v = set_table_get(block->variants, x)) == 0)
      continue;

    if(map_get(mod->variant_var, MM(v)))
      continue;

    fatal(var_denormalize, mod->value_parser, v, vars, &bag);
    bagg = bag;
    fatal(map_put, mod->variant_var, MM(v), bag, 0);
    bag = 0;

    fatal(exec_builder_xreset, &local_exec_builder);
    fatal(exec_render_context_xreset, &local_exec_render);
    exec_render_context_configure(&local_exec_render, &local_exec_builder, mod, bagg, 0);
    fatal(exec_render_env_vars, &local_exec_render);
    fatal(exec_builder_build, &local_exec_builder, 0);
    exec_builder_take(&local_exec_builder, &e);
    fatal(map_put, mod->variant_envs, MM(v), e, 0);
    e = 0;
  }

finally:
  exec_free(e);
coda;
}

static xapi rebuild_variant_envs(module * restrict mod)
{
  enter;

  vertex ** ancestry = 0;
  size_t ancestryz = 0;
  size_t ancestryl = 0;
  int x;
  statement_block *block;
  vertex *v;
  vertex * var_v;
  value * vars = 0;
  node * env_n;

  fatal(map_recycle, mod->variant_var);
  fatal(map_recycle, mod->variant_envs);
  exec_ifree(&mod->novariant_envs);

  // assemble list of ancestor nodes
  v = vertex_containerof(mod->dir_node);
  while(v)
  {
    fatal(assure, &ancestry, sizeof(*ancestry), ancestryl + 1, &ancestryz);
    ancestry[ancestryl++] = v;
    v = vertex_up(v);
  }

  /* start with the vars from config */
  fatal(value_merge, mod->value_parser, &vars, g_var, 0);

  /* apply ancestor var.bam files in reverse order */
  for(x = ancestryl - 1; x >= 0; x--)
  {
    if((var_v = vertex_downs(ancestry[x], NODE_VAR_BAM)))
    {
      env_n = vertex_value(var_v);
      fatal(var_node_parse, env_n);
      fatal(value_merge, mod->value_parser, &vars, env_n->self_var, 0);
    }
  }

  /* apply the var from module.bam if any */
  fatal(value_merge, mod->value_parser, &vars, mod->var_value, mod->var_merge_overwrite ? VALUE_MERGE_SET : 0);

  if(vars)
  {
    /* no-variant vars */
    fatal(build_block_variant_envs, mod, 0, vars);

    /* top-level block, if it's not also a no-variant block */
    if(mod->unscoped_block && mod->unscoped_block->variants->size)
    {
      fatal(build_block_variant_envs, mod, mod->unscoped_block, vars);
    }

    llist_foreach(&mod->scoped_blocks, block, lln) {
      fatal(build_block_variant_envs, mod, block, vars);
    }
  }

finally:
  wfree(ancestry);
coda;
}

struct associate_rules_context
{
  module * mod;
  statement_block * block;
};

/*
 * associate rules in the statement block with the module
 *
 * mod_self - module to associate rules to
 * mod_used - module which defines the rules
 * block    - block of rules
 * variants - variants to use with the association - must be part of the owning module
 */
static xapi associate_block_rules(module * mod_self, module * mod_used, statement_block * restrict block, set * restrict variants)
{
  enter;

  rule *r;
  rule_module_association *rma;

  llist_foreach(&block->rules, r, lln) {
    rma = 0;
    fatal(module_rule_associate, mod_self, mod_used, r, variants, &rma);
    graph_rma_enqueue(rma);
  }

  finally : coda;
}

static xapi module_rule_disassociate(rule_module_association * restrict rma, graph_invalidation_context * restrict invalidation)
{
  enter;

  struct node_edge *ne;

  STATS_DEC(rmas);

  /* remove edges - connections to directory nodes - from this rma */
  llist_foreach(&rma->edges, ne, lln) {
    fatal(node_edge_disconnect, ne, invalidation);
  }

  /*
   * Remove this rma from the run-queue. It might be in the rma run queue because the rule was attached
   * to a directory which was changed while unloading the module
   */
  if(llist_attached(rma, changed[0].lln)) {
    llist_delete(rma, changed[0].lln);
  }

  if(llist_attached(rma, changed[1].lln)) {
    llist_delete(rma, changed[1].lln);
  }

  finally : coda;
}

static xapi rule_remove(rule * restrict r, graph_invalidation_context * restrict invalidation)
{
  enter;

  vertex *v;
  edge *e;
  llist *cursor, lln;

  v = vertex_containerof(r);

  /* disconnect edges connecting this rule to dirnodes */
  llist_init_node(&lln);
  rbtree_foreach(&v->down, e, rbn_down) {
    llist_append(&lln, e, lln);
  }

  llist_foreach_safe(&lln, e, lln, cursor) {
    fatal(graph_edge_disconnect, g_graph, e);
  }

  STATS_DEC(rules);
  fatal(graph_delete_vertex, v);

  finally : coda;
}

/* run all rules in a module A (which defines the rules) in context of module B (module being loaded) */
static xapi module_rules_uses_visitor(vertex * mod_dir_v, void * ctxp, traversal_mode mode, int distance, int * result)
{
  enter;

  node * mod_dir_n;
  module * mod_self;
  module * mod_used;
  statement_block *block;

  mod_self = ctxp;
  mod_dir_n = vertex_value(mod_dir_v);
  mod_used = mod_dir_n->mod;

  /* for rules in the unscoped block, use variants from the unscoped block of the module being loaded */
  if(mod_used->unscoped_block) {
    fatal(associate_block_rules, mod_self, mod_used, mod_used->unscoped_block, 0);// mod_self->unscoped_block.variants);
  }

  /* however for scoped blocks, use variants from the block in the module which defines the rules */
  llist_foreach(&mod_used->scoped_blocks, block, lln) {
    fatal(associate_block_rules, mod_self, mod_used, block, block->variants);
  }

  finally : coda;
}

static xapi module_parse(module * restrict mod, module_parser * restrict parser, graph_invalidation_context * restrict invalidation)
{
  enter;

  xapi exit;
  char trace[4096 << 1];
  char * text = 0;
  size_t text_len;
  node * mod_file_n;
  char * mod_file_relpath;
  rule *r;
  statement_block *block;

  mod_file_n = mod->self_node;
  mod_file_relpath = mod->self_node_relpath;

  if(!mod_file_n->not_parsed)
    goto XAPI_FINALIZE;

  fatal(snarfats, &text, &text_len, g_params.proj_dirfd, mod_file_relpath);

  if(!text)
  {
    mod_file_n->not_parsed = 0;
  }
  else if((exit = invoke(module_parser_parse, parser, mod, invalidation, text, text_len + 2, mod_file_n->name->name)))
  {
#if DEBUG || DEVEL || XAPI
    xapi_trace_full(trace, sizeof(trace), 0);
#else
    xapi_trace_pithy(trace, sizeof(trace), 0);
#endif
    xapi_calltree_unwind();

    xlogs(L_WARN, L_NOCATEGORY, trace);

    // cleanup blocks which were parsed but which will not be used
    if(parser->unscoped_block) {
      llist_append(&parser->scoped_blocks, parser->unscoped_block, lln);
      parser->unscoped_block = 0;
    }
    llist_foreach(&parser->scoped_blocks, block, lln) {
      llist_foreach(&block->rules, r, lln) {
        STATS_DEC(rules);
        fatal(graph_vertex_delete, g_graph, vertex_containerof(r));
      }
      llist_init_node(&block->rules);
    }
    llist_splice_head(&parser->statement_block_freelist, &parser->scoped_blocks);
  }
  else
  {
    mod_file_n->not_parsed = 0;
  }

finally:
  xapi_infos("path", mod_file_relpath);
  wfree(text);
coda;
}

struct module_scope_context {
  module *mod;
  graph_invalidation_context *invalidation;
};

/*
 * attach each child node to the scope/ directory (there can be no collisions at this point)
 */
static xapi module_scope_visitor(vertex * v, void * ctxp, traversal_mode mode, int distance, int * result)
{
  enter;

  struct module_scope_context *ctx = ctxp;
  node * n;

  n = vertex_value(v);
  fatal(shadow_graft_scope, ctx->mod, n, n->name->name, n->name->namel, true, 0, ctx->invalidation);

  finally : coda;
}

/*
 * associate directory nodes with the module they belong to
 */
static xapi module_mark_visitor(vertex * v, void * ctx, traversal_mode mode, int distance, int * result)
{
  enter;

  module * mod = ctx;
  node * n;

  // module dirs only
  n = vertex_value(v);

  // stop on nested modules
  if(vertex_downs(v, NODE_MODULE_BAM))
  {
    *result = MORIA_TRAVERSE_PRUNE;
    goto XAPI_FINALLY;
  }

  n->mod = mod;

  finally : coda;
}

/*
 * load the module for this node, if it is a directory node with a module.bam file
 */
static xapi module_load_visitor(vertex * mod_dir_v, void * ctx, traversal_mode mode, int distance, int * result)
{
  enter;

  graph_invalidation_context * invalidation = ctx;
  node * mod_dir_n;
  node * mod_file_n;
  vertex * mod_file_v;

  // no module.bam file present
  if((mod_file_v = vertex_downs(mod_dir_v, NODE_MODULE_BAM)) == 0)
  {
    if(distance == 0) {
      fail(MODULE_NOMODULE);
    }
    goto XAPI_FINALLY;
  }

  mod_file_n = vertex_value(mod_file_v);
  mod_dir_n = vertex_value(mod_dir_v);
  fatal(module_load, mod_dir_n, mod_file_n, invalidation);

  finally : coda;
}

static xapi module_refresh(module * restrict mod, graph_invalidation_context * restrict invalidation)
{
  enter;

  node * mod_dir_n;
  vertex * mod_dir_v;
  node * mod_file_n;

  mod_dir_n = mod->dir_node;
  mod_dir_v = vertex_containerof(mod_dir_n);
  mod_file_n = mod->self_node;

  if(node_nodetype_get(mod_file_n) == VERTEX_NODETYPE_MODULE)
  {
    STATS_INC(module_refresh);
  }
  else
  {
    STATS_INC(model_refresh);
  }

  if(node_nodetype_get(mod_file_n) == VERTEX_NODETYPE_MODEL) {
    goto XAPI_FINALIZE;
  }

  /* remove any un-refreshed imports from //module/scope and //module/imports */
  fatal(shadow_prune_imports, mod, invalidation);

  // build variant envs
  fatal(rebuild_variant_envs, mod);

  // visit this module and its USES transitively to invalidate rules
  fatal(graph_traverse_vertices
    , g_graph
    , mod_dir_v
    , module_rules_uses_visitor
    , 0
    , (traversal_criteria[]) {{
          edge_travel : EDGE_TYPE_USES
        , edge_visit : EDGE_TYPE_USES
      }}
    , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_POST
    , mod
  );

  finally : coda;
}

static xapi module_initialize(module * restrict mod, graph_invalidation_context * restrict invalidation)
{
  enter;

  uint64_t u64;
  char mod_dir_abspath[512];
  char mod_dir_realpath[512];
  char mod_file_relpath[512];
  uint16_t mod_dir_realpathl;
  node * mod_dir_n;
  node * mod_file_n;
  struct module_scope_context scope_ctx;

  mod_dir_n = mod->dir_node;
  mod_file_n = mod->self_node;

  /* module paths needed during loading */
  node_get_absolute_path(mod_dir_n, mod_dir_abspath, sizeof(mod_dir_abspath));
  fatal(xrealpaths, 0, mod_dir_realpath, mod_dir_abspath);
  mod_dir_realpathl = strlen(mod_dir_realpath);

  // module path hash
  u64 = hash64(0, mod_dir_realpath, mod_dir_realpathl);
  snprintf(mod_dir_n->mod->id, sizeof(mod_dir_n->mod->id), "%"PRIx64, u64);

  node_get_project_relative_path(mod_file_n, mod_file_relpath, sizeof(mod_file_relpath));
  fatal(ixstrdup, &mod->self_node_relpath, mod_file_relpath);

  // enable the module directory to mirror updates to the scope shadow directory
  node_kind_set(mod_dir_n, VERTEX_MODULE_DIR);

  // graft onto the shadow node for rule evaluation
  fatal(shadow_graft_module, mod, invalidation);

  // build out the virtual scope/ dir from top-level names
  scope_ctx.mod = mod;
  scope_ctx.invalidation = invalidation;
  fatal(graph_traverse_vertices
    , g_graph
    , vertex_containerof(mod_dir_n)
    , module_scope_visitor
    , 0
    , (traversal_criteria[]) {{
          edge_travel: EDGE_TYPE_FS
        , edge_visit : EDGE_TYPE_FS
        , min_depth : 1
        , max_depth : 1
      }}
    , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE | MORIA_TRAVERSE_DEPTH
    , &scope_ctx
  );

  // associate subdirectories to this module
  fatal(graph_traverse_vertices
    , g_graph
    , vertex_containerof(mod_dir_n)
    , module_mark_visitor
    , 0
    , (traversal_criteria[]) {{
          edge_travel: EDGE_TYPE_FS
        , edge_visit : EDGE_TYPE_FS
        , vertex_travel : VERTEX_FILETYPE_DIR
        , vertex_visit : VERTEX_FILETYPE_DIR
        , min_depth : 1
        , max_depth : UINT16_MAX
      }}
    , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE | MORIA_TRAVERSE_DEPTH
    , mod
  );

  // open a rdonly file descriptor to the module directory for formula execution
  fatal(xopens, &mod->dirnode_fd, O_RDONLY, mod_dir_abspath);

  finally : coda;
}

static xapi module_reload(module * restrict mod, module_parser * restrict parser, graph_invalidation_context * restrict invalidation)
{
  enter;

  llist blocks;
  statement_block *block;
  llist *cursor;
  rule *r;
  rule_module_association *rma;

  /* re-parse the module.bam file */
  mod->self_node->not_parsed = 1;
  fatal(module_parse, mod, parser, invalidation);

  if(mod->self_node->not_parsed) {
    /* parse not successful */
    goto XAPI_FINALIZE;
  }

  /* disassociate all rmas owned by this module */
  llist_foreach_safe(&mod->rmas_owner, rma, lln_rmas_owner, cursor) {

    fatal(module_rule_disassociate, rma, invalidation);
    rbtree_delete(&rma->mod->rmas, rma, rbn_rmas);
    llist_delete(rma, lln_rmas_owner);
    free(rma);
  }

  /* destroy existing/used blocks and return to the freelist */
  llist_init_node(&blocks);
  if(mod->unscoped_block) {
    llist_append(&blocks, mod->unscoped_block, lln);
  }
  llist_splice_head(&blocks, &mod->scoped_blocks);

  llist_foreach(&blocks, block, lln) {
    llist_foreach(&block->rules, r, lln) {
      fatal(rule_remove, r, invalidation);
    }

    llist_init_node(&block->rules);
  }

  /* replace the guts */
  mod->unscoped_block = parser->unscoped_block;
  parser->unscoped_block = 0;
  llist_splice_head(&mod->scoped_blocks, &parser->scoped_blocks);
  mod->var_value = parser->var_value;
  mod->var_merge_overwrite = parser->var_merge_overwrite;

  /* return previously used blocks to the freelist */
  llist_splice_head(&parser->statement_block_freelist, &blocks);

  /* re-build */
  fatal(module_refresh, mod, invalidation);

  finally : coda;
}

xapi module_load(node * restrict mod_dir_n, node * restrict mod_file_n, graph_invalidation_context * restrict invalidation)
{
  enter;

  module *mod;
  module_parser *parser = 0;

  // module has already been loaded
  if(!mod_dir_n->not_loaded)
    goto XAPI_FINALLY;

  mod_dir_n->not_loaded = 0;
  fatal(module_create, &mod_dir_n->mod);
  mod = mod_dir_n->mod;
  mod->dir_node = mod_dir_n;
  mod->self_node = mod_file_n;
  mod_file_n->self_mod = mod;

  if(strcmp(mod_file_n->name->name, NODE_MODULE_BAM) == 0)
  {
    STATS_INC(modules);
    node_kind_set(mod_file_n, VERTEX_MODULE_BAM);
  }
  else if(strcmp(mod_file_n->name->name, NODE_MODEL_BAM) == 0)
  {
    STATS_INC(models);
    node_kind_set(mod_file_n, VERTEX_MODEL_BAM);
  }
  else
  {
    RUNTIME_ABORT();
  }

  fatal(module_initialize, mod, invalidation);

  if((parser = llist_shift(&parsers, typeof(*parser), lln)) == 0)
  {
    fatal(module_parser_create, &parser);
  }

  fatal(module_reload, mod, parser, invalidation);

  // register the module
  llist_append(&g_modules, mod, lln);

finally:
  if(parser)
  {
    llist_append(&parsers, parser, lln);
  }
coda;
}

xapi statement_block_xinit(statement_block * restrict block)
{
  enter;

  fatal(set_create, &block->variants);
  llist_init_node(&block->rules);

  finally : coda;
}

xapi statement_block_xdestroy(statement_block * restrict block)
{
  enter;

  fatal(set_xfree, block->variants);

  finally : coda;
}

//
// internal
//

xapi module_block_variants(statement_block * restrict block, const pattern * restrict variants_pat)
{
  enter;

  pattern_render_result * rendered = 0;
  pattern_render_fragment * fragment;
  variant * variant;
  int x;

  fatal(pattern_render, variants_pat, &rendered);

  fragment = rendered->fragments;
  for(x = 0; x < rendered->size; x++)
  {
    fatal(variant_get, fragment->text, fragment->len, &variant);
    fatal(set_put, block->variants, variant, 0);

    fragment = pattern_render_fragment_next(fragment);
  }

finally:
  wfree(rendered);
coda;
}

xapi module_create(module ** restrict modp)
{
  enter;

  module * mod = 0;

  fatal(xmalloc, &mod, sizeof(*mod));

  llist_init_node(&mod->scoped_blocks);

#if 0
  fatal(statement_block_xinit, &mod->unscoped_block);
  fatal(array_createx
    , &mod->scoped_blocks
    , sizeof(statement_block)
    , 0
    , 0
    , 0
    , statement_block_xinit
    , 0
    , statement_block_xdestroy
  );
#endif
  fatal(map_create, &mod->variant_var);
  fatal(map_createx, &mod->variant_envs, 0, (void*)exec_free, 0);
  rbtree_init(&mod->rmas);
  llist_init_node(&mod->rmas_owner);
  fatal(value_parser_create, &mod->value_parser);
  llist_init_node(&mod->lln_invalidated);

  *modp = mod;

  finally : coda;
}

static xapi module_xdestroy(module * restrict mod)
{
  enter;

  llist lln, *cursor;
  rule_module_association *rma;
  statement_block *block;

  wfree(mod->self_node_relpath);

  if(mod->unscoped_block) {
    llist_append(&mod->scoped_blocks, mod->unscoped_block, lln);
  }

  llist_foreach_safe(&mod->scoped_blocks, block, lln, cursor) {
    fatal(statement_block_xdestroy, block);
    wfree(block);
  }

  fatal(map_xfree, mod->variant_var);
  fatal(map_xfree, mod->variant_envs);
  exec_free(mod->novariant_envs);

  llist_init_node(&lln);
  rbtree_foreach(&mod->rmas, rma, rbn_rmas) {
    llist_append(&lln, rma, lln);
  }

  llist_foreach_safe(&lln, rma, lln, cursor) {
    free(rma);
  }

  fatal(value_parser_xfree, mod->value_parser);

  finally : coda;
}

xapi module_xfree(module * restrict mod)
{
  enter;

  if(mod)
  {
    fatal(module_xdestroy, mod);
  }

  wfree(mod);

  finally : coda;
}

xapi module_ixfree(module ** restrict modp)
{
  enter;

  fatal(module_xfree, *modp);
  *modp = 0;

  finally : coda;
}

struct rma_rbn_key {
  const struct rule *rule;
};

static int rma_rbn_key_cmp(void *_key, const rbnode * _n)
{
  const struct rma_rbn_key *key = _key;
  const rule_module_association *rma = containerof(_n, typeof(*rma), rbn_rmas);

  return INTCMP(key->rule, rma->rule);
}

void rule_module_association_init(rule_module_association * restrict rma)
{
  rbnode_init(&rma->nohits_rbn);
  llist_init_node(&rma->edges);
  llist_init_node(&rma->changed[0].lln);
  llist_init_node(&rma->changed[1].lln);
}

/*
 * idempotent rule <--> module association
 *
 * mod       - module to associate
 * mod_owner - module which defines the rule
 * rule      - rule to associate
 * variants  - variants for rule execution
 * rmap      - (returns) rma
 */
xapi module_rule_associate(module * mod, module * mod_owner, rule * restrict rule, set * restrict variants, rule_module_association ** restrict rmap)
{
  enter;

  rule_module_association *rma;
  rbtree_search_context search_ctx;
  struct rma_rbn_key key;
  rbnode *rbn;

  key = (typeof(key)) {
      rule : rule
  };
  if((rbn = rbtree_search(&mod->rmas, &search_ctx, &key, rma_rbn_key_cmp)))
  {
    rma = containerof(rbn, typeof(*rma), rbn_rmas);
  }
  else
  {
    STATS_INC(rmas);
    fatal(xmalloc, &rma, sizeof(*rma));
    rule_module_association_init(rma);
    rma->rule = rule;
    rma->mod = mod;
    rma->mod_owner = mod_owner;
    rma->variants = variants;

    rbtree_insert_node(&mod->rmas, &search_ctx, &rma->rbn_rmas);

    /* also add to owner list */
    llist_append(&mod_owner->rmas_owner, rma, lln_rmas_owner);
  }

  *rmap = rma;

  finally : coda;
}

//
// public
//

xapi module_load_project(node * restrict project_root, const char * restrict project_dir, graph_invalidation_context * restrict invalidation)
{
  enter;

  /* load this module and its descendant modules */
  fatal(graph_traverse_vertices
    , g_graph
    , vertex_containerof(project_root)
    , module_load_visitor
    , 0
    , (traversal_criteria[]) {{
          edge_travel: EDGE_TYPE_FS
        , edge_visit : EDGE_TYPE_FS
        , vertex_travel : VERTEX_FILETYPE_DIR
        , vertex_visit : VERTEX_FILETYPE_DIR
      }}
    , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE
    , invalidation
  );

  finally : coda;
}

void module_invalidated(module * mod)
{
  if(llist_attached(mod, lln_invalidated)) {
    llist_delete(mod, lln_invalidated);
  }

  llist_append(&modules_invalidated, mod, lln_invalidated);
}

static xapi module_invalidate_visitor(vertex * mod_dir_v, void * ctxp, traversal_mode mode, int distance, int * result)
{
  enter;

  node * mod_dir_n = vertex_value(mod_dir_v);
  module * mod = mod_dir_n->mod;

  module_invalidated(mod);

  finally : coda;
}

xapi module_full_refresh(graph_invalidation_context * restrict invalidation)
{
  enter;

  module *mod;
  module_parser * parser = 0;

  if((parser = llist_shift(&parsers, typeof(*parser), lln)) == 0)
  {
    fatal(module_parser_create, &parser);
  }

  shadow_generation();

  while((mod = llist_shift(&modules_invalidated, typeof(*mod), lln_invalidated))) {
    fatal(module_reload, mod, parser, invalidation);

    if(mod->self_node->not_parsed) {
      /* parse not successful */
      continue;
    }

    /* reload modules which use this module */
    fatal(graph_traverse_vertices
      , g_graph
      , vertex_containerof(mod->dir_node)
      , module_invalidate_visitor
      , 0
      , (traversal_criteria[]) {{
            edge_travel : EDGE_TYPE_USES
          , edge_visit : EDGE_TYPE_USES
          , min_depth : 1
          , max_depth : 1
        }}
      , MORIA_TRAVERSE_UP | MORIA_TRAVERSE_PRE | MORIA_TRAVERSE_DEPTH
      , 0
    );
  }

  fatal(usage_report);
  fatal(stats_report);

finally:
  if(parser)
  {
    llist_append(&parsers, parser, lln);
  }
coda;
}

xapi module_setup()
{
  enter;

  llist_init_node(&g_modules);
  llist_init_node(&modules_invalidated);
  fatal(exec_builder_xinit, &local_exec_builder);
  fatal(exec_render_context_xinit, &local_exec_render);

  finally : coda;
}

xapi module_cleanup()
{
  enter;

  module_parser *parser;
  llist *T;

  fatal(exec_builder_xdestroy, &local_exec_builder);
  fatal(exec_render_context_xdestroy, &local_exec_render);

  llist_foreach_safe(&parsers, parser, lln, T) {
    fatal(module_parser_xfree, parser);
  }

  finally : coda;
}
