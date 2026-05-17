/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>
#include <inttypes.h>

#include "bacon/parser.h"
#include "xlinux/xstdlib.h"
#include "bam/ipc.h"
#include "common/snarf.h"
#include "common/hash.h"
#include "narrator.h"
#include "moria/foreach.h"
#include "bacon/builder.h"

#include "ruleset.h"
#include "ruleset_parser.h"
#include "fsent.h"
#include "params.h"
#include "system.h"
#include "events.h"
#include "channel.h"
#include "stats.h"
#include "vertex.h"
#include "edge.h"
#include "module_edge.h"
#include "special_edge.h"
#include "rule_module_association.h"
#include "entlist.h"
#include "pattern_parser.internal.h"
#include "vertex/probes.h"
#include "shadow.h"

#include "zcat.h"

llist ruleset_list = LLIST_INITIALIZER(ruleset_list);                 // active rulesets
static llist ruleset_freelist = LLIST_INITIALIZER(ruleset_freelist);  // freelist

static ruleset_parser *parser;

//
// static
//

void ruleset_rebuild_enqueue(ruleset * restrict rules)
{
  rules->invalid.disconnect = true;
/*
  this is set when the rules file node is invalidated
  rules->invalid.reparse = true;
*/

  /* move to the end of the rebuild/invalid list */
  llist_delete(&rules->invalid.lln);
  llist_unshift(&module_rebuild_list, &rules->invalid.lln);
}

void ruleset_dispose(ruleset * restrict rules)
{
  block_dispose_bulk(parser->bacon_parser, &rules->blocks);

  llist_delete(&rules->vertex.owner);
  llist_append(&ruleset_freelist, &rules->vertex.owner);
}

void ruleset_release(ruleset * restrict rules, graph_invalidation_context * restrict invalidation)
{
  moria_vertex __attribute__((unused)) *v;

  ruleset_disconnect(rules, invalidation);
  ruleset_excise(rules, invalidation);

  /* must have been fully excised from the graph previously */
  v = &rules->vertex;
  RUNTIME_ASSERT(v->up_identity == 0);
  RUNTIME_ASSERT(rbtree_empty(&v->up));
  RUNTIME_ASSERT(rbtree_empty(&v->down));

  llist_delete(&rules->invalid.lln);

  block_release_bulk(parser->bacon_parser, &rules->blocks);

  vertex_deleted_probe(&rules->vertex);

  llist_delete(&rules->vertex.owner);
  llist_append(&ruleset_freelist, &rules->vertex.owner);
}

void ruleset_alloc(ruleset ** restrict rp)
{
  ruleset * rules;

  if((rules = llist_shift(&ruleset_freelist, typeof(*rules), vertex.owner)) == 0) {
    xmalloc(&rules, sizeof(*rules));
  } else {
    memset(rules, 0, sizeof(*rules));
  }

  moria_vertex_init(&rules->vertex, &g_graph, VERTEX_RULESET | VERTEX_OK);

  llist_init(&rules->blocks);
  llist_init(&rules->rmas_owner);
  llist_init(&rules->invalid.lln);
  rules->invalid.attrs = rules->vertex.attrs;

  llist_append(&ruleset_list, &rules->vertex.owner);

  vertex_alloc_probe(&rules->vertex);
  *rp = rules;
}

static void rules_parse(ruleset * restrict rules)
{
  int exit;
  char * text = 0;
  size_t text_len;
  event_bacon *msg;
  events_context ectx;

  rules->ruleset_stats.parse++;
  system_stats.module_parse++;

  snarfs(&text, &text_len, rules->self_node_abspath);

  parser->rules = rules;
  parser->mod = 0;
  if(text && (exit = ruleset_parser_parse(parser, text, text_len + 2, rules->self_node_abspath, rules->self_node_abspath_len)))
  {
    msg = system_error_event(&ectx);
    msg->code = EINVAL;
    if(parser->bacon_parser->yyu && parser->bacon_parser->yyu->error_len) {
      msg->size += zcatw(msg->text + msg->size, sizeof(msg->text) - msg->size, parser->bacon_parser->yyu->error_str, parser->bacon_parser->yyu->error_len);
    } else if(parser->pattern_parser->yyu && parser->pattern_parser->yyu->error_len) {
      msg->size += zcatw(msg->text + msg->size, sizeof(msg->text) - msg->size, parser->pattern_parser->yyu->error_str, parser->pattern_parser->yyu->error_len);
    } else {
      msg->size += zcatw(msg->text + msg->size, sizeof(msg->text) - msg->size, parser->yyu.error_str, parser->yyu.error_len);
    }
    events_publish(&ectx);
  }
  else
  {
    rules->ruleset_stats.parse_ok++;
    system_stats.module_parse_ok++;
  }

  free(text);
}

void ruleset_reparse(ruleset * restrict rules)
{
  /* re-parse the rules.bam file */
  rules_parse(rules);
  if(system_has_error) {
    return;
  }

  /* replace the guts with the results from parsing */
  block_release_bulk(parser->bacon_parser, &rules->blocks);
  llist_splice_head(&rules->blocks, &parser->blocks);
}

static void rules_initialize(fsent * restrict rules_file_n)
{
  uint64_t u64;
  ruleset *rules = 0;

  ruleset_alloc(&rules);

  rules->self_node_abspath_len = fsent_absolute_path_cat(rules->self_node_abspath, sizeof(rules->self_node_abspath), rules_file_n);
  rules->self_node = rules_file_n;
  rules_file_n->self_rules = rules;

  // realpath hash
  u64 = hash64(0, rules->self_node_abspath, rules->self_node_abspath_len);
  rules->id_len = zcatf(rules->id, sizeof(rules->id), "%016"PRIx64, u64);
  rules->vertex.label = rules->id;
  rules->vertex.label_len = rules->id_len;

  /* protect the special node from disintegration */
  vertex_protect_set(&rules->vertex, true);

  /* magick the file node */
  vertex_type_set(&rules_file_n->vertex, VERTEX_RULESET_FILE);
}

void ruleset_bootstrap(fsent * restrict rules_file_n, graph_invalidation_context * restrict invalidation)
{
  struct ruleset *rules;

  /* already bootstrapped */
  if(vertex_type_get(&rules_file_n->vertex) == VERTEX_RULESET_FILE) {
    return;
  }

  rules_initialize(rules_file_n);

  // module-dir realpath hash
  rules = rules_file_n->self_rules;

  special_connect(&rules_file_n->vertex, &rules->vertex, invalidation);

  rules->invalid.reparse = true;
  ruleset_rebuild_enqueue(rules);

  /* shadow link to the special node itself */
  shadow_link(g_shadow_rulesets, rules->id, 16, &rules->vertex, 0);
}

void ruleset_disconnect(ruleset * restrict rules, graph_invalidation_context * restrict invalidation)
{
  entlist edges, vertices;
  block *block;

  /* disconnect edges from rules defined by the module */
  entlist_init(&edges);
  llist_foreach(&rules->blocks, block, lln) {
    block_gather_rule_edges(&edges, block);
  }
  entlist_init(&vertices);
  graph_disintegrate_bulk(&edges, &vertices, invalidation);
  entlist_destroy(&edges);
  entlist_destroy(&vertices);
}

void ruleset_setup()
{
  ruleset_parser_alloc(&parser);
}

void ruleset_cleanup()
{
  ruleset *rules;
  llist *T;

  llist_foreach_safe(&ruleset_list, rules, vertex.owner, T) {
    ruleset_dispose(rules);
  }

  llist_foreach_safe(&ruleset_freelist, rules, vertex.owner, T) {
    free(rules);
  }

  ruleset_parser_free(parser);
}

/* remove a module from the graph */
void ruleset_excise(ruleset * restrict rules, graph_invalidation_context * restrict invalidation)
{
  entlist edges, *eln;
  entlist vertices;
  moria_edge *edge;
  moria_foreach_state st;
  rule_module_association *rma;

  /* kill rmas for rules in this rules file */
  entlist_init(&edges);
  moria_foreach_edge_down(&st, &rules->vertex, edge) {
    switch(edge_type_get(edge)) {
      case EDGE_RMA_RULESET:
        rma = containerof(edge->B, typeof(*rma), vertex);
        entlist_push(&edges, rma);
      default:
        break;
    }
  }
  entlist_foreach(&edges, rma, eln) {
    rule_module_association_release(rma, invalidation);
  }
  entlist_destroy(&edges);

  /* disconnect other edges */
  moria_foreach_edge(&st, &rules->vertex, edge) {
    switch(edge_type_get(edge)) {
      case EDGE_RMA_RULESET:
      case EDGE_SPECIAL:
      case EDGE_RULESET_MOD:
        entlist_push(&edges, edge);
        break;
      default:
        RTA;
    }
  }

  entlist_init(&vertices);
  graph_disintegrate_bulk(&edges, &vertices, invalidation);
  entlist_destroy(&edges);
  entlist_destroy(&vertices);

  /* un-magick the file node */
  vertex_type_set(&rules->self_node->vertex, VERTEX_FILE);
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

  /* uses */
  count = 0;
  moria_traverse_vertex_edges(&g_graph, &rules->vertex, moria_edge_count, 0
    , (moria_traversal_criteria[]) {{
          edge_travel : EDGE_RULESET_MOD
        , edge_visit : EDGE_RULESET_MOD
      }}
    , MORIA_TRAVERSE_UP | MORIA_TRAVERSE_PRE | MORIA_TRAVERSE_DEPTH
    , &count
  );
  rules->ruleset_stats.uses_up = count;

  /* ruleset */
  count = 0;
  llist_foreach(&rules->blocks, block, lln) {
    count += block_count_rules(block);
  }
  rules->ruleset_stats.rules = count;
}

static void blocks_describe_rules(const llist * restrict blocks, bacon_writer * restrict builder_writer)
{
  const struct block *block;
  const struct rule *rule;

  llist_foreach(blocks, block, lln) {
    blocks_describe_rules(&block->blocks, builder_writer);

    llist_foreach(&block->rules, rule, lln) {
      vertex_path_describe(&rule->vertex, builder_writer);
    }
  }
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
  moria_foreach_state st;
  moria_edge *e;

  { MAPPING
    bacon_writer_push_string(builder_writer);
    vertex_path_say(&ruleset->vertex, builder_writer->N);
    bacon_writer_pop_string(builder_writer);
    { SET
      {
        vertex_mappings(&ruleset->vertex, builder_writer);
        { MAPPING
          STRING("self")
          BYTES(ruleset->self_node_abspath, ruleset->self_node_abspath_len)
        }
        { MAPPING
          STRING("modules")
          { SET
            moria_foreach_edge_up(&st, &ruleset->vertex, e) {
              switch(edge_type_get(e)) {
                case EDGE_RULESET_MOD:
                  vertex_path_describe(e->A, builder_writer);
                default:
              }
            }
          }
        }
        { MAPPING
          STRING("rules")
          { SET
            blocks_describe_rules(&ruleset->blocks, builder_writer);
          }
        }
        { MAPPING
          STRING("stats")
          { SET
            ruleset_stats_build((struct ruleset *)ruleset, false, builder_writer);
          }
        }
      }
    }
  }
}

void ruleset_file_stats_build(fsent * restrict fse, bool reset, bacon_writer * restrict builder_writer)
{
  fsent_stats_build(fse, reset,  builder_writer);
}

void ruleset_file_describe(const fsent * restrict fse, struct bacon_writer * restrict builder_writer)
{
  { MAPPING
    bacon_writer_push_string(builder_writer);
    vertex_path_say(&fse->vertex, builder_writer->N);
    bacon_writer_pop_string(builder_writer);
    { SET
      fsent_mappings(fse, builder_writer);
      { MAPPING
        STRING("ruleset")
        LOADF("//rulesets/%.*s",  fse->self_rules->id_len, fse->self_rules->id)
      }
    }
  }
}

size_t ruleset_path_cat(void * restrict dst, size_t sz, const struct ruleset * restrict ruleset)
{
  return zcatf(dst, sz, "//rulesets/%.*s", ruleset->id_len, ruleset->id);
}

void ruleset_identity_say(const struct ruleset * restrict ruleset, struct narrator * restrict N)
{
  attrs32_say(ruleset->vertex.attrs, graph_vertex_attrs, N);
  narrator_sayf(N, " id %.*s path %s", ruleset->id_len, ruleset->id, ruleset->self_node_abspath);
}

void ruleset_file_identity_say(const struct fsent * restrict fse, struct narrator * restrict N)
{
  attrs32_say(fse->vertex.attrs, graph_vertex_attrs, N);
  narrator_sayf(N, " id %.*s path %s", fse->self_rules->id_len, fse->self_rules->id, fse->self_rules->self_node_abspath);
}
