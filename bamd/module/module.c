/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <string.h>
#include <inttypes.h>
#include <errno.h>

#include "types.h"

#include "moria/edge.h"
#include "moria/foreach.h"
#include "bacon/builder.h"
#include "bacon/merge.h"
#include "bacon/parser.h"
#include "bacon/writer.h"
#include "bacon/coerce.h"
#include "bacon/eval.h"
#include "bacon/expr.h"
#include "xlinux/xstring.h"
#include "xlinux/xstdlib.h"
#include "narrator/fixed.h"

#include "module.h"
#include "module/probes.h"
#include "exec.h"
#include "exec_builder.h"
#include "exec_render.h"
#include "fsedge.h"
#include "fsent.h"
#include "vertex.h"
#include "ruleset_parser.h"
#include "pattern_parser.internal.h"
#include "params.h"
#include "pattern.h"
#include "resolve.h"
#include "generate.h"
#include "rule.h"
#include "rma.h"
#include "filesystem.h"
#include "module_edge.h"
#include "dir_rma_edge.h"
#include "rule_system.h"
#include "shadow.h"
#include "stats.h"
#include "varset.h"
#include "detail.h"
#include "channel.h"
#include "system.h"
#include "events/events.h"
#include "block.h"
#include "import.h"
#include "rule/probes.h"
#include "ruleset.h"
#include "reconcile.h"
#include "special_edge.h"
#include "edge.h"
#include "edges.h"
#include "vertices.h"
#include "vertex/probes.h"
#include "namespace.h"
#include "identity.h"
#include "sysvar.h"
#include "config.h"
#include "disintegrate.h"

#include "common/assure.h"
#include "common/snarf.h"
#include "marshal.h"
#include "zcat.h"

moria_vertex *g_module = (moria_vertex[]) {{ }};

llist module_list = LLIST_INITIALIZER(module_list);                 // active modules
static llist module_freelist = LLIST_INITIALIZER(module_freelist);  // freelist

static exec_builder local_exec_builder;
static exec_render_context local_exec_render;
static ruleset_parser *parser;

//
// static
//

/* find special named files in the ancestry of the base dir */
static void locate_ascending(
    vertices * restrict list
  , moria_vertex * restrict base
  , const ruleset * restrict build_mod
  , const ruleset * restrict anchor_mod
  , int kind
)
{
  moria_vertex *v;
  moria_vertex *child;
  ruleset_special_name *special_name;
  const llist * restrict special_names;
  const config_specials_entry *cse;
  int x;
  const rbtree *rbt;

  for(x = 0; x < 2; x++)
  {
    special_names = 0;
    if(x == 0 && kind == 1) {
      if(build_mod) {
        special_names = &build_mod->varsets_names;
      }
    } else if(kind == 1) {
      if(anchor_mod) {
        special_names = &anchor_mod->varsets_names;
      }
    } else if(x == 0) {
      if(build_mod) {
        special_names = &build_mod->rulesets_names;
      }
    } else {
      if(anchor_mod) {
        special_names = &anchor_mod->rulesets_names;
      }
    }

    if(!special_names) {
      continue;
    }

    llist_foreach(special_names, special_name, lln) {
      for(v = base; v != &g_root->vertex; v = moria_vertex_up(v)) {
        if(!(child = moria_vertex_downw(v, special_name->name, special_name->len))) {
          continue;
        }
        if(!vertex_exists_get(child)) {
          continue;
        }

        vertices_push(list, child);
      }
    }
  }

  /* global modules config */
  if(kind == 1) {
    rbt = modules_varsets;
  } else {
    rbt = modules_rulesets;
  }
  rbtree_foreach(rbt, cse, rbn) {
    for(v = base; v != &g_root->vertex; v = moria_vertex_up(v)) {
      if(!(child = moria_vertex_downw(v, cse->name->s, cse->name->len))) {
        continue;
      }
      if(!vertex_exists_get(child)) {
        continue;
      }

      vertices_push(list, child);
    }
  }
}

void module_bootstrap_anchor_visitor(
    moria_vertex *build_dir_v
  , void *_ctx
  , moria_traversal_mode mode
  , int distance
  , moria_traverse_result * restrict result
)
{
  moria_vertex *build_mod_v;
  moria_vertex *mod_v;
  module *mod;
  struct module_bootstrap_context *ctx;

  if((build_mod_v = moria_vertex_downw(build_dir_v, MMS(FSENT_NAME_MODULE))) == 0) {
    return;
  }

  if((mod_v = edge_follow_up(build_dir_v, EDGE_BUILD_DIR_MOD)) == 0) {
    return;
  }
  mod = containerof(mod_v, typeof(*mod), vertex);

  if(mod->anchor_dir == mod->build_dir || !mod->disjoint) {
    return;
  }

  ctx = _ctx;
  ctx->mod = mod;
  moria_traverse_vertices(
      &g_graph
    , &mod->anchor_dir->vertex
    , module_bootstrap_anchored_visitor
    , 0
    , (moria_traversal_criteria[]) {{
          edge_travel: EDGE_FS
        , edge_visit : EDGE_FS
        , vertex_travel : VERTEX_FILETYPE_DIR
        , vertex_visit : VERTEX_FILETYPE_DIR
        , min_depth : 1
      }}
    , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE
    , ctx
  );
}

void module_bootstrap_visitor(
    moria_vertex *build_dir_v
  , void *_ctx
  , moria_traversal_mode mode
  , int distance
  , moria_traverse_result * restrict result
)
{
  fsent *build_dir;
  //fsent *build_mod;
  moria_vertex *build_mod_v;
  graph_invalidation_context *invalidation;
  struct module_bootstrap_context *ctx;

  ctx = _ctx;
  invalidation = ctx->invalidation;
  if((build_mod_v = moria_vertex_downw(build_dir_v, MMS(FSENT_NAME_MODULE))) == 0) {
    /* no module file in the directory */
    return;
  }

  //build_mod = containerof(build_mod_v, typeof(*build_mod), vertex);
  build_dir = containerof(build_dir_v, typeof(*build_dir), vertex);

  /* build dir == anchor dir */
  module_bootstrap(build_dir, build_dir, invalidation);
}

void module_bootstrap_anchored_visitor(
    moria_vertex *anchor_dir_v
  , void *arg
  , moria_traversal_mode mode
  , int distance
  , moria_traverse_result * restrict result
)
{
  fsent *build_dir;
  //fsent *build_mod;
  fsent *anchor_dir;
  moria_vertex *build_mod_v;
  struct module_bootstrap_context *ctx;

  ctx = arg;
  anchor_dir = containerof(anchor_dir_v, typeof(*anchor_dir), vertex);
  if((build_mod_v = moria_vertex_downw(&anchor_dir->vertex, MMS(FSENT_NAME_MODULE))) == 0) {
    /* there is no module file in this directory */
    return;
  }

  /* translate to a build dir that is relative to the module being re-created */
  build_dir = fsent_translate(anchor_dir, ctx->mod->anchor_dir, ctx->mod->build_dir, ctx->invalidation);
  //build_mod = containerof(build_mod_v, typeof(*build_mod), vertex);

  if(vertex_type_get(&build_dir->vertex) == VERTEX_FILE && vertex_state_get(&build_dir->vertex) == VERTEX_UNCREATED) {
    vertex_type_set(&build_dir->vertex, VERTEX_DIR);
  }

  /* build dir != anchor dir */
  module_bootstrap(build_dir, anchor_dir, ctx->invalidation);
}

static __attribute__((nonnull))
void anchor_unresolved_error(module * restrict mod, lookup_pattern * restrict anchor)
{
  narrator_fixed fixed;
  bacon_writer writer, *builder_writer;
  events_context ectx;
  bamipc_message *msg;

  msg = system_error_event(&ectx);
  narrator_fixed_init(&fixed, msg->text, msg->size);
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, &fixed.base);

  { MAPPING
    STRING("unresolved anchor");
    { SET
      { MAPPING STRING("build")
        fsent_absolute_path_describe(mod->build_dir, builder_writer);
      }
      { MAPPING STRING("location")
        LOADF("%s [%d,%d - %d,%d]", anchor->pattern.loc.fname, anchor->pattern.loc.f_lin + 1, anchor->pattern.loc.f_col + 1, anchor->pattern.loc.l_lin + 1, anchor->pattern.loc.l_col + 1);
      }
      { MAPPING STRING("pattern")
        bacon_writer_push_bytes(builder_writer);
        pattern_say(&anchor->pattern, builder_writer->N);
        bacon_writer_pop_bytes(builder_writer);
      }
    }
  }
  bacon_writer_close(&writer);

  msg->code = EINVAL;
  msg->len = fixed.l;
  events_publish(&ectx);
}

static void __attribute__((nonnull)) anchor_move_error(module * restrict mod, const fsent * restrict anchor_dir, const lookup_pattern * restrict anchor)
{
  narrator_fixed fixed;
  bacon_writer writer, *builder_writer;
  events_context ectx;
  bamipc_message *msg;

  msg = system_error_event(&ectx);
  narrator_fixed_init(&fixed, msg->text, msg->size);
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, &fixed.base);

  { MAPPING
    STRING("inherited anchor cannot be moved");
    { SET
      { MAPPING STRING("build")
        fsent_absolute_path_describe(mod->build_dir, builder_writer);
      }
      { MAPPING STRING("anchor pattern")
        bacon_writer_push_bytes(builder_writer);
        pattern_say(&anchor->pattern, builder_writer->N);
        bacon_writer_pop_bytes(builder_writer);
      }
      { MAPPING STRING("location")
        LOADF("%s [%d,%d - %d,%d]", anchor->pattern.loc.fname, anchor->pattern.loc.f_lin + 1, anchor->pattern.loc.f_col + 1, anchor->pattern.loc.l_lin + 1, anchor->pattern.loc.l_col + 1);
      }
      { MAPPING STRING("anchor")
        fsent_absolute_path_describe(mod->anchor_dir, builder_writer);
      }
      { MAPPING STRING("target")
        fsent_absolute_path_describe(anchor_dir, builder_writer);
      }
    }
  }
  bacon_writer_close(&writer);

  msg->code = EINVAL;
  msg->len = fixed.l;
  events_publish(&ectx);
}

static __attribute__((nonnull)) fsent *anchor_process(module * restrict mod, lookup_pattern * restrict anchor)
{
  resolve_context rctx;

  /* lookup the target directory */
  memset(&rctx, 0, sizeof(rctx));
  rctx.vars = mod->vars;
  rctx.base = mod->build_dir;
  if(pattern_resolve_lookup(&rctx, anchor) == 1) {
    return containerof(rctx.vertices[0], fsent, vertex);
  }

  anchor_unresolved_error(mod, anchor);
  return 0;
}

/*
 * Setup a var bind between var.bam file B -> module A
 */
static void bind_varset(module * restrict mod, fsent * restrict fse, graph_invalidation_context * restrict invalidation)
{
  module_edge *mme;
  varset *var;

  var = fsent_varset_bootstrap(fse, invalidation);

  // link @ //module/varsets/id -> varset
  shadow_mkref(mod->shadow_varsets, var->id, var->id_len, &var->vertex);

  module_edge_connect(&var->vertex, mod, EDGE_VARSET_MOD, &mme);
}

static bool expr_eval(bacon_parser * restrict parser, bacon_value * restrict vars, bacon_expr * restrict expr)
{
  bool b;
  bacon_value *rv0, *rv1;

  bacon_eval(parser, expr, vars, &rv0);
  bacon_value_coerce(parser, rv0, BACON_TYPE_BOOLEAN, &rv1);
  b = rv1->b;
  bacon_value_free(parser, rv0);
  if(rv0 != rv1) {
    bacon_value_free(parser, rv1);
  }

  return b;
}

/* import rules.bam file B -> module A */
bool module_import_rules_file(
    fsent * restrict rules_file_n
  , module * restrict mod
  , graph_invalidation_context * restrict invalidation
)
{
  module_edge *mme;
  ruleset *rules;
  bool new;

  rules = ruleset_bootstrap(rules_file_n, invalidation);

  // link @ //module/rules/id -> ruleset
  shadow_mkref(mod->shadow_rulesets, rules->id, rules->id_len, &rules->vertex);

  new = module_edge_connect(&rules->vertex, mod, EDGE_RULESET_USES_MOD, &mme);

  return new;
}

static bool import_process(
    module * restrict mod
  , import * restrict import
  , rule_run_context * restrict ctx
)
{
  fsent *target;
  bool new;

  target = import_resolve(import, mod, ctx);
  if(system_has_error) {
    return false;
  }

  new = false;

  switch(import->type) {
    case IMPORT_TYPE_USES:
      new |= module_import_rules_file(target, mod, &ctx->invalidation);
      break;
    default:
      RTA;
  }

  return new;
}

static bool block_process_imports(
    module * restrict mod
  , block * restrict block
  , rule_run_context * restrict ctx
)
{
  struct import *import;
  struct block *sb;
  bacon_expr *expr;
  bool new;

  llist_foreach(&block->exprs, expr, lln) {
    if(!expr_eval(parser->bacon_parser, mod->vars, expr)) {
      return false;
    }
  }

  new = false;
  llist_foreach(&block->blocks, sb, lln) {
    new |= block_process_imports(mod, sb, ctx);
  }

  llist_foreach(&block->imports, import, lln) {
    new |= import_process(mod, import, ctx);
  }

  return new;
}

void module_alloc(module ** restrict modp)
{
  module * mod;

  if((mod = llist_shift(&module_freelist, typeof(*mod), vertex.owner))) {
    ASAN_CLEAN(mod);
    memset(mod, 0, sizeof(*mod));
  } else {
    xmalloc(&mod, sizeof(*mod));
  }

  moria_vertex_init(&mod->vertex, &g_graph, VERTEX_MODULE | VERTEX_OK);

  llist_init(&mod->invalid_lln);
  mod->managed = false;

  llist_append(&module_list, &mod->vertex.owner);

  vertex_alloc_probe(&mod->vertex);
  *modp = mod;
}

struct module_scope_context {
  module *mod;
  graph_invalidation_context *invalidation;
};

void module_initialize(module * restrict mod, fsent *build_dir)
{
  /* build dir */
  mod->build_dir_abspath_len = fsent_absolute_path_cat(mod->build_dir_abspath, sizeof(mod->build_dir_abspath), build_dir);
  mod->build_dir = build_dir;
}

static void module_anchor_dir_set(module *mod, fsent *mod_anchor_dir, const lookup_pattern *anchor, graph_invalidation_context * restrict invalidation)
{
  const fsent *fse;
  const moria_vertex *mod_anchor_mod_v;
  fsent *anchor_mod;
  fsent *anchor_dir;
  ruleset *rules;

  anchor_dir = mod_anchor_dir ?: mod->build_dir;

  if(anchor_dir != mod->anchor_dir) {
    if(mod->anchor_inherited) {
      anchor_move_error(mod, anchor_dir, anchor);
      return;
    }
  }
  mod->anchor_dir = anchor_dir;

  if(mod->anchor_dir != mod->build_dir) {
    if((mod_anchor_mod_v = moria_vertex_downw(&mod->anchor_dir->vertex, MMS(FSENT_NAME_MODULE)))) {
      anchor_mod = containerof(mod_anchor_mod_v, typeof(*anchor_mod), vertex);
      rules = ruleset_bootstrap(anchor_mod, invalidation);
      module_edge_connect(&rules->vertex, mod, EDGE_RULESET_ANCHOR_MOD, 0);
      shadow_mkref(mod->shadow_rulesets, rules->id, rules->id_len, &rules->vertex);
    }
  }

  mod->anchor_dir_abspath_len = fsent_absolute_path_cat(mod->anchor_dir_abspath, sizeof(mod->anchor_dir_abspath), mod->anchor_dir);
  shadow_mkref(mod->shadow, MMS("anchor"), &mod->anchor_dir->vertex);

  mod->disjoint = true;
  fse = mod->build_dir;
  while(1) {
    if(fse == mod->anchor_dir) {
      mod->disjoint = false;
      break;
    }
    if(fse == g_root) {
      break;
    }
    fse = fsent_parent(fse);
  }
}

void module_bootstrap(fsent *build_dir, fsent *anchor_dir, graph_invalidation_context * restrict invalidation)
{
  module *mod;
  moria_vertex *mod_v;
  detail *vars;
  detail *envs;
  identity_ctx id;
  moria_vertex *build_mod_v;
  fsent *build_mod;
  ruleset *rules;

  /* module already bootstrapped in this build dir */
  if((mod_v = edge_follow_up(&build_dir->vertex, EDGE_BUILD_DIR_MOD))) {
    mod = containerof(mod_v, typeof(*mod), vertex);
    mod->epoch = reconciliation_epoch;
    return;
  }

  module_alloc(&mod);
  module_initialize(mod, build_dir);
  mod->epoch = reconciliation_epoch;

  identity_init(&id);
  module_compute_id(mod, &id);
  mod->id_len = identity_mk(&id, mod->id, sizeof(mod->id));

  mod->vertex.label = mod->id;
  mod->vertex.label_len = mod->id_len;

  /* default name */
  mod->name_len = zcatw(mod->name, sizeof(mod->name), mod->build_dir->vertex.label, mod->build_dir->vertex.label_len);

  // build out the modules shadow node
  shadow_create(g_shadow_modules, &mod->shadow, mod->id, mod->id_len, VERTEX_SHADOW_DIR, invalidation);
  special_connect(&mod->shadow->vertex, &mod->vertex, EDGE_SPECIAL, invalidation);

  /* edge lists */
  shadow_create(mod->shadow, &mod->shadow_rulesets, MMS("rulesets"), VERTEX_SHADOW_DIR, invalidation);
  shadow_create(mod->shadow, &mod->shadow_varsets, MMS("varsets"), VERTEX_SHADOW_DIR, invalidation);
  shadow_create(mod->shadow, &mod->shadow_rules, MMS("rules"), VERTEX_SHADOW_DIR, invalidation);
  shadow_create(mod->shadow, &mod->shadow_rules_by, MMS("rules-by"), VERTEX_SHADOW_DIR, invalidation);
  shadow_create(mod->shadow_rules_by, &mod->shadow_rules_byname, MMS("name"), VERTEX_SHADOW_DIR, invalidation);
  shadow_create(mod->shadow, &mod->shadow_rmas, MMS("rmas"), VERTEX_SHADOW_DIR, invalidation);
  shadow_create(mod->shadow, &mod->shadow_rmas_by, MMS("rmas-by"), VERTEX_SHADOW_DIR, invalidation);
  shadow_create(mod->shadow_rmas_by, &mod->shadow_rmas_byname, MMS("name"), VERTEX_SHADOW_DIR, invalidation);

  /* effective views */
  vars = detail_alloc(&mod->vertex, VERTEX_MODULE_VARS);
  shadow_mkref(mod->shadow, MMS("vars"), &vars->vertex);
  envs = detail_alloc(&mod->vertex, VERTEX_MODULE_ENVS);
  shadow_mkref(mod->shadow, MMS("envs"), &envs->vertex);

  /* build dir */
  shadow_mkref(mod->shadow, MMS("build"), &mod->build_dir->vertex);
  special_connect(&mod->build_dir->vertex, &mod->vertex, EDGE_BUILD_DIR_MOD, invalidation);

  vertex_alloc_probe(&mod->shadow->vertex);
  vertex_alloc_probe(&mod->shadow_rulesets->vertex);
  vertex_alloc_probe(&mod->shadow_varsets->vertex);
  vertex_alloc_probe(&mod->shadow_rmas->vertex);

  /* init build file in the build dir */
  if((build_mod_v = moria_vertex_downw(&build_dir->vertex, MMS(FSENT_NAME_MODULE)))) {
    build_mod = containerof(build_mod_v, typeof(*build_mod), vertex);
    rules = ruleset_bootstrap(build_mod, invalidation);
    module_edge_connect(&rules->vertex, mod, EDGE_RULESET_BUILD_MOD, 0);
    shadow_mkref(mod->shadow_rulesets, rules->id, rules->id_len, &rules->vertex);
  }

  /* module build-dir should always be under g_base ! */
  RUNTIME_ASSERT(fsent_is_self_or_descendant(g_base, mod->build_dir));

  /* set the initial anchor */
  module_anchor_dir_set(mod, anchor_dir, 0, invalidation);

  /* only true when called from module_bootstrap_anchored_visitor */
  mod->anchor_inherited = build_dir != anchor_dir;
}

void module_rebuild(module * restrict mod, rule_run_context * restrict ctx)
{
  fsent *anchor_dir;
  struct module_bootstrap_context bootstrap_ctx;
  ruleset *build_mod;
  moria_vertex *build_mod_v;

  module_rebuild_probe(mod);

  /* re-configure the anchor */
  build_mod = 0;
  if((build_mod_v = edge_follow_down(&mod->vertex, EDGE_RULESET_BUILD_MOD))) {
    build_mod = containerof(build_mod_v, typeof(*build_mod), vertex);
  }

  mod->managed = false;
  if(build_mod) {
    mod->managed = build_mod->managed;
  }

  graph_invalidation_begin(&ctx->invalidation);

  /* set or re-set the module anchor dir */
  anchor_dir = 0;
  if(build_mod && build_mod->anchor) {
    anchor_dir = anchor_process(mod, build_mod->anchor);
    if(system_has_error) {
      goto end;
    }

    module_anchor_dir_set(mod, anchor_dir, build_mod->anchor, &ctx->invalidation);
    if(system_has_error) {
      goto end;
    }
  } else if(mod->anchor_inherited) {
    module_anchor_dir_set(mod, mod->anchor_dir, 0, &ctx->invalidation);
  } else {
    module_anchor_dir_set(mod, 0, 0, &ctx->invalidation);
  }

  if(mod->anchor_dir != mod->build_dir && mod->disjoint) {
    /* bootstrap additional modules under the anchor */
    bootstrap_ctx = (typeof(bootstrap_ctx)){
        .mod = mod
      , .invalidation = &ctx->invalidation
    };
    moria_traverse_vertices(
        &g_graph
      , &mod->anchor_dir->vertex
      , module_bootstrap_anchored_visitor
      , 0
      , (moria_traversal_criteria[]) {{
            edge_travel: EDGE_FS
          , edge_visit : EDGE_FS
          , vertex_travel : VERTEX_FILETYPE_DIR
          , vertex_visit : VERTEX_FILETYPE_DIR
          , min_depth : 1
        }}
      , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE
      , &bootstrap_ctx
    );
  }

  /* anchor dir */
  if(mod->anchor_dir != mod->build_dir) {
    special_connect(&mod->anchor_dir->vertex, &mod->vertex, EDGE_ANCHOR_DIR_MOD, &ctx->invalidation);
  }
  shadow_mkref(mod->shadow, MMS("anchor"), &mod->anchor_dir->vertex);

end:
  graph_invalidation_end(&ctx->invalidation);
}

void module_remake(module * restrict mod, rule_run_context * restrict ctx)
{
  moria_foreach_state st;
  moria_edge *edge;
  varset *var;
  moria_vertex *base;
  moria_vertex *v;
  module_edge *mme;
  int x;
  vertices list, *eln;
  module_edge *me;
  ruleset *build_mod;
  ruleset *anchor_mod;
  struct block *block;

  graph_invalidation_begin(&ctx->invalidation);

  /* renew build dir mod */
  build_mod = 0;
  if((edge = edges_check_down(&mod->vertex, EDGE_RULESET_BUILD_MOD))) {
    me = containerof(edge, typeof(*me), edge);
    me->epoch = reconciliation_epoch;
    build_mod = containerof(me->edge.B, typeof(*build_mod), vertex);
  }

  anchor_mod = 0;
  if((edge = edges_check_down(&mod->vertex, EDGE_RULESET_ANCHOR_MOD))) {
    me = containerof(edge, typeof(*me), edge);
    anchor_mod = containerof(me->edge.B, typeof(*anchor_mod), vertex);
  }

  /* gather up varsets files in the ancestry */
  vertices_init(&list);
  for(x = 0; x < 2; x++) {
    if(x == 0) {
      base = &mod->anchor_dir->vertex;
    } else if(base == &mod->build_dir->vertex) {
      break;
    } else {
      base = &mod->build_dir->vertex;
    }

    locate_ascending(&list, base, build_mod, anchor_mod, 1);
  }

  /* bootstrap and connect to each of those var.bam files */
  vertices_foreach(&list, v, eln) {
    bind_varset(mod, containerof(v, fsent, vertex), &ctx->invalidation);
  }
  vertices_destroy(&list);

  /* gather up rulesets in the ancestry */
  vertices_init(&list);
  for(x = 0; x < 2; x++) {
    if(x == 0) {
      base = &mod->anchor_dir->vertex;
    } else if(base == &mod->build_dir->vertex) {
      break;
    } else {
      base = &mod->build_dir->vertex;
    }

    locate_ascending(&list, base, build_mod, anchor_mod, 2);
  }

  /* bootstrap and connect to each of those rules.bam list */
  vertices_foreach(&list, v, eln) {
    module_import_rules_file(containerof(v, fsent, vertex), mod, &ctx->invalidation);
  }
  vertices_destroy(&list);

  /* cleanup edges not refreshed */
  module_prune(mod, &ctx->invalidation);

  /* rebuild effective vars */
  bacon_value_free(parser->bacon_parser, mod->vars);
  mod->vars = 0;

  moria_foreach_edge_down(&st, &mod->vertex, edge) {
    if(edge_type_get(edge) != EDGE_VARSET_MOD) {
      continue;
    }

    /* skip edges not refreshed by the above ; they will be excised in module_prune */
    mme = containerof(edge, typeof(*mme), edge);
    if(mme->epoch != reconciliation_epoch) {
      continue;
    }

    var = containerof(edge->B, typeof(*var), vertex);
    varset_reparse(var);
    bacon_merge(parser->bacon_parser, &mod->vars, var->val);
  }

  graph_invalidation_end(&ctx->invalidation);

  /* sysvars */
  bacon_merge(parser->bacon_parser, &mod->vars, sysvar_globals);
  sysvar_module_render(parser->bacon_parser, mod->vars, mod);

  graph_invalidation_begin(&ctx->invalidation);

  /* immediate imports -- its necessary to do this here, because it bootstraps and invalidates
   * rules files, which need to have been parsed before module_reimport */
  if(build_mod) {
    llist_foreach(&build_mod->blocks, block, lln) {
      block_process_imports(mod, block, ctx);
    }
  }
  if(anchor_mod) {
    llist_foreach(&anchor_mod->blocks, block, lln) {
      block_process_imports(mod, block, ctx);
    }
  }

  graph_invalidation_end(&ctx->invalidation);
}

bool module_reimport(module * restrict mod, rule_run_context * restrict ctx)
{
  struct block *block;
  struct ruleset *rules;
  moria_foreach_state st;
  moria_edge *edge;
  bool new;

  /* transitive imports */
  new = false;
  moria_foreach_edge_down(&st, &mod->vertex, edge) {
    if(edge_type_get(edge) != EDGE_RULESET_USES_MOD) {
      continue;
    }
    rules = containerof(edge->B, typeof(*rules), vertex);

    llist_foreach(&rules->blocks, block, lln) {
      new |= block_process_imports(mod, block, ctx);
    }
  }

  return new;
}

/*
 * mod - module being reloaded
 * rules - rules file which defines this block (can be null)
 * block - block of rules
 */
static __attribute__((nonnull)) void block_reload(
    module * mod
  , struct ruleset * restrict rules
  , struct block * restrict block
  , graph_invalidation_context * restrict invalidation
)
{
  struct rule *rule;
  struct block *sb;
  bacon_expr *expr;

  llist_foreach(&block->blocks, sb, lln) {
    block_reload(mod, rules, sb, invalidation);
  }

  llist_foreach(&block->exprs, expr, lln) {
    if(!expr_eval(parser->bacon_parser, mod->vars, expr)) {
      return;
    }
  }

  llist_foreach(&block->rules, rule, lln) {
    rma_create(rule, mod, rules, invalidation);
  }
}

void module_reload(module * restrict mod, graph_invalidation_context * restrict invalidation)
{
  block *block;
  ruleset *rules;
  moria_foreach_state st;
  moria_edge *edge;
  fsent *fse;
  vertices list, *eln;
  moria_vertex *v;
  struct edges edges;
  struct vertices unlinking;
  struct vertices releasing;

  mod->module_stats.reloaded++;
  mod->epoch = reconciliation_epoch;

  edges_init(&edges);
  vertices_init(&unlinking);
  vertices_init(&releasing);

  /* teardown all rmas */
  moria_foreach_edge(&st, &mod->vertex, edge) {
    switch(edge_type_get(edge)) {
      case EDGE_RMA_MOD:
        vertices_push(&unlinking, edge->B);
        edges_push(&edges, edge);
      default:
        break;
    }
  }

  graph_invalidation_begin(invalidation);
  graph_disintegrate(&edges, &unlinking, &releasing, invalidation);
  graph_invalidation_end(invalidation);

  /* reload all rulesets
   *
   * N.B. because block_reload can create new edges, its necessary to assemble the list
   * of edges first
   */
  vertices_init(&list);
  moria_foreach_edge_down(&st, &mod->vertex, edge) {
    switch(edge_type_get(edge)) {
      case EDGE_RULESET_USES_MOD:
      case EDGE_RULESET_BUILD_MOD:
      case EDGE_RULESET_ANCHOR_MOD:
        break;
      default:
        continue;
    }

    rules = containerof(edge->B, typeof(*rules), vertex);
    vertices_push(&list, &rules->vertex);
  }

  graph_invalidation_begin(invalidation);
  vertices_foreach(&list, v, eln) {
    rules = containerof(v, typeof(*rules), vertex);
    llist_foreach(&rules->blocks, block, lln) {
      block_reload(mod, rules, block, invalidation);
    }
  }
  vertices_destroy(&list);
  graph_invalidation_end(invalidation);

  /* prepare for filesystem-reconcile to lazily create the directory structure down
   * to but not inside the build dir, which may not yet exist if the module is disjoint.
   * note that build dirs are always under g_base, so this traversal need not ascend all
   * the way to the root */
  for(fse = mod->build_dir; fse != g_base; fse = fsent_parent(fse)) {
    if(vertex_state_get(&fse->vertex) == VERTEX_UNCREATED) {
      fsent_dir_mkdir_unshift(fse);
    }
  }
}

void module_rebuild_enqueue(module * restrict mod)
{
  llist_delete(&mod->invalid_lln);
  llist_unshift(&module_rebuilds, &mod->invalid_lln);
}

void modules_invalidate_for_specials(fsent * restrict fse, graph_invalidation_context * restrict invalidation)
{
  module *mod;
  fsent *dir;
  fsent *fse_dir;
  fsent *p;
  ruleset_special_name *special_name;
  int x;
  ruleset *build_mod;
  ruleset *anchor_mod;
  moria_edge *edge;
  config_specials_entry *cse;

  /* the ascending ancestry traversal has symmetry with locate_ascending */
  fse_dir = fsent_parent(fse);

  llist_foreach(&module_list, mod, vertex.owner) {
    build_mod = 0;
    if((edge = edges_check_down(&mod->vertex, EDGE_RULESET_BUILD_MOD))) {
      build_mod = containerof(edge->B, typeof(*build_mod), vertex);
    }

    anchor_mod = 0;
    if((edge = edges_check_down(&mod->vertex, EDGE_RULESET_ANCHOR_MOD))) {
      anchor_mod = containerof(edge->B, typeof(*anchor_mod), vertex);
    }

    for(x = 0; x < 2; x++) {
      /* consider both the tree from the build dir, and from the anchor dir */
      if(x == 0) {
        p = mod->build_dir;
      } else if(p == mod->anchor_dir) {
        break;
      } else {
        p = mod->anchor_dir;
      }

      /* is it a module.bam file in the anchor dir */
      if(x == 1 && p == fse_dir && memncmp(MMS(FSENT_NAME_MODULE), fse->vertex.label, fse->vertex.label_len) == 0) {
        vertex_invalidate(&mod->vertex, invalidation);
        continue;
      }

      for(dir = p; dir != g_root; dir = fsent_parent(dir)) {
        if(dir == fse_dir) {
          break;
        }
      }

      /* check if fse is in an ancestor directory */
      if(dir != fse_dir) {
        continue;
      }

      /* is it a varset ? build module varsets */
      if(build_mod) {
        llist_foreach(&build_mod->varsets_names, special_name, lln) {
          if(memncmp(special_name->name, special_name->len, fse->vertex.label, fse->vertex.label_len) == 0) {
            vertex_invalidate(&mod->vertex, invalidation);
            break;
          }
        }
      }
      /* is it a varset ? anchor module varsets */
      if(anchor_mod) {
        llist_foreach(&anchor_mod->varsets_names, special_name, lln) {
          if(memncmp(special_name->name, special_name->len, fse->vertex.label, fse->vertex.label_len) == 0) {
            vertex_invalidate(&mod->vertex, invalidation);
            break;
          }
        }
      }
      /* is it a varset ? global module config varsets */
      rbtree_foreach(modules_varsets, cse, rbn) {
        if(memncmp(cse->name->s, cse->name->len, fse->vertex.label, fse->vertex.label_len) == 0) {
          vertex_invalidate(&mod->vertex, invalidation);
          break;
        }
      }

      /* is it a ruleset ? */
      if(build_mod) {
        llist_foreach(&build_mod->rulesets_names, special_name, lln) {
          if(memncmp(special_name->name, special_name->len, fse->vertex.label, fse->vertex.label_len) == 0) {
            vertex_invalidate(&mod->vertex, invalidation);
            break;
          }
        }
      }
      if(anchor_mod) {
        llist_foreach(&anchor_mod->rulesets_names, special_name, lln) {
          if(memncmp(special_name->name, special_name->len, fse->vertex.label, fse->vertex.label_len) == 0) {
            vertex_invalidate(&mod->vertex, invalidation);
            break;
          }
        }
      }
      rbtree_foreach(modules_rulesets, cse, rbn) {
        if(memncmp(cse->name->s, cse->name->len, fse->vertex.label, fse->vertex.label_len) == 0) {
          vertex_invalidate(&mod->vertex, invalidation);
          break;
        }
      }
    }
  }
}

//
// internal
//

static void module_destroy(module * restrict mod)
{
  bacon_value_free(parser->bacon_parser, mod->vars);
}

void module_release(module * restrict mod)
{
  moria_vertex __attribute__((unused)) *v;

  if(mod->anchor_dir && mod->anchor_dir != mod->build_dir) {
    /* un-magic the previous anchor dir, lazy reparent */
    vertex_type_set(&mod->anchor_dir->vertex, VERTEX_DIR);
  }

  llist_delete(&mod->invalid_lln);
  module_destroy(mod);

  /* must have been fully excised from the graph previously */
  v = &mod->vertex;
  RUNTIME_ASSERT(v->up_identity == 0);
  RUNTIME_ASSERT(rbtree_empty(&v->up));
  RUNTIME_ASSERT(rbtree_empty(&v->down));

  llist_delete(&mod->vertex.owner);
  llist_append(&module_freelist, &mod->vertex.owner);
  ASAN_TAINT(mod);
}

//
// public
//

void module_setup()
{
  exec_builder_init(&local_exec_builder);
  exec_render_context_init(&local_exec_render);

  ruleset_parser_alloc(&parser);
  moria_vertex_initw(g_module, &g_graph, VERTEX_CONTEXT_MODULE | VERTEX_OK, MMS("(module)"));
  shadow_mkref(g_shadow, MMS("module"), g_module);
}

void module_cleanup()
{
  module *mod;

  while((mod = llist_shift(&module_freelist, typeof(*mod), vertex.owner))) {
    ASAN_CLEAN(mod);
    free(mod);
  }

  exec_builder_destroy(&local_exec_builder);
  exec_render_context_destroy(&local_exec_render);
  ruleset_parser_free(parser);
}

static void module_stats_collate(module *mod)
{
  moria_edge *edge;
  moria_foreach_state st;
  stats_module *stats;

  /* module vertex specific stats */
  stats = &mod->module_stats;

  stats->varsets = 0;
  stats->fsnames = 0;
  stats->rulesets = 0;
  stats->rmas = 0;
  moria_foreach_edge_down(&st, &mod->vertex, edge) {
    switch(edge_type_get(edge)) {
      case EDGE_RULESET_USES_MOD:
      case EDGE_RULESET_BUILD_MOD:
      case EDGE_RULESET_ANCHOR_MOD:
        stats->rulesets++;
        break;
      case EDGE_VARSET_MOD:
        stats->varsets++;
        break;
      case EDGE_FSNAME_MOD:
        stats->fsnames++;
        break;
      case EDGE_RMA_MOD:
        stats->rmas++;
      default:
        break;
    }
  }
}

void module_stats_build(module * restrict mod, bool reset, bacon_writer * restrict builder_writer)
{
  module_stats_collate(mod);

  stats_build(&mod->stats.record, &descriptor_stats_vertex, builder_writer);
  stats_build(&mod->module_stats.record, &descriptor_stats_module, builder_writer);

  if(reset) {
    memset(&mod->stats, 0, sizeof(mod->stats));
    memset(&mod->module_stats, 0, sizeof(mod->stats));
  }
}

void module_describe(const module * restrict mod, struct bacon_writer * restrict builder_writer)
{
  ruleset *build_mod;
  ruleset *anchor_mod;
  ruleset_special_name *special_name;
  moria_edge *edge;
  config_specials_entry *cse;

  /* renew build dir mod */
  build_mod = 0;
  if((edge = edges_check_down(&mod->vertex, EDGE_RULESET_BUILD_MOD))) {
    build_mod = containerof(edge->B, typeof(*build_mod), vertex);
  }

  anchor_mod = 0;
  if((edge = edges_check_down(&mod->vertex, EDGE_RULESET_ANCHOR_MOD))) {
    anchor_mod = containerof(edge->B, typeof(*anchor_mod), vertex);
  }

  { MAPPING
    vertex_path_describe(&mod->vertex, builder_writer);
    { SET
      {
        vertex_mappings(&mod->vertex, builder_writer);

        { MAPPING STRING("disjoint") BOOL(mod->disjoint) }
        { MAPPING STRING("managed") BOOL(mod->managed) }
        { MAPPING STRING("build-dir")
          BYTES(mod->build_dir_abspath, mod->build_dir_abspath_len)
        }
        { MAPPING STRING("anchor-dir")
          BYTES(mod->anchor_dir_abspath, mod->anchor_dir_abspath_len)
        }
        { MAPPING STRING("rulesets-names")
          { LIST
            if(build_mod) {
              llist_foreach(&build_mod->rulesets_names, special_name, lln) {
                BYTES(special_name->name, special_name->len)
              }
            }
            if(anchor_mod) {
              llist_foreach(&anchor_mod->rulesets_names, special_name, lln) {
                BYTES(special_name->name, special_name->len)
              }
            }
            rbtree_foreach(modules_rulesets, cse, rbn) {
              BYTES(cse->name->s, cse->name->len)
            }
          }
        }
        { MAPPING STRING("varsets-names")
          { LIST
            if(build_mod) {
              llist_foreach(&build_mod->varsets_names, special_name, lln) {
                BYTES(special_name->name, special_name->len)
              }
            }
            if(anchor_mod) {
              llist_foreach(&anchor_mod->varsets_names, special_name, lln) {
                BYTES(special_name->name, special_name->len)
              }
            }
            rbtree_foreach(modules_varsets, cse, rbn) {
              BYTES(cse->name->s, cse->name->len)
            }
          }
        }
      }
      { MAPPING
        STRING("stats")
        { SET
          module_stats_build((module *)mod, false, builder_writer);
        }
      }

      vertex_relations_describe(&mod->vertex, builder_writer);
    }
  }
}

size_t module_path_cat(void * restrict dst, size_t sz, const module * restrict module)
{
  return zcatf(dst, sz, "//bm/modules/%.*s", module->id_len, module->id);
}

void module_identity_describe(const struct module * restrict module, struct bacon_writer * restrict builder_writer)
{
  attrs32_describe(module->vertex.attrs, graph_vertex_attrs, builder_writer);

  { MAPPING
    STRING("id")
    BYTES(module->id, module->id_len);
  }
  { MAPPING
    STRING("build-dir")
    STRING(module->build_dir_abspath);
  }
}

void module_unlink(module * restrict mod, struct graph_disintegrate_context * restrict ctx)
{
  moria_foreach_state st;
  moria_edge *edge;

  llist_delete(&mod->invalid_lln);

  moria_foreach_edge(&st, &mod->vertex, edge) {
    switch(edge_type_get(edge)) {
      case EDGE_SPECIAL:
      case EDGE_RMA_MOD:
        vertices_push(ctx->unlinking, edge->B);
      case EDGE_RULESET_USES_MOD:
      case EDGE_VARSET_MOD:
      case EDGE_FSNAME_MOD:
      case EDGE_RULESET_BUILD_MOD:
      case EDGE_RULESET_ANCHOR_MOD:
      case EDGE_ANCHOR_DIR_MOD:
      case EDGE_BUILD_DIR_MOD:
        edges_push(ctx->edges, edge);
      default:
        break;
    }
  }
}

void module_prune(module * restrict mod, graph_invalidation_context * restrict invalidation)
{
  struct edges edges;
  struct vertices unlinking;
  struct vertices releasing;
  moria_foreach_state st;
  module_edge *mme;
  special_edge *se;
  moria_edge *edge;
  uint16_t epoch;

  /*
   * If the edges are simply destroyed and re-created each time the module is rebuilt, then nodes on the other
   * side of those edges will similarly be destroyed and re-created, if they dont have some other edge linking
   * them to the working set. This breaks the parse counting in e.g. integ/test-var-modify.
   *
   * Thus, edges are only cleaned up after having had a chance to be renewed.
   */

  edges_init(&edges);
  moria_foreach_edge(&st, &mod->vertex, edge) {
    switch(edge_type_get(edge)) {
      case EDGE_RULESET_USES_MOD:
      case EDGE_VARSET_MOD:
        mme = containerof(edge, typeof(*mme), edge);
        epoch = mme->epoch;
        break;
      case EDGE_RULESET_BUILD_MOD:
      case EDGE_RULESET_ANCHOR_MOD:
      case EDGE_ANCHOR_DIR_MOD:
        se = containerof(edge, typeof(*se), edge);
        epoch = se->epoch;
        break;
      default:
        continue;
    }

    if(epoch == reconciliation_epoch) {
      continue;
    }

    edges_push(&edges, edge);
  }

  vertices_init(&unlinking);
  vertices_init(&releasing);
  graph_disintegrate(&edges, &unlinking, &releasing, invalidation);
}

bool module_reap(struct module * restrict mod, struct graph_disintegrate_context * restrict ctx)
{
  if(edges_check_down(&mod->vertex, EDGE_RULESET_BUILD_MOD)) {
    return false;
  }
  if(edges_check_down(&mod->vertex, EDGE_RULESET_ANCHOR_MOD)) {
    return false;
  }

  return true;
}

void module_id(const struct module * restrict module, const char ** restrict id, uint16_t * restrict idl)
{
  *id = module->id;
  *idl = module->id_len;
}

void module_compute_id(const struct module * restrict module, identity_ctx * restrict id)
{
  identity_type(id, vertex_type_get(&module->vertex));
  identity_add(id, " ", 1);
  fsent_id_path_compute(module->build_dir, id);
}

#if CONFIG_DEVEL
void modules_show(void)
{
  module *mod;
  int x = 0;

  printf("modules, live : %zu free : %zu\n", llist_count(&module_list), llist_count(&module_freelist));
  llist_foreach(&module_list, mod, vertex.owner) {
    printf("%3u ", x++);
    vertex_identity_show(&mod->vertex);
  }
}
#endif
