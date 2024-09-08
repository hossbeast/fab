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
#include "system.h"
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

llist formula_list = LLIST_INITIALIZER(formula_list);                         // active
static llist formula_freelist = LLIST_INITIALIZER(formula_freelist);          // free
static llist formula_invalid_list = LLIST_INITIALIZER(formula_invalid_list);  // invalidated

static formula_parser *parser;

static void formula_parse(formula * restrict fml)
{
  char * text = 0;
  size_t text_len;
  int exit;
  events_context ectx;
  event_bacon *msg;
  bacon_writer writer, *builder_writer;
  narrator_fixed fixed;

  // open the file, both to read its contents, and to exec later
  ixclose(&fml->fd);
  fml->fd = xopenats(O_RDONLY, 0, fml->abspath);

  formula_value_ifree(&fml->file);
  formula_value_ifree(&fml->envs);
  formula_value_ifree(&fml->args);

  fml->formula_stats.parse++;
  system_stats.formula_parse++;

  fsnarf(&text, &text_len, fml->fd);

  if(text_len)
  {
    if((exit = formula_parser_parse(parser, text, text_len, fml->abspath, fml)))
    {
      msg = system_error_event(&ectx);
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
      events_publish(&ectx);
      goto end;
    }
  }

  fml->formula_stats.parse_ok++;
  system_stats.formula_parse_ok++;

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
          break;
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

void formula_invalidated(formula * restrict fml)
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

void formula_stats_build(formula * restrict fml, bool reset, bacon_writer * restrict builder_writer)
{
  stats_build(&fml->stats.record, &descriptor_stats_vertex, builder_writer);
  stats_build(&fml->formula_stats.record, &descriptor_stats_formula, builder_writer);

  if(reset) {
    memset(&fml->stats, 0, sizeof(fml->stats));
    memset(&fml->formula_stats, 0, sizeof(fml->formula_stats));
  }
}

void formula_describe(const formula * restrict formula, bacon_writer * restrict builder_writer)
{
  moria_foreach_state st;
  moria_edge *e;

  { MAPPING
    vertex_path_describe(&formula->vertex, builder_writer);
    { SET
      vertex_mappings(&formula->vertex, builder_writer);
      { MAPPING STRING("self")
        BYTES(formula->abspath, formula->abspath_len)
      }
      { MAPPING STRING("rmas")
        { LIST
          moria_foreach_edge_up(&st, &formula->vertex, e) {
            switch(edge_type_get(e)) {
              case EDGE_FML_RMA:
                vertex_path_describe(e->A, builder_writer);
              default:
            }
          }
        }
      }
      { MAPPING STRING("stats")
        { SET
          formula_stats_build((struct formula *)formula, false, builder_writer);
        }
      }
    }
  }
}

void formula_file_stats_build(fsent * restrict fse, bool reset, bacon_writer * restrict builder_writer)
{
  fsent_stats_build(fse, reset, builder_writer);
}

void formula_file_describe(const fsent * restrict fse, struct bacon_writer * restrict builder_writer)
{
  { MAPPING
    vertex_path_describe(&fse->vertex, builder_writer);
    { SET
      fsent_mappings(fse, builder_writer);
      { MAPPING STRING("formula")
        vertex_path_describe(&fse->self_fml->vertex, builder_writer);
      }
      { MAPPING STRING("stats")
        { SET
          formula_file_stats_build((struct fsent *)fse, false, builder_writer);
        }
      }
    }
  }
}

size_t formula_path_cat(void * restrict dst, size_t sz, const formula * restrict formula)
{
  return zcatf(dst, sz, "//formulas/%.*s", formula->id_len, formula->id);
}

void formula_identity_say(const struct formula * restrict formula, struct narrator * restrict N)
{
  attrs32_say(formula->vertex.attrs, graph_vertex_attrs, N);
  narrator_sayf(N, " id %.*s path %s", formula->id_len, formula->id, formula->abspath);
}

void formula_file_identity_say(const struct fsent * restrict fse, struct narrator * restrict N)
{
  attrs32_say(fse->vertex.attrs, graph_vertex_attrs, N);
  narrator_sayf(N, " id %.*s path %s", fse->self_fml->id_len, fse->self_fml->id, fse->self_fml->abspath);
}
