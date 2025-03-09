/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>
#include <inttypes.h>

#include "bacon/parser.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xstring.h"
#include "bam/ipc.h"
#include "common/snarf.h"
#include "narrator.h"
#include "moria/foreach.h"
#include "bacon/builder.h"
#include "narrator/fixed.h"

#include "ruleset.h"
#include "module/probes.h"
#include "ruleset_parser.h"
#include "fsent.h"
#include "params.h"
#include "system.h"
#include "events/events.h"
#include "channel.h"
#include "stats.h"
#include "vertex.h"
#include "edge.h"
#include "module_edge.h"
#include "special_edge.h"
#include "rma.h"
#include "edges.h"
#include "vertices.h"
#include "pattern_parser.internal.h"
#include "vertex/probes.h"
#include "shadow.h"
#include "identity.h"
#include "disintegrate.h"

#include "zcat.h"

llist ruleset_list = LLIST_INITIALIZER(ruleset_list);                 // active rulesets
static llist ruleset_freelist = LLIST_INITIALIZER(ruleset_freelist);  // freelist

static ruleset_parser *parser;

//
// static
//

void ruleset_rebuild_enqueue(ruleset * restrict rules)
{
  /* move to the end of the rebuild/invalid list */
  llist_delete(&rules->invalid_lln);
  llist_unshift(&ruleset_rebuilds, &rules->invalid_lln);
}

void ruleset_release(ruleset * restrict rules)
{
  moria_vertex __attribute__((unused)) *v;

  /* must have been fully excised from the graph previously */
  v = &rules->vertex;
  RUNTIME_ASSERT(v->up_identity == 0);
  RUNTIME_ASSERT(rbtree_empty(&v->up));
  RUNTIME_ASSERT(rbtree_empty(&v->down));

  llist_delete(&rules->invalid_lln);

  block_release_bulk(parser->bacon_parser, &rules->blocks);
  ruleset_special_name_list_free(&rules->varsets_names);
  ruleset_special_name_list_free(&rules->rulesets_names);
  pattern_free(&rules->anchor->pattern);

  llist_delete(&rules->vertex.owner);
  llist_append(&ruleset_freelist, &rules->vertex.owner);
  ASAN_TAINT(rules);
}

void ruleset_alloc(ruleset ** restrict rp)
{
  ruleset *rules;

  if((rules = llist_shift(&ruleset_freelist, typeof(*rules), vertex.owner))) {
    ASAN_CLEAN(rules);
    memset(rules, 0, sizeof(*rules));
  } else {
    xmalloc(&rules, sizeof(*rules));
  }

  moria_vertex_init(&rules->vertex, &g_graph, VERTEX_RULESET | VERTEX_OK);

  llist_init(&rules->blocks);
  llist_init(&rules->rmas_owner);
  llist_init(&rules->invalid_lln);
  llist_init(&rules->varsets_names);
  llist_init(&rules->rulesets_names);

  llist_append(&ruleset_list, &rules->vertex.owner);

  vertex_alloc_probe(&rules->vertex);
  *rp = rules;
}

static void rules_parse(ruleset * restrict rules)
{
  int exit;
  char *text = 0;
  size_t text_len;
  bamipc_message *msg;
  events_context ectx;
  bacon_writer writer, *builder_writer;
  narrator_fixed fixed;

  snarfs(&text, &text_len, rules->self_node_abspath);
  if(!text_len) {
    rules->ruleset_stats.parse_ok++;
    system_stats_events.module_parse_ok++;
    goto end;
  }

  parser->container = rules;
  if((exit = ruleset_parser_parse(parser, text, text_len + 2, rules->self_node_abspath, rules->self_node_abspath_len)) == 0)
  {
    rules->ruleset_stats.parse_ok++;
    system_stats_events.module_parse_ok++;
  }
  else
  {
    rules->ruleset_stats.parse_err++;
    system_stats_events.module_parse_err++;

    msg = system_error_event(&ectx);
    narrator_fixed_init(&fixed, msg->text, msg->size);
    bacon_writer_init(&writer);
    builder_writer = bacon_writer_open(&writer, &fixed.base);

    { MAPPING STRING("ruleset parse error")
      { SET
        if(parser->error_len) {
          MAPPING STRING("error")
          BYTES(parser->error, parser->error_len);
        }
        { MAPPING STRING("file")
          BYTES(rules->self_node_abspath, rules->self_node_abspath_len)
        }
      }
    }
    bacon_writer_close(&writer);

    msg->code = EINVAL;
    msg->len = fixed.l;
    events_publish(&ectx);
  }

end:
  free(text);
}

void ruleset_reparse(ruleset * restrict rules)
{
  char *name;

  /* re-parse the rules.bam file */
  rules_parse(rules);
  if(system_has_error) {
    return;
  }

  /* replace the guts with the results from parsing */
  block_release_bulk(parser->bacon_parser, &rules->blocks);
  llist_splice_head(&rules->blocks, &parser->blocks);

  pattern_free(&rules->anchor->pattern);
  rules->anchor = parser->anchor;
  parser->anchor = 0;

  /* apply default varsets names */
  if(!parser->varsets_set && llist_empty(&parser->varsets_names)) {
    name = 0;
    ixstrdup(&name, FSENT_NAME_VARSET);
    ruleset_parser_varsets_append(parser, MMS(name));
  }

  ruleset_special_name_list_free(&rules->varsets_names);
  llist_splice_head(&rules->varsets_names, &parser->varsets_names);

  /* apply default ruleset names */
  if(!parser->rulesets_set && llist_empty(&parser->rulesets_names)) {
    name = 0;
    ixstrdup(&name, FSENT_NAME_RULESET);
    ruleset_parser_rulesets_append(parser, MMS(name));
  }

  ruleset_special_name_list_free(&rules->rulesets_names);
  llist_splice_head(&rules->rulesets_names, &parser->rulesets_names);

  rules->managed = parser->managed;
}

static ruleset *rules_initialize(fsent * restrict rules_file_n)
{
  identity_ctx id;
  ruleset *rules = 0;

  ruleset_alloc(&rules);

  rules->self_node_abspath_len = fsent_absolute_path_cat(rules->self_node_abspath, sizeof(rules->self_node_abspath), rules_file_n);
  rules->self_node = rules_file_n;

  identity_init(&id);
  ruleset_compute_id(rules, &id);
  rules->id_len = identity_mk(&id, rules->id, sizeof(rules->id));

  // path hash
  rules->vertex.label = rules->id;
  rules->vertex.label_len = rules->id_len;

  return rules;
}

void ruleset_rebuild(ruleset * restrict rules, graph_invalidation_context * restrict invalidation)
{
  struct vertices unlinking;
  struct vertices releasing;
  moria_foreach_state st;
  moria_edge *edge;
  struct edges edges;

  ruleset_rebuild_probe(rules);

  edges_init(&edges);
  vertices_init(&unlinking);
  vertices_init(&releasing);

  /* teardown */
  moria_foreach_edge(&st, &rules->vertex, edge) {
    switch(edge_type_get(edge)) {
      case EDGE_RULE_RULESET:
        edges_push(&edges, edge);
      default:
        break;
    }
  }

  graph_invalidation_begin(invalidation);
  graph_disintegrate(&edges, &unlinking, &releasing, invalidation);
  graph_invalidation_end(invalidation);

  /* re-parse */
  ruleset_reparse(rules);
}

ruleset *ruleset_bootstrap(fsent * restrict rules_file_n, graph_invalidation_context * restrict invalidation)
{
  struct ruleset *rules;

  /* already bootstrapped */
  if((rules = fsent_is_ruleset_file(rules_file_n))) {
    return rules;
  }

  rules = rules_initialize(rules_file_n);

  special_connect(&rules_file_n->vertex, &rules->vertex, EDGE_FSENT_RULESET, invalidation);

  ruleset_rebuild_enqueue(rules);

  /* shadow link to the special node itself */
  shadow_mkref(g_shadow_rulesets, rules->id, rules->id_len, &rules->vertex);

  return rules;
}

void ruleset_setup()
{
  ruleset_parser_alloc(&parser);
}

void ruleset_cleanup()
{
  ruleset *rules;

  while((rules = llist_shift(&ruleset_freelist, typeof(*rules), vertex.owner))) {
    ASAN_CLEAN(rules);
    free(rules);
  }

  ruleset_parser_free(parser);
}

static uint32_t block_count_rules(block * restrict block)
{
  uint32_t count;
  struct block *sb;

  count = 0;
  llist_foreach(&block->blocks, sb, lln) {
    count += block_count_rules(sb);
  }
  count += llist_count(&block->rules);

  return count;
}

static void ruleset_stats_collate(struct ruleset * restrict rules)
{
  uint32_t count;
  block *block;
  struct vertices vertices;
  moria_foreach_state st;
  moria_edge *edge;

  vertices_init(&vertices);
  moria_foreach_edge(&st, &rules->vertex, edge) {
    switch(edge_type_get(edge)) {
      case EDGE_RULESET_BUILD_MOD:
      case EDGE_RULESET_ANCHOR_MOD:
      case EDGE_RULESET_USES_MOD:
        vertices_push(&vertices, edge->A);
      default:
        break;
    }
  }

  rules->ruleset_stats.modules = llist_count(&vertices.lln);
  vertices_destroy(&vertices);

  /* ruleset */
  count = 0;
  llist_foreach(&rules->blocks, block, lln) {
    count += block_count_rules(block);
  }
  rules->ruleset_stats.rules = count;
}

void ruleset_stats_build(ruleset * restrict rules, bool reset, bacon_writer * restrict builder_writer)
{
  ruleset_stats_collate(rules);

  stats_build(&rules->stats.record, &descriptor_stats_vertex, builder_writer);
  stats_build(&rules->ruleset_stats.record, &descriptor_stats_ruleset, builder_writer);

  /* stats for the ruleset */
  if(reset) {
    memset(&rules->stats, 0, sizeof(rules->stats));
    memset(&rules->ruleset_stats, 0, sizeof(rules->ruleset_stats));
  }
}

void ruleset_describe(const ruleset * restrict ruleset, struct bacon_writer * restrict builder_writer)
{
  { MAPPING
    bacon_writer_push_string(builder_writer);
    vertex_path_say(&ruleset->vertex, builder_writer->N);
    bacon_writer_pop_string(builder_writer);
    { SET
      vertex_mappings(&ruleset->vertex, builder_writer);
      { MAPPING
        STRING("self")
        BYTES(ruleset->self_node_abspath, ruleset->self_node_abspath_len)
      }
      { MAPPING
        STRING("stats")
        { SET
          ruleset_stats_build((struct ruleset *)ruleset, false, builder_writer);
        }
      }
      vertex_relations_describe(&ruleset->vertex, builder_writer);
    }
  }
}

size_t ruleset_path_cat(void * restrict dst, size_t sz, const struct ruleset * restrict ruleset)
{
  return zcatf(dst, sz, "//bm/rulesets/%.*s", ruleset->id_len, ruleset->id);
}

void ruleset_identity_describe(const struct ruleset * restrict ruleset, struct bacon_writer * restrict builder_writer)
{
  attrs32_describe(ruleset->vertex.attrs, graph_vertex_attrs, builder_writer);

  { MAPPING
    STRING("id")
    BYTES(ruleset->id, ruleset->id_len)
  }
  { MAPPING
    STRING("path")
    STRING(ruleset->self_node_abspath);
  }
}

void ruleset_unlink(struct ruleset * restrict rules, struct graph_disintegrate_context * restrict ctx)
{
  /* also unlink rules */
  moria_foreach_state st;
  moria_edge *edge;

  llist_delete(&rules->invalid_lln);

  moria_foreach_edge(&st, &rules->vertex, edge) {
    switch(edge_type_get(edge)) {
      case EDGE_RULE_RULESET:
        vertices_push(ctx->unlinking, edge->B);
      case EDGE_RULESET_BUILD_MOD:
      case EDGE_RULESET_ANCHOR_MOD:
      case EDGE_RULESET_USES_MOD:
      case EDGE_FSENT_RULESET:
        edges_push(ctx->edges, edge);
      default:
        break;
    }
  }
}

bool ruleset_reap(struct ruleset * restrict rules, struct graph_disintegrate_context * restrict ctx)
{
  if(edges_check_up(&rules->vertex, EDGE_RULESET_USES_MOD)) {
    return false;
  }
  if(edges_check_up(&rules->vertex, EDGE_RULESET_BUILD_MOD)) {
    return false;
  }
  if(edges_check_up(&rules->vertex, EDGE_RULESET_ANCHOR_MOD)) {
    return false;
  }

  /* no longer referenced */
  return true;
}

void ruleset_id(const struct ruleset * restrict ruleset, const char ** restrict id, uint16_t * restrict idl)
{
  *id = ruleset->id;
  *idl = ruleset->id_len;
}

void ruleset_compute_id(const struct ruleset * restrict rules, struct identity_ctx * restrict id)
{
  identity_type(id, vertex_type_get(&rules->vertex));
  identity_add(id, " ", 1);
  fsent_id_path_compute(rules->self_node, id);
}

#if CONFIG_DEVEL
void rulesets_show(void)
{
  ruleset *rules;
  int x = 0;

  printf("rulesets, live : %zu free : %zu\n", llist_count(&ruleset_list), llist_count(&ruleset_freelist));
  llist_foreach(&ruleset_list, rules, vertex.owner) {
    printf("%3u ", x++);
    vertex_identity_show(&rules->vertex);
    vertex_edges_show(&rules->vertex);
  }
}
#endif
