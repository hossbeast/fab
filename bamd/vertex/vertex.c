/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "moria.h"
#include "moria/traverse.h"
#include "moria/foreach.h"
#include "moria/vertex.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "bam/vertex.h"
#include "bam/stats.desc.h"
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
#include "var.h"
#include "channel.h"
#include "stats.h"
#include "events.h"
#include "rules.h"
#include "edge.h"
#include "rule.h"
#include "config.h"
#include "zcat.h"
#include "marshal.h"
#include "rule_system.h"
#include "vertex/probes.h"
#include "buildplan_entity.h"
#include "entlist.h"
#include "config_system.h"
#include "index_vertex.h"

uint64_t vertex_token_counter;

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
  uint16_t z;
  channel *chan;
  bamipc_message *msg;
  fsent *fse = 0;
  module *mod;
  rules *rules;
  struct var *var;
  struct config *config;
  bam_vertex_stats *vertex_stats = 0;
  formula *fml;
  rule_module_association *rma;

  if(vertex_invalid_get(v)) {
    return false;
  }

  if(vertex_kind_get(v) == VERTEX_KIND_FSENT) {
    fse = containerof(v, typeof(*fse), vertex);
  }

  switch(vertex_type_get(v)) {
    /* rules are never invalid */
    case VERTEX_RULE:
    case VERTEX_SHADOW_MODULE:
    case VERTEX_SHADOW_RULE:
    case VERTEX_CONTEXT_MODULE:
      return false;
    case VERTEX_BPE:
      return false;
    case VERTEX_INDEX:
      return false;
    /* directories are not invalidated */
    case VERTEX_DIR:
    case VERTEX_MODULE_DIR:
      return false;
    case VERTEX_SHADOW_DIR:
      return false;
    case VERTEX_FILE:
      /* primary files are never invalid */
      if(!fse->is_target) {
        return false;
      }
      vertex_stats = &fse->stats;
      break;
    case VERTEX_RULES_FILE:
      fse->self_rules->invalid.reparse = true;
      vertex_stats = &fse->stats;
      break;
    case VERTEX_RULES:
      rules = containerof(v, typeof(*rules), vertex);
      rules_rebuild_enqueue(rules);
      vertex_stats = &rules->stats;
      break;
    case VERTEX_MODULE_FILE:
      //fse->self_mod->invalid.exists = true;
      fse->self_mod->invalid.reparse = true;
      vertex_stats = &fse->stats;
      break;
    case VERTEX_MODULE:
      mod = containerof(v, typeof(*mod), vertex);
      module_rebuild_enqueue(mod);
      vertex_stats = &mod->stats;
      break;
    case VERTEX_CONFIG_FILE:
    case VERTEX_FORMULA_FILE:
    case VERTEX_SHADOW_FILE:
    case VERTEX_VAR_FILE:
      vertex_stats = &fse->stats;
      break;
    case VERTEX_VAR:
      var = containerof(v, typeof(*var), vertex);
      /* vars are parsed on-demand during module_reconcile */
      vertex_stats = &var->stats;
      break;
    case VERTEX_CONFIG:
      config_system_invalidate();
      config = containerof(v, typeof(*config), vertex);
      vertex_stats = &config->stats;
      break;
    case VERTEX_FORMULA:
      fml = containerof(v, typeof(*fml), vertex);
      formula_parse_enqueue(fml);
      vertex_stats = &fml->stats;
      break;
    case VERTEX_RMA:
      rma = containerof(v, typeof(*rma), vertex);
      vertex_stats = &rma->vertex_stats;
      rule_system_work_enqueue(&rma->work);
      break;
    default:
      vertex_show(v);
      RTA;
  }

  vertex_invalid_set(v);
  STATS_INC(vertex_stats->stale);
  STATS_INC(g_stats.vertex_stale);

  if(events_would(BAMIPC_EVENT_NODE_STALE, &chan, &msg))
  {
    z = vertex_path_load(msg->text, sizeof(msg->text), v);
    msg->size = z;
    msg->id = 0;
    events_publish(chan, msg);
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
}

bool vertex_mark_fresh(moria_vertex * restrict v)
{
  uint16_t z;
  channel *chan;
  bamipc_message *msg;
  fsent *fse = 0;
  bam_vertex_stats *vertex_stats = 0;
  module *mod;
  rules *rules;
  struct var *var;
  struct config *config;
  formula *fml;
  rule_module_association *rma;
//  bind_module_association *bma;

  /* fsent nodes are subject to the fsent_valid_epoch */
  if(vertex_kind_get(v) == VERTEX_KIND_FSENT) {
    fse = containerof(v, typeof(*fse), vertex);
    if(vertex_state_get(v) == VERTEX_OK && fse->valid_epoch == fsent_valid_epoch) {
      return false;
    }
    fse->valid_epoch = fsent_valid_epoch;
  } else if(vertex_state_get(v) == VERTEX_OK) {
    return false;
  }

  vertex_state_set(v, VERTEX_OK);

  switch(vertex_type_get(v)) {
    case VERTEX_MODULE_FILE:
    case VERTEX_VAR_FILE:
    case VERTEX_RULES_FILE:
    case VERTEX_CONFIG_FILE:
    case VERTEX_FORMULA_FILE:
    case VERTEX_FILE:
    case VERTEX_SHADOW_FILE:
      fse = containerof(v, typeof(*fse), vertex);
      vertex_stats = &fse->stats;
      break;
    /* meta */
    case VERTEX_RMA:
      rma = containerof(v, typeof(*rma), vertex);
      vertex_stats = &rma->vertex_stats;
      break;
    /* special */
    case VERTEX_RULES:
      rules = containerof(v, typeof(*rules), vertex);
      vertex_stats = &rules->stats;
      break;
    case VERTEX_MODULE:
      mod = containerof(v, typeof(*mod), vertex);
      vertex_stats = &mod->stats;
      break;
    case VERTEX_VAR:
      var = containerof(v, typeof(*var), vertex);
      vertex_stats = &var->stats;
      break;
    case VERTEX_CONFIG:
      config = containerof(v, typeof(*config), vertex);
      vertex_stats = &config->stats;
      break;
    case VERTEX_FORMULA:
      fml = containerof(v, typeof(*fml), vertex);
      vertex_stats = &fml->stats;
      break;
    default:
      RTA;
  }

  STATS_INC(vertex_stats->fresh);
  STATS_INC(g_stats.vertex_fresh);

  if(events_would(BAMIPC_EVENT_NODE_FRESH, &chan, &msg))
  {
    z = vertex_path_load(msg->text, sizeof(msg->text), v);
    msg->size = z;
    msg->id = 0;
    events_publish(chan, msg);
  }

  return true;
}

bool vertex_invalid_get(moria_vertex * restrict v)
{
  uint32_t attrs;
  fsent *fse;

  attrs = v->attrs;
  attrs &= VERTEX_INVALID_BIT;
  if(attrs) {
    return true;
  }

  if(vertex_kind_get(v) != VERTEX_KIND_FSENT) {
    return false;
  }
  fse = containerof(v, typeof(*fse), vertex);

  /* primary files are never invalid */
  if(vertex_type_get(v) == VERTEX_FILE && !fse->is_target) {
    return false;
  }

  return fse->valid_epoch != fsent_valid_epoch;
}

void vertex_invalid_set(moria_vertex * restrict v)
{
  uint32_t attrs;

  attrs = v->attrs;
  attrs &= ~VERTEX_INVALID_BIT;
  attrs |= VERTEX_INVALID_BIT;

  vertex_attrs_set(v, attrs);
}

enum bam_vertex_state vertex_bam_vertex_state_get(moria_vertex * restrict v)
{
  switch(vertex_state_get(v)) {
    case VERTEX_UNCREATED:
      return BAM_VERTEX_STATE_UNCREATED;
    case VERTEX_UNLINKED:
      return BAM_VERTEX_STATE_UNLINKED;
    default:
  }

  if(vertex_invalid_get(v)) {
    return BAM_VERTEX_STATE_STALE;
  }

  switch(vertex_state_get(v)) {
    case VERTEX_OK:
      return BAM_VERTEX_STATE_FRESH;
    case VERTEX_INVALID:
      return BAM_VERTEX_STATE_STALE;
    default:
  }

  vertex_show(v);
  RTA;
  return 0;
}

size_t vertex_path_load(void * restrict dst, size_t sz, const moria_vertex * v)
{
  struct rule *rule;
  struct module *module;
  struct formula *formula;
  struct var *var;
  struct config *config;
  struct rules *rules;
  struct rule_module_association *rma;
  struct buildplan_entity *bpe;

  if(vertex_kind_get(v) == VERTEX_KIND_FSENT) {
    return fsent_absolute_path_cat(dst, sz, containerof(v, fsent, vertex));
  }

  switch(vertex_type_get(v)) {
    case VERTEX_DIR:
    case VERTEX_MODULE_DIR:
    case VERTEX_FILE:
    case VERTEX_SYMLINK:
    case VERTEX_MODULE_FILE:
    case VERTEX_RULES_FILE:
    case VERTEX_FORMULA_FILE:
    case VERTEX_VAR_FILE:
    case VERTEX_CONFIG_FILE:
    case VERTEX_SHADOW_DIR:
    case VERTEX_SHADOW_FILE:
    case VERTEX_SHADOW_LINK:
    case VERTEX_SHADOW_MODULE:
    case VERTEX_SHADOW_RULE:
    case VERTEX_CONTEXT_MODULE:
    case VERTEX_INDEX:
      return fsent_absolute_path_cat(dst, sz, containerof(v, fsent, vertex));

    case VERTEX_RULE:
      rule = containerof(v, typeof(*rule), vertex);
      return zcatf(dst, sz, "/(rules)/%.*s", rule->id_len, rule->id);
    case VERTEX_MODULE:
      module = containerof(v, typeof(*module), vertex);
      return zcatf(dst, sz, "/(modules)/%.*s", module->id_len, module->id);
    case VERTEX_FORMULA:
      formula = containerof(v, typeof(*formula), vertex);
      return zcatf(dst, sz, "/(formula)/%.*s", formula->id_len, formula->id);
    case VERTEX_VAR:
      var = containerof(v, typeof(*var), vertex);
      return zcatf(dst, sz, "/(var)/%.*s", var->id_len, var->id);
    case VERTEX_CONFIG:
      config = containerof(v, typeof(*config), vertex);
      return zcatf(dst, sz, "/(config)/%.*s", config->id_len, config->id);
    case VERTEX_RULES:
      rules = containerof(v, typeof(*rules), vertex);
      return zcatf(dst, sz, "/(rules)/%.*s", rules->id_len, rules->id);
    case VERTEX_RMA:
      rma = containerof(v, typeof(*rma), vertex);
      return zcatf(dst, sz, "/(rma)/%.*s", rma->id_len, rma->id);
    case VERTEX_BPE:
      bpe = containerof(v, typeof(*bpe), vertex);
      return zcatf(dst, sz, "/(bpe)/%.*s", bpe->id_len, bpe->id);
  }

  vertex_identity_show(v);
  RTA;
  return 0;
}

size_t vertex_id_load(void * restrict dst, size_t sz, const moria_vertex * v)
{
  struct fsent *fse;
  struct rule *rule;
  struct module *module;
  struct formula *formula;
  struct var *var;
  struct config *config;
  struct rules *rules;
  struct rule_module_association *rma;
  struct buildplan_entity *bpe;
  struct index_vertex *iv;

  switch(vertex_type_get(v)) {
    case VERTEX_DIR:
    case VERTEX_MODULE_DIR:
    case VERTEX_FILE:
    case VERTEX_SYMLINK:
    case VERTEX_MODULE_FILE:
    case VERTEX_RULES_FILE:
    case VERTEX_FORMULA_FILE:
    case VERTEX_VAR_FILE:
    case VERTEX_CONFIG_FILE:
    case VERTEX_SHADOW_DIR:
    case VERTEX_SHADOW_FILE:
    case VERTEX_SHADOW_LINK:
    case VERTEX_SHADOW_MODULE:
    case VERTEX_SHADOW_RULE:
    case VERTEX_CONTEXT_MODULE:
      fse = containerof(v, typeof(*fse), vertex);
      return zcatw(dst, sz, fse->id, fse->id_len);
    case VERTEX_INDEX:
      iv = containerof(v, typeof(*iv), vertex);
      return zcatw(dst, sz, iv->id, iv->id_len);
    case VERTEX_RULE:
      rule = containerof(v, typeof(*rule), vertex);
      return zcatw(dst, sz, rule->id, rule->id_len);
    case VERTEX_MODULE:
      module = containerof(v, typeof(*module), vertex);
      return zcatw(dst, sz, module->id, module->id_len);
    case VERTEX_FORMULA:
      formula = containerof(v, typeof(*formula), vertex);
      return zcatw(dst, sz, formula->id, formula->id_len);
    case VERTEX_VAR:
      var = containerof(v, typeof(*var), vertex);
      return zcatw(dst, sz, var->id, var->id_len);
    case VERTEX_CONFIG:
      config = containerof(v, typeof(*config), vertex);
      return zcatw(dst, sz, config->id, config->id_len);
    case VERTEX_RULES:
      rules = containerof(v, typeof(*rules), vertex);
      return zcatw(dst, sz, rules->id, rules->id_len);
    case VERTEX_RMA:
      rma = containerof(v, typeof(*rma), vertex);
      return zcatw(dst, sz, rma->id, rma->id_len);
    case VERTEX_BPE:
      bpe = containerof(v, typeof(*bpe), vertex);
      return zcatw(dst, sz, bpe->id, bpe->id_len);
  }

  vertex_identity_show(v);
  RTA;
  return 0;
}

size_t vertex_token_load(void * restrict dst, size_t sz, const moria_vertex * v)
{
  struct fsent *fse;
  struct rule *rule;
  struct module *module;
  struct formula *formula;
  struct var *var;
  struct config *config;
  struct rules *rules;
  struct rule_module_association *rma;
  struct buildplan_entity *bpe;
  struct index_vertex *iv;

  switch(vertex_type_get(v)) {
    case VERTEX_DIR:
    case VERTEX_MODULE_DIR:
    case VERTEX_FILE:
    case VERTEX_SYMLINK:
    case VERTEX_MODULE_FILE:
    case VERTEX_RULES_FILE:
    case VERTEX_FORMULA_FILE:
    case VERTEX_VAR_FILE:
    case VERTEX_CONFIG_FILE:
    case VERTEX_SHADOW_DIR:
    case VERTEX_SHADOW_FILE:
    case VERTEX_SHADOW_LINK:
    case VERTEX_SHADOW_MODULE:
    case VERTEX_SHADOW_RULE:
    case VERTEX_CONTEXT_MODULE:
      fse = containerof(v, typeof(*fse), vertex);
      return zcatw(dst, sz, fse->token, fse->token_len);
    case VERTEX_INDEX:
      iv = containerof(v, typeof(*iv), vertex);
      return zcatw(dst, sz, iv->token, iv->token_len);
    case VERTEX_RULE:
      rule = containerof(v, typeof(*rule), vertex);
      return zcatw(dst, sz, rule->token, rule->token_len);
    case VERTEX_MODULE:
      module = containerof(v, typeof(*module), vertex);
      return zcatw(dst, sz, module->token, module->token_len);
    case VERTEX_FORMULA:
      formula = containerof(v, typeof(*formula), vertex);
      return zcatw(dst, sz, formula->token, formula->token_len);
    case VERTEX_VAR:
      var = containerof(v, typeof(*var), vertex);
      return zcatw(dst, sz, var->token, var->token_len);
    case VERTEX_CONFIG:
      config = containerof(v, typeof(*config), vertex);
      return zcatw(dst, sz, config->token, config->token_len);
    case VERTEX_RULES:
      rules = containerof(v, typeof(*rules), vertex);
      return zcatw(dst, sz, rules->token, rules->token_len);
    case VERTEX_RMA:
      rma = containerof(v, typeof(*rma), vertex);
      return zcatw(dst, sz, rma->token, rma->token_len);
    case VERTEX_BPE:
      bpe = containerof(v, typeof(*bpe), vertex);
      return zcatw(dst, sz, bpe->token, bpe->token_len);
  }

  vertex_identity_show(v);
  RTA;
  return 0;
}

struct collate_node_handlers {
  descriptor_marshal_handlers handlers;
  const moria_vertex *v;
};

static size_t handler(
    void * restrict dst
  , size_t sz
  , const descriptor_field * restrict field
  , const void * restrict object
  , struct descriptor_marshal_handlers *ctx
)
{
  size_t z;
  struct collate_node_handlers *handlers;
  uint16_t len;
  size_t len_off;

  handlers = containerof(ctx, typeof(*handlers), handlers);

  z = 0;
  if(field == &descriptor_bam_vertex_stats_path) {
    len_off = z;
    z += marshal_u16(dst + z, sz - z, 0);
    len = vertex_path_load(dst + z, sz - z, handlers->v);
    marshal_u16(dst + len_off, sz - len_off, len);
    z += len;
  } else if(field == &descriptor_bam_vertex_stats_id) {
    len_off = z;
    z += marshal_u16(dst + z, sz - z, 0);
    len = vertex_id_load(dst + z, sz - z, handlers->v);
    marshal_u16(dst + len_off, sz - len_off, len);
    z += len;
  } else if(field == &descriptor_bam_vertex_stats_token) {
    len_off = z;
    z += marshal_u16(dst + z, sz - z, 0);
    len = vertex_token_load(dst + z, sz - z, handlers->v);
    marshal_u16(dst + len_off, sz - len_off, len);
    z += len;
  }

  return z;
}

size_t vertex_stats_collate(void *dst, size_t sz, bool reset, moria_vertex * restrict v)
{
  size_t z;
  fsent *fse;
  bam_vertex_stats *stats = 0;
  bam_vertex_stats tmp;
  struct rule *rule;
  struct module *module;
  struct formula *formula;
  struct var *var;
  struct config *config;
  struct rules *rules;
  struct collate_node_handlers ctx;

  if(vertex_kind_get(v) == VERTEX_KIND_FSENT) {
    fse = containerof(v, typeof(*fse), vertex);
    stats = &fse->stats;
  } else {
    fse = 0;
    switch(vertex_type_get(v)) {
      case VERTEX_RULE:
        rule = containerof(v, typeof(*rule), vertex);
        stats = &rule->stats;
        break;
      case VERTEX_MODULE:
        module = containerof(v, typeof(*module), vertex);
        stats = &module->stats;
        break;
      case VERTEX_FORMULA:
        formula = containerof(v, typeof(*formula), vertex);
        stats = &formula->stats;
        break;
      case VERTEX_VAR:
        var = containerof(v, typeof(*var), vertex);
        stats = &var->stats;
        break;
      case VERTEX_CONFIG:
        config = containerof(v, typeof(*config), vertex);
        stats = &config->stats;
        break;
      case VERTEX_RULES:
        rules = containerof(v, typeof(*rules), vertex);
        stats = &rules->stats;
        break;
      default:
        RTA;
    }
  }

  memcpy(&tmp, stats, sizeof(tmp));
  if(reset) {
    memset(stats, 0, sizeof(*stats));
  }
  stats = &tmp;

  stats->type = vertex_type_remap(vertex_type_get(v));
  stats->state = vertex_bam_vertex_state_get(v);

  stats->indexed_notify = 0;
  if(fse && fse->in_wd != -1) {
    stats->indexed_notify = 1;
  }

  stats->indexed_lookup = 0;
  if(fse && fse->ie) {
    stats->indexed_lookup = 1;
  }

  ctx.v = v;
  ctx.handlers.handler = handler;

  z = 0;
  z += descriptor_marshal_object(dst + z, sz - z, &descriptor_bam_vertex_stats, &stats->record, &ctx.handlers);

  return z;
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
  fsent *fse;

  if(attrs == v->attrs) {
    return;
  }

  before = v->attrs;
  v->attrs = attrs;

  if(!(v->attrs & VERTEX_EXISTS_BIT)) {
    if((v->attrs & VERTEX_KIND_OPT) == VERTEX_KIND_FSENT) {
      fse = containerof(v, typeof(*fse), vertex);
      notify_thread_rm_watch(fse);
    }
  }

  vertex_state_probe(v, before);
}

static void attrs32_say(uint32_t attrs, const attrs32 * restrict list, struct narrator * restrict N)
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

void vertex_describe(const moria_vertex * restrict v, bacon_writer * restrict builder_writer)
{
  module *mod;
  rule_module_association *rma;
//  bind_module_association *bma;
  fsent *fse;
  struct rule *rule;
  struct rules *rules;
  struct var *var;
  formula *fml;
  buildplan_entity *bpe;
  config *config;
  char buf[128];
  char path[PATH_MAX];
  entlist list, *eln;
  narrator_fixed fixed;

  { SET
    narrator_fixed_init(&fixed, buf, sizeof(buf));
    vertex_identity_say(v, &fixed.base);
    BYTES(buf, fixed.l)

    switch(vertex_type_get(v)) {
      case VERTEX_RMA:
        rma = containerof(v, typeof(*rma), vertex);
        if(rma->rule) {
          vertex_describe(&rma->rule->vertex, builder_writer);
        }
        if(rma->mod) {
          MAPPING
          STRING("mod")
          vertex_identity_describe(&rma->mod->vertex, builder_writer);
        }
        break;
      case VERTEX_BPE:
        bpe = containerof(v, typeof(*bpe), vertex);
        if(bpe->rma) {
          MAPPING
          STRING("bpe")
          vertex_describe(&bpe->rma->vertex, builder_writer);
        }
        { MAPPING
          STRING("targets")
          { LIST
            buildplan_entity_targets(bpe, &list);
            entlist_foreach(&list, fse, eln) {
              vertex_identity_describe(&fse->vertex, builder_writer);
            }
            entlist_destroy(&list);
          }
        }
        { MAPPING
          STRING("sources")
          { LIST
            buildplan_entity_sources(bpe, &list);
            entlist_foreach(&list, fse, eln) {
              vertex_identity_describe(&fse->vertex, builder_writer);
            }
            entlist_destroy(&list);
          }
        }
        if(bpe->fml) {
          MAPPING
          STRING("formula")
          vertex_identity_describe(&bpe->fml->vertex, builder_writer);
        }
        break;
      case VERTEX_RULE:
        rule = containerof(v, typeof(*rule), vertex);
        rule_describe(rule, builder_writer);
        break;
      case VERTEX_MODULE_DIR:
        mod = containerof(v, typeof(*fse), vertex)->mod;
        { MAPPING
          STRING("id")
          STRING(mod->id)
        }
        { MAPPING
          STRING("path")
          STRING(mod->dir_node_abspath)
        }
        break;
      case VERTEX_MODULE:
        mod = containerof(v, typeof(*mod), vertex);
        { MAPPING
          STRING("id")
          STRING(mod->id)
        }
        { MAPPING
          STRING("path")
          STRING(mod->dir_node_abspath)
        }
        break;
      case VERTEX_RULES:
        rules = containerof(v, typeof(*rules), vertex);
        { MAPPING
          STRING("id")
          STRING(rules->id)
        }
        { MAPPING
          STRING("path")
          STRING(rules->self_node_abspath)
        }
        break;
      case VERTEX_FORMULA:
        fml = containerof(v, typeof(*fml), vertex);
        { MAPPING
          STRING("id")
          STRING(fml->id)
        }
        { MAPPING
          STRING("path")
          STRING(fml->abspath)
        }
        break;
      case VERTEX_SHADOW_MODULE:
      case VERTEX_MODULE_FILE:
        fse = containerof(v, typeof(*fse), vertex);
        if(fse->self_mod) {
          MAPPING
          STRING("module")
          vertex_describe(&fse->self_mod->vertex, builder_writer);
        }
        break;
      case VERTEX_RULES_FILE:
        fse = containerof(v, typeof(*fse), vertex);
        { MAPPING
          STRING("rules")
          vertex_describe(&fse->self_rules->vertex, builder_writer);
        }
        break;
      case VERTEX_CONFIG_FILE:
        fse = containerof(v, typeof(*fse), vertex);
        { MAPPING
          STRING("config")
          vertex_describe(&fse->self_config->vertex, builder_writer);
        }
        break;
      case VERTEX_FORMULA_FILE:
        fse = containerof(v, typeof(*fse), vertex);
        { MAPPING
          STRING("formula")
          vertex_describe(&fse->self_fml->vertex, builder_writer);
        }
        break;
      case VERTEX_VAR_FILE:
        fse = containerof(v, typeof(*fse), vertex);
        { MAPPING
          STRING("var")
          vertex_describe(&fse->self_var->vertex, builder_writer);
        }
        break;
      case VERTEX_SHADOW_RULE:
        fse = containerof(v, typeof(*fse), vertex);
        if(fse->self_rma) {
          MAPPING
          STRING("rma")
          vertex_describe(&fse->self_rma->vertex, builder_writer);
        }
        break;
      case VERTEX_VAR:
        var = containerof(v, typeof(*var), vertex);
        { MAPPING
          STRING("id")
          STRING(var->id)
        }
        { MAPPING
          STRING("path")
          STRING(var->self_node_abspath)
        }
        break;
      case VERTEX_CONFIG:
        config = containerof(v, typeof(*config), vertex);
        { MAPPING
          STRING("id")
          STRING(config->id)
        }
        { MAPPING
          STRING("path")
          STRING(config->self_node_abspath)
        }
        break;
      case VERTEX_SYMLINK:
      case VERTEX_SHADOW_LINK:
        fse = containerof(v, typeof(*fse), vertex);
        { MAPPING
          STRING("ref")
          if(v->ref) {
            fsent_absolute_path_cat(path, sizeof(path), containerof(v->ref, fsent, vertex));
          } else {
            zcats(path, sizeof(path), "-none-");
          }
          STRING(path)
        }
        break;
      case VERTEX_SHADOW_FILE:
      case VERTEX_SHADOW_DIR:
      case VERTEX_DIR:
      case VERTEX_FILE:
      case VERTEX_INDEX:
        break;
      default:
        LOADF("IDK %.*s : type %s", (int)v->label_len, v->label, buf);
        break;
    }
  }
}

void vertex_describe_edges(const moria_vertex * restrict v, bacon_writer * restrict builder_writer)
{
  moria_foreach_state st;
  moria_edge *e;

  { SET
    { MAPPING
      STRING("vertex")
      vertex_describe(v, builder_writer);
    }
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

size_t vertex_cat(void * restrict dst, size_t dst_size, const struct moria_vertex * restrict v)
{
  narrator_fixed fixed;

  narrator_fixed_init(&fixed, dst, dst_size);
  vertex_say(v, &fixed.base);

  return fixed.l;
}

void vertex_say(const moria_vertex * restrict v, narrator * restrict N)
{
  bacon_writer writer;

  bacon_writer_init(&writer);
  bacon_writer_open(&writer, N);
  vertex_describe(v, &writer);
  bacon_writer_destroy(&writer);
}

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

void vertex_identity_say(const moria_vertex * restrict v, struct narrator * restrict N)
{
  module *mod;
  rule_module_association *rma;
  fsent *fse;
  struct rule *rule;
  rules *rules;
  var *var;
  formula *fml;
  buildplan_entity *bpe;
  config *config;
  index_vertex *index;

  switch(vertex_type_get(v)) {
    /* meta */
    case VERTEX_CONTEXT_MODULE:
      narrator_sayf(N, "%p ", v);
      attrs32_say(v->attrs, graph_vertex_attrs, N);
      break;
    case VERTEX_RMA:
      rma = containerof(v, typeof(*rma), vertex);
      narrator_sayf(N, "%p ", v);
      attrs32_say(v->attrs, graph_vertex_attrs, N);
      if(rma->rule) {
        narrator_sayf(N, " rule id %s", rma->rule->id);
      } else {
        narrator_says(N, " rule --");
      }
      if(rma->mod) {
        narrator_sayf(N, " mod id %s", rma->mod->id);
      } else {
        narrator_says(N, " mod --");
      }
      break;
    case VERTEX_SHADOW_RULE:
      fse = containerof(v, typeof(*fse), vertex);
      rma = fse->self_rma;
      narrator_sayf(N, "%p ", v);
      attrs32_say(v->attrs, graph_vertex_attrs, N);
      narrator_says(N, " ");
      fsent_absolute_path_say(fse, N);
      if(rma && rma->rule) {
        narrator_sayf(N, " rule id %s", rma->rule->id);
      } else {
        narrator_says(N, " rule --");
      }
      break;
    case VERTEX_BPE:
      bpe = containerof(v, typeof(*bpe), vertex);
      narrator_sayf(N, "%p ", v);
      attrs32_say(v->attrs, graph_vertex_attrs, N);
      break;
    case VERTEX_RULE:
      rule = containerof(v, typeof(*rule), vertex);
      narrator_sayf(N, "%p ", v);
      attrs32_say(v->attrs, graph_vertex_attrs, N);
      break;
    /* special */
    case VERTEX_MODULE:
      mod = containerof(v, typeof(*mod), vertex);
      narrator_sayf(N, "%p ", v);
      attrs32_say(v->attrs, graph_vertex_attrs, N);
      narrator_sayf(N, " id %s path %s", mod->id, mod->dir_node_abspath);
      break;
    case VERTEX_SHADOW_MODULE:
      fse = containerof(v, typeof(*fse), vertex);
      narrator_sayf(N, "%p ", v);
      attrs32_say(v->attrs, graph_vertex_attrs, N);
      narrator_says(N, " ");
      fsent_absolute_path_say(fse, N);
      if((mod = fse->self_mod)) {
        narrator_sayf(N, " id %s path %s", mod->id, mod->dir_node_abspath);
      }
      break;
    case VERTEX_RULES:
      rules = containerof(v, typeof(*rules), vertex);
      narrator_sayf(N, "%p ", v);
      attrs32_say(v->attrs, graph_vertex_attrs, N);
      narrator_sayf(N, " id %s path %s", rules->id, rules->self_node_abspath);
      break;
    case VERTEX_FORMULA:
      fml = containerof(v, typeof(*fml), vertex);
      narrator_sayf(N, "%p ", v);
      attrs32_say(v->attrs, graph_vertex_attrs, N);
      narrator_sayf(N, " id %s path %s", fml->id, fml->abspath);
      break;
    case VERTEX_VAR:
      var = containerof(v, typeof(*var), vertex);
      narrator_sayf(N, "%p ", v);
      attrs32_say(v->attrs, graph_vertex_attrs, N);
      narrator_sayf(N, " id %s path %s", var->id, var->self_node_abspath);
      break;
    case VERTEX_CONFIG:
      config = containerof(v, typeof(*config), vertex);
      narrator_sayf(N, "%p ", v);
      attrs32_say(v->attrs, graph_vertex_attrs, N);
      narrator_sayf(N, " id %s path %s", config->id, config->self_node_abspath);
      break;
    /* links */
    case VERTEX_SYMLINK:
    case VERTEX_SHADOW_LINK:
      fse = containerof(v, typeof(*fse), vertex);
      narrator_sayf(N, "%p ", v);
      attrs32_say(v->attrs, graph_vertex_attrs, N);
      narrator_says(N, " ");
      fsent_absolute_path_say(fse, N);
      if(v->ref) {
        narrator_says(N, " -> ");
        fsent_absolute_path_say(containerof(v->ref, fsent, vertex), N);
      } else {
        narrator_says(N, " -none- ");
      }
      break;
    /* index */
    case VERTEX_INDEX:
      index = containerof(v, typeof(*index), vertex);
      narrator_sayf(N, "%p ", v);
      attrs32_say(v->attrs, graph_vertex_attrs, N);
      narrator_sayf(N, " label %.*s", (int)v->label_len, v->label);
      break;
    /* fsent */
    case VERTEX_MODULE_FILE:
    case VERTEX_RULES_FILE:
    case VERTEX_CONFIG_FILE:
    case VERTEX_FORMULA_FILE:
    case VERTEX_VAR_FILE:
    case VERTEX_SHADOW_FILE:
    case VERTEX_SHADOW_DIR:
    case VERTEX_MODULE_DIR:
    case VERTEX_DIR:
    case VERTEX_FILE:
      fse = containerof(v, typeof(*fse), vertex);
      narrator_sayw(N, fse->id, fse->id_len);
      narrator_says(N, " ");
      attrs32_say(v->attrs, graph_vertex_attrs, N);
      narrator_says(N, " ");
      fsent_absolute_path_say(fse, N);
      break;
    default:
      narrator_says(N, "IDK ");
      attrs32_say(v->attrs, graph_vertex_attrs, N);
      narrator_says(N, " ");
      narrator_sayw(N, v->label, v->label_len);
  }
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
