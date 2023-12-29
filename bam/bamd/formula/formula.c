/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>

#include "xlinux/xstdlib.h"
#include "xlinux/xunistd.h"
#include "xlinux/xfcntl.h"
#include "moria/foreach.h"
#include "narrator/fixed.h"
#include "bacon/builder.h"

#include "formula.h"
#include "formula_parser.h"
#include "formula_value.h"
#include "fsent.h"
#include "vertex.h"
#include "rule_module_association.h"
#include "edge.h"
#include "stats.h"
#include "marshal.h"
#include "channel.h"
#include "system_state.h"
#include "events.h"
#include "formula_parser.internal.h"
#include "selector_parser.h"
#include "reconcile.h"
#include "meta_edge.h"
#include "special_edge.h"
#include "entlist.h"
#include "vertex/probes.h"

#include "common/snarf.h"
#include "zcat.h"

static llist formula_list = LLIST_INITIALIZER(formula_list);                  // active
static llist formula_freelist = LLIST_INITIALIZER(formula_freelist);          // free
static llist formula_invalid_list = LLIST_INITIALIZER(formula_invalid_list);  // invalidated

static formula_parser *parser;

static void formula_parse(formula * restrict fml)
{
  char * text = 0;
  size_t text_len;
  int exit;
  channel *chan;
  bamipc_message *msg;
  bacon_writer writer, *builder_writer;
  narrator_fixed fixed;

  // open the file, both to read its contents, and to exec later
  ixclose(&fml->fd);
  fml->fd = xopenats(O_RDONLY, 0, fml->abspath);

  formula_value_ifree(&fml->file);
  formula_value_ifree(&fml->envs);
  formula_value_ifree(&fml->args);

  STATS_INC(fml->fml_stats.parse);
  STATS_INC(g_stats.formula_parse);

  fsnarf(&text, &text_len, fml->fd);

  if(text_len)
  {
    if((exit = formula_parser_parse(parser, text, text_len, fml->abspath, fml)))
    {
      msg = system_error_event(&chan);
      narrator_fixed_init(&fixed, msg->text, sizeof(msg->text));
      bacon_writer_init(&writer);
      builder_writer = bacon_writer_open(&writer, &fixed.base);

      { SET
        if(parser->selector_parser->yyu.error_len) {
          BYTES(parser->selector_parser->yyu.error_str, parser->selector_parser->yyu.error_len)
        } else {
          BYTES(parser->yyu.error_str, parser->yyu.error_len)
        }
        { MAPPING
          STRING("location")
          STRING(fml->abspath)
        }
      }
      bacon_writer_close(&writer);

      msg->code = EINVAL;
      msg->size = fixed.l;
      events_publish(chan, msg);
      goto end;
    }
  }

  STATS_INC(fml->fml_stats.parse_ok);
  STATS_INC(g_stats.formula_parse_ok);

end:
  free(text);
}

static void formula_destroy(formula * restrict fml)
{
  xclose(fml->fd);
  formula_value_free(fml->file);
  formula_value_free(fml->envs);
  formula_value_free(fml->args);
}

static void formula_dispose(formula * restrict fml)
{
  formula_destroy(fml);

  llist_delete(&fml->vertex.owner);
  llist_append(&formula_freelist, &fml->vertex.owner);
}

void formula_release(formula * restrict fml, graph_invalidation_context * restrict invalidation)
{
  moria_vertex *v;

  /* must have been fully excised from the graph previously */
  v = &fml->vertex;
  RUNTIME_ASSERT(v->up_identity == 0);
  RUNTIME_ASSERT(rbtree_empty(&v->up));
  RUNTIME_ASSERT(rbtree_empty(&v->down));

  /* un-magick the file node */
  vertex_type_set(&fml->self_node->vertex, VERTEX_FILE);

  vertex_deleted_probe(v);
  formula_dispose(fml);
}

//
// public
//

void formula_setup()
{
  formula_parser_create(&parser);
}

void formula_cleanup()
{
  formula *fml;
  llist *T;

  formula_parser_free(parser);

  llist_foreach_safe(&formula_list, fml, vertex.owner, T) {
    formula_dispose(fml);
  }

  llist_foreach_safe(&formula_freelist, fml, vertex.owner, T) {
    free(fml);
  }
}

void formula_system_reconcile(graph_invalidation_context * restrict invalidation)
{
  moria_edge *edge;
  entlist edges, vertices;
  formula *fml;
  moria_foreach_state st;
  bool connected;
  llist list, *T;

  entlist_init(&edges);
  entlist_init(&vertices);
  llist_init(&list);

  /* destroy invalidated formulas which are no longer connected to any rma */
  llist_foreach_safe(&formula_invalid_list, fml, invalid_lln, T) {
    connected = false;
    moria_foreach_edge_up(&st, &fml->vertex, edge) {
      switch(edge_type_get(edge)) {
        case EDGE_FML_RMA:
          connected = true;
          break;
        default:
      }
    }

    if(connected) {
      continue;
    }

    llist_delete(&fml->invalid_lln);
    llist_push(&list, &fml->invalid_lln);

    moria_foreach_edge(&st, &fml->vertex, edge) {
      entlist_push(&edges, edge);
    }
  }

  graph_disintegrate_bulk(&edges, &vertices, invalidation);
  entlist_destroy(&edges);
  entlist_destroy(&vertices);

  llist_foreach(&list, fml, invalid_lln) {
    formula_release(fml, invalidation);
  }

  /* re-parse all remaining, invalidated formulas */
  while((fml = llist_shift(&formula_invalid_list, typeof(*fml), invalid_lln))) {
    formula_parse(fml);
    if(system_error) {
      llist_push(&formula_invalid_list, &fml->invalid_lln);
      break;
    }
    vertex_mark_fresh(&fml->vertex);
    vertex_mark_fresh(&fml->self_node->vertex);
  }
}

void formula_parse_enqueue(formula * restrict fml)
{
  llist_push(&formula_invalid_list, &fml->invalid_lln);
}

void formula_create(formula ** restrict fmlp, moria_graph * restrict g)
{
  formula *fml;

  if((fml = llist_shift(&formula_freelist, typeof(*fml), vertex.owner)) == 0) {
    xmalloc(&fml, sizeof(*fml));
  } else {
    memset(fml, 0, sizeof(*fml));
  }

  moria_vertex_init(&fml->vertex, g, VERTEX_FORMULA | VERTEX_OK);
  llist_append(&formula_list, &fml->vertex.owner);
  llist_init(&fml->invalid_lln);

  fml->fd = -1;

  vertex_protect_set(&fml->vertex, true);
  vertex_alloc_probe(&fml->vertex);
  *fmlp = fml;
}

size_t formula_stats_collate(void *dst, size_t sz, bool reset, formula * restrict fml)
{
  size_t z;
  bam_formula_stats *stats;
  bam_formula_stats tmp;

  /* stats for the formula vertex */
  z = 0;
  z += vertex_stats_collate(dst + z, sz - z, reset, &fml->vertex);

  /* stats for the formula */
  memcpy(&tmp, &fml->fml_stats, sizeof(tmp));
  if(reset) {
    memset(&fml->fml_stats, 0, sizeof(fml->fml_stats));
  }
  stats = &tmp;

  z += descriptor_marshal_object(dst + z, sz - z, &descriptor_bam_formula_stats, &stats->record, 0);

  return z;
}
