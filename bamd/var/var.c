/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>
#include <inttypes.h>

#include "common/snarf.h"
#include "descriptor.h"
#include "bam/stats.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "bacon/value.h"
#include "bacon/make.h"
#include "bacon/merge.h"
#include "bacon/parser.h"
#include "valyria/pstring.h"
#include "valyria/hashtable.h"
#include "xlinux/xstdlib.h"
#include "moria/edge.h"
#include "moria/foreach.h"

#include "var.h"
#include "fsent.h"
#include "vertex.h"
#include "edge.h"
#include "reconcile.h"
#include "marshal.h"
#include "stats.h"
#include "handler.h"
#include "channel.h"
#include "system_state.h"
#include "events.h"
#include "entlist.h"
#include "vertex/probes.h"

#include "zcat.h"

llist var_list = LLIST_INITIALIZER(var_list);                         // active
static llist var_freelist = LLIST_INITIALIZER(var_freelist);          // free
static llist var_invalid_list = LLIST_INITIALIZER(var_invalid_list);  // invalidated

static bacon_parser *parser;

//
// static
//

static void var_parse(var * restrict vp)
{
  char * text = 0;
  size_t text_len;
  bacon_value * val;
  int exit;
  bamipc_message *msg;
  channel *chan;

  vp->val = 0;

  STATS_INC(vp->var_stats.parse);
  STATS_INC(g_stats.var_parse);

  snarfs(&text, &text_len, vp->self_node_abspath);

  if(text)
  {
    if((exit = bacon_parser_parse(parser, text, text_len, vp->self_node_abspath, BACON_TYPE_SET, &val)))
    {
      bacon_value_free(parser, val);

      msg = system_error_event(&chan);
      msg->code = EINVAL;
      msg->size += zcatw(msg->text + msg->size, sizeof(msg->text) - msg->size, parser->yyu->error_str, parser->yyu->error_len);
      events_publish(chan, msg);
      goto end;
    }

    vp->val = val;
  }

  STATS_INC(vp->var_stats.parse_ok);
  STATS_INC(g_stats.var_parse_ok);

end:
  free(text);
}

static void var_dispose(struct var * restrict var)
{
  bacon_value_free(parser, var->val);

  llist_delete(&var->vertex.owner);
  llist_append(&var_freelist, &var->vertex.owner);
}

void var_release(var * restrict var, graph_invalidation_context * restrict invalidation)
{
  moria_vertex __attribute__((unused)) *v;
  moria_edge *edge;
  moria_foreach_state st;
  entlist edges, vertices;

  /* proactive edge disconnect */
  v = &var->vertex;
  entlist_init(&edges);
  moria_foreach_edge(&st, v, edge) {
    switch(edge_type_get(edge)) {
      case EDGE_VAR_MOD:
      case EDGE_VAR_SPECIAL:
        entlist_push(&edges, edge);
        break;
      default:
        break;
    }
  }

  entlist_init(&vertices);
  graph_disintegrate_bulk(&edges, &vertices, invalidation);
  entlist_destroy(&edges);
  entlist_destroy(&vertices);

  /* should be fully excised from the graph previously */
  RUNTIME_ASSERT(v->up_identity == 0);
  RUNTIME_ASSERT(rbtree_empty(&v->up));
  RUNTIME_ASSERT(rbtree_empty(&v->down));

  /* un-magick the file node */
  vertex_type_set(&var->self_node->vertex, VERTEX_FILE);

  vertex_deleted_probe(&var->vertex);
  var_dispose(var);
}

//
// public
//

void var_alloc(var ** restrict vp, moria_graph * restrict g)
{
  struct var *var;

  if((var = llist_shift(&var_freelist, typeof(*var), vertex.owner)) == 0) {
    xmalloc(&var, sizeof(*var));
  } else {
    memset(var, 0, sizeof(*var));
  }

  moria_vertex_init(&var->vertex, g, VERTEX_VAR | VERTEX_OK);

  llist_append(&var_list, &var->vertex.owner);

  vertex_protect_set(&var->vertex, true);
  vertex_alloc_probe(&var->vertex);
  *vp = var;
}

void var_reparse(var * restrict var)
{
  if(!vertex_invalid_get(&var->vertex)) {
    return;
  }

  bacon_value_free(parser, var->val);
  var->val = 0;

  var_parse(var);
  if(!system_error) {
    vertex_mark_fresh(&var->vertex);
    vertex_mark_fresh(&var->self_node->vertex);
  }
}

void var_setup()
{
  bacon_parser_alloc(&parser);
}

void var_cleanup()
{
  var *v;
  llist *T;

  llist_foreach_safe(&var_list, v, vertex.owner, T) {
    var_dispose(v);
  }

  llist_foreach_safe(&var_freelist, v, vertex.owner, T) {
    free(v);
  }

  bacon_parser_free(parser);
}

size_t var_stats_collate(void *dst, size_t sz, bool reset, struct var * restrict var)
{
  size_t z;
  bam_var_stats *stats;
  bam_var_stats tmp;

  /* stats for the var vertex */
  z = 0;
  z += vertex_stats_collate(dst + z, sz - z, reset, &var->vertex);

  /* var stats */
  memcpy(&tmp, &var->var_stats, sizeof(tmp));
  if(reset) {
    memset(&var->var_stats, 0, sizeof(var->var_stats));
  }
  stats = &tmp;

  z += descriptor_marshal_object(dst + z, sz - z, &descriptor_bam_var_stats, &stats->record, 0);

  return z;
}

void var_system_reconcile(graph_invalidation_context * restrict invalidation)
{
  struct var *var;
  llist *T;
  moria_foreach_state st;
  moria_edge *edge;

  /* destroy vars that are no longer in use */
  llist_foreach_safe(&var_list, var, vertex.owner, T) {
    moria_foreach_edge(&st, &var->vertex, edge) {
      switch(edge_type_get(edge)) {
        case EDGE_VAR_SPECIAL:
          continue;
        default:
          goto next;
      }
    }

    var_release(var, invalidation);
  next:
    var = 0;
  }
}
