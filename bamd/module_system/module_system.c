/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "bacon/value.h"
#include "valyria/rbtree.h"
#include "common/hash.h"

#include "module/probes.h"
#include "fsent.h"
#include "rule.h"
#include "system.h"
#include "ruleset.h"
#include "reconcile.h"
#include "config.h"
#include "disintegrate.h"

static rbtree rbempty = RBTREE_INITIALIZER;
rbtree *modules_varsets = &rbempty;
rbtree *modules_rulesets = &rbempty;

llist ruleset_rebuilds = LLIST_INITIALIZER(ruleset_rebuilds);
llist module_rebuilds = LLIST_INITIALIZER(module_rebuilds);
static llist module_reparses = LLIST_INITIALIZER(module_reparses);
static llist module_remakes = LLIST_INITIALIZER(module_remakes);
static llist module_reimports = LLIST_INITIALIZER(module_reimports);
static llist module_reloads = LLIST_INITIALIZER(module_reloads);

void module_system_reconcile(rule_run_context * restrict rule_run_ctx)
{
  module *mod = 0;
  llist *T;
  ruleset *rules = 0;
  graph_invalidation_context *invalidation;
  struct module_bootstrap_context bootstrap_ctx;

  invalidation = &rule_run_ctx->invalidation;
  graph_invalidation_begin(invalidation);

  /* bootstrap modules under g_base */
  bootstrap_ctx = (typeof(bootstrap_ctx)){
    .invalidation = &rule_run_ctx->invalidation
  };
  moria_traverse_vertices(
      &g_graph
    , &g_base->vertex
    , module_bootstrap_visitor
    , 0
    , (moria_traversal_criteria[]) {{
          edge_travel: EDGE_FS
        , edge_visit : EDGE_FS
        , vertex_travel : VERTEX_FILETYPE_DIR
        , vertex_visit : VERTEX_FILETYPE_DIR
      }}
    , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE
    , &bootstrap_ctx
  );
  graph_invalidation_end(invalidation);

  /* rebuild invalidated modules and rulesets */
  while(1) {
    if((rules = llist_shift(&ruleset_rebuilds, typeof(*rules), invalid_lln))) {
      ruleset_rebuild(rules, invalidation);

      if(system_has_error) {
        llist_unshift(&ruleset_rebuilds, &rules->invalid_lln);
        break;
      }

      vertex_mark_fresh(&rules->vertex);
      vertex_mark_fresh(&rules->self_node->vertex);

      /* only process modules once all rules are loaded */
      RUNTIME_ASSERT(!llist_attached(&rules->invalid_lln));
      continue;
    }

    if((mod = llist_shift(&module_rebuilds, typeof(*mod), invalid_lln))) {
      module_rebuild(mod, rule_run_ctx);
      RUNTIME_ASSERT(!llist_attached(&mod->invalid_lln));
      llist_unshift(&module_remakes, &mod->invalid_lln);
      continue;
    }

    if((mod = llist_shift(&module_remakes, typeof(*mod), invalid_lln))) {
      module_remake_probe(mod);

      module_remake(mod, rule_run_ctx);

      if(system_has_error) {
        llist_unshift(&module_remakes, &mod->invalid_lln);
        break;
      }

      RUNTIME_ASSERT(!llist_attached(&mod->invalid_lln));
      llist_unshift(&module_reimports, &mod->invalid_lln);

      /* only reimport after all remakes are done */
      continue;
    }

    if((mod = llist_shift(&module_reimports, typeof(*mod), invalid_lln))) {
      module_reimport_probe(mod);

      graph_invalidation_begin(invalidation);
      if(module_reimport(mod, rule_run_ctx)) {
        llist_push(&module_reimports, &mod->invalid_lln);
      } else if(system_has_error) {
        llist_push(&module_reimports, &mod->invalid_lln);
        break;
      } else {
        llist_push(&module_reloads, &mod->invalid_lln);
      }
      graph_invalidation_end(invalidation);

      /* only reload after all imports are done */
      continue;
    }

    if((mod = llist_shift(&module_reloads, typeof(*mod), invalid_lln))) {
      module_reload_probe(mod);
      module_reload(mod, invalidation);

      RUNTIME_ASSERT(!llist_attached(&mod->invalid_lln));
      vertex_mark_fresh(&mod->vertex);
      continue;
    }

    break;
  }

  /* follow anchors */
  moria_traverse_vertices(
      &g_graph
    , &g_base->vertex
    , module_bootstrap_anchor_visitor
    , 0
    , (moria_traversal_criteria[]) {{
          edge_travel: EDGE_FS
        , edge_visit : EDGE_FS
        , vertex_travel : VERTEX_FILETYPE_DIR
        , vertex_visit : VERTEX_FILETYPE_DIR
      }}
    , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE
    , &bootstrap_ctx
  );

  /* cleanup un-referenced modules */
  llist_foreach_safe(&module_list, mod, vertex.owner, T) {
    if(mod->epoch == reconciliation_epoch) {
      continue;
    }

    graph_demolish(&mod->vertex);
  }
}

static void invalidate_primary_visitor(
    moria_vertex *v
  , void *arg
  , moria_traversal_mode mode
  , int distance
  , moria_traverse_result * restrict result
)
{
  fsent *fse;
  graph_invalidation_context * restrict invalidation;

  if(vertex_invalid_get(v)) {
    return;
  }

  fse = containerof(v, typeof(*fse), vertex);
  if(!fsent_is_primary(fse)) {
    return;
  }

  invalidation = arg;
  vertex_invalidate(&fse->vertex, invalidation);
}

void modules_invalidate_primary(graph_invalidation_context * restrict invalidation)
{
  module *mod;

#if 0
should have the same behavior as bam touch with no arguments, which issues this request
but without the overhead of constructing the selection

  select : [
    search : {
      pattern : //bm/modules/*/anchor+(/)
    }
    traverse : {
      graph : fs
      cross-module : true
      cross-fstree : true
      min-distance : 0
      max-distance : 65535
      visit : primary
      nofollow : true
      hidden : true
    }
  ]
  invalidate
]
#endif

  llist_foreach(&module_list, mod, vertex.owner) {
    moria_traverse_vertices(
        &g_graph
      , &mod->anchor_dir->vertex
      , invalidate_primary_visitor
      , 0
      , (moria_traversal_criteria[]) {{
            edge_travel_mask: EDGE_TYPE_OPT
          , edge_travel : EDGE_FS
          , vertex_visit_mask : VERTEX_KIND_OPT
          , vertex_visit : VERTEX_KIND_FSENT
        }}
      , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE | MORIA_TRAVERSE_NOFOLLOW
      , invalidation
    );
  }
}

void module_system_reconfigure(struct config *cfg, bool dryrun, graph_invalidation_context * restrict invalidation)
{
  module *mod;
  config_specials_entry *cse;
  uint64_t h0;
  uint64_t h1;
  bool invalid = false;

  if(dryrun) {
    return;
  }

  h0 = 0;
  if(modules_varsets) {
    rbtree_foreach(modules_varsets, cse, rbn) {
      h0 = hash64(h0, MM(cse->name->hash));
    }
  }
  h1 = 0;
  rbtree_foreach(&cfg->modules.varsets.set, cse, rbn) {
    h1 = hash64(h1, MM(cse->name->hash));
  }
  if(h0 != h1) {
    invalid = true;
  }

  h0 = 0;
  if(modules_rulesets) {
    rbtree_foreach(modules_rulesets, cse, rbn) {
      h0 = hash64(h0, MM(cse->name->hash));
    }
  }
  h1 = 0;
  rbtree_foreach(&cfg->modules.rulesets.set, cse, rbn) {
    h1 = hash64(h1, MM(cse->name->hash));
  }
  if(h0 != h1) {
    invalid = true;
  }

  modules_varsets = &cfg->modules.varsets.set;
  modules_rulesets = &cfg->modules.rulesets.set;

  if(!invalid) {
    return;
  }

  /* when these settings change, invalidate all modules */
  llist_foreach(&module_list, mod, vertex.owner) {
    vertex_invalidate(&mod->vertex, invalidation);
  }
}
