/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>
#include <inttypes.h>

#include "bacon/parser.h"
#include "xlinux/xstdlib.h"
#include "bam/ipc.h"
#include "common/snarf.h"
#include "common/hash.h"
#include "moria/foreach.h"

#include "rules.h"
#include "rules_parser.h"
#include "fsent.h"
#include "params.h"
#include "system_state.h"
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

#include "zcat.h"

static llist reload_list = LLIST_INITIALIZER(reload_list);

llist rules_list = LLIST_INITIALIZER(rules_list);                 // active modules
static llist rules_freelist = LLIST_INITIALIZER(rules_freelist);  // freelist

static rules_parser *parser;

//
// static
//

void rules_rebuild_enqueue(rules * restrict rules)
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

void rules_dispose(rules * restrict rules)
{
  block_dispose_bulk(parser->bacon_parser, &rules->blocks);

  llist_delete(&rules->vertex.owner);
  llist_append(&rules_freelist, &rules->vertex.owner);
}

void rules_release(rules * restrict rules, graph_invalidation_context * restrict invalidation)
{
  moria_vertex __attribute__((unused)) *v;

  rules_disconnect(rules, invalidation);
  rules_excise(rules, invalidation);

  /* must have been fully excised from the graph previously */
  v = &rules->vertex;
  RUNTIME_ASSERT(v->up_identity == 0);
  RUNTIME_ASSERT(rbtree_empty(&v->up));
  RUNTIME_ASSERT(rbtree_empty(&v->down));

  llist_delete(&rules->invalid.lln);

  block_release_bulk(parser->bacon_parser, &rules->blocks);

  vertex_deleted_probe(&rules->vertex);

  llist_delete(&rules->vertex.owner);
  llist_append(&rules_freelist, &rules->vertex.owner);
}


void rules_alloc(rules ** restrict rp)
{
  rules * rules;

  if((rules = llist_shift(&rules_freelist, typeof(*rules), vertex.owner)) == 0) {
    xmalloc(&rules, sizeof(*rules));
  } else {
    memset(rules, 0, sizeof(*rules));
  }

  moria_vertex_init(&rules->vertex, &g_graph, VERTEX_RULES | VERTEX_OK);

  llist_init(&rules->blocks);
  llist_init(&rules->rmas_owner);
  llist_init(&rules->invalid.lln);
  rules->invalid.attrs = rules->vertex.attrs;

  llist_append(&rules_list, &rules->vertex.owner);

  vertex_alloc_probe(&rules->vertex);
  *rp = rules;
}

static void rules_parse(rules * restrict rules)
{
  int exit;
  char * text = 0;
  size_t text_len;
  bamipc_message *msg;
  channel *chan;

  STATS_INC(rules->rules_stats.parse);
  STATS_INC(g_stats.module_parse);

  snarfs(&text, &text_len, rules->self_node_abspath);

  if(text && (exit = rules_parser_parse(parser, text, text_len + 2, rules->self_node_abspath, rules->self_node_abspath_len)))
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
    STATS_INC(rules->rules_stats.parse_ok);
    STATS_INC(g_stats.module_parse_ok);
  }

  free(text);
}

void rules_reparse(rules * restrict rules)
{
  /* re-parse the rules.bam file */
  rules_parse(rules);
  if(system_error) {
    return;
  }

  /* replace the guts with the results from parsing */
  block_release_bulk(parser->bacon_parser, &rules->blocks);
  llist_splice_head(&rules->blocks, &parser->blocks);
}

static void rules_initialize(fsent * restrict rules_file_n)
{
  rules *rules = 0;
  uint64_t u64;
  uint64_t token;

  rules_alloc(&rules);

  rules->self_node_abspath_len = fsent_absolute_path_cat(rules->self_node_abspath, sizeof(rules->self_node_abspath), rules_file_n);
  rules->self_node = rules_file_n;
  rules_file_n->self_rules = rules;

  // realpath hash
  u64 = hash64(0, rules->self_node_abspath, rules->self_node_abspath_len);
  rules->id_len = zcatf(rules->id, sizeof(rules->id), "%016"PRIx64, u64);
  token = atomic_fetch_inc(&vertex_token_counter);
  rules->token_len = zcatf(rules->token, sizeof(rules->token), "%016"PRIx64, hash64(u64, MM(token)));

  /* protect the special node from disintegration */
  vertex_protect_set(&rules->vertex, true);

  /* magick the file node */
  vertex_type_set(&rules_file_n->vertex, VERTEX_RULES_FILE);
}

void rules_bootstrap(fsent * restrict rules_file_n, graph_invalidation_context * restrict invalidation)
{
  rules *rules;

  /* already bootstrapped */
  if(vertex_type_get(&rules_file_n->vertex) == VERTEX_RULES_FILE) {
    return;
  }

  rules_initialize(rules_file_n);

  // module-dir realpath hash
  rules = rules_file_n->self_rules;

  special_connect(&rules_file_n->vertex, &rules->vertex, EDGE_RULES_SPECIAL, invalidation);

  rules->invalid.reparse = true;

  rules_rebuild_enqueue(rules);
}

void rules_disconnect(rules * restrict rules, graph_invalidation_context * restrict invalidation)
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

void rules_setup()
{
  rules_parser_alloc(&parser);
}

void rules_cleanup()
{
  rules *rules;
  llist *T;

  llist_foreach_safe(&rules_list, rules, vertex.owner, T) {
    rules_dispose(rules);
  }

  llist_foreach_safe(&rules_freelist, rules, vertex.owner, T) {
    free(rules);
  }

  rules_parser_free(parser);
}

/* remove a module from the graph */
void rules_excise(rules * restrict rules, graph_invalidation_context * restrict invalidation)
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
      case EDGE_RMA_RULES:
        rma = containerof(edge->B, typeof(*rma), vertex);
        entlist_push(&edges, rma);
      default:
    }
  }
  entlist_foreach(&edges, rma, eln) {
    rule_module_association_release(rma, invalidation);
  }
  entlist_destroy(&edges);

  /* disconnect other edges */
  moria_foreach_edge(&st, &rules->vertex, edge) {
    switch(edge_type_get(edge)) {
      case EDGE_RMA_RULES:
      case EDGE_RULES_SPECIAL:
      case EDGE_RULES_MOD:
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

size_t rules_stats_collate(void *dst, size_t sz, bool reset, struct rules * restrict rules)
{
  size_t z;
  bam_rules_stats *stats;
  bam_rules_stats tmp;
  uint32_t count;
  block *block;

  /* stats for the rules vertex */
  z = 0;
  z += vertex_stats_collate(dst + z, sz - z, reset, &rules->vertex);

  /* stats for the rules */
  memcpy(&tmp, &rules->rules_stats, sizeof(tmp));
  if(reset) {
    memset(&rules->rules_stats, 0, sizeof(rules->rules_stats));
  }
  stats = &tmp;

  /* uses */
  count = 0;
  moria_traverse_vertex_edges(&g_graph, &rules->vertex, moria_edge_count, 0
    , (moria_traversal_criteria[]) {{
          edge_travel : EDGE_RULES_MOD
        , edge_visit : EDGE_RULES_MOD
      }}
    , MORIA_TRAVERSE_UP | MORIA_TRAVERSE_PRE | MORIA_TRAVERSE_DEPTH
    , &count
  );
  stats->uses_up = count;

  /* rules */
  count = 0;
  llist_foreach(&rules->blocks, block, lln) {
    count += block_count_rules(block);
  }
  stats->rules = count;

  z += descriptor_marshal_object(dst + z, sz - z, &descriptor_bam_rules_stats, &stats->record, 0);

  return z;
}
