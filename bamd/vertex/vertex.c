/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "moria/foreach.h"
#include "moria/vertex.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "common/attrs.h"
#include "bacon/writer.h"
#include "bacon/builder.h"

#include "vertex.h"
#include "vertex/probes.h"
#include "bpe.h"
#include "build.h"
#include "build_record.h"
#include "buildplan.h"
#include "client.h"
#include "config_system.h"
#include "configset.h"
#include "detail.h"
#include "disintegrate.h"
#include "edge.h"
#include "events/events.h"
#include "formula.h"
#include "formula_record.h"
#include "fsent.h"
#include "fsname.h"
#include "fstree.h"
#include "goals.h"
#include "notify.h"
#include "plugin.h"
#include "request_handler.h"
#include "rma.h"
#include "rule.h"
#include "ruleset.h"
#include "shadow.h"
#include "subsys.h"
#include "system.h"
#include "varset.h"

#include "zcat.h"

//
// static
//

/*
 * moria callback to invalidate a node
 *
 * arg      - invalidation
 * mode     - unused
 * distance - unused
 * result   - unused
 */
static void vertex_invalidate_visitor(
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

static void identity_edges_list(const moria_vertex * restrict v, bacon_writer * restrict builder_writer)
{
  moria_foreach_state st;
  moria_edge *e;

  { LIST
    moria_foreach_edge_up(&st, v, e) {
      edge_identity_describe(e, builder_writer);
    }
    moria_foreach_edge_down(&st, v, e) {
      edge_identity_describe(e, builder_writer);
    }
  }
}

//
// public
//

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
    , MORIA_TRAVERSE_UP | MORIA_TRAVERSE_PRE | MORIA_TRAVERSE_NOFOLLOW
    , invalidation
  );
}

bool vertex_mark_stale(moria_vertex * v)
{
  events_context ectx;
  bamipc_message *msg;
  fsent *fse = 0;
  module *mod;
  fstree *fst;
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
    case VERTEX_NAMESPACE:
    case VERTEX_SYS_SYSTEM:
    case VERTEX_SYS_NOTIFY:
    case VERTEX_SYS_FILESYSTEM:
    case VERTEX_SYS_RCU:
    case VERTEX_SYS_CONFIG:
    case VERTEX_SYS_GOALS:
    case VERTEX_SYS_BUILD:
    case VERTEX_SYS_PLAN:
    case VERTEX_FSTREE:
    case VERTEX_CLIENT:
    case VERTEX_FSNAME:
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
    case VERTEX_BUILD_RECORD:
    case VERTEX_FORMULA_RECORD:
      return false;
    /* directories are not invalidated */
    case VERTEX_DIR:
    case VERTEX_SHADOW_DIR:
      return false;
    case VERTEX_SHADOW_LINK:
      return false;
    case VERTEX_SHADOW_FILE:
      vertex_stats = (void*)&fse->stats;
      break;
    /* files */
    case VERTEX_FILE:
    case VERTEX_SYMLINK:
      /* if the module itself is already invalid, this is still necessary to shift it
       * back onto the rebuild queue */
      if((mod = fsent_is_build_mod_file(fse))) {
        module_rebuild_enqueue(mod);
      }
      vertex_stats = (void*)&fse->stats;
      break;
    /* specials */
    case VERTEX_RULESET:
      rules = containerof(v, typeof(*rules), vertex);
      ruleset_rebuild_enqueue(rules);
      vertex_stats = (void*)&rules->stats;
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
      vertex_stats = (void*)&fml->stats;
      break;
    case VERTEX_RMA:
      rma = containerof(v, typeof(*rma), vertex);
      vertex_stats = (void*)&rma->stats;
      rule_system_work_enqueue(rma);
      break;
    default:
      RTA;
  }

  vertex_invalid_set(v);

  vertex_stats->stale++;
  system_stats_events.vertex_stale++;

  if((msg = events_bacon_would(&ectx, BAMIPC_EVENT_NODE_STALE)))
  {
    narrator_fixed_init(&fixed, msg->text, msg->size);
    bacon_writer_init(&writer);
    builder_writer = bacon_writer_open(&writer, &fixed.base);
    builder_writer->flat = true;

    { MAPPING
      STRING("node-stale")
      { SET
        { MAPPING STRING("id")
          vertex_id_describe(v, builder_writer);
        }
        { MAPPING STRING("path")
          vertex_path_describe(v, builder_writer);
        }
        if((mod = vertex_module_of(v))) {
          MAPPING STRING("module")
          BYTES(mod->id, mod->id_len)
        }
        if((fst = vertex_fstree_of(v))) {
          MAPPING STRING("fstree")
          BYTES(fst->id, fst->id_len)
        }
      }
    }

    bacon_writer_close(&writer);
    msg->id = 0;
    msg->len = fixed.l;
    events_publish(&ectx);
  }

  return true;
}

bool vertex_mark_fresh(moria_vertex * restrict v)
{
  events_context ectx;
  bamipc_message *msg;
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
    case VERTEX_FILE:
    case VERTEX_SYMLINK:
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
      RTA;
  }

  vertex_stats->fresh++;
  system_stats_events.vertex_fresh++;

  if((msg = events_bacon_would(&ectx, BAMIPC_EVENT_NODE_FRESH)))
  {
    narrator_fixed_init(&fixed, msg->text, msg->size);
    bacon_writer_init(&writer);
    builder_writer = bacon_writer_open(&writer, &fixed.base);
    builder_writer->flat = true;

    { MAPPING
      STRING("node-fresh")
      { SET
        { MAPPING STRING("id")
          vertex_id_describe(v, builder_writer);
        }
        { MAPPING STRING("path")
          vertex_path_describe(v, builder_writer);
        }
      }
    }

    bacon_writer_close(&writer);
    msg->id = 0;
    msg->len = fixed.l;
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

//
// path
//

size_t vertex_path_cat(void * restrict dst, size_t sz, const moria_vertex * v)
{
  switch(vertex_type_get(v)) {
    case VERTEX_DIR:
    case VERTEX_FILE:
    case VERTEX_SYMLINK:
    case VERTEX_SHADOW_DIR:
    case VERTEX_SHADOW_FILE:
    case VERTEX_SHADOW_LINK:
      return fsent_absolute_path_cat(dst, sz, containerof(v, fsent, vertex));
    /* globals / subsystems */
    case VERTEX_SYS_SYSTEM:
      return zcats(dst, sz, "//bm/sys-system");
    case VERTEX_SYS_NOTIFY:
      return zcats(dst, sz, "//bm/sys-notify");
    case VERTEX_SYS_CONFIG:
      return zcats(dst, sz, "//bm/sys-config");
    case VERTEX_SYS_GOALS:
      return zcats(dst, sz, "//bm/sys-goals");
    case VERTEX_SYS_BUILD:
      return zcats(dst, sz, "//bm/sys-build");
    case VERTEX_SYS_PLAN:
      return zcats(dst, sz, "//bm/sys-plan");
    case VERTEX_SYS_FILESYSTEM:
      return zcats(dst, sz, "//bm/sys-filesystem");
    case VERTEX_SYS_RCU:
      return zcats(dst, sz, "//bm/sys-rcu");
    case VERTEX_CONTEXT_MODULE:
      return zcats(dst, sz, "//module");
    case VERTEX_CLIENT:
      return client_path_cat(dst, sz, containerof(v, client, vertex));
    case VERTEX_FSTREE:
      return fstree_path_cat(dst, sz, containerof(v, fstree, vertex));
    case VERTEX_NAMESPACE:
      return namespace_path_cat(dst, sz, containerof(v, namespace, vertex));
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
    case VERTEX_BUILD_RECORD:
      return build_record_path_cat(dst, sz, containerof(v, build_record, vertex));
    case VERTEX_FORMULA_RECORD:
      return formula_record_path_cat(dst, sz, containerof(v, formula_record, vertex));
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
      return detail_path_cat(dst, sz, containerof(v, detail, vertex));
  }

  return zcatf(dst, sz, "?? %p attrs 0x%08x ??", v, v->attrs);
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

//
// describe
//

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



size_t vertex_identity_cat(void * restrict s, size_t sz, const moria_vertex * restrict v)
{
  narrator_fixed fixed;

  narrator_fixed_init(&fixed, s, sz);
  vertex_identity_say(v, &fixed.base);

  return fixed.l;
}

void vertex_identity_say(const moria_vertex * restrict v, narrator * restrict N)
{
  bacon_writer writer;

  bacon_writer_init(&writer);
  bacon_writer_open(&writer, N);
  writer.flat = true;
  vertex_identity_describe(v, &writer);
  bacon_writer_destroy(&writer);
}

//
// show
//

void vertex_show(const moria_vertex * restrict v)
{
  bacon_writer writer, *builder_writer;

  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, g_narrator_stdout);

  vertex_describe(v, builder_writer);

  bacon_writer_close(&writer);
  bacon_writer_destroy(&writer);
  printf("\n");
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

void vertex_identity_edges_show(const moria_vertex * restrict v)
{
  bacon_writer writer, *builder_writer;

  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, g_narrator_stdout);

  vertex_identity_describe_edges(v, builder_writer);

  bacon_writer_close(&writer);
  bacon_writer_destroy(&writer);
  printf("\n");
}

//
// ops
//

void vertex_tell(const moria_vertex * restrict v, request_handler * restrict handler)
{
  switch(vertex_type_get(v)) {
    /* globals / subsystems */
    case VERTEX_SYS_CONFIG:
      return config_sys_tell(v, handler);
    case VERTEX_SYS_PLAN:
      return plan_sys_tell(v, handler);
    case VERTEX_SYS_SYSTEM:
      return system_sys_tell(v, handler);
    case VERTEX_SYS_NOTIFY:
      return notify_sys_tell(v, handler);
    case VERTEX_SYS_BUILD:
    case VERTEX_SYS_GOALS:
    case VERTEX_SYS_FILESYSTEM:
    case VERTEX_SYS_RCU:
      return;
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
      return detail_tell(containerof(v, detail, vertex), handler);
    case VERTEX_RULE:
      return rule_tell(containerof(v, rule, vertex), handler);
    case VERTEX_NAMESPACE:
    case VERTEX_DIR:
    case VERTEX_FILE:
    case VERTEX_SYMLINK:
    case VERTEX_SHADOW_DIR:
    case VERTEX_SHADOW_FILE:
    case VERTEX_SHADOW_LINK:
    case VERTEX_CONTEXT_MODULE:
    case VERTEX_MODULE:
    case VERTEX_FORMULA:
    case VERTEX_VARSET:
    case VERTEX_CONFIGSET:
    case VERTEX_PLUGIN:
    case VERTEX_BUILD_RECORD:
    case VERTEX_FORMULA_RECORD:
    case VERTEX_RULESET:
    case VERTEX_RMA:
    case VERTEX_BPE:
    case VERTEX_CLIENT:
    case VERTEX_FSTREE:
    case VERTEX_FSNAME:
      return;
  }

  vertex_identity_show(v);
  RTA;
}

//
// getters
//

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

  if(!fse) {
    return 0;
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

  if(!fse) {
    return 0;
  }

  RUNTIME_ASSERT(fse->fst);
  return fse->fst;
}

//
// id
//

size_t vertex_id_cat(void * restrict dst, size_t sz, const moria_vertex * v)
{
  const char *id;
  uint16_t idl;

  vertex_id(v, &id, &idl);
  return zcatw(dst, sz, id, idl);
}

void vertex_id_describe(const moria_vertex * restrict v, bacon_writer * restrict builder_writer)
{
  const char *id;
  uint16_t idl;

  vertex_id(v, &id, &idl);
  if(idl) {
    BYTES(id, idl)
  } else {
    STRING("_")
  }
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

bool vertex_reap(moria_vertex * restrict v, struct graph_disintegrate_context * restrict ctx)
{
  RUNTIME_ASSERT(!vertex_is_unlinked(v));

  switch(vertex_type_get(v)) {
    case VERTEX_FORMULA:
      return formula_reap(containerof(v, formula, vertex), ctx);
    case VERTEX_RULE:
      return rule_reap(containerof(v, rule, vertex), ctx);
    case VERTEX_RULESET:
      return ruleset_reap(containerof(v, ruleset, vertex), ctx);
    case VERTEX_DIR:
      return fsent_dir_reap(containerof(v, fsent, vertex), ctx);
    case VERTEX_SYMLINK:
      return fsent_symlink_reap(containerof(v, fsent, vertex), ctx);
    case VERTEX_FSNAME:
      return fsname_reap(containerof(v, fsname, vertex), ctx);
    case VERTEX_VARSET:
      return varset_reap(containerof(v, varset, vertex), ctx);
    case VERTEX_MODULE:
      return module_reap(containerof(v, module, vertex), ctx);
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
      return detail_reap(containerof(v, detail, vertex), ctx);
    case VERTEX_BPE:
      return bpe_reap(containerof(v, buildplan_entity, vertex), ctx);
    case VERTEX_FILE:
    case VERTEX_SHADOW_DIR:
    case VERTEX_SHADOW_FILE:
    case VERTEX_SHADOW_LINK:
    case VERTEX_CLIENT:
    case VERTEX_BUILD_RECORD:
    case VERTEX_FORMULA_RECORD:
    case VERTEX_RMA:
    case VERTEX_FSTREE:
    case VERTEX_CONFIGSET:
    case VERTEX_PLUGIN:
    case VERTEX_NAMESPACE:
    case VERTEX_CONTEXT_MODULE:
    case VERTEX_SYS_NOTIFY:
    case VERTEX_SYS_BUILD:
    case VERTEX_SYS_PLAN:
    case VERTEX_SYS_SYSTEM:
    case VERTEX_SYS_FILESYSTEM:
    case VERTEX_SYS_RCU:
    case VERTEX_SYS_GOALS:
    case VERTEX_SYS_CONFIG:
      break;
  }

  return false;
}

void vertex_unlink(moria_vertex * restrict v, struct graph_disintegrate_context * restrict ctx)
{
  bamipc_message *msg;
  uint16_t z;
  events_context ectx;
  moria_foreach_state st;
  moria_edge *edge;

  /*
   * N.B. this function ONLY to be called from graph_disintegrate
   */

  RUNTIME_ASSERT(vertex_exists_get(v));

#if 0
  /* idempotency */
  if(!vertex_exists_get(v)) {
    return;
  }
#endif

  vertex_invalidate(v, ctx->invalidation);
  vertex_state_set(v, VERTEX_UNLINKED);
  vertex_unlink_probe(v);

  moria_foreach_edge_down(&st, v, edge) {
    if(edge->attrs & EDGE_UNLINK) {
      edges_push(ctx->edges, edge);
      vertices_push(ctx->unlinking, edge->B);
    }
  }

  switch(vertex_type_get(v)) {
    /* fsents */
    case VERTEX_DIR:
    case VERTEX_FILE:
    case VERTEX_SYMLINK:
    case VERTEX_SHADOW_DIR:
    case VERTEX_SHADOW_FILE:
    case VERTEX_SHADOW_LINK:
      fsent_unlink(containerof(v, fsent, vertex), ctx);
      break;
    case VERTEX_FORMULA:
      formula_unlink(containerof(v, formula, vertex), ctx);
      break;
    case VERTEX_RULESET:
      ruleset_unlink(containerof(v, ruleset, vertex), ctx);
      break;
    case VERTEX_BUILD_RECORD:
      build_record_unlink(containerof(v, build_record, vertex), ctx);
      break;
    case VERTEX_FORMULA_RECORD:
      formula_record_unlink(containerof(v, formula_record, vertex), ctx);
      break;
    case VERTEX_RMA:
      rma_unlink(containerof(v, rule_module_association, vertex), ctx);
      break;
    case VERTEX_BPE:
      bpe_unlink(containerof(v, buildplan_entity, vertex), ctx);
      break;
    case VERTEX_FSTREE:
      fstree_unlink(containerof(v, fstree, vertex), ctx);
      break;
    case VERTEX_MODULE:
      module_unlink(containerof(v, module, vertex), ctx);
      break;
    case VERTEX_PLUGIN:
      plugin_unlink(containerof(v, plugin, vertex), ctx);
      break;
    case VERTEX_FSNAME:
      fsname_unlink(containerof(v, fsname, vertex), ctx);
      break;
    case VERTEX_VARSET:
      varset_unlink(containerof(v, varset, vertex), ctx);
      break;
    case VERTEX_RULE:
      rule_unlink(containerof(v, rule, vertex), ctx);
      break;
    case VERTEX_CLIENT:
    case VERTEX_CONFIGSET:
    case VERTEX_NAMESPACE:
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
    case VERTEX_CONTEXT_MODULE:
    case VERTEX_SYS_NOTIFY:
    case VERTEX_SYS_BUILD:
    case VERTEX_SYS_PLAN:
    case VERTEX_SYS_SYSTEM:
    case VERTEX_SYS_FILESYSTEM:
    case VERTEX_SYS_RCU:
    case VERTEX_SYS_GOALS:
    case VERTEX_SYS_CONFIG:
      break;
  }

  if((msg = events_bacon_would(&ectx, BAMIPC_EVENT_NODE_DELETE))) {
    z = vertex_path_cat(msg->text, msg->size, v);
    msg->len = z;
    msg->id = 0;
    events_publish(&ectx);
  }
}

void vertex_release(moria_vertex * restrict v)
{
  vertex_free_probe(v);

  /* should have been unlinked first/separately */
  RUNTIME_ASSERT(!vertex_exists_get(v));

  switch(vertex_type_get(v)) {
    /* records */
    case VERTEX_BUILD_RECORD:
      return build_record_release(containerof(v, build_record, vertex));
    case VERTEX_FORMULA_RECORD:
      return formula_record_release(containerof(v, formula_record, vertex));
    /* specials */
    case VERTEX_CONTEXT_MODULE:
    case VERTEX_DIR:
    case VERTEX_FILE:
    case VERTEX_SHADOW_DIR:
    case VERTEX_SHADOW_FILE:
    case VERTEX_SHADOW_LINK:
    case VERTEX_SYMLINK:
      return fsent_release(containerof(v, fsent, vertex));
    case VERTEX_BPE:
      return bpe_release(containerof(v, buildplan_entity, vertex));
    case VERTEX_CLIENT:
      return client_release(containerof(v, client, vertex));
    case VERTEX_RMA:
      return rma_release(containerof(v, rule_module_association, vertex));
    case VERTEX_RULE:
      return rule_release(containerof(v, rule, vertex));
    case VERTEX_FORMULA:
      return formula_release(containerof(v, formula, vertex));
    case VERTEX_CONFIGSET:
      return configset_release(containerof(v, configset, vertex));
    case VERTEX_VARSET:
      return varset_release(containerof(v, varset, vertex));
    case VERTEX_MODULE:
      return module_release(containerof(v, module, vertex));
    case VERTEX_NAMESPACE:
      return namespace_release(containerof(v, namespace, vertex));
    case VERTEX_RULESET:
      return ruleset_release(containerof(v, ruleset, vertex));
    case VERTEX_FSNAME:
      return fsname_release(containerof(v, fsname, vertex));
    case VERTEX_FSTREE:
      return fstree_release(containerof(v, fstree, vertex));
    case VERTEX_PLUGIN:
      return plugin_release(containerof(v, plugin, vertex));
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
      return detail_release(containerof(v, detail, vertex));
    /* globals / subsystems */
    case VERTEX_SYS_SYSTEM:
    case VERTEX_SYS_BUILD:
    case VERTEX_SYS_CONFIG:
    case VERTEX_SYS_FILESYSTEM:
    case VERTEX_SYS_GOALS:
    case VERTEX_SYS_NOTIFY:
    case VERTEX_SYS_PLAN:
    case VERTEX_SYS_RCU:
      RTA;
  }
}
