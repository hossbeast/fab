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
#include "namespace.h"
#include "notify.h"

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

void vertex_describe(const moria_vertex * restrict v, bacon_writer * restrict builder_writer)
{
  switch(vertex_type_get(v)) {
    /* fsents */
    case VERTEX_DIR:
      return fsent_describe(containerof(v, fsent, vertex), builder_writer);
    case VERTEX_FILE:
    case VERTEX_SHADOW_FILE:
    case VERTEX_SHADOW_DIR:
      return fsent_describe(containerof(v, fsent, vertex), builder_writer);
    case VERTEX_SYMLINK:
      return fsent_symlink_describe(containerof(v, fsent, vertex), builder_writer);
    case VERTEX_SHADOW_LINK:
      return fsent_shadow_link_describe(containerof(v, fsent, vertex), builder_writer);
    /* other vertices */
    case VERTEX_CLIENT:
      return client_describe(containerof(v, client, vertex), builder_writer);
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
    case VERTEX_FORMULA:
      return formula_describe(containerof(v, typeof(formula), vertex), builder_writer);
    case VERTEX_MODULE:
      return module_describe(containerof(v, typeof(module), vertex), builder_writer);
    case VERTEX_CONFIGSET:
      return configset_describe(containerof(v, configset, vertex), builder_writer);
    case VERTEX_VARSET:
      return varset_describe(containerof(v, varset, vertex), builder_writer);
    case VERTEX_PLUGIN:
      return plugin_describe(containerof(v, plugin, vertex), builder_writer);
    case VERTEX_MODULE_VARS:
    case VERTEX_MODULE_ENVS:
    case VERTEX_PLUGIN_STDIN:
    case VERTEX_PLUGIN_STDOUT:
    case VERTEX_PLUGIN_STDERR:
    case VERTEX_PLUGIN_AUXOUT:
    case VERTEX_FORMULA_STDIN:
    case VERTEX_FORMULA_STDOUT:
    case VERTEX_FORMULA_STDERR:
    case VERTEX_FORMULA_AUXOUT:
      return detail_describe(containerof(v, detail, vertex), builder_writer);
    case VERTEX_NAMESPACE:
      return namespace_describe(containerof(v, namespace, vertex), builder_writer);
    case VERTEX_BUILD_RECORD:
      return build_record_describe(containerof(v, build_record, vertex), builder_writer);
    case VERTEX_FORMULA_RECORD:
      return formula_record_describe(containerof(v, formula_record, vertex), builder_writer);

    /* globals / systems */
    case VERTEX_SYS_SYSTEM:
      return system_sys_describe(v, builder_writer);
    case VERTEX_SYS_BUILD:
      return build_sys_describe(v, builder_writer);
    case VERTEX_SYS_CONFIG:
      return config_sys_describe(v, builder_writer);
    case VERTEX_SYS_FILESYSTEM:
      return filesystem_sys_describe(v, builder_writer);
    case VERTEX_SYS_GOALS:
      return goals_sys_describe(v, builder_writer);
    case VERTEX_SYS_NOTIFY:
      return notify_sys_describe(v, builder_writer);
    case VERTEX_SYS_PLAN:
      return plan_sys_describe(v, builder_writer);
    case VERTEX_SYS_RCU:
      return rcu_sys_describe(v, builder_writer);

    /* unimpl */
    case VERTEX_CONTEXT_MODULE:
      return;
    default:
      vertex_path_describe(v, builder_writer);
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

void vertex_identity_describe_edges(const moria_vertex * restrict v, bacon_writer * restrict builder_writer)
{
  { SET
    { MAPPING
      STRING("vertex")
      { SET
        vertex_identity_describe(v, builder_writer);
        { MAPPING
          STRING("edges")
          identity_edges_list(v, builder_writer);
        }
      }
    }
  }
}

void vertex_identity_describe(const moria_vertex * restrict v, bacon_writer * restrict builder_writer)
{
  moria_edge *e;
  const fsent *fse;

#if CONFIG_DEVEL || CONFIG_DEBUG
  HEX(v)
#endif

  switch(vertex_type_get(v))
  {
    case VERTEX_DIR:
    case VERTEX_FILE:
    case VERTEX_SHADOW_DIR:
    case VERTEX_SHADOW_FILE:
    case VERTEX_SYS_CONFIG:
    case VERTEX_SYS_SYSTEM:
    case VERTEX_SYS_FILESYSTEM:
    case VERTEX_SYS_RCU:
    case VERTEX_SYS_GOALS:
    case VERTEX_SYS_BUILD:
    case VERTEX_SYS_PLAN:
    case VERTEX_SYS_NOTIFY:
    case VERTEX_CONTEXT_MODULE:
    case VERTEX_MODULE_VARS:
    case VERTEX_MODULE_ENVS:
    case VERTEX_FORMULA_STDIN:
    case VERTEX_FORMULA_STDOUT:
    case VERTEX_FORMULA_STDERR:
    case VERTEX_FORMULA_AUXOUT:
    case VERTEX_FORMULA_ARGS:
    case VERTEX_FORMULA_ENVS:
    case VERTEX_FORMULA_FILE:
    case VERTEX_FORMULA_CWD:
    case VERTEX_PLUGIN_STDIN:
    case VERTEX_PLUGIN_STDOUT:
    case VERTEX_PLUGIN_STDERR:
    case VERTEX_PLUGIN_AUXOUT:
      { MAPPING
        attrs32_describe(v->attrs, graph_vertex_attrs, builder_writer);
        vertex_path_describe(v, builder_writer);
      }
      return;
    case VERTEX_SHADOW_LINK:
      { MAPPING
        attrs32_describe(v->attrs, graph_vertex_attrs, builder_writer);
        vertex_path_describe(v, builder_writer);
      }
      { MAPPING
        STRING("ref")
        if(v->ref) {
          vertex_path_describe(v->ref, builder_writer);
        } else {
          STRING("_")
        }
      }
      return;
    case VERTEX_SYMLINK:
      fse = containerof(v, typeof(*fse), vertex);
      { MAPPING
        attrs32_describe(v->attrs, graph_vertex_attrs, builder_writer);
        vertex_path_describe(v, builder_writer);
      }
      { MAPPING
        STRING("target")
        if(fse->symlink_target_len == 0) {
          STRING("_");
        } else {
          BYTES(fse->symlink_target, fse->symlink_target_len);
        }
      }
      { MAPPING
        STRING("ref")
        if((e = edges_check_down(v, EDGE_REF_SYMLINK))) {
          vertex_path_describe(e->B, builder_writer);
        } else {
          STRING("_")
        }
      }
      return;

    case VERTEX_RMA:
      return rma_identity_describe(containerof(v, rule_module_association, vertex), builder_writer);
    case VERTEX_CLIENT:
      return client_identity_describe(containerof(v, client, vertex), builder_writer);
    case VERTEX_FSTREE:
      return fstree_identity_describe(containerof(v, fstree, vertex), builder_writer);
    case VERTEX_NAMESPACE:
      return namespace_identity_describe(containerof(v, namespace, vertex), builder_writer);
    case VERTEX_FSNAME:
      return fsname_identity_describe(containerof(v, fsname, vertex), builder_writer);
    case VERTEX_RULE:
      return rule_identity_describe(containerof(v, rule, vertex), builder_writer);
    case VERTEX_BUILD_RECORD:
      return build_record_identity_describe(containerof(v, build_record, vertex), builder_writer);
    case VERTEX_FORMULA_RECORD:
      return formula_record_identity_describe(containerof(v, formula_record, vertex), builder_writer);
    case VERTEX_PLUGIN:
      return plugin_identity_describe(containerof(v, plugin, vertex), builder_writer);
    case VERTEX_BPE:
      return bpe_identity_describe(containerof(v, buildplan_entity, vertex), builder_writer);
    case VERTEX_MODULE:
      return module_identity_describe(containerof(v, module, vertex), builder_writer);
    case VERTEX_RULESET:
      return ruleset_identity_describe(containerof(v, ruleset, vertex), builder_writer);
    case VERTEX_FORMULA:
      return formula_identity_describe(containerof(v, formula, vertex), builder_writer);
    case VERTEX_VARSET:
      return varset_identity_describe(containerof(v, varset, vertex), builder_writer);
    case VERTEX_CONFIGSET:
      return configset_identity_describe(containerof(v, configset, vertex), builder_writer);
  }

  RTA;
}

void vertex_relations_describe(const moria_vertex * restrict v, bacon_writer * restrict builder_writer)
{
  moria_edge *edge;
  moria_foreach_state st;
  int x;
  bool any;

  any = false;
  moria_foreach_edge_up(&st, v, edge) {
    switch(edge_type_get(edge)) {
      case EDGE_SHADOW_REF:
      case EDGE_FS:
        continue;
      default:
        any = true;
    }
  }
  if(any) {
    MAPPING STRING("up")
    { LIST
      moria_foreach_edge_up(&st, v, edge) {
        switch(edge_type_get(edge)) {
          case EDGE_SHADOW_REF:
          case EDGE_FS:
            continue;
          default:
            break;
        }
        { MAPPING
          attrs32_describe(edge->attrs, graph_edge_attrs, builder_writer);
          if((edge->attrs & MORIA_EDGE_HYPER)) {
            SET
            for(x = 0; x < edge->Alen; x++) {
              if(rbnode_attached(&edge->Alist[x].rbn)) {
                vertex_path_describe(edge->Alist[x].v, builder_writer);
              }
            }
          } else {
            vertex_path_describe(edge->A, builder_writer);
          }
        }
      }
    }
  }

  any = false;
  moria_foreach_edge_down(&st, v, edge) {
    switch(edge_type_get(edge)) {
      case EDGE_SHADOW_REF:
      case EDGE_FS:
        continue;
      default:
        any = true;
    }
  }
  if(any) {
    MAPPING STRING("down")
    { LIST
      moria_foreach_edge_down(&st, v, edge) {
        switch(edge_type_get(edge)) {
          case EDGE_SHADOW_REF:
          case EDGE_FS:
            continue;
          default:
            break;
        }
        { MAPPING
          attrs32_describe(edge->attrs, graph_edge_attrs, builder_writer);
          if((edge->attrs & MORIA_EDGE_HYPER)) {
            SET
            for(x = 0; x < edge->Blen; x++) {
              if(rbnode_attached(&edge->Blist[x].rbn)) {
                vertex_path_describe(edge->Blist[x].v, builder_writer);
              }
            }
          } else {
            vertex_path_describe(edge->B, builder_writer);
          }
        }
      }
    }
  }
}

void vertex_stats_describe(moria_vertex * restrict v, bool reset, bacon_writer * restrict builder_writer)
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
    case VERTEX_CLIENT:
      return client_stats_build(containerof(v, client, vertex), reset, builder_writer);
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
    case VERTEX_FORMULA:
      return formula_stats_build(containerof(v, typeof(formula), vertex), reset, builder_writer);
    case VERTEX_MODULE:
      return module_stats_build(containerof(v, typeof(module), vertex), reset, builder_writer);
    case VERTEX_CONFIGSET:
      return configset_stats_build(containerof(v, configset, vertex), reset, builder_writer);
    case VERTEX_VARSET:
      return varset_stats_build(containerof(v, varset, vertex), reset, builder_writer);
    case VERTEX_PLUGIN:
      return plugin_stats_build(containerof(v, plugin, vertex), reset, builder_writer);
    case VERTEX_NAMESPACE:
      return namespace_stats_build(containerof(v, namespace, vertex), reset, builder_writer);
    case VERTEX_BUILD_RECORD:
      return build_record_stats_build(containerof(v, build_record, vertex), reset, builder_writer);
    case VERTEX_FORMULA_RECORD:
      return formula_record_stats_build(containerof(v, formula_record, vertex), reset, builder_writer);

    /* globals / subsystems */
    case VERTEX_SYS_SYSTEM:
      return system_sys_stats_build(v, reset, builder_writer);
    case VERTEX_SYS_CONFIG:
      return config_sys_stats_build(v, reset, builder_writer);
    case VERTEX_SYS_GOALS:
      return goals_sys_stats_build(v, reset, builder_writer);
    case VERTEX_SYS_NOTIFY:
      return notify_sys_stats_build(v, reset, builder_writer);
    case VERTEX_SYS_BUILD:
      return build_sys_stats_build(v, reset, builder_writer);
    case VERTEX_SYS_PLAN:
      return plan_sys_stats_build(v, reset, builder_writer);
    case VERTEX_SYS_FILESYSTEM:
      return filesystem_sys_stats_build(v, reset, builder_writer);
    case VERTEX_SYS_RCU:
      return rcu_sys_stats_build(v, reset, builder_writer);

    /* details */
    case VERTEX_FORMULA_STDIN:
    case VERTEX_FORMULA_STDOUT:
    case VERTEX_FORMULA_STDERR:
    case VERTEX_FORMULA_AUXOUT:
    case VERTEX_FORMULA_ARGS:
    case VERTEX_FORMULA_ENVS:
    case VERTEX_FORMULA_FILE:
    case VERTEX_FORMULA_CWD:
    case VERTEX_PLUGIN_STDIN:
    case VERTEX_PLUGIN_STDOUT:
    case VERTEX_PLUGIN_STDERR:
    case VERTEX_PLUGIN_AUXOUT:
    case VERTEX_MODULE_VARS:
    case VERTEX_MODULE_ENVS:
    case VERTEX_CONTEXT_MODULE:
      break;
  }
}

void vertex_mappings(const moria_vertex * restrict v, bacon_writer * restrict builder_writer)
{
#if CONFIG_DEVEL
  { MAPPING STRING("address")
    HEX(v)
  }
#endif
  { MAPPING STRING("id")
    vertex_id_describe(v, builder_writer);
  }
  { MAPPING STRING("attrs")
    attrs32_describe(v->attrs, graph_vertex_attrs, builder_writer);
  }
#if CONFIG_DEVEL
  /* type and state are also encoded in attrs */
  { MAPPING STRING("type")
    STRING(attrs32_name_byvalue(graph_vertex_type_attrs, vertex_type_get(v)))
  }
  { MAPPING STRING("state")
    STRING(attrs32_name_byvalue(graph_vertex_state_attrs, vertex_state_get(v)))
  }
#endif
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

struct module *vertex_module_of(const moria_vertex * restrict v)
{
  const fsent *fse;
  enum vertex_filetype filetype;
  const moria_edge *edge;
  module *mod;
  bool shadow;

  if((v = moria_vertex_follow(v)) == 0) {
    return 0;
  }

  if(vertex_type_get(v) == VERTEX_MODULE) {
    return containerof(v, module, vertex);
  }

  if(vertex_kind_get(v) != VERTEX_KIND_FSENT) {
    return 0;
  }

  fse = containerof(v, typeof(*fse), vertex);
  filetype = vertex_filetype_get(&fse->vertex);
  if(filetype != VERTEX_FILETYPE_DIR) {
    fse = fsent_parent(fse);
  }

  shadow = !!(fse->vertex.attrs & VERTEX_SHADOW_BIT);

  while(fse) {
    if((edge = edges_check_up(&fse->vertex, EDGE_BUILD_DIR_MOD))) {
      mod = containerof(edge->A, typeof(*mod), vertex);
      return mod;
    }
    /* N.B. a directory may have multiple anchor-dir-mod edges, this returns the first one */
    if((edge = edges_check_up(&fse->vertex, EDGE_ANCHOR_DIR_MOD))) {
      mod = containerof(edge->A, typeof(*mod), vertex);
      return mod;
    }
    if(shadow && (edge = edges_check_up(&fse->vertex, EDGE_SPECIAL))) {
      if(vertex_type_get(edge->A) == VERTEX_MODULE) {
        mod = containerof(edge->A, typeof(*mod), vertex);
        return mod;
      }
    }

    fse = fsent_parent(fse);
  }

  return 0;
}

struct fstree *vertex_fstree_of(const moria_vertex * restrict v)
{
  const fsent *fse;
  enum vertex_filetype filetype;

  if((v = moria_vertex_follow(v)) == 0) {
    return 0;
  }

  if(vertex_type_get(v) == VERTEX_FSTREE) {
    return containerof(v, fstree, vertex);
  }

  if(vertex_kind_get(v) != VERTEX_KIND_FSENT) {
    return 0;
  }

  fse = containerof(v, typeof(*fse), vertex);
  filetype = vertex_filetype_get(&fse->vertex);
  if(filetype != VERTEX_FILETYPE_DIR) {
    fse = fsent_parent(fse);
  }

  RUNTIME_ASSERT(fse->fst);
  return fse->fst;
}

void vertex_compute_id(const moria_vertex * restrict v, struct identity_ctx * restrict id)
{
  switch(vertex_type_get(v)) {
    case VERTEX_DIR:
    case VERTEX_FILE:
    case VERTEX_SYMLINK:
    case VERTEX_SHADOW_DIR:
    case VERTEX_SHADOW_FILE:
    case VERTEX_SHADOW_LINK:
      return fsent_compute_id(containerof(v, fsent, vertex), id);
    /* details */
    case VERTEX_MODULE_VARS:
    case VERTEX_MODULE_ENVS:
    case VERTEX_PLUGIN_STDIN:
    case VERTEX_PLUGIN_STDOUT:
    case VERTEX_PLUGIN_STDERR:
    case VERTEX_PLUGIN_AUXOUT:
    case VERTEX_FORMULA_STDIN:
    case VERTEX_FORMULA_STDOUT:
    case VERTEX_FORMULA_STDERR:
    case VERTEX_FORMULA_AUXOUT:
    case VERTEX_FORMULA_ARGS:
    case VERTEX_FORMULA_ENVS:
    case VERTEX_FORMULA_FILE:
    case VERTEX_FORMULA_CWD:
      return detail_compute_id(containerof(v, detail, vertex), id);
    /* specials */
    case VERTEX_MODULE:
      return module_compute_id(containerof(v, module, vertex), id);
    case VERTEX_RULE:
      return rule_compute_id(containerof(v, rule, vertex), id);
    case VERTEX_FORMULA:
      return formula_compute_id(containerof(v, formula, vertex), id);
    case VERTEX_VARSET:
      return varset_compute_id(containerof(v, varset, vertex), id);
    case VERTEX_CONFIGSET:
      return configset_compute_id(containerof(v, configset, vertex), id);
    case VERTEX_RULESET:
      return ruleset_compute_id(containerof(v, ruleset, vertex), id);
    /* details */
    case VERTEX_BUILD_RECORD:
      return build_record_compute_id(containerof(v, build_record, vertex), id);
    case VERTEX_FORMULA_RECORD:
      return formula_record_compute_id(containerof(v, formula_record, vertex), id);
    /* composites */
    case VERTEX_RMA:
      return rma_compute_id(containerof(v, rule_module_association, vertex), id);
    case VERTEX_BPE:
      return bpe_compute_id(containerof(v, buildplan_entity, vertex), id);

    /* globals / subsystems */
    case VERTEX_SYS_SYSTEM:
    case VERTEX_SYS_BUILD:
    case VERTEX_SYS_CONFIG:
    case VERTEX_SYS_FILESYSTEM:
    case VERTEX_SYS_GOALS:
    case VERTEX_SYS_NOTIFY:
    case VERTEX_SYS_PLAN:
    case VERTEX_SYS_RCU:
      return subsys_compute_id(containerof(v, subsys, vertex), id);

    case VERTEX_NAMESPACE:
      return namespace_compute_id(containerof(v, namespace, vertex), id);
    case VERTEX_FSNAME:
      return fsname_compute_id(containerof(v, fsname, vertex), id);
    case VERTEX_FSTREE:
      return fstree_compute_id(containerof(v, fstree, vertex), id);
    case VERTEX_PLUGIN:
      return plugin_compute_id(containerof(v, plugin, vertex), id);

    case VERTEX_CONTEXT_MODULE:
    case VERTEX_CLIENT:
      RTA;
  }
}

void vertex_id(const moria_vertex * restrict v, const char ** restrict id, uint16_t * restrict idl)
{
  switch(vertex_type_get(v)) {
    case VERTEX_DIR:
    case VERTEX_FILE:
    case VERTEX_SYMLINK:
    case VERTEX_SHADOW_DIR:
    case VERTEX_SHADOW_FILE:
    case VERTEX_SHADOW_LINK:
      return fsent_id(containerof(v, fsent, vertex), id, idl);
    /* details */
    case VERTEX_MODULE_VARS:
    case VERTEX_MODULE_ENVS:
    case VERTEX_PLUGIN_STDIN:
    case VERTEX_PLUGIN_STDOUT:
    case VERTEX_PLUGIN_STDERR:
    case VERTEX_PLUGIN_AUXOUT:
    case VERTEX_FORMULA_STDIN:
    case VERTEX_FORMULA_STDOUT:
    case VERTEX_FORMULA_STDERR:
    case VERTEX_FORMULA_AUXOUT:
    case VERTEX_FORMULA_ARGS:
    case VERTEX_FORMULA_ENVS:
    case VERTEX_FORMULA_FILE:
    case VERTEX_FORMULA_CWD:
      return detail_id(containerof(v, detail, vertex), id, idl);
    /* specials */
    case VERTEX_MODULE:
      return module_id(containerof(v, module, vertex), id, idl);
    case VERTEX_FORMULA:
      return formula_id(containerof(v, formula, vertex), id, idl);
    case VERTEX_VARSET:
      return varset_id(containerof(v, varset, vertex), id, idl);
    case VERTEX_CONFIGSET:
      return configset_id(containerof(v, configset, vertex), id, idl);
    case VERTEX_PLUGIN:
      return plugin_id(containerof(v, plugin, vertex), id, idl);
    case VERTEX_RULESET:
      return ruleset_id(containerof(v, ruleset, vertex), id, idl);
    /* other */
    case VERTEX_RULE:
      return rule_id(containerof(v, rule, vertex), id, idl);
    case VERTEX_RMA:
      return rma_id(containerof(v, rule_module_association, vertex), id, idl);
    case VERTEX_BPE:
      return bpe_id(containerof(v, buildplan_entity, vertex), id, idl);
    case VERTEX_CLIENT:
      return client_id(containerof(v, client, vertex), id, idl);
    case VERTEX_FSTREE:
      return fstree_id(containerof(v, fstree, vertex), id, idl);
    case VERTEX_NAMESPACE:
      return namespace_id(containerof(v, namespace, vertex), id, idl);
    case VERTEX_FSNAME:
      return fsname_id(containerof(v, fsname, vertex), id, idl);
    case VERTEX_BUILD_RECORD:
      return build_record_id(containerof(v, build_record, vertex), id, idl);
    case VERTEX_FORMULA_RECORD:
      return formula_record_id(containerof(v, formula_record, vertex), id, idl);
    /* globals / subsystems */
    case VERTEX_SYS_SYSTEM:
    case VERTEX_SYS_NOTIFY:
    case VERTEX_SYS_BUILD:
    case VERTEX_SYS_PLAN:
    case VERTEX_SYS_FILESYSTEM:
    case VERTEX_SYS_RCU:
    case VERTEX_SYS_CONFIG:
    case VERTEX_SYS_GOALS:
      return subsys_id(containerof(v, subsys, vertex), id, idl);

    case VERTEX_CONTEXT_MODULE:
      RTA;
  }
}
