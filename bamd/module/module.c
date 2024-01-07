/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <string.h>
#include <inttypes.h>
#include <errno.h>

#include "types.h"

#include "moria/edge.h"
#include "moria/foreach.h"
#include "bacon/builder.h"
#include "bacon/value.h"
#include "bacon/merge.h"
#include "bacon/parser.h"
#include "bacon/writer.h"
#include "bacon/coerce.h"
#include "bacon/eval.h"
#include "bacon/expr.h"
#include "valyria/llist.h"
#include "valyria/map.h"
#include "valyria/hashtable.h"
#include "xlinux/xfcntl.h"
#include "xlinux/xunistd.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xstat.h"
#include "bam/vertex.h"
#include "narrator/fixed.h"

#include "module.h"
#include "exec.h"
#include "exec_builder.h"
#include "exec_render.h"
#include "fsedge.h"
#include "fsent.h"
#include "vertex.h"
#include "rules_parser.h"
#include "pattern_parser.internal.h"
#include "params.h"
#include "pattern.h"
#include "lookup.h"
#include "generate.h"
#include "render.h"
#include "rule.h"
#include "rule_module_association.h"
#include "filesystem.h"
#include "module_edge.h"
#include "dir_rma.h"
#include "rule_system.h"
#include "shadow.h"
#include "stats.h"
#include "var.h"
#include "channel.h"
#include "system_state.h"
#include "events.h"
#include "block.h"
#include "import.h"
#include "rule/probes.h"
#include "rules.h"
#include "anchor.h"
#include "reconcile.h"
#include "special_edge.h"
#include "edge.h"
#include "entlist.h"
#include "vertex/probes.h"

#include "common/assure.h"
#include "common/snarf.h"
#include "common/hash.h"
#include "marshal.h"
#include "zcat.h"

llist module_rebuild_list = LLIST_INITIALIZER(module_rebuild_list);

llist module_list = LLIST_INITIALIZER(module_list);                 // active modules
static llist module_freelist = LLIST_INITIALIZER(module_freelist);  // freelist

static exec_builder local_exec_builder;
static exec_render_context local_exec_render;
static rules_parser *parser;

//
// static
//

/* find special named files in the ancestry of the base dir */
static void locate_ascending(
    entlist * restrict list
  , llist * restrict special_names
  , const char *special_name
  , moria_vertex * restrict base
)
{
  moria_vertex *v;
  moria_vertex *child;
  pattern *ref;
  pattern_render_result *rendered = 0;
  pattern_render_fragment *frag = 0;
  const char *name = 0;
  uint16_t name_len = 0;

#ifdef CONFIG_DEVEL
  char buf[246];
  const char *label = 0;
  uint16_t label_len = 0;
#endif

  ref = (void*)(uintptr_t)0x1;
  while(1)
  {
    if(ref == (void*)(uintptr_t)0x1) {
      name = special_name;
      name_len = strlen(special_name);
      ref = 0;
    } else if(ref == 0) {
      if((ref = llist_first(special_names, typeof(*ref), lln)) == 0) {
        break;
      }
    } else if((ref = llist_next(special_names, ref, lln)) == 0) {
      break;
    }

    if(!name)
    {
      free(rendered);
      pattern_render(ref, 0, &rendered, 0);

      /* this should be a foreach instead */
      RUNTIME_ASSERT(rendered->size == 1);

      frag = rendered->fragments;
      name = frag->text;
      name_len = frag->len;

#ifdef CONFIG_DEVEL
      label = name;
      label_len = name_len;
      name_len = 0;
      name_len += zcatw(buf + name_len, sizeof(buf) - name_len, label, label_len);
      name_len += zcats(buf + name_len, sizeof(buf) - name_len, "+devel");
      name = buf;
#endif
    }

    for(v = base; v != &g_root->vertex; v = moria_vertex_up(v))
    {
      if(!(child = moria_vertex_downw(v, name, name_len))) {
        continue;
      }
      if(!vertex_exists_get(child)) {
        continue;
      }

      entlist_push(list, child);
    }

    name = 0;
    name_len = 0;
  }
  free(rendered);
}

void module_bootstrap_visitor(
    moria_vertex * mod_dir_v
  , void * ctx
  , moria_traversal_mode mode
  , int distance
  , moria_traverse_result * restrict result
)
{
  fsent * mod_dir_n;
  fsent * mod_file_n;
  moria_vertex * mod_file_v;
  graph_invalidation_context *invalidation = ctx;

  if((mod_file_v = moria_vertex_downw(mod_dir_v, MMS(fsent_name_module))) == 0) {
    return;
  }

  mod_file_n = containerof(mod_file_v, typeof(*mod_file_n), vertex);
  mod_dir_n = containerof(mod_dir_v, typeof(*mod_dir_n), vertex);

  module_bootstrap(mod_dir_n, mod_dir_n, mod_file_n, invalidation);
}

struct module_bootstrap_anchored_context {
  module *mod;
  graph_invalidation_context *invalidation;
};

void module_bootstrap_anchored_visitor(moria_vertex * mod_dir_v, void * arg, moria_traversal_mode mode, int distance, moria_traverse_result * restrict result)
{
  fsent *mod_dir_n;
  fsent *mod_file_n;
  fsent *mod_anchor_n;
  moria_vertex *mod_file_v;
  struct module_bootstrap_anchored_context *ctx = arg;

  if((mod_file_v = moria_vertex_downw(mod_dir_v, MMS(fsent_name_module))) == 0) {
    /* no module file in the directory */
    return;
  }

  mod_file_n = containerof(mod_file_v, typeof(*mod_file_n), vertex);
  mod_anchor_n = containerof(mod_dir_v, typeof(*mod_dir_n), vertex);

  mod_dir_n = fsent_translate(mod_anchor_n, ctx->mod->anchor_node, ctx->mod->dir_node, ctx->invalidation);

  module_bootstrap(mod_dir_n, mod_anchor_n, mod_file_n, ctx->invalidation);
}

static fsent *anchor_process(module * restrict mod, anchor * restrict anchor)
{
  fsent *target;

  /* lookup the target directory */
  entlist_destroy(&parser->lookup_hits);
  pattern_resolve_lookup(anchor->lookup, 0, mod->dir_node, &parser->lookup_hits, &target);
  if(!target) {
    /* system state event emitted in pattern lookup */
    return 0;
  }

  return target;
}

/*
 * Setup a var bind between modules A -> var.bam file B
 */
static bool bind_var(module * restrict A, fsent * restrict var_file_n, graph_invalidation_context * restrict invalidation)
{
  fsedge *ue;
  module_edge *mme;
  var *var;
  uint64_t u64;
  char id[32];
  uint16_t id_len;
  char path[PATH_MAX];
  uint16_t len;
  bool res;

  ue = 0;

  fsent_var_bootstrap(var_file_n, invalidation);
  var = var_file_n->self_var;

  len = fsent_absolute_path_cat(path, sizeof(path), var_file_n);
  u64 = hash64(0, path, len);
  id_len = snprintf(id, sizeof(id), "%016"PRIx64, u64);

  // link @ //module/uses/$name -> var.bam
  shadow_graft_var(A, var_file_n, id, id_len, &ue, invalidation);

  res = module_edge_connect(A, &var->vertex, EDGE_VAR_MOD, &mme);
  mme->related = ue;

  return res;
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
    module * restrict A
  , fsent * restrict rules_file_n
  , graph_invalidation_context * restrict invalidation
)
{
  fsedge *ue;
  module_edge *mme;
  rules *rules;
  bool new;

  ue = 0;

  rules_bootstrap(rules_file_n, invalidation);
  rules = rules_file_n->self_rules;

  // link @ //module/rules/$name -> rules.bam
  shadow_graft_rules(A, rules_file_n, rules->id, rules->id_len, &ue, invalidation);

  new = module_edge_connect(A, &rules->vertex, EDGE_RULES_MOD, &mme);
  mme->related = ue;

  return new;
}

/*
 * Setup a requires binding between two modules A and B
 */
static void require_module(
    module * restrict A
  , fsent * mod_dir_n
  , const char * restrict name
  , uint16_t name_len
  , graph_invalidation_context * restrict invalidation
)
{
  fsedge *ue;
  module_edge *mme;
  module *B;

  B = mod_dir_n->mod;

  // link @ //module/uses/$name
  ue = 0;
  shadow_graft_requires(A, B->dir_node, name, name_len, &ue, invalidation);

  module_edge_connect(A, &B->vertex, EDGE_REQUIRES, &mme);
  mme->related = ue;
}

static bool import_process(
    module * restrict mod
  , import * restrict import
  , rule_run_context * restrict ctx
)
{
  fsent *target;
  const char *name;
  uint16_t name_len;
  bool new;
  entlist *eln;
  moria_vertex *v;

  import_resolve_targets(import, mod, ctx);
  if(system_error) {
    return false;
  }

  new = false;
  entlist_foreach(&ctx->target_hits, v, eln) {
    target = containerof(v, typeof(*target), vertex);

    if(import->attrs & IMPORT_FLAGS_NONAME) {
      name = 0;
      name_len = 0;
    } else if(import->name_len) {
      name = import->name;
      name_len = import->name_len;
    } else {
      name = target->vertex.label;
      name_len = target->vertex.label_len;
    }

    switch(import->type) {
      case IMPORT_TYPE_USES:
        new |= module_import_rules_file(mod, target, &ctx->invalidation);
        break;
      case IMPORT_TYPE_REQUIRES:
        require_module(mod, target, name, name_len, &ctx->invalidation);
        break;
      default:
        RTA;
    }
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

  if((mod = llist_shift(&module_freelist, typeof(*mod), vertex.owner)) == 0) {
    xmalloc(&mod, sizeof(*mod));
  } else {
    memset(mod, 0, sizeof(*mod));
  }

  moria_vertex_init(&mod->vertex, &g_graph, VERTEX_MODULE | VERTEX_OK);

  llist_init(&mod->blocks);
  llist_init(&mod->invalid.lln);
  mod->invalid.attrs = mod->vertex.attrs;
  llist_init(&mod->varfiles);
  llist_init(&mod->rulesfiles);

  llist_append(&module_list, &mod->vertex.owner);

  vertex_alloc_probe(&mod->vertex);
  *modp = mod;
}

static void module_parse(module * restrict mod)
{
  int exit;
  char * text = 0;
  size_t text_len;
  bamipc_message *msg;
  channel *chan;

  STATS_INC(mod->mod_stats.parse);
  STATS_INC(g_stats.module_parse);

  snarfs(&text, &text_len, mod->self_node_abspath);

  if(text && (exit = rules_parser_parse(parser, text, text_len + 2, mod->self_node_abspath, mod->self_node_abspath_len)))
  {
    msg = system_error_event(&chan);
    msg->code = EINVAL;
    if(parser->bacon_parser->yyu && parser->bacon_parser->yyu->error_len) {
      msg->size += zcatw(msg->text + msg->size, sizeof(msg->text) - msg->size, parser->bacon_parser->yyu->error_str, parser->bacon_parser->yyu->error_len);
    } else if(parser->pattern_parser->yyu && parser->pattern_parser->yyu->error_len) {
      msg->size += zcatw(msg->text + msg->size, sizeof(msg->text) - msg->size, parser->pattern_parser->yyu->error_str, parser->pattern_parser->yyu->error_len);
    } else {
      msg->size += zcatw(msg->text + msg->size, sizeof(msg->text) - msg->size, parser->yyu.error_str, parser->yyu.error_len);
    }
    events_publish(chan, msg);
  }
  else
  {
    STATS_INC(mod->mod_stats.parse_ok);
    STATS_INC(g_stats.module_parse_ok);
  }

  free(text);
}

struct module_scope_context {
  module *mod;
  graph_invalidation_context *invalidation;
};

void module_initialize(fsent * mod_dir_n, fsent * mod_anchor_n, fsent * restrict mod_file_n)
{
  module *mod = 0;

  module_alloc(&mod);

  /* dirnode */
  mod->dir_node_abspath_len = fsent_absolute_path_cat(mod->dir_node_abspath, sizeof(mod->dir_node_abspath), mod_dir_n);
  mod->dir_node = mod_dir_n;
  mod_dir_n->mod = mod;
  vertex_type_set(&mod_dir_n->vertex, VERTEX_MODULE_DIR);

  if(!mod_file_n) {
    /* unit-test only code branch */
    return;
  }

  /* self node */
  mod->self_node_abspath_len = fsent_absolute_path_cat(mod->self_node_abspath, sizeof(mod->self_node_abspath), mod_file_n);
  mod->self_node = mod_file_n;
  mod_file_n->self_mod = mod;

  /* protect the special node from disintegration */
  vertex_protect_set(&mod->vertex, true);

  /* magick the file node */
  vertex_type_set(&mod_file_n->vertex, VERTEX_MODULE_FILE);
}

static void module_anchor_set(module *mod, fsent * mod_anchor_n, graph_invalidation_context * restrict invalidation)
{
  const fsent *fse;

  if(mod->anchor_node == mod_anchor_n) {
    return;
  }

  mod->anchor_node = mod_anchor_n;
  mod->anchor_node_abspath_len = fsent_absolute_path_cat(mod->anchor_node_abspath, sizeof(mod->anchor_node_abspath), mod->anchor_node);
  shadow_link(mod->shadow, MMS("anchor"), mod->anchor_node, 0, 0, invalidation);

  mod->anchor_disjoint = true;
  for(fse = mod->dir_node; fse != g_root; fse = fsent_parent(fse)) {
    if(fse == mod->anchor_node) {
      mod->anchor_disjoint = false;
      break;
    }
  }
}

void module_bootstrap(
    fsent * mod_dir_n
  , fsent * mod_anchor_n
  , fsent * restrict mod_file_n
  , graph_invalidation_context * restrict invalidation
)
{
  uint64_t u64;
  uint64_t token;
  module *mod;

  /* already bootstrapped */
  if(vertex_type_get(&mod_dir_n->vertex) == VERTEX_MODULE_DIR) {
    return;
  }

  module_initialize(mod_dir_n, mod_anchor_n, mod_file_n);
  mod = mod_dir_n->mod;

  special_connect(&mod_file_n->vertex, &mod->vertex, EDGE_MODULE_SPECIAL, invalidation);

  // module-dir realpath hash
  u64 = hash64(0, mod->dir_node_abspath, mod->dir_node_abspath_len);
  mod->id_len = zcatf(mod->id, sizeof(mod->id), "%016"PRIx64, u64);
  token = atomic_fetch_inc(&vertex_token_counter);
  mod->token_len = zcatf(mod->token, sizeof(mod->token), "%016"PRIx64, hash64(u64, MM(token)));

  mod->vertex.label = mod->id;
  mod->vertex.label_len = mod->id_len;

  // build out the modules shadow node
  shadow_create(g_shadow_modules, &mod->shadow, mod->id, 16, VERTEX_SHADOW_MODULE, invalidation);
  shadow_create(mod->shadow, &mod->shadow_requires, MMS("requires"), VERTEX_SHADOW_DIR, invalidation);
  shadow_create(mod->shadow, &mod->shadow_rules, MMS("rules"), VERTEX_SHADOW_DIR, invalidation);
  shadow_create(mod->shadow, &mod->shadow_var, MMS("var"), VERTEX_SHADOW_DIR, invalidation);
  shadow_create(mod->shadow, &mod->shadow_rma, MMS("rma"), VERTEX_SHADOW_DIR, invalidation);
  shadow_link(mod->shadow, MMS("fs"), mod->dir_node, 0, 0, invalidation);

  mod->shadow->self_mod = mod;
  mod->shadow->mod = mod;

  vertex_alloc_probe(&mod->shadow->vertex);
  vertex_alloc_probe(&mod->shadow_requires->vertex);
  vertex_alloc_probe(&mod->shadow_rules->vertex);
  vertex_alloc_probe(&mod->shadow_var->vertex);
  vertex_alloc_probe(&mod->shadow_rma->vertex);

  mod->invalid.reparse = true;

  /* lazy re-module directory nodes */
  fsent_module_epoch++;

  /* module is in-tree if g_base is in its ancestry */
  mod->in_tree = fsent_is_self_or_descendant(g_base, mod->dir_node);

  module_anchor_set(mod, mod_anchor_n, invalidation);
}

void module_rebuild(module * restrict mod, rule_run_context * restrict ctx)
{
  moria_foreach_state st;
  moria_edge *edge;
  struct var *var;
  moria_vertex *base;
  moria_vertex *v;
  module_edge *mme;
  int x;
  fsent *anchor;
  struct module_bootstrap_anchored_context bootstrap_ctx;
  entlist list, *eln;
  struct block *block;

  /* re-establish the module anchor node */
  if(mod->anchor) {
    anchor = anchor_process(mod, mod->anchor);
    if(system_error) {
      return;
    }
    module_anchor_set(mod, anchor, &ctx->invalidation);
  }

  /* bootstrap modules under the anchor */
  if(mod->anchor_node != mod->dir_node && mod->in_tree && mod->anchor_disjoint) {
    bootstrap_ctx = (typeof(bootstrap_ctx)){
        .mod = mod
      , .invalidation = &ctx->invalidation
    };
    moria_traverse_vertices(
        &g_graph
      , &mod->anchor_node->vertex
      , module_bootstrap_anchored_visitor
      , 0
      , (moria_traversal_criteria[]) {{
            edge_travel: EDGE_FSTREE
          , edge_visit : EDGE_FSTREE
          , vertex_travel : VERTEX_FILETYPE_DIR
          , vertex_visit : VERTEX_FILETYPE_DIR
        }}
      , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE
      , &bootstrap_ctx
    );
  }

  /* gather up var.bam files in the ancestry */
  entlist_init(&list);
  for(x = 0; x < 2; x++) {
    if(x == 0) {
      base = &mod->anchor_node->vertex;
    } else if(base == &mod->dir_node->vertex) {
      break;
    } else {
      base = &mod->dir_node->vertex;
    }

    locate_ascending(&list, &mod->varfiles, fsent_name_var, base);
  }

  /* bootstrap and connect to each of those var.bam files */
  entlist_foreach(&list, v, eln) {
    bind_var(mod, containerof(v, fsent, vertex), &ctx->invalidation);
  }
  entlist_destroy(&list);

  /* gather up rules.bam files in the ancestry */
  entlist_init(&list);
  for(x = 0; x < 2; x++) {
    if(x == 0) {
      base = &mod->anchor_node->vertex;
    } else if(base == &mod->dir_node->vertex) {
      break;
    } else {
      base = &mod->dir_node->vertex;
    }

    locate_ascending(&list, &mod->rulesfiles, fsent_name_rules, base);
  }

  /* bootstrap and connect to each of those rules.bam list */
  entlist_foreach(&list, v, eln) {
    module_import_rules_file(mod, containerof(v, fsent, vertex), &ctx->invalidation);
  }
  entlist_destroy(&list);

  /* rebuild effective vars */
  bacon_value_free(parser->bacon_parser, mod->vars);
  mod->vars = 0;

  moria_foreach_edge_down(&st, &mod->vertex, edge) {
    if(edge_type_get(edge) != EDGE_VAR_MOD) {
      continue;
    }

    /* skip edges not refreshed by the above ; they will be excised in module_prune */
    mme = containerof(edge, typeof(*mme), edge);
    if(mme->reconciliation_epoch != reconciliation_epoch) {
      continue;
    }

    var = containerof(edge->B, typeof(*var), vertex);
    var_reparse(var);
    bacon_merge(parser->bacon_parser, &mod->vars, var->val);
  }

  /* pre-render the effective vars to envs strings for exec */
  exec_ifree(&mod->exec);
  if(mod->vars)
  {
    exec_builder_reset(&local_exec_builder);
    exec_render_context_reset(&local_exec_render);
    exec_render_context_configure(&local_exec_render, &local_exec_builder, mod->dir_node, mod, 0, mod->vars, 0);
    exec_render_env_vars(&local_exec_render);
    exec_builder_build(&local_exec_builder, 0);
    exec_builder_take(&local_exec_builder, &mod->exec);
  }

  /* immediate imports -- its necessary to do this here, because it bootstraps and invalidates
   * rules files, which will be parsed by the time module_reimport is called on this module */
  llist_foreach(&mod->blocks, block, lln) {
    block_process_imports(mod, block, ctx);
  }
}

static uint32_t blocks_count_rules(llist * restrict list)
{
  struct block *sb;
  uint32_t c;

  c = 0;
  llist_foreach(list, sb, lln) {
    c += llist_count(&sb->rules);
    c += blocks_count_rules(&sb->blocks);
  }

  return c;
}

bool module_reimport(module * restrict mod, rule_run_context * restrict ctx)
{
  struct block *block;
  struct rules *rules;
  moria_foreach_state st;
  moria_edge *edge;
  bool new;

  /* transitive imports */
  new = false;
  moria_foreach_edge_down(&st, &mod->vertex, edge) {
    if(edge_type_get(edge) != EDGE_RULES_MOD) {
      continue;
    }
    rules = containerof(edge->B, typeof(*rules), vertex);
    llist_foreach(&rules->blocks, block, lln) {
      new |= block_process_imports(mod, block, ctx);
    }
  }

  return new;
}

void module_prune(module * restrict mod, graph_invalidation_context * restrict invalidation)
{
  entlist edges, vertices;
  moria_foreach_state st;
  module_edge *mme;
  moria_edge *edge;

  /* remove module eges not refreshed */
  entlist_init(&edges);
  moria_foreach_edge_down(&st, &mod->vertex, edge) {
    switch(edge_type_get(edge)) {
      case EDGE_RULES_MOD:
      case EDGE_REQUIRES:
      case EDGE_VAR_MOD:
        break;
      default:
        continue;
    }

    mme = containerof(edge, typeof(*mme), edge);
    if(mme->reconciliation_epoch == reconciliation_epoch) {
      continue;
    }

    entlist_push(&edges, mme);
  }

  entlist_init(&vertices);
  graph_disintegrate_bulk(&edges, &vertices, invalidation);
  entlist_destroy(&edges);
  entlist_destroy(&vertices);
}

/*
 * Disconnects rmas attached to this module, and rule-fml edges
 *
 * disconnect rmas connected to this module, including those for rules defined by the module,
 * and for rules defined by models used by the module
 */
void module_disconnect_rules(module * restrict mod, graph_invalidation_context * restrict invalidation)
{
  entlist list, *eln;
  moria_edge *edge;
  moria_foreach_state st;
  rule_module_association *rma;

  /* disconnect rule edges from the module itself, possibly to rules defined in other modules
   * note that this can visit edges also visited by the preceding block */
  entlist_init(&list);
  moria_foreach_edge_down(&st, &mod->vertex, edge) {
    switch(edge_type_get(edge)) {
      case EDGE_RMA_MOD:
        rma = containerof(edge->B, typeof(*rma), vertex);
        entlist_push(&list, rma);
        break;
      case EDGE_RULES_MOD:
      case EDGE_REQUIRES:
      case EDGE_VAR_MOD:
      case EDGE_MODULE_SPECIAL:
      case EDGE_INDEX_MOD:
        break;
      default:
        edge_show(edge);
        RTA;
    }
  }
  entlist_foreach(&list, rma, eln) {
    rule_module_association_release(rma, invalidation);
  }
  entlist_destroy(&list);
}

/* remove a module from the graph */
void module_excise(module * restrict mod, graph_invalidation_context * restrict invalidation)
{
  entlist edges, vertices;
  moria_edge *edge;
  moria_foreach_state st;

  /* visit USES, REQUIRES, and IMPORTS, and SPECIAL edges */
  entlist_init(&edges);
  moria_foreach_edge_up(&st, &mod->vertex, edge) {
    if(!(edge->attrs & EDGE_MODULES)) {
      continue;
    }

    entlist_push(&edges, edge);
  }
  moria_foreach_edge_down(&st, &mod->vertex, edge) {
    if(!(edge->attrs & EDGE_MODULES) && edge_type_get(edge) != EDGE_MODULE_SPECIAL) {
      continue;
    }

    entlist_push(&edges, edge);
  }

  entlist_init(&vertices);
  graph_disintegrate_bulk(&edges, &vertices, invalidation);
  entlist_destroy(&edges);
  entlist_destroy(&vertices);

  /* teardown the modules' shadow tree */
  edge = moria_edge_between(&mod->shadow->vertex, &g_shadow_modules->vertex);
  fsedge_disintegrate(containerof(edge, fsedge, edge), invalidation);

  /* un-magic this dir, cause directory nodes to be reparented */
  vertex_type_set(&mod->dir_node->vertex, VERTEX_DIR);
  fsent_module_epoch++;
}

void module_reparse(module * restrict mod)
{
  /* re-parse the module.bam file */
  module_parse(mod);
  if(system_error) {
    return;
  }

  /* replace the guts with the results from parsing */
  block_release_bulk(parser->bacon_parser, &mod->blocks);
  llist_splice_head(&mod->blocks, &parser->blocks);

  anchor_release(mod->anchor);
  mod->anchor = parser->anchor;
  parser->anchor = 0;

  pattern_list_free(&mod->varfiles);
  llist_splice_head(&mod->varfiles, &parser->varfiles);

  pattern_list_free(&mod->rulesfiles);
  llist_splice_head(&mod->rulesfiles, &parser->rulesfiles);
}

/*
 * mod - module being reloaded
 * rules - rules file which defines this block (can be null)
 * block - block of rules
 */
static void block_reload(
    module * mod
  , struct rules * restrict rules
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
    rule_module_association_create(rule, mod, rules, invalidation);
  }
}

void module_reload(module * restrict mod)
{
  block *block;
  rules *rules;
  moria_foreach_state st;
  moria_edge *edge;
  fsent *fse;
  graph_invalidation_context invalidation = { };
  entlist list, *eln;

  STATS_INC(mod->mod_stats.reloaded);
  STATS_INC(g_stats.module_reloaded);

  graph_invalidation_begin(&invalidation);

  /* reload rules in the module.bam file */
  llist_foreach(&mod->blocks, block, lln) {
    block_reload(mod, 0, block, &invalidation);
  }

  /* reload rules in USES rules.bam files
   * because block_reload can create new edges, its necessary to assemble the list
   * of edges first */
  entlist_init(&list);
  moria_foreach_edge_down(&st, &mod->vertex, edge) {
    if(edge_type_get(edge) != EDGE_RULES_MOD) {
      continue;
    }

    rules = containerof(edge->B, typeof(*rules), vertex);
    entlist_push(&list, rules);
  }

  entlist_foreach(&list, rules, eln) {
    llist_foreach(&rules->blocks, block, lln) {
      block_reload(mod, rules, block, &invalidation);
    }
  }
  entlist_destroy(&list);

  /* prepare for filesystem-reconcile to lazily create the directory structure from
   * the root to the module node, which may not have been created yet because of having
   * an anchor disjoint from the module node */
  for(fse = mod->dir_node; fse != g_root; fse = fsent_parent(fse)) {
    filesystem_mkdir_unshift(fse);
  }

  graph_invalidation_end(&invalidation);
}

void module_rebuild_enqueue(module * restrict mod)
{
//  mod->invalid.exists = true;
  mod->invalid.disconnect = true;
/*
  this is set when the rules file node is invalidated -- other events can cause
  the module to need to be rebuilt (var invalidation etc)
  mod->invalid.reparse = true;
*/
  mod->invalid.rebuild = true;
  mod->invalid.reimport = true;
  mod->invalid.prune = true;

  /* move to the end of the rebuild/invalid list */
  llist_delete(&mod->invalid.lln);
  llist_unshift(&module_rebuild_list, &mod->invalid.lln);
}

//
// internal
//

void module_dispose(module * restrict mod)
{
  block_dispose_bulk(parser->bacon_parser, &mod->blocks);
  if(mod->anchor) {
    anchor_dispose(mod->anchor);
  }
  exec_free(mod->exec);
  bacon_value_free(parser->bacon_parser, mod->vars);
  pattern_list_free(&mod->varfiles);
  pattern_list_free(&mod->rulesfiles);

  llist_delete(&mod->vertex.owner);
  llist_append(&module_freelist, &mod->vertex.owner);
}

void module_release(module * restrict mod, graph_invalidation_context *invalidation)
{
  moria_vertex __attribute__((unused)) *v;

  if(mod->anchor_node && mod->anchor_node != mod->dir_node) {
    /* un-magic the previous anchor dir, lazy reparent */
    vertex_type_set(&mod->anchor_node->vertex, VERTEX_DIR);
    fsent_module_epoch++;
  }

  module_disconnect_rules(mod, invalidation);
  module_excise(mod, invalidation);

  /* must have been fully excised from the graph previously */
  v = &mod->vertex;
  RUNTIME_ASSERT(v->up_identity == 0);
  RUNTIME_ASSERT(rbtree_empty(&v->up));
  RUNTIME_ASSERT(rbtree_empty(&v->down));

  llist_delete(&mod->invalid.lln);

  block_release_bulk(parser->bacon_parser, &mod->blocks);
  anchor_release(mod->anchor);
  exec_free(mod->exec);
  bacon_value_free(parser->bacon_parser, mod->vars);

  vertex_deleted_probe(&mod->vertex);

  llist_delete(&mod->vertex.owner);
  llist_append(&module_freelist, &mod->vertex.owner);
}

//
// public
//

void module_setup()
{
  exec_builder_init(&local_exec_builder);
  exec_render_context_init(&local_exec_render);

  rules_parser_alloc(&parser);
}

void module_cleanup()
{
  module *mod;
  llist *T;

  exec_builder_destroy(&local_exec_builder);
  exec_render_context_destroy(&local_exec_render);

  llist_foreach_safe(&module_list, mod, vertex.owner, T) {
    module_dispose(mod);
  }

  llist_foreach_safe(&module_freelist, mod, vertex.owner, T) {
    free(mod);
  }

  rules_parser_free(parser);
}

size_t module_stats_collate(void *dst, size_t sz, bool reset, module *mod)
{
  bam_module_stats *stats;
  bam_module_stats tmp;
  uint32_t count;
  size_t z;

  /* module vertex stats */
  z = 0;
  z += vertex_stats_collate(dst + z, sz - z, reset, &mod->vertex);

  /* module vertex specific stats */
  memcpy(&tmp, &mod->mod_stats, sizeof(tmp));
  if(reset) {
    memset(&mod->mod_stats, 0, sizeof(mod->stats));
  }
  stats = &tmp;

  /* uses */
  count = 0;
  moria_traverse_vertex_edges(&g_graph, &mod->vertex, moria_edge_count, 0
    , (moria_traversal_criteria[]) {{
          edge_travel : EDGE_RULES_MOD
        , edge_visit : EDGE_RULES_MOD
        , min_depth : 0
        , max_depth : 0
      }}
    , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE | MORIA_TRAVERSE_DEPTH
    , &count
  );
  stats->uses_down = count;

  /* requires */
  count = 0;
  moria_traverse_vertex_edges(&g_graph, &mod->vertex, moria_edge_count, 0
    , (moria_traversal_criteria[]) {{
          edge_travel : EDGE_REQUIRES
        , edge_visit : EDGE_REQUIRES
        , min_depth : 0
        , max_depth : 0
      }}
    , MORIA_TRAVERSE_UP | MORIA_TRAVERSE_PRE | MORIA_TRAVERSE_DEPTH
    , &count
  );
  stats->requires_up = count;

  count = 0;
  moria_traverse_vertex_edges(&g_graph, &mod->vertex, moria_edge_count, 0
    , (moria_traversal_criteria[]) {{
          edge_travel : EDGE_REQUIRES
        , edge_visit : EDGE_REQUIRES
        , min_depth : 0
        , max_depth : 0
      }}
    , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE | MORIA_TRAVERSE_DEPTH
    , &count
  );
  stats->requires_down = count;

  /* modules_up */
  count = 0;
  moria_traverse_vertices(&g_graph, &mod->dir_node->vertex, moria_vertex_count_once, 0
    , (moria_traversal_criteria[]) {{
          edge_travel : EDGE_FSTREE
        , vertex_visit : VERTEX_MODULE_DIR
        , min_depth : 1
        , max_depth : UINT16_MAX
      }}
    , MORIA_TRAVERSE_UP | MORIA_TRAVERSE_PRE | MORIA_TRAVERSE_DEPTH
    , &count
  );
  stats->modules_up = count;

  /* modules_down */
  count = 0;
  moria_traverse_vertices(&g_graph, &mod->dir_node->vertex, moria_vertex_count_once, 0
    , (moria_traversal_criteria[]) {{
          edge_travel : EDGE_FSTREE
        , vertex_visit : VERTEX_MODULE_DIR
        , min_depth : 1
        , max_depth : UINT16_MAX
      }}
    , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE | MORIA_TRAVERSE_DEPTH
    , &count
  );
  stats->modules_down = count;

  /* rules in the module.bam file */
  stats->rules = blocks_count_rules(&mod->blocks);

  z += descriptor_marshal_object(dst + z, sz - z, &descriptor_bam_module_stats, &stats->record, 0);

  return z;
}
