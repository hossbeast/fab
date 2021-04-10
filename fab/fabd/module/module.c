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
#include <inttypes.h>

#include "types.h"
#include "xapi.h"

#include "moria/edge.h"
#include "value.h"
#include "value/merge.h"
#include "value/parser.h"
#include "valyria/llist.h"
#include "valyria/map.h"
#include "valyria/set.h"
#include "xapi/calltree.h"
#include "xapi/trace.h"
#include "xlinux/xfcntl.h"
#include "xlinux/xstdlib.h"

#include "module.internal.h"
#include "MODULE.errtab.h"
#include "exec.h"
#include "exec_builder.h"
#include "exec_render.h"
#include "fsedge.h"
#include "fsent.h"
#include "logging.h"
#include "module_parser.h"
#include "params.h"
#include "pattern.h"
#include "render.h"
#include "rule.h"
#include "rule_module.h"
#include "rule_system.h"
#include "shadow.h"
#include "stats.h"
#include "var.h"
#include "variant.h"

#include "common/assure.h"
#include "common/snarf.h"
#include "common/hash.h"
#include "marshal.h"

llist module_list = LLIST_INITIALIZER(module_list);                 // active modules
static llist module_freelist = LLIST_INITIALIZER(module_freelist);  // freelist

static llist module_edge_list = LLIST_INITIALIZER(module_edge_list);          // active module_edges
static llist module_edge_freelist = LLIST_INITIALIZER(module_edge_freelist);  // freelist

static llist parsers = LLIST_INITIALIZER(parsers);  // struct module_parser
static exec_builder local_exec_builder;
static exec_render_context local_exec_render;
uint16_t module_system_reconcile_epoch;

//
// static
//

xapi module_edge_alloc(module_edge ** restrict mep)
{
  enter;

  module_edge *me;

  if((me = llist_shift(&module_freelist, typeof(*me), edge.owner)) == 0)
  {
    fatal(xmalloc, &me, sizeof(*me));
  }

  moria_edge_init(&me->edge, &g_graph);

  llist_append(&module_edge_list, me, edge.owner);
  *mep = me;

  finally : coda;
}

static void module_edge_release(module_edge * restrict me)
{
  RUNTIME_ASSERT(me->edge.attrs & EDGE_MODULES);

  llist_delete_node(&me->edge.owner);
  llist_append(&module_edge_freelist, me, edge.owner);
}

static xapi module_edge_disconnect(module_edge * restrict me)
{
  enter;

  fatal(graph_disconnect, &me->edge);
  module_edge_release(me);

  finally : coda;
}

xapi module_alloc(module ** restrict modp)
{
  enter;

  module * mod;

  if((mod = llist_shift(&module_freelist, typeof(*mod), vertex.owner)) == 0)
  {
    fatal(xmalloc, &mod, sizeof(*mod));
    fatal(map_create, &mod->variant_var);
    fatal(map_createx, &mod->variant_envs, 0, (void*)exec_free, 0);
    fatal(value_parser_create, &mod->value_parser);
  }
  else
  {
    fatal(map_recycle, mod->variant_var);
    fatal(map_recycle, mod->variant_envs);
    fatal(value_parser_recycle, mod->value_parser);
  }

  moria_vertex_init(&mod->vertex, &g_graph, VERTEX_MODULE);

  llist_init_node(&mod->scoped_blocks);
  llist_init_node(&mod->rmas_owner);
  llist_init_node(&mod->lln_reconcile);

  llist_append(&module_list, mod, vertex.owner);

  *modp = mod;

  finally : coda;
}

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
    if((v = set_table_get(block->variants, x)) == 0) {
      continue;
    }

    if(map_get(mod->variant_var, MM(v))) {
      continue;
    }

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

struct associate_rules_context
{
  module * mod;
  statement_block * block;
};

/*
 * connect rules in statement block of module used (defines the rules) with the module self (module being loaded)
 *
 * mod_self - module to associate rules to
 * mod_used - module which defines the rules
 * block    - block of rules
 * variants - variants to use with the association - must be part of the owning module
 */
static xapi block_rules_connect(module * mod_self, module * mod_used, statement_block * restrict block, set * restrict variants)
{
  enter;

  rule *r;
  rule_module_edge *rma;

  llist_foreach(&block->rules, r, lln) {
    fatal(rule_module_connect, &rma, mod_self, mod_used, r, variants);
    rule_system_rma_enqueue(rma);
  }

  finally : coda;
}

static xapi rule_remove(rule * restrict r)
{
  enter;

  moria_vertex *v;
  moria_edge *e;
  llist lln, *T;

  v = &r->vertex;

  /* disconnect edges connecting this rule to dirnodes and formula nodes */
  llist_init_node(&lln);
  rbtree_foreach(&v->down, e, rbn_down) {
    llist_append(&lln, e, lln);
  }

  /* note - upward edges, connecting to modules, are disconnected in module_rule_disassociate */
  llist_foreach_safe(&lln, e, lln, T) {
    fatal(graph_disconnect, e);
  }

  rule_release(r);

  finally : coda;
}

/* adds the module to a list */
static xapi module_vertex_visitor(moria_vertex * v, void * ctx, moria_traversal_mode mode, int distance, int * result)
{
  enter;

  llist * mods = ctx;
  module * mod;

  mod = containerof(v, typeof(*mod), vertex);

  llist_append(mods, mod, lln);

  finally : coda;
}

/* adds the edge to a list */
static xapi module_edge_visitor(moria_edge * e, void * ctx, moria_traversal_mode mode, int distance, int * result)
{
  enter;

  llist * edges = ctx;
  llist_append(edges, e, lln);

  finally : coda;
}

static xapi module_parse(module * restrict mod, module_parser * restrict parser, graph_invalidation_context * restrict invalidation, bool * restrict reconciled)
{
  enter;

  xapi exit;
  char trace[4096 << 1];
  char * text = 0;
  size_t text_len;
  fsent * mod_file_n;
  char * mod_file_relpath;
  rule *r;
  statement_block *block;

  mod_file_n = mod->self_node;
  mod_file_relpath = mod->self_node_relpath;

  STATS_INC(mod_file_n->modfile_stats.parsed_try);
  if(fsent_fsenttype_get(mod_file_n) == VERTEX_FSENTTYPE_MODULE) {
    STATS_INC(g_stats.module_parsed_try);
  } else {
    STATS_INC(g_stats.model_parsed_try);
  }

  fatal(snarfats, &text, &text_len, g_params.proj_dirfd, mod_file_relpath);

  if(!text)
  {
    mod_file_n->not_parsed = 0;
  }
  else if((exit = invoke(module_parser_parse, parser, mod, invalidation, text, text_len + 2, mod_file_relpath)))
  {
#if DEBUG || DEVEL || XAPI
    xapi_trace_full(trace, sizeof(trace), 0);
#else
    xapi_trace_pithy(trace, sizeof(trace), 0);
#endif
    xapi_calltree_unwind();

    xlogs(L_WARN, L_NOCATEGORY, trace);

    // release blocks which were parsed but which will not be used
    if(parser->unscoped_block) {
      llist_append(&parser->scoped_blocks, parser->unscoped_block, lln);
      parser->unscoped_block = 0;
    }
    llist_foreach(&parser->scoped_blocks, block, lln) {
      llist_foreach(&block->rules, r, lln) {
        rule_release(r);
      }
      llist_init_node(&block->rules);
    }
    llist_splice_head(&parser->statement_block_freelist, &parser->scoped_blocks);

    mod_file_n->not_parsed = 1;
    *reconciled = 0;
  }
  else if(parser->errlen)
  {
    mod_file_n->not_parsed = 1;
    *reconciled = 0;
  }
  else
  {
    mod_file_n->not_parsed = 0;
  }

  if(!mod_file_n->not_parsed)
  {
    STATS_INC(mod_file_n->modfile_stats.parsed);
    if(fsent_fsenttype_get(mod_file_n) == VERTEX_FSENTTYPE_MODULE) {
      STATS_INC(g_stats.module_parsed);
    } else {
      STATS_INC(g_stats.model_parsed);
    }
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
 * load the module for this node, if it is a directory node with a module.bam file
 */
static xapi module_bootstrap_visitor(moria_vertex * mod_dir_v, void * ctx, moria_traversal_mode mode, int distance, int * result)
{
  enter;

  graph_invalidation_context * invalidation = ctx;
  fsent * mod_dir_n;
  fsent * mod_file_n;
  moria_vertex * mod_file_v;

  // no module.bam file present
  if((mod_file_v = moria_vertex_downw(mod_dir_v, fsent_module_name, fsent_module_name_len)) == 0) {
    if(distance == 0) {
      fail(MODULE_NOMODULE);
    }
    goto XAPI_FINALLY;
  }

  mod_file_n = containerof(mod_file_v, typeof(*mod_file_n), vertex);
  mod_dir_n = containerof(mod_dir_v, typeof(*mod_dir_n), vertex);
  fatal(module_bootstrap, mod_dir_n, mod_file_n, invalidation);

  finally : coda;
}

static xapi rebuild_variant_envs(module * restrict mod, bool * restrict reconciled)
{
  enter;

  int x;
  statement_block *block;
  moria_vertex *v;
  moria_vertex * var_v;
  value * vars = 0;
  fsent * var_n;
  moria_vertex ** ancestry = 0;
  size_t ancestryz = 0;
  size_t ancestryl = 0;

  fatal(map_recycle, mod->variant_var);
  fatal(map_recycle, mod->variant_envs);
  exec_ifree(&mod->novariant_envs);

  /* assemble list of ancestor nodes */
  v = &mod->dir_node->vertex;
  while(v)
  {
    fatal(assure, &ancestry, sizeof(*ancestry), ancestryl + 1, &ancestryz);
    ancestry[ancestryl++] = v;
    v = moria_vertex_up(v);
  }

  /* apply ancestor var.bam files in reverse order */
  for(x = ancestryl - 1; x >= 0; x--)
  {
    if((var_v = moria_vertex_downw(ancestry[x], fsent_var_name, fsent_var_name_len)))
    {
      var_n = containerof(var_v, fsent, vertex);
      fatal(fsent_var_bootstrap, var_n, reconciled);
      if(!*reconciled) {
        goto XAPI_FINALLY;
      }

      fatal(value_merge, mod->value_parser, &vars, var_n->self_var->val, 0);
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

static xapi edge_modules_visitor(moria_edge * e, void * ctx, moria_traversal_mode mode, int distance, int * result)
{
  enter;

  llist *lln = ctx;
  module_edge *nem;

  nem = containerof(e, typeof(*nem), edge);

  if(nem->module_system_reconcile_epoch != module_system_reconcile_epoch) {
    llist_append_node(lln, &e->lln);
  }

  finally : coda;
}

/* modules only, models are not rebuilt */
static xapi module_rebuild(module * restrict mod, graph_invalidation_context * restrict invalidation)
{
  enter;

  statement_block *block;
  llist list;
  module * mod_used;
  module_edge *nem;
  moria_edge *e;

  STATS_INC(mod->stats.reloaded);
  STATS_INC(g_stats.module_reloaded);

  /* remove any un-refreshed USES, REQUIRES, and IMPORTS edges */
  llist_init_node(&list);
  fatal(moria_traverse_vertex_edges
    , &g_graph
    , &mod->vertex
    , edge_modules_visitor
    , 0
    , (moria_traversal_criteria[]) {{
          edge_travel: EDGE_MODULES
        , edge_visit : EDGE_MODULES
        , min_depth : 0
        , max_depth : 0
      }}
    , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE | MORIA_TRAVERSE_DEPTH | MORIA_TRAVERSE_NOFOLLOW
    , &list
  );

  llist_foreach(&list, e, lln) {
    nem = containerof(e, typeof(*nem), edge);

    /* also removed related edges, e.g. //module/imports/foo */
    if(nem->edges[0])
    {
      fatal(fsedge_disconnect, nem->edges[0]);
    }
    if(nem->edges[1])
    {
      fatal(fsedge_disconnect, nem->edges[1]);
    }

    fatal(moria_edge_disconnect, &g_graph, e);
  }

  /* visit this module and its USES transitively
   * NOTE: the mod-rule association cannot be made while the mod_rules_uses_visitor traversal
   * is underway, because it modifies the edge on the module directory node
   */
  llist_init_node(&list);
  fatal(moria_traverse_vertices
    , &g_graph
    , &mod->vertex
    , module_vertex_visitor
    , 0
    , (moria_traversal_criteria[]) {{
          edge_travel : EDGE_USES
      }}
    , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_POST | MORIA_TRAVERSE_NOFOLLOW
    , &list
  );

  // enqueue rules
  llist_foreach(&list, mod_used, lln) {
    /* for rules in the unscoped block, use variants from the unscoped block of the module being loaded */
    if(mod_used->unscoped_block) {
      fatal(block_rules_connect, mod, mod_used, mod_used->unscoped_block, mod->unscoped_block->variants);
    }

    /* however for scoped blocks, use variants from the block in the module which defines the rules */
    llist_foreach(&mod_used->scoped_blocks, block, lln) {
      fatal(block_rules_connect, mod, mod_used, block, block->variants);
    }
  }

  finally : coda;
}

static xapi module_rules_dismantle(module * restrict mod, module_parser * restrict parser, graph_invalidation_context * restrict invalidation)
{
  enter;

  llist list;
  moria_edge *e;
  rule_module_edge *rma;
  llist *cursor;
  statement_block *block;
  rule *r;
  enum vertex_fsenttype enttype;

  enttype = fsent_fsenttype_get(mod->self_node);
  if(enttype == VERTEX_FSENTTYPE_MODULE) {
    /*
     * disconnect rmas connected to this module, including those for rules defined by the module,
     * and for rules defined by models used by the module
     */
    llist_init_node(&list);
    fatal(moria_traverse_vertex_edges
      , &g_graph
      , &mod->vertex
      , module_edge_visitor
      , 0
      , (moria_traversal_criteria[]) {{
            edge_travel: EDGE_MOD_RULE
          , edge_visit : EDGE_MOD_RULE
          , min_depth : 0
          , max_depth : 0
        }}
      , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE | MORIA_TRAVERSE_DEPTH | MORIA_TRAVERSE_NOFOLLOW
      , &list
    );

    llist_foreach_safe(&list, e, lln, cursor) {
      rma = containerof(e, typeof(*rma), edge);
      fatal(rule_module_disconnect, rma, invalidation);
    }
  }
  else
  {
    RUNTIME_ASSERT(enttype == VERTEX_FSENTTYPE_MODEL);

    /* disconnected rmas owned by this model */
    llist_foreach_safe(&mod->rmas_owner, rma, lln_rmas_owner, cursor) {
      fatal(rule_module_disconnect, rma, invalidation);
    }
  }

  /* release rules from the statement blocks */
  llist_init_node(&list);
  if(mod->unscoped_block) {
    llist_append(&list, mod->unscoped_block, lln);
  }
  llist_splice_head(&list, &mod->scoped_blocks);

  llist_foreach(&list, block, lln) {
    llist_foreach(&block->rules, r, lln) {
      fatal(rule_remove, r);
    }

    llist_init_node(&block->rules);
  }

  llist_splice_head(&parser->statement_block_freelist, &list);

  finally : coda;
}

/* remove a module from the graph */
static xapi module_excise(module * restrict mod, graph_invalidation_context * restrict invalidation)
{
  enter;

  llist list;
  moria_edge *e;
  fsent *mod_dir_n;
  moria_vertex *v;

  v = &mod->vertex;

  /* teardown relations to other modules */
  llist_init_node(&list);
  fatal(moria_traverse_vertex_edges
    , &g_graph
    , v
    , module_edge_visitor
    , 0
    , (moria_traversal_criteria[]) {{
          edge_travel : EDGE_IMPORTS
        , edge_visit : EDGE_IMPORTS
      }}
    , MORIA_TRAVERSE_UP | MORIA_TRAVERSE_POST | MORIA_TRAVERSE_DEPTH | MORIA_TRAVERSE_NOFOLLOW
    , &list
  );
  fatal(moria_traverse_vertex_edges
    , &g_graph
    , v
    , module_edge_visitor
    , 0
    , (moria_traversal_criteria[]) {{
          edge_travel : EDGE_USES
        , edge_visit : EDGE_USES
      }}
    , MORIA_TRAVERSE_UP | MORIA_TRAVERSE_POST | MORIA_TRAVERSE_DEPTH | MORIA_TRAVERSE_NOFOLLOW
    , &list
  );
  fatal(moria_traverse_vertex_edges
    , &g_graph
    , v
    , module_edge_visitor
    , 0
    , (moria_traversal_criteria[]) {{
          edge_travel : EDGE_REQUIRES
        , edge_visit : EDGE_REQUIRES
      }}
    , MORIA_TRAVERSE_UP | MORIA_TRAVERSE_POST | MORIA_TRAVERSE_DEPTH | MORIA_TRAVERSE_NOFOLLOW
    , &list
  );
  llist_foreach(&list, e, lln) {
    mod_dir_n = containerof(e->A, fsent, vertex);
    fatal(fsent_invalidate, mod_dir_n, invalidation);
    fatal(module_edge_disconnect, containerof(e, module_edge, edge));
  }

  llist_init_node(&list);
  fatal(moria_traverse_vertex_edges
    , &g_graph
    , v
    , module_edge_visitor
    , 0
    , (moria_traversal_criteria[]) {{
          edge_travel : EDGE_IMPORTS
        , edge_visit : EDGE_IMPORTS
      }}
    , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_POST | MORIA_TRAVERSE_DEPTH | MORIA_TRAVERSE_NOFOLLOW
    , &list
  );
  fatal(moria_traverse_vertex_edges
    , &g_graph
    , v
    , module_edge_visitor
    , 0
    , (moria_traversal_criteria[]) {{
          edge_travel : EDGE_USES
        , edge_visit : EDGE_USES
      }}
    , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_POST | MORIA_TRAVERSE_DEPTH | MORIA_TRAVERSE_NOFOLLOW
    , &list
  );
  fatal(moria_traverse_vertex_edges
    , &g_graph
    , v
    , module_edge_visitor
    , 0
    , (moria_traversal_criteria[]) {{
          edge_travel : EDGE_REQUIRES
        , edge_visit : EDGE_REQUIRES
      }}
    , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_POST | MORIA_TRAVERSE_DEPTH | MORIA_TRAVERSE_NOFOLLOW
    , &list
  );

  llist_foreach(&list, e, lln) {
    mod_dir_n = containerof(e->B, fsent, vertex);
    fatal(fsent_invalidate, mod_dir_n, invalidation);
    fatal(module_edge_disconnect, containerof(e, module_edge, edge));
  }

  /* edge between the module vertex and the module directory fsent */
  llist_init_node(&list);
  fatal(moria_traverse_vertex_edges
    , &g_graph
    , v
    , module_edge_visitor
    , 0
    , (moria_traversal_criteria[]) {{
        edge_travel : EDGE_MOD_DIR
      }}
    , MORIA_TRAVERSE_UP | MORIA_TRAVERSE_POST | MORIA_TRAVERSE_DEPTH | MORIA_TRAVERSE_NOFOLLOW
    , &list
  );

  llist_foreach(&list, e, lln) {
    fatal(graph_disconnect, e);
    graph_edge_release(e);
  }

  /* teardown the modules shadow tree */
  e = moria_edge_between(&g_shadow_modules->vertex, &mod->shadow->vertex);
  fatal(fsedge_disintegrate, containerof(e, fsedge, edge), invalidation);

  /* arrange for directory nodes to be re-parented */
  fsent_kind_set(mod->dir_node, VERTEX_DIR);
  fsent_module_epoch++;

  finally : coda;
}

static xapi module_reparse(module * restrict mod, module_parser * restrict parser, graph_invalidation_context * restrict invalidation, bool * restrict reconciled)
{
  enter;

  /* re-parse the module.bam file */
  fatal(module_parse, mod, parser, invalidation, reconciled);

  /* parse not successful */
  if(!*reconciled) {
    goto XAPI_FINALIZE;
  }

  fatal(module_rules_dismantle, mod, parser, invalidation);

  /* replace the guts with the results from parsing */
  mod->unscoped_block = parser->unscoped_block;
  parser->unscoped_block = 0;
  llist_splice_head(&mod->scoped_blocks, &parser->scoped_blocks);
  mod->var_value = parser->var_value;
  mod->var_merge_overwrite = parser->var_merge_overwrite;

  // rebuild variant envs from var.bams
  fatal(rebuild_variant_envs, mod, reconciled);

  finally : coda;
}

xapi module_initialize(fsent * restrict mod_dir_n, fsent * restrict mod_file_n, graph_invalidation_context * restrict invalidation)
{
  enter;

  module *mod = 0;
  moria_connect_context connect_ctx;
  int __attribute__((unused)) r;
  moria_edge *e;

  fatal(module_alloc, &mod);

  /* module paths needed during loading */
  fsent_absolute_path_znload(mod->dir_node_abspath, sizeof(mod->dir_node_abspath), mod_dir_n);
  fsent_project_relative_path_znload(mod->self_node_relpath, sizeof(mod->self_node_relpath), mod_file_n);

  mod_dir_n->mod = mod;
  fsent_kind_set(mod_dir_n, VERTEX_MODULE_DIR);

  mod->dir_node = mod_dir_n;
  mod->self_node = mod_file_n;
  mod_file_n->self_mod = mod;

  if(memncmp(mod_file_n->name.name, mod_file_n->name.namel, fsent_module_name, fsent_module_name_len) == 0)
  {
    fsent_kind_set(mod_file_n, VERTEX_MODULE_FILE);
  }
  else if(memncmp(mod_file_n->name.name, mod_file_n->name.namel, fsent_model_name, fsent_model_name_len) == 0)
  {
    fsent_kind_set(mod_file_n, VERTEX_MODEL_FILE);
  }
  else
  {
    RUNTIME_ABORT();
  }

  /* connect the module vertex and the module directory fsent */
  r = moria_preconnect(&connect_ctx, &g_graph, &mod_dir_n->vertex, &mod->vertex, EDGE_MOD_DIR, 0);
  RUNTIME_ASSERT(r == MORIA_NOEDGE);
  fatal(graph_edge_alloc, &e);
  fatal(graph_connect, &connect_ctx, &mod_dir_n->vertex, &mod->vertex, e, EDGE_MOD_DIR);

  finally : coda;
}

xapi module_bootstrap(fsent * restrict mod_dir_n, fsent * restrict mod_file_n, graph_invalidation_context * restrict invalidation)
{
  enter;

  uint64_t u64;
  char mod_dir_realpath[512];
  uint16_t mod_dir_realpathl;
  char id[32];
  module *mod;

  if(!mod_dir_n->not_loaded)
    goto XAPI_FINALLY;

  fatal(module_initialize, mod_dir_n, mod_file_n, invalidation);
  mod_dir_n->not_loaded = 0;

  // module-dir realpath hash
  mod = mod_dir_n->mod;
  fatal(xrealpaths, 0, mod_dir_realpath, mod->dir_node_abspath);
  mod_dir_realpathl = strlen(mod_dir_realpath);
  u64 = hash64(0, mod_dir_realpath, mod_dir_realpathl);
  snprintf(id, sizeof(id), "%016"PRIx64, u64);
  memcpy(mod->id, id, 16);

  mod->vertex.label = mod->id;
  mod->vertex.label_len = 16;

  // rdonly file descriptor to the module directory for formula execution
  fatal(xopens, &mod->dirnode_fd, O_RDONLY, mod->dir_node_abspath);

  // build out the modules shadow node
  fatal(shadow_module_init, mod, invalidation);

  /* arrange to rebuild */
  fsent_invalid_set(mod->dir_node);

  /* arrange for directory nodes to be re-parented */
  fsent_module_epoch++;

  finally : coda;
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

static xapi module_dispose(module * restrict mod, module_parser * restrict parser)
{
  enter;

  /* return recovered statement blocks to the freelist */
  if(mod->unscoped_block) {
    llist_append(&parser->statement_block_freelist, mod->unscoped_block, lln);
  }

  llist_splice_head(&parser->statement_block_freelist, &mod->scoped_blocks);

  exec_free(mod->novariant_envs);
  fatal(map_xfree, mod->variant_var);
  fatal(map_xfree, mod->variant_envs);
  fatal(value_parser_xfree, mod->value_parser);

  llist_delete_node(&mod->vertex.owner);
  llist_append(&module_freelist, mod, vertex.owner);

  finally : coda;
}

xapi module_xrelease(module * restrict mod, module_parser * restrict parser)
{
  enter;

  moria_vertex __attribute__((unused)) *v;

  /* should be fully excised from the graph previously */
  v = &mod->vertex;
  RUNTIME_ASSERT(v->up_identity == 0);
  RUNTIME_ASSERT(rbtree_empty(&v->up));
  RUNTIME_ASSERT(rbtree_empty(&v->down));

  fatal(module_dispose, mod, parser);

  finally : coda;
}

//
// public
//

xapi module_system_bootstrap()
{
  enter;

  fsent * mod_file_n;
  graph_invalidation_context invalidation = { 0 };
  moria_connect_context ctx;
  fsedge *fse;
  int __attribute__((unused)) r;

  /* bootstrap just the project module */
  fatal(graph_invalidation_begin, &invalidation);
  fatal(fsent_create, &mod_file_n, VERTEX_MODULE_FILE, VERTEX_OK, fsent_module_name, fsent_module_name_len);
  fsent_protect_set(mod_file_n);

  r = moria_preconnect(&ctx, &g_graph, &g_project_root->vertex, &mod_file_n->vertex, EDGE_FSTREE, 0);
  RUNTIME_ASSERT(r == MORIA_NOEDGE);
  fatal(fsedge_alloc, &fse, &g_graph);
  fatal(graph_connect, &ctx, &g_project_root->vertex, &mod_file_n->vertex, &fse->edge, EDGE_FSTREE);

  fatal(module_bootstrap, g_project_root, mod_file_n, &invalidation);

  g_project_self = mod_file_n;

finally:
  graph_invalidation_end(&invalidation);
coda;
}

xapi module_system_reconcile(graph_invalidation_context * restrict invalidation, bool * restrict reconciled)
{
  enter;

  module *mod;
  module_parser * parser = 0;
  llist *T;

  llist rebuild = LLIST_INITIALIZER(rebuild);

  fatal(graph_invalidation_begin, invalidation);
  module_system_reconcile_epoch++;

  /* bootstrap modules under the project module */
  fatal(moria_traverse_vertices
    , &g_graph
    , &g_project_root->vertex
    , module_bootstrap_visitor
    , 0
    , (moria_traversal_criteria[]) {{
          edge_travel: EDGE_FSTREE
        , edge_visit : EDGE_FSTREE
        , vertex_travel : VERTEX_FILETYPE_DIR
        , vertex_visit : VERTEX_FILETYPE_DIR
      }}
    , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE
    , invalidation
  );

  if((parser = llist_shift(&parsers, typeof(*parser), lln)) == 0) {
    fatal(module_parser_create, &parser);
  }

  /* destroy modules whose module.bam file has been deleted */
  llist_foreach_safe(&module_list, mod, vertex.owner, T) {
    if(fsent_exists_get(mod->self_node)) {
      continue;
    }

    fatal(module_rules_dismantle, mod, parser, invalidation);

    /* excise from the graph */
    fatal(module_excise, mod, invalidation);

    /* delete, move to freelist */
    fatal(module_xrelease, mod, parser);
  }

  /* reload modules which have been invalidated */
  /* can modules become invalidated while traversing this list ?? */
  llist_foreach(&module_list, mod, vertex.owner) {
    if(!fsent_invalid_get(mod->dir_node)) {
      continue;
    }

    fatal(module_reparse, mod, parser, invalidation, reconciled);
    if(!*reconciled) {
      goto XAPI_FINALIZE;
    }

    fatal(fsent_ok, mod->self_node);

    /* models are parsed only */
    if(fsent_fsenttype_get(mod->self_node) == VERTEX_FSENTTYPE_MODEL) {
      fatal(fsent_ok, mod->dir_node);
      continue;
    }

    llist_push(&rebuild, mod, lln_reconcile);
  }

  /* before rebuilding a given module, all of its imports/uses/requires must have been parsed */
  while((mod = llist_shift(&rebuild, typeof(*mod), lln_reconcile))) {
    fatal(module_rebuild, mod, invalidation);
    fatal(fsent_ok, mod->dir_node);
  }

finally:
  if(parser) {
    llist_append(&parsers, parser, lln);
  }
  graph_invalidation_end(invalidation);
coda;
}

xapi module_setup()
{
  enter;

  module_parser *parser;

  fatal(exec_builder_xinit, &local_exec_builder);
  fatal(exec_render_context_xinit, &local_exec_render);

  fatal(module_parser_create, &parser);
  llist_append(&parsers, parser, lln);

  finally : coda;
}

xapi module_cleanup()
{
  enter;

  module_parser *parser;
  module *mod;
  module_edge *me;
  llist *T;

  fatal(exec_builder_xdestroy, &local_exec_builder);
  fatal(exec_render_context_xdestroy, &local_exec_render);

  parser = llist_first(&parsers, typeof(*parser), lln);

  llist_foreach_safe(&module_list, mod, vertex.owner, T) {
    fatal(module_dispose, mod, parser);
  }

  llist_foreach_safe(&module_freelist, mod, vertex.owner, T) {
    wfree(mod);
  }

  llist_foreach_safe(&module_edge_list, me, edge.owner, T) {
    module_edge_release(me);
  }

  llist_foreach_safe(&module_edge_freelist, me, edge.owner, T) {
    wfree(me);
  }

  llist_foreach_safe(&parsers, parser, lln, T) {
    fatal(module_parser_xfree, parser);
  }

  finally : coda;
}

xapi module_collate_stats(void *dst, size_t sz, module *mod, bool reset, size_t *zp)
{
  enter;

  size_t z;
  fab_module_stats *stats;
  fab_module_stats lstats;
  descriptor_field *field;
  int x;
//  uint16_t len;
//  size_t len_off;
  uint32_t count;
  statement_block *block;

  stats = &mod->stats;
  if(reset)
  {
    memcpy(&lstats, &stats, sizeof(lstats));
    memset(stats, 0, sizeof(*stats));
    stats = &lstats;
  }

  z = 0;
  z += marshal_u32(dst + z, sz - z, descriptor_fab_module_stats.id);

  /* uses */
  count = 0;
  fatal(moria_traverse_vertex_edges, &g_graph, &mod->vertex, moria_edge_count, 0
    , (moria_traversal_criteria[]) {{
          edge_travel : EDGE_USES
        , edge_visit : EDGE_USES
        , min_depth : 0
        , max_depth : 0
      }}
    , MORIA_TRAVERSE_UP | MORIA_TRAVERSE_PRE | MORIA_TRAVERSE_DEPTH
    , &count
  );
  z += marshal_u32(dst + z, sz - z, count);

  count = 0;
  fatal(moria_traverse_vertex_edges, &g_graph, &mod->vertex, moria_edge_count, 0
    , (moria_traversal_criteria[]) {{
          edge_travel : EDGE_USES
        , edge_visit : EDGE_USES
        , min_depth : 0
        , max_depth : 0
      }}
    , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE | MORIA_TRAVERSE_DEPTH
    , &count
  );
  z += marshal_u32(dst + z, sz - z, count);

  /* imports */
  count = 0;
  fatal(moria_traverse_vertex_edges, &g_graph, &mod->vertex, moria_edge_count, 0
    , (moria_traversal_criteria[]) {{
          edge_travel : EDGE_IMPORTS
        , edge_visit : EDGE_IMPORTS
        , min_depth : 0
        , max_depth : 0
      }}
    , MORIA_TRAVERSE_UP | MORIA_TRAVERSE_PRE | MORIA_TRAVERSE_DEPTH
    , &count
  );
  z += marshal_u32(dst + z, sz - z, count);

  count = 0;
  fatal(moria_traverse_vertex_edges, &g_graph, &mod->vertex, moria_edge_count, 0
    , (moria_traversal_criteria[]) {{
          edge_travel : EDGE_IMPORTS
        , edge_visit : EDGE_IMPORTS
        , min_depth : 0
        , max_depth : 0
      }}
    , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE | MORIA_TRAVERSE_DEPTH
    , &count
  );
  z += marshal_u32(dst + z, sz - z, count);

  /* requires */
  count = 0;
  fatal(moria_traverse_vertex_edges, &g_graph, &mod->vertex, moria_edge_count, 0
    , (moria_traversal_criteria[]) {{
          edge_travel : EDGE_REQUIRES
        , edge_visit : EDGE_REQUIRES
        , min_depth : 0
        , max_depth : 0
      }}
    , MORIA_TRAVERSE_UP | MORIA_TRAVERSE_PRE | MORIA_TRAVERSE_DEPTH
    , &count
  );
  z += marshal_u32(dst + z, sz - z, count);

  count = 0;
  fatal(moria_traverse_vertex_edges, &g_graph, &mod->vertex, moria_edge_count, 0
    , (moria_traversal_criteria[]) {{
          edge_travel : EDGE_REQUIRES
        , edge_visit : EDGE_REQUIRES
        , min_depth : 0
        , max_depth : 0
      }}
    , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE | MORIA_TRAVERSE_DEPTH
    , &count
  );
  z += marshal_u32(dst + z, sz - z, count);

  /* module tree */
  count = 0;
  fatal(moria_traverse_vertices, &g_graph, &mod->dir_node->vertex, moria_vertex_count_once, 0
    , (moria_traversal_criteria[]) {{
          edge_travel : EDGE_FSTREE
        , vertex_visit : VERTEX_MODULE_DIR
        , min_depth : 1
        , max_depth : UINT16_MAX
      }}
    , MORIA_TRAVERSE_UP | MORIA_TRAVERSE_PRE | MORIA_TRAVERSE_DEPTH
    , &count
  );
  z += marshal_u32(dst + z, sz - z, count);

  count = 0;
  fatal(moria_traverse_vertices, &g_graph, &mod->dir_node->vertex, moria_vertex_count_once, 0
    , (moria_traversal_criteria[]) {{
          edge_travel : EDGE_FSTREE
        , vertex_visit : VERTEX_MODULE_DIR
        , min_depth : 1
        , max_depth : UINT16_MAX
      }}
    , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE | MORIA_TRAVERSE_DEPTH
    , &count
  );
  z += marshal_u32(dst + z, sz - z, count);

  /* variants */
  z += marshal_u16(dst + z, sz - z, mod->variant_var->size);

  /* rules */
  count = 0;
  if(mod->unscoped_block) {
    count += llist_count(&mod->unscoped_block->rules);
  }
  llist_foreach(&mod->scoped_blocks, block, lln) {
    count += llist_count(&block->rules);
  }
  z += marshal_u16(dst + z, sz - z, count);

  /* rules-effective */
  count = 0;
  fatal(moria_traverse_vertex_edges, &g_graph, &mod->vertex, moria_edge_count, 0
    , (moria_traversal_criteria[]) {{
          edge_travel : EDGE_MOD_RULE
        , edge_visit : EDGE_MOD_RULE
        , min_depth : 0
        , max_depth : 0
      }}
    , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE | MORIA_TRAVERSE_DEPTH, &count
  );
  z += marshal_u32(dst + z, sz - z, count);

  /* rules-used-by */
  z += marshal_u32(dst + z, sz - z, llist_count(&mod->rmas_owner));

  /* event counters */
  for(x = 12; x < descriptor_fab_module_stats.members_len; x++)
  {
    field = descriptor_fab_module_stats.members[x];

    if(field->size == 8) {
      z += marshal_u64(dst + z, sz - z, stats->u64[field->offset / 8]);
    } else if(field->size == 4) {
      z += marshal_u32(dst + z, sz - z, stats->u32[field->offset / 4]);
    } else if(field->size == 2) {
      z += marshal_u16(dst + z, sz - z, stats->u16[field->offset / 2]);
    } else if(field->size == 1) {
      z += marshal_u16(dst + z, sz - z, stats->u8[field->offset / 1]);
    } else {
      RUNTIME_ABORT();
    }
  }

  *zp += z;

  finally : coda;
}

xapi module_file_collate_stats(void *dst, size_t sz, fsent *mod_file, bool reset, size_t *zp)
{
  enter;

  size_t z;
  fab_module_file_stats *stats;
  fab_module_file_stats lstats;
  descriptor_field *field;
  int x;

  stats = &mod_file->modfile_stats;
  if(reset)
  {
    memcpy(&lstats, &stats, sizeof(lstats));
    memset(stats, 0, sizeof(*stats));
    stats = &lstats;
  }

  z = 0;
  z += marshal_u32(dst + z, sz - z, descriptor_fab_module_file_stats.id);

  /* event counters */
  for(x = 0; x < descriptor_fab_module_file_stats.members_len; x++)
  {
    field = descriptor_fab_module_file_stats.members[x];

    if(field->size == 8) {
      z += marshal_u64(dst + z, sz - z, stats->u64[field->offset / 8]);
    } else if(field->size == 4) {
      z += marshal_u32(dst + z, sz - z, stats->u32[field->offset / 4]);
    } else if(field->size == 2) {
      z += marshal_u16(dst + z, sz - z, stats->u16[field->offset / 2]);
    } else if(field->size == 1) {
      z += marshal_u16(dst + z, sz - z, stats->u8[field->offset / 1]);
    } else {
      RUNTIME_ABORT();
    }
  }

  *zp += z;

  finally : coda;
}

xapi module_resolve_require(module * restrict A, module * restrict B, graph_invalidation_context * restrict invalidation)
{
  enter;

  fsedge *ne;
  moria_edge *e;
  module_edge *nem;
  moria_connect_context ctx;
  int __attribute__((unused)) r;
  char path[512];

  // attach to //module/requires/
  fatal(shadow_graft_requires, A, B->dir_node, B->dir_node->name.name, B->dir_node->name.namel, &ne, invalidation);

  r = moria_preconnect(&ctx, &g_graph, &A->vertex, &B->vertex, EDGE_REQUIRES, &e);
  if(r == MORIA_NOEDGE)
  {
    fatal(module_edge_alloc, &nem);
    fatal(graph_connect, &ctx, &A->vertex, &B->vertex, &nem->edge, EDGE_REQUIRES);
    nem->edges[0] = ne;
  }
  else
  {
    RUNTIME_ASSERT(r == MORIA_HASEDGE);
    nem = containerof(e, typeof(*nem), edge);
  }

  /* refresh */
  nem->module_system_reconcile_epoch = module_system_reconcile_epoch;

  if(log_would(L_MODULE))
  {
    fsent_path_znload(path, sizeof(path), B->dir_node);
    logf(L_MODULE, " %s : requires %s -> %s", A->dir_node->name.name, B->dir_node->name.name, path);
  }

  finally : coda;
}

xapi module_resolve_use(module * restrict A, module * restrict B, const char * restrict refname, uint16_t refname_len, bool scoped, graph_invalidation_context * restrict invalidation)
{
  enter;

  fsedge *ue;
  fsedge *se;
  module_edge *nem;
  moria_edge *e;
  moria_connect_context ctx;
  int __attribute__((unused)) r;
  char path[512];

  ue = 0;
  se = 0;
  if(scoped)
  {
    // attach to //module/uses/
    fatal(shadow_graft_uses, A, B->dir_node, refname, refname_len, &ue, invalidation);
  }

  r = moria_preconnect(&ctx, &g_graph, &A->vertex, &B->vertex, EDGE_USES, &e);
  if(r == MORIA_NOEDGE)
  {
    fatal(module_edge_alloc, &nem);
    fatal(graph_connect, &ctx, &A->vertex, &B->vertex, &nem->edge, EDGE_USES);
    nem->edges[0] = se;
    nem->edges[0] = ue;
  }
  else
  {
    RUNTIME_ASSERT(r == MORIA_HASEDGE);
    nem = containerof(e, typeof(*nem), edge);
  }

  /* refresh */
  nem->module_system_reconcile_epoch = module_system_reconcile_epoch;

  if(log_would(L_MODULE))
  {
    fsent_path_znload(path, sizeof(path), B->dir_node);
    logf(L_MODULE, " %s : uses %.*s -> %s", A->dir_node->name.name, (int)refname_len, refname, path);
  }

  finally : coda;
}

xapi module_resolve_import(module * restrict A, fsent * restrict B, const char * restrict refname, uint16_t refname_len, bool scoped, graph_invalidation_context * restrict invalidation)
{
  enter;

  char path[512];
  fsedge *ie;
  fsedge *se;
  module_edge *nem;
  moria_edge *e;
  moria_connect_context ctx;
  int __attribute__((unused)) r;

  ie = 0;
  se = 0;
  if(scoped)
  {
    // attach to //module/imports/
    fatal(shadow_graft_imports, A, B, refname, refname_len, &ie, invalidation);
  }

  r = moria_preconnect(&ctx, &g_graph, &A->vertex, &B->vertex, EDGE_IMPORTS, &e);
  if(r == MORIA_NOEDGE)
  {
    fatal(module_edge_alloc, &nem);
    fatal(graph_connect, &ctx, &A->vertex, &B->vertex, &nem->edge, EDGE_IMPORTS);
    nem->edges[0] = se;
    nem->edges[1] = ie;
  }
  else
  {
    RUNTIME_ASSERT(r == MORIA_HASEDGE);
    nem = containerof(e, typeof(*nem), edge);
  }

  /* refresh */
  nem->module_system_reconcile_epoch = module_system_reconcile_epoch;

  if(log_would(L_MODULE))
  {
    fsent_path_znload(path, sizeof(path), B);
    logf(L_MODULE, " %s : imports %.*s -> %s", A->dir_node->name.name, (int)refname_len, refname, path);
  }

  finally : coda;
}
