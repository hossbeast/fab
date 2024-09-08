/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "moria.h"
#include "moria/traverse.h"
#include "moria/foreach.h"
#include "moria/vertex.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "xlinux/xstdlib.h"
#include "common/attrs.h"
#include "bacon/builder.h"
#include "common/hash.h"

#include "vertex.h"
#include "graph.h"
#include "rule_module_association.h"
#include "dependency.h"
#include "fsent.h"
#include "formula.h"
#include "varset.h"
#include "channel.h"
#include "stats.h"
#include "events.h"
#include "ruleset.h"
#include "edge.h"
#include "rule.h"
#include "configset.h"
#include "config.h"
#include "plugin.h"
#include "zcat.h"
#include "marshal.h"
#include "system.h"
#include "rule_system.h"
#include "vertex/probes.h"
#include "buildplan_entity.h"
#include "entlist.h"
#include "config_system.h"
#include "fsname.h"
#include "shadow.h"
#include "goals.h"
#include "params.h"
#include "build.h"
#include "buildplan.h"
#include "fstree.h"
#include "fsindex.h"
#include "notify.h"
#include "path_cache.h"

#include "module_vars.h"
#include "module_envs.h"
#include "module_dir.h"

void attrs32_say(uint32_t attrs, const attrs32 * restrict list, struct narrator * restrict N)
{
  char buf[128];
  char *s;
  size_t z;
  size_t sz;

  s = buf;
  sz = sizeof(buf);
  z = 0;
  z += attrs32_zcat(s + z, sz - z, list, attrs);
  narrator_sayw(N, buf, z);
}

void attrs32_describe(uint32_t attrs, const attrs32 * restrict list, bacon_writer * restrict builder_writer)
{
  char buf[128];
  char *s;
  size_t z;
  size_t sz;

  s = buf;
  sz = sizeof(buf);
  z = 0;
  z += attrs32_zcat(s + z, sz - z, list, attrs);
  BYTES(buf, z);
}

void vertex_invalidate_direct(moria_vertex * restrict v, graph_invalidation_context * restrict invalidation)
{
  vertex_invalidate_visitor(v, invalidation, MORIA_TRAVERSE_POST, 0, 0);
}

void vertex_invalidate(moria_vertex * restrict v, graph_invalidation_context * restrict invalidation)
{
  RUNTIME_ASSERT(invalidation->vertex_traversal);
  RUNTIME_ASSERT(invalidation->edge_traversal);

  moria_traverse_vertices(
      &g_graph
    , v
    , vertex_invalidate_visitor
    , invalidation->vertex_traversal
    , (moria_traversal_criteria[]) {{
          edge_travel : EDGE_INVALIDATIONS
      }}
    , MORIA_TRAVERSE_UP | MORIA_TRAVERSE_PRE
    , invalidation
  );
}

void vertex_invalidate_visitor_first(
    moria_vertex * v
  , void * arg
  , moria_traversal_mode mode
  , int distance
  , moria_traverse_result * restrict result
)
{
  result->flags = MORIA_TRAVERSE_PRUNE;
  vertex_invalidate_visitor(v, arg, mode, distance, 0);
}

bool vertex_mark_stale(moria_vertex * v)
{
  events_context ectx;
  event_bacon *msg;
  fsent *fse = 0;
  module *mod;
  ruleset *rules;
  struct varset *var;
  struct configset *config;
  struct plugin *plugin;
  stats_vertex *vertex_stats = 0;
  formula *fml;
  rule_module_association *rma;
  bacon_writer writer, *builder_writer;
  narrator_fixed fixed;

  if(vertex_invalid_get(v)) {
    return false;
  }

  if(vertex_kind_get(v) == VERTEX_KIND_FSENT) {
    fse = containerof(v, typeof(*fse), vertex);
  }

  switch(vertex_type_get(v))
  {
    /* some magic nodes that are never invalid */
    case VERTEX_RULE:
    case VERTEX_CONTEXT_MODULE:
    case VERTEX_BPE:
    case VERTEX_FSINDEX:
    case VERTEX_NOTIFY:
    case VERTEX_FSTREE:
    case VERTEX_PATH_CACHE:
    case VERTEX_FSNAME:
    case VERTEX_CONFIG:
    case VERTEX_GOALS:
    case VERTEX_BUILD:
    case VERTEX_BUILDPLAN:
    case VERTEX_MODULE_VARS:
    case VERTEX_MODULE_ENVS:
    case VERTEX_SYSTEM:
    case VERTEX_FILESYSTEM:
      return false;
    /* directories are not invalidated */
    case VERTEX_DIR:
    case VERTEX_MODULE_DIR:
    case VERTEX_SHADOW_DIR:
      return false;
    case VERTEX_SYMLINK:
    case VERTEX_SHADOW_LINK:
      return false;
    /* files */
    case VERTEX_FILE:
    case VERTEX_CONFIGSET_FILE:
    case VERTEX_FORMULA_FILE:
    case VERTEX_SHADOW_FILE:
    case VERTEX_VARSET_FILE:
    case VERTEX_PLUGIN_FILE:
      vertex_stats = (void*)&fse->stats;
      break;
    /* specials */
    case VERTEX_RULESET_FILE:
      fse->self_rules->invalid.reparse = true;
      vertex_stats = (void*)&fse->stats;
      break;
    case VERTEX_RULESET:
      rules = containerof(v, typeof(*rules), vertex);
      ruleset_rebuild_enqueue(rules);
      vertex_stats = (void*)&rules->stats;
      break;
    case VERTEX_MODULE_FILE:
      fse->self_mod->invalid.reparse = true;
      vertex_stats = (void*)&fse->stats;
      break;
    case VERTEX_MODULE:
      mod = containerof(v, typeof(*mod), vertex);
      module_rebuild_enqueue(mod);
      vertex_stats = (void*)&mod->stats;
      break;
    case VERTEX_VARSET:
      var = containerof(v, typeof(*var), vertex);
      /* vars are parsed on-demand during module_reconcile */
      vertex_stats = (void*)&var->stats;
      break;
    case VERTEX_CONFIGSET:
      config_system_invalidate();
      config = containerof(v, typeof(*config), vertex);
      vertex_stats = (void*)&config->stats;
      break;
    case VERTEX_PLUGIN:
      plugin = containerof(v, typeof(*plugin), vertex);
      vertex_stats = (void*)&plugin->stats;
      break;
    case VERTEX_FORMULA:
      fml = containerof(v, typeof(*fml), vertex);
      formula_invalidated(fml);
      vertex_stats = (void*)&fml->stats;
      break;
    case VERTEX_RMA:
      rma = containerof(v, typeof(*rma), vertex);
      vertex_stats = (void*)&rma->stats;
      rule_system_work_enqueue(&rma->work);
      break;
      RTA;
  }

  vertex_invalid_set(v);

  vertex_stats->stale++;
  system_stats.vertex_stale++;

  if((msg = events_bacon_would(&ectx, BAMIPC_EVENT_NODE_STALE)))
  {
    narrator_fixed_init(&fixed, msg->text, sizeof(msg->text));
    bacon_writer_init(&writer);
    builder_writer = bacon_writer_open(&writer, &fixed.base);

    { MAPPING
      STRING("node-stale")
      { SET
        { MAPPING
          STRING("id")
          vertex_id_describe(v, builder_writer);
        }
        { MAPPING
          STRING("path")
          vertex_path_describe(v, builder_writer);
        }
      }
    }

    bacon_writer_close(&writer);
    msg->id = 0;
    msg->size = fixed.l;
    events_publish(&ectx);
  }

  return true;
}

/*
 * moria callback to invalidate a node
 *
 * arg      - invalidation
 * mode     - unused
 * distance - unused
 * result   - unused
 */
void vertex_invalidate_visitor(
    moria_vertex * v
  , void * arg
  , moria_traversal_mode mode
  , int distance
  , moria_traverse_result * restrict result
)
{
  struct graph_invalidation_context *invalidation;

  if(!vertex_mark_stale(v)) {
    return;
  }

  invalidation = arg;
  invalidation->any = true;

  if(vertex_kind_get(v) == VERTEX_KIND_FSENT) {
    fsent_freshen_candidate(containerof(v, fsent, vertex));
  }
}

bool vertex_mark_fresh(moria_vertex * restrict v)
{
  events_context ectx;
  event_bacon *msg;
  fsent *fse = 0;
  stats_vertex *vertex_stats = 0;
  module *mod;
  ruleset *rules;
  struct varset *var;
  struct configset *config;
  struct plugin *plugin;
  formula *fml;
  rule_module_association *rma;
  bacon_writer writer, *builder_writer;
  narrator_fixed fixed;

  if(vertex_kind_get(v) == VERTEX_KIND_FSENT) {
    fse = containerof(v, typeof(*fse), vertex);
  }

  if(vertex_state_get(v) == VERTEX_OK) {
    return false;
  }

  vertex_state_set(v, VERTEX_OK);

  switch(vertex_type_get(v)) {
    case VERTEX_MODULE_FILE:
    case VERTEX_VARSET_FILE:
    case VERTEX_RULESET_FILE:
    case VERTEX_CONFIGSET_FILE:
    case VERTEX_PLUGIN_FILE:
    case VERTEX_FORMULA_FILE:
    case VERTEX_FILE:
    case VERTEX_SHADOW_FILE:
      fse = containerof(v, typeof(*fse), vertex);
      vertex_stats = (void*)&fse->stats;
      break;
    /* meta */
    case VERTEX_RMA:
      rma = containerof(v, typeof(*rma), vertex);
      vertex_stats = (void*)&rma->stats;
      break;
    /* special */
    case VERTEX_RULESET:
      rules = containerof(v, typeof(*rules), vertex);
      vertex_stats = (void*)&rules->stats;
      break;
    case VERTEX_MODULE:
      mod = containerof(v, typeof(*mod), vertex);
      vertex_stats = (void*)&mod->stats;
      break;
    case VERTEX_VARSET:
      var = containerof(v, typeof(*var), vertex);
      vertex_stats = (void*)&var->stats;
      break;
    case VERTEX_CONFIGSET:
      config = containerof(v, typeof(*config), vertex);
      vertex_stats = (void*)&config->stats;
      break;
    case VERTEX_PLUGIN:
      plugin = containerof(v, typeof(*plugin), vertex);
      vertex_stats = (void*)&plugin->stats;
      break;
    case VERTEX_FORMULA:
      fml = containerof(v, typeof(*fml), vertex);
      vertex_stats = (void*)&fml->stats;
      break;
    default:
      vertex_shape_show(v);
      RTA;
  }

  vertex_stats->fresh++;
  system_stats.vertex_fresh++;

  if((msg = events_bacon_would(&ectx, BAMIPC_EVENT_NODE_FRESH)))
  {
    narrator_fixed_init(&fixed, msg->text, sizeof(msg->text));
    bacon_writer_init(&writer);
    builder_writer = bacon_writer_open(&writer, &fixed.base);

    { MAPPING
      STRING("node-fresh")
      { SET
        { MAPPING
          STRING("id")
          vertex_id_describe(v, builder_writer);
        }
        { MAPPING
          STRING("path")
          vertex_path_describe(v, builder_writer);
        }
      }
    }

    bacon_writer_close(&writer);
    msg->id = 0;
    msg->size = fixed.l;
    events_publish(&ectx);
  }

  return true;
}

bool vertex_invalid_get(const moria_vertex * restrict v)
{
  if(v->attrs & VERTEX_INVALID_BIT) {
    return true;
  }

  return false;
}

void vertex_invalid_set(moria_vertex * restrict v)
{
  uint32_t attrs;

  attrs = v->attrs;
  attrs &= ~VERTEX_INVALID_BIT;
  attrs |= VERTEX_INVALID_BIT;

  vertex_attrs_set(v, attrs);
}

size_t vertex_path_cat(void * restrict dst, size_t sz, const moria_vertex * v)
{
  switch(vertex_type_get(v)) {
    case VERTEX_DIR:
    case VERTEX_FILE:
    case VERTEX_SYMLINK:
    case VERTEX_SHADOW_DIR:
    case VERTEX_SHADOW_FILE:
    case VERTEX_SHADOW_LINK:
    case VERTEX_MODULE_DIR:
    case VERTEX_MODULE_FILE:
    case VERTEX_RULESET_FILE:
    case VERTEX_FORMULA_FILE:
    case VERTEX_VARSET_FILE:
    case VERTEX_CONFIGSET_FILE:
    case VERTEX_PLUGIN_FILE:
      return fsent_absolute_path_cat(dst, sz, containerof(v, fsent, vertex));
    case VERTEX_PATH_CACHE:
      return zcats(dst, sz, "//path-cache");
    case VERTEX_FSINDEX:
      return zcats(dst, sz, "//index");
    case VERTEX_NOTIFY:
      return zcats(dst, sz, "//notify");
    case VERTEX_CONFIG:
      return zcats(dst, sz, "//config");
    case VERTEX_GOALS:
      return zcats(dst, sz, "//goals");
    case VERTEX_BUILD:
      return zcats(dst, sz, "//build");
    case VERTEX_BUILDPLAN:
      return zcats(dst, sz, "//buildplan");
    case VERTEX_CONTEXT_MODULE:
      return zcats(dst, sz, "//module");
    case VERTEX_SYSTEM:
      return zcats(dst, sz, "//system");
    case VERTEX_FILESYSTEM:
      return zcats(dst, sz, "//filesystem");
    case VERTEX_FSTREE:
      return fstree_path_cat(dst, sz, containerof(v, fstree, vertex));
    case VERTEX_FSNAME:
      return fsname_path_cat(dst, sz, containerof(v, fsname, vertex));
    case VERTEX_RULE:
      return rule_path_cat(dst, sz, containerof(v, rule, vertex));
    case VERTEX_MODULE:
      return module_path_cat(dst, sz, containerof(v, module, vertex));
    case VERTEX_FORMULA:
      return formula_path_cat(dst, sz, containerof(v, formula, vertex));
    case VERTEX_VARSET:
      return varset_path_cat(dst, sz, containerof(v, varset, vertex));
    case VERTEX_CONFIGSET:
      return configset_path_cat(dst, sz, containerof(v, configset, vertex));
    case VERTEX_PLUGIN:
      return plugin_path_cat(dst, sz, containerof(v, plugin, vertex));
    case VERTEX_RULESET:
      return ruleset_path_cat(dst, sz, containerof(v, ruleset, vertex));
    case VERTEX_RMA:
      return rma_path_cat(dst, sz, containerof(v, rule_module_association, vertex));
    case VERTEX_BPE:
      return bpe_path_cat(dst, sz, containerof(v, buildplan_entity, vertex));
    case VERTEX_MODULE_VARS:
      return module_vars_path_cat(dst, sz, containerof(v, typeof(module_vars), vertex));
    case VERTEX_MODULE_ENVS:
      return module_envs_path_cat(dst, sz, containerof(v, typeof(module_envs), vertex));
  }

  vertex_identity_show(v);
  RTA;
  return 0;
}

void vertex_path_say(const moria_vertex * restrict v, narrator * restrict N)
{
  char path[PATH_MAX];
  size_t pathl;

  pathl = vertex_path_cat(path, sizeof(path), v);
  xsayw(path, pathl);
}

void vertex_path_describe(const moria_vertex * restrict v, bacon_writer * restrict writer)
{
  char path[PATH_MAX];
  size_t pathl;

  pathl = vertex_path_cat(path, sizeof(path), v);
  bacon_writer_bytes(writer, path, pathl);
}

size_t vertex_id_cat(void * restrict dst, size_t sz, const moria_vertex * v)
{
  const fsent *fse;
  const rule *rule;
  const module *module;
  const formula *formula;
  const varset *var;
  const configset *config;
  const plugin *plugin;
  const ruleset *rules;
  const rule_module_association *rma;
  const buildplan_entity *bpe;
  const fstree *fst;
  const module_vars *vars;
  const module_envs *envs;

  switch(vertex_type_get(v)) {
    /* fsents */
    case VERTEX_DIR:
    case VERTEX_MODULE_DIR:
    case VERTEX_FILE:
    case VERTEX_SYMLINK:
    case VERTEX_MODULE_FILE:
    case VERTEX_RULESET_FILE:
    case VERTEX_FORMULA_FILE:
    case VERTEX_VARSET_FILE:
    case VERTEX_CONFIGSET_FILE:
    case VERTEX_PLUGIN_FILE:
    case VERTEX_SHADOW_DIR:
    case VERTEX_SHADOW_FILE:
    case VERTEX_SHADOW_LINK:
      fse = containerof(v, typeof(*fse), vertex);
      return zcatw(dst, sz, fse->id, fse->id_len);
    /* specials */
    case VERTEX_MODULE:
      module = containerof(v, typeof(*module), vertex);
      return zcatw(dst, sz, module->id, module->id_len);
    case VERTEX_FORMULA:
      formula = containerof(v, typeof(*formula), vertex);
      return zcatw(dst, sz, formula->id, formula->id_len);
    case VERTEX_VARSET:
      var = containerof(v, typeof(*var), vertex);
      return zcatw(dst, sz, var->id, var->id_len);
    case VERTEX_CONFIGSET:
      config = containerof(v, typeof(*config), vertex);
      return zcatw(dst, sz, config->id, config->id_len);
    case VERTEX_PLUGIN:
      plugin = containerof(v, typeof(*plugin), vertex);
      return zcatw(dst, sz, plugin->id, plugin->id_len);
    case VERTEX_RULESET:
      rules = containerof(v, typeof(*rules), vertex);
      return zcatw(dst, sz, rules->id, rules->id_len);
    /* other */
    case VERTEX_RULE:
      rule = containerof(v, typeof(*rule), vertex);
      return zcatw(dst, sz, rule->id, rule->id_len);
    case VERTEX_RMA:
      rma = containerof(v, typeof(*rma), vertex);
      return zcatw(dst, sz, rma->id, rma->id_len);
    case VERTEX_BPE:
      bpe = containerof(v, typeof(*bpe), vertex);
      return zcatw(dst, sz, bpe->id, bpe->id_len);
    case VERTEX_FSTREE:
      fst = containerof(v, typeof(*fst), vertex);
      return zcatw(dst, sz, fst->id, fst->id_len);
    case VERTEX_MODULE_VARS:
      vars = containerof(v, typeof(*vars), vertex);
      return zcatw(dst, sz, vars->id, vars->id_len);
    case VERTEX_MODULE_ENVS:
      envs = containerof(v, typeof(*envs), vertex);
      return zcatw(dst, sz, envs->id, envs->id_len);

    /* magicks */
    case VERTEX_FSINDEX:
      return fsindex_id_cat(dst, sz);
    case VERTEX_NOTIFY:
      return notify_id_cat(dst, sz);
    case VERTEX_BUILD:
      return buildplan_id_cat(dst, sz);
    case VERTEX_BUILDPLAN:
      return buildplan_id_cat(dst, sz);
    case VERTEX_SYSTEM:
      return system_id_cat(dst, sz);
    case VERTEX_FILESYSTEM:
      return filesystem_id_cat(dst, sz);

    case VERTEX_GOALS:
    case VERTEX_CONTEXT_MODULE:
    case VERTEX_PATH_CACHE:
    case VERTEX_CONFIG:

    case VERTEX_FSNAME:
  }

  vertex_identity_show(v);
  RTA;
  return 0;
}

void vertex_id_describe(const moria_vertex * restrict v, bacon_writer * restrict builder_writer)
{
  char id[256];
  size_t id_len;

  if((id_len = vertex_id_cat(id, sizeof(id), v))) {
    BYTES(id, id_len)
  } else {
    STRING("_")
  }
}

struct collate_node_handlers {
  descriptor_marshal_handlers handlers;
  const moria_vertex *v;
};

void vertex_state_set(moria_vertex * restrict v, vertex_state state)
{
  uint32_t attrs;

  RUNTIME_ASSERT((state & VERTEX_STATE_OPT) == state);

  attrs = v->attrs;
  attrs &= ~VERTEX_STATE_OPT;
  attrs |= state;

  vertex_attrs_set(v, attrs);
}

void vertex_attrs_set(moria_vertex * restrict v, uint32_t attrs)
{
  uint32_t before;

  if(attrs == v->attrs) {
    return;
  }

  before = v->attrs;
  v->attrs = attrs;

  vertex_state_probe(v, before);
}

void vertex_describe(const moria_vertex * restrict v, bacon_writer * restrict builder_writer)
{
  const fsent *fse;

  switch(vertex_type_get(v)) {
    /* fsents */
    case VERTEX_DIR:
    case VERTEX_FILE:
    case VERTEX_SHADOW_FILE:
    case VERTEX_SHADOW_DIR:
      return fsent_describe(containerof(v, fsent, vertex), builder_writer);
    case VERTEX_SYMLINK:
    case VERTEX_SHADOW_LINK:
      fse = containerof(v, typeof(*fse), vertex);
      { MAPPING
        STRING("ref")
        bacon_writer_push_string(builder_writer);
        if(v->ref) {
          vertex_path_say(v->ref, builder_writer->N);
        } else {
          narrator_says(builder_writer->N, "-none-");
        }
        bacon_writer_pop_string(builder_writer);
      }
      break;
    /* other vertices */
    case VERTEX_FSNAME:
      return fsname_describe(containerof(v, fsname, vertex), builder_writer);
    case VERTEX_FSTREE:
      return fstree_describe(containerof(v, fstree, vertex), builder_writer);
    case VERTEX_RMA:
      return rma_describe(containerof(v, typeof(rule_module_association), vertex), builder_writer);
    case VERTEX_BPE:
      return bpe_describe(containerof(v, typeof(buildplan_entity), vertex), builder_writer);
    case VERTEX_RULE:
      return rule_describe(containerof(v, typeof(rule), vertex), builder_writer);
    case VERTEX_RULESET:
      return ruleset_describe(containerof(v, typeof(ruleset), vertex), builder_writer);
    case VERTEX_RULESET_FILE:
      return ruleset_file_describe(containerof(v, fsent, vertex), builder_writer);
    case VERTEX_FORMULA:
      return formula_describe(containerof(v, typeof(formula), vertex), builder_writer);
    case VERTEX_FORMULA_FILE:
      return formula_file_describe(containerof(v, fsent, vertex), builder_writer);
    case VERTEX_MODULE:
      return module_describe(containerof(v, typeof(module), vertex), builder_writer);
    case VERTEX_MODULE_DIR:
      return module_dir_describe(containerof(v, typeof(fsent), vertex), builder_writer);
    case VERTEX_MODULE_FILE:
      return module_file_describe(containerof(v, fsent, vertex), builder_writer);
    case VERTEX_CONFIGSET:
      return configset_describe(containerof(v, configset, vertex), builder_writer);
    case VERTEX_CONFIGSET_FILE:
      return configset_file_describe(containerof(v, fsent, vertex), builder_writer);
    case VERTEX_VARSET:
      return varset_describe(containerof(v, varset, vertex), builder_writer);
    case VERTEX_VARSET_FILE:
      return varset_file_describe(containerof(v, fsent, vertex), builder_writer);
    case VERTEX_PLUGIN:
      return plugin_describe(containerof(v, plugin, vertex), builder_writer);
    case VERTEX_PLUGIN_FILE:
      return plugin_file_describe(containerof(v, fsent, vertex), builder_writer);
    case VERTEX_MODULE_VARS:
      return module_vars_describe(containerof(v, module_vars, vertex), builder_writer);
    case VERTEX_MODULE_ENVS:
      return module_envs_describe(containerof(v, module_envs, vertex), builder_writer);

    /* magicks */
    case VERTEX_CONFIG:
      return config_vertex_describe(builder_writer);
    case VERTEX_GOALS:
      return goals_describe(builder_writer);
    case VERTEX_FSINDEX:
      return fsindex_describe(builder_writer);
    case VERTEX_NOTIFY:
      return notify_describe(builder_writer);
    case VERTEX_BUILD:
      return build_describe(builder_writer);
    case VERTEX_BUILDPLAN:
      return buildplan_describe(builder_writer);
    case VERTEX_PATH_CACHE:
      return path_cache_describe(builder_writer);
    case VERTEX_SYSTEM:
      return system_describe(builder_writer);
    case VERTEX_FILESYSTEM:
      return filesystem_describe(builder_writer);

    /* unimpl */
    case VERTEX_CONTEXT_MODULE:
      RTA;
  }
}

void vertex_describe_edges(const moria_vertex * restrict v, bacon_writer * restrict builder_writer)
{
  moria_foreach_state st;
  moria_edge *e;

  { SET
    { MAPPING
      STRING("vertex")
      { SET
        vertex_describe(v, builder_writer);
        { MAPPING
          STRING("edges")
          { LIST
            moria_foreach_edge_up(&st, v, e) {
              edge_describe(e, builder_writer);
            }
            moria_foreach_edge_down(&st, v, e) {
              edge_describe(e, builder_writer);
            }
          }
        }
      }
    }
  }
}

void vertex_shape_say(const moria_vertex * restrict v, narrator * restrict N)
{
  bacon_writer writer;

  bacon_writer_init(&writer);
  bacon_writer_open(&writer, N);
  vertex_describe(v, &writer);
  bacon_writer_destroy(&writer);
}

void vertex_shape_show(const moria_vertex * restrict v)
{
  bacon_writer writer, *builder_writer;

  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, g_narrator_stdout);

  vertex_describe(v, builder_writer);

  bacon_writer_close(&writer);
  bacon_writer_destroy(&writer);
  printf("\n");
}

void vertex_show(const moria_vertex * restrict v)
{
  vertex_shape_show(v);
}

void vertex_identity_say(const moria_vertex * restrict v, struct narrator * restrict N)
{
  switch(vertex_type_get(v))
  {
    case VERTEX_DIR:
    case VERTEX_FILE:
    case VERTEX_SYMLINK:
    case VERTEX_SHADOW_DIR:
    case VERTEX_SHADOW_FILE:
    case VERTEX_SHADOW_LINK:
    case VERTEX_PATH_CACHE:
    case VERTEX_CONFIG:
    case VERTEX_SYSTEM:
    case VERTEX_FILESYSTEM:
    case VERTEX_GOALS:
    case VERTEX_BUILD:
    case VERTEX_BUILDPLAN:
    case VERTEX_FSINDEX:
    case VERTEX_NOTIFY:
    case VERTEX_CONTEXT_MODULE:
    case VERTEX_MODULE_VARS:
    case VERTEX_MODULE_ENVS:
      attrs32_say(v->attrs, graph_vertex_attrs, N);
      narrator_says(N, " ");
      return vertex_path_say(v, N);

    case VERTEX_RMA:
      return rma_identity_say(containerof(v, rule_module_association, vertex), N);
    case VERTEX_FSTREE:
      return fstree_identity_say(containerof(v, fstree, vertex), N);
    case VERTEX_FSNAME:
      return fsname_identity_say(containerof(v, fsname, vertex), N);
    case VERTEX_RULE:
      return rule_identity_say(containerof(v, rule, vertex), N);
    case VERTEX_PLUGIN:
      return plugin_identity_say(containerof(v, plugin, vertex), N);
    case VERTEX_PLUGIN_FILE:
      return plugin_file_identity_say(containerof(v, fsent, vertex), N);
    case VERTEX_BPE:
      return bpe_identity_say(containerof(v, buildplan_entity, vertex), N);
    case VERTEX_MODULE_DIR:
      return module_dir_identity_say(containerof(v, fsent, vertex), N);
    case VERTEX_MODULE_FILE:
      return module_file_identity_say(containerof(v, fsent, vertex), N);
    case VERTEX_MODULE:
      return module_identity_say(containerof(v, module, vertex), N);
    case VERTEX_RULESET_FILE:
      return ruleset_file_identity_say(containerof(v, fsent, vertex), N);
    case VERTEX_RULESET:
      return ruleset_identity_say(containerof(v, ruleset, vertex), N);
    case VERTEX_FORMULA_FILE:
      return formula_file_identity_say(containerof(v, fsent, vertex), N);
    case VERTEX_FORMULA:
      return formula_identity_say(containerof(v, formula, vertex), N);
    case VERTEX_VARSET_FILE:
      return varset_file_identity_say(containerof(v, fsent, vertex), N);
    case VERTEX_VARSET:
      return varset_identity_say(containerof(v, varset, vertex), N);
    case VERTEX_CONFIGSET_FILE:
      return configset_file_identity_say(containerof(v, fsent, vertex), N);
    case VERTEX_CONFIGSET:
      return configset_identity_say(containerof(v, configset, vertex), N);

    RTA;
  }

  vertex_show(v);
  RTA;
}

size_t vertex_identity_cat(void * restrict s, size_t sz, const moria_vertex * restrict v)
{
  narrator_fixed fixed;

  narrator_fixed_init(&fixed, s, sz);
  vertex_identity_say(v, &fixed.base);

  return fixed.l;
}

void vertex_identity_describe(const moria_vertex * restrict v, bacon_writer * restrict writer)
{
  bacon_writer_push_string(writer);
  vertex_identity_say(v, writer->N);
  bacon_writer_pop_string(writer);
}

void vertex_identity_show(const moria_vertex * restrict v)
{
  vertex_identity_say(v, g_narrator_stdout);
  printf("\n");
}

void vertex_edges_show(const moria_vertex * restrict v)
{
  bacon_writer writer, *builder_writer;

  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, g_narrator_stdout);

  vertex_describe_edges(v, builder_writer);

  bacon_writer_close(&writer);
  bacon_writer_destroy(&writer);
  printf("\n");
}

uint64_t vertex_compute_hash(moria_vertex * restrict v, uint64_t u64)
{
  RUNTIME_ASSERT(u64);

  return hash64(u64, v->label, v->label_len);
}

void vertex_tell(const moria_vertex * restrict v, struct channel * restrict channel)
{
  switch(vertex_type_get(v)) {
    case VERTEX_CONFIG:
      return config_vertex_tell(channel);
    case VERTEX_BUILDPLAN:
      return buildplan_tell(channel);
    case VERTEX_MODULE_VARS:
      return module_vars_tell(containerof(v, module_vars, vertex), channel);
    case VERTEX_MODULE_ENVS:
      return module_envs_tell(containerof(v, module_envs, vertex), channel);
    case VERTEX_SYSTEM:
      return system_tell(channel);

    case VERTEX_BUILD:
    case VERTEX_DIR:
    case VERTEX_MODULE_DIR:
    case VERTEX_FILE:
    case VERTEX_SYMLINK:
    case VERTEX_MODULE_FILE:
    case VERTEX_RULESET_FILE:
    case VERTEX_FORMULA_FILE:
    case VERTEX_VARSET_FILE:
    case VERTEX_CONFIGSET_FILE:
    case VERTEX_PLUGIN_FILE:
    case VERTEX_SHADOW_DIR:
    case VERTEX_SHADOW_FILE:
    case VERTEX_SHADOW_LINK:
    case VERTEX_GOALS:
    case VERTEX_CONTEXT_MODULE:
    case VERTEX_FSINDEX:
    case VERTEX_NOTIFY:
    case VERTEX_RULE:
    case VERTEX_MODULE:
    case VERTEX_FORMULA:
    case VERTEX_VARSET:
    case VERTEX_CONFIGSET:
    case VERTEX_PLUGIN:
    case VERTEX_RULESET:
    case VERTEX_RMA:
    case VERTEX_BPE:
    case VERTEX_FSTREE:
    case VERTEX_PATH_CACHE:
    case VERTEX_FSNAME:
    case VERTEX_FILESYSTEM:
      return;
  }

  vertex_identity_show(v);
  RTA;
}

moria_vertex *vertex_follow(const moria_vertex * restrict v)
{
  do {
    if(!v) {
      return 0;
    }
    if((v->attrs & MORIA_VERTEX_LINK) == 0) {
      return (moria_vertex *)v;
    }
    v = v->ref;
  } while(1);
}

struct module *vertex_module_get(const moria_vertex * restrict v)
{
  const fsent *fse;
  enum vertex_filetype filetype;

  if((v = vertex_follow(v)) == 0) {
    return 0;
  }

  if(vertex_kind_get(v) == VERTEX_KIND_FSENT) {
    fse = containerof(v, typeof(*fse), vertex);
    filetype = vertex_filetype_get(&fse->vertex);
    if(filetype != VERTEX_FILETYPE_DIR) {
      fse = fsent_parent(fse);
    }

    /* directory nodes have the owning module cached */
    return fse->mod;
  }

  if(vertex_type_get(v) == VERTEX_MODULE) {
    return containerof(v, module, vertex);
  }

  return 0;
}

struct fstree *vertex_fstree_get(const moria_vertex * restrict v)
{
  const fsent *fse;
  enum vertex_filetype filetype;

  if((v = vertex_follow(v)) == 0) {
    return 0;
  }

  if(vertex_kind_get(v) == VERTEX_KIND_FSENT) {
    fse = containerof(v, typeof(*fse), vertex);
    filetype = vertex_filetype_get(&fse->vertex);
    if(filetype != VERTEX_FILETYPE_DIR) {
      fse = fsent_parent(fse);
    }

    RUNTIME_ASSERT(fse->fst);
    return fse->fst;
  }

  if(vertex_type_get(v) == VERTEX_FSTREE) {
    return containerof(v, fstree, vertex);
  }

  return 0;
}

void vertex_mappings(const moria_vertex * restrict v, bacon_writer * restrict builder_writer)
{
  { MAPPING STRING("attrs")
    attrs32_describe(v->attrs, graph_vertex_attrs, builder_writer);
  }
  { MAPPING STRING("id")
    vertex_id_describe(v, builder_writer);
  }
  { MAPPING STRING("address")
    HEX(v)
  }
  { MAPPING STRING("type")
    STRING(attrs32_name_byvalue(graph_vertex_type_attrs, vertex_type_get(v)))
  }
  { MAPPING STRING("state")
    STRING(attrs32_name_byvalue(graph_vertex_state_attrs, vertex_state_get(v)))
  }
}

void vertex_stats_build(moria_vertex * restrict v, bool reset, bacon_writer * restrict builder_writer)
{
  switch(vertex_type_get(v)) {
    /* fsents */
    case VERTEX_DIR:
    case VERTEX_FILE:
    case VERTEX_SHADOW_FILE:
    case VERTEX_SHADOW_DIR:
    case VERTEX_SYMLINK:
    case VERTEX_SHADOW_LINK:
      return fsent_stats_build(containerof(v, fsent, vertex), reset, builder_writer);

    /* other vertices */
    case VERTEX_FSNAME:
      return fsname_stats_build(containerof(v, fsname, vertex), reset, builder_writer);
    case VERTEX_FSTREE:
      return fstree_stats_build(containerof(v, fstree, vertex), reset, builder_writer);
    case VERTEX_RMA:
      return rma_stats_build(containerof(v, typeof(rule_module_association), vertex), reset, builder_writer);
    case VERTEX_BPE:
      return bpe_stats_build(containerof(v, typeof(buildplan_entity), vertex), reset, builder_writer);
    case VERTEX_RULE:
      return rule_stats_build(containerof(v, typeof(rule), vertex), reset, builder_writer);
    case VERTEX_RULESET:
      return ruleset_stats_build(containerof(v, typeof(ruleset), vertex), reset, builder_writer);
    case VERTEX_RULESET_FILE:
      return ruleset_file_stats_build(containerof(v, fsent, vertex), reset, builder_writer);
    case VERTEX_FORMULA:
      return formula_stats_build(containerof(v, typeof(formula), vertex), reset, builder_writer);
    case VERTEX_FORMULA_FILE:
      return formula_file_stats_build(containerof(v, fsent, vertex), reset, builder_writer);
    case VERTEX_MODULE:
      return module_stats_build(containerof(v, typeof(module), vertex), reset, builder_writer);
    case VERTEX_MODULE_DIR:
      return module_dir_stats_build(containerof(v, typeof(fsent), vertex), reset, builder_writer);
    case VERTEX_MODULE_FILE:
      return module_file_stats_build(containerof(v, fsent, vertex), reset, builder_writer);
    case VERTEX_CONFIGSET:
      return configset_stats_build(containerof(v, configset, vertex), reset, builder_writer);
    case VERTEX_CONFIGSET_FILE:
      return configset_file_stats_build(containerof(v, fsent, vertex), reset, builder_writer);
    case VERTEX_VARSET:
      return varset_stats_build(containerof(v, varset, vertex), reset, builder_writer);
    case VERTEX_VARSET_FILE:
      return varset_file_stats_build(containerof(v, fsent, vertex), reset, builder_writer);
    case VERTEX_PLUGIN:
      return plugin_stats_build(containerof(v, plugin, vertex), reset, builder_writer);
    case VERTEX_PLUGIN_FILE:
      return plugin_file_stats_build(containerof(v, fsent, vertex), reset, builder_writer);

    /* globals */
    case VERTEX_CONFIG:
      return config_vertex_stats_build(reset, builder_writer);
    case VERTEX_GOALS:
      return goals_stats_build(reset, builder_writer);
    case VERTEX_FSINDEX:
      return fsindex_stats_build(reset, builder_writer);
    case VERTEX_NOTIFY:
      return notify_stats_build(reset, builder_writer);
    case VERTEX_BUILD:
      return build_stats_build(reset, builder_writer);
    case VERTEX_BUILDPLAN:
      return buildplan_stats_build(reset, builder_writer);
    case VERTEX_PATH_CACHE:
      return path_cache_stats_build(reset, builder_writer);
    case VERTEX_SYSTEM:
      return system_stats_build(reset, builder_writer);
    case VERTEX_FILESYSTEM:
      return filesystem_stats_build(reset, builder_writer);

    case VERTEX_MODULE_VARS:
    case VERTEX_MODULE_ENVS:
    case VERTEX_CONTEXT_MODULE:
      break;
  }
}
