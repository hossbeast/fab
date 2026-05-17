/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>
#include <inttypes.h>

#include "common/snarf.h"
#include "descriptor.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "bacon/value.h"
#include "bacon/make.h"
#include "bacon/merge.h"
#include "bacon/parser.h"
#include "bacon/builder.h"
#include "valyria/pstring.h"
#include "valyria/hashtable.h"
#include "xlinux/xstdlib.h"
#include "moria/edge.h"
#include "moria/foreach.h"

#include "varset.h"
#include "fsent.h"
#include "vertex.h"
#include "edge.h"
#include "reconcile.h"
#include "marshal.h"
#include "stats.h"
#include "handler.h"
#include "channel.h"
#include "system.h"
#include "events.h"
#include "entlist.h"
#include "vertex/probes.h"

#include "zcat.h"

llist varset_list = LLIST_INITIALIZER(varset_list);                         // active
static llist varset_freelist = LLIST_INITIALIZER(varset_freelist);          // free
static llist varset_invalid_list = LLIST_INITIALIZER(varset_invalid_list);  // invalidated

static bacon_parser *parser;

//
// static
//

static void var_parse(varset * restrict vp)
{
  char * text = 0;
  size_t text_len;
  bacon_value * val;
  int exit;
  event_bacon *msg;
  events_context ectx;

  vp->val = 0;

  vp->varset_stats.parse++;
  system_stats.varset_parse++;

  snarfs(&text, &text_len, vp->self_node_abspath);

  if(text)
  {
    if((exit = bacon_parser_parse(parser, text, text_len, vp->self_node_abspath, BACON_TYPE_SET, &val)))
    {
      bacon_value_free(parser, val);

      msg = system_error_event(&ectx);
      msg->code = EINVAL;
      msg->size += zcatw(msg->text + msg->size, sizeof(msg->text) - msg->size, parser->yyu->error_str, parser->yyu->error_len);
      events_publish(&ectx);
      goto end;
    }

    vp->val = val;
  }

  vp->varset_stats.parse_ok++;
  system_stats.varset_parse_ok++;

end:
  free(text);
}

static void var_dispose(struct varset * restrict var)
{
  bacon_value_free(parser, var->val);

  llist_delete(&var->vertex.owner);
  llist_append(&varset_freelist, &var->vertex.owner);
}

void varset_release(varset * restrict var, graph_invalidation_context * restrict invalidation)
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
      case EDGE_VARSET_MOD:
      case EDGE_SPECIAL:
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

void varset_alloc(varset ** restrict vp, moria_graph * restrict g)
{
  struct varset *var;

  if((var = llist_shift(&varset_freelist, typeof(*var), vertex.owner)) == 0) {
    xmalloc(&var, sizeof(*var));
  } else {
    memset(var, 0, sizeof(*var));
  }

  moria_vertex_init(&var->vertex, g, VERTEX_VARSET | VERTEX_OK);

  llist_append(&varset_list, &var->vertex.owner);

  vertex_protect_set(&var->vertex, true);
  vertex_alloc_probe(&var->vertex);
  *vp = var;
}

void varset_reparse(varset * restrict var)
{
  if(!vertex_invalid_get(&var->vertex)) {
    return;
  }

  bacon_value_free(parser, var->val);
  var->val = 0;

  var_parse(var);
  if(!system_has_error) {
    vertex_mark_fresh(&var->vertex);
    vertex_mark_fresh(&var->self_node->vertex);
  }
}

void varset_setup()
{
  bacon_parser_alloc(&parser);
}

void varset_cleanup()
{
  varset *v;
  llist *T;

  llist_foreach_safe(&varset_list, v, vertex.owner, T) {
    var_dispose(v);
  }

  llist_foreach_safe(&varset_freelist, v, vertex.owner, T) {
    free(v);
  }

  bacon_parser_free(parser);
}

void varset_stats_collate(struct varset * restrict var, bool reset)
{
  if(reset) {
    memset(&var->stats, 0, sizeof(var->stats));
    memset(&var->varset_stats, 0, sizeof(var->varset_stats));
  }
}

void varset_system_reconcile(graph_invalidation_context * restrict invalidation)
{
  struct varset *var;
  llist *T;
  moria_foreach_state st;
  moria_edge *edge;

  /* destroy vars that are no longer in use */
  llist_foreach_safe(&varset_list, var, vertex.owner, T) {
    moria_foreach_edge(&st, &var->vertex, edge) {
      switch(edge_type_get(edge)) {
        case EDGE_SPECIAL:
          continue;
        default:
          goto next;
      }
    }

    varset_release(var, invalidation);
  next:
    var = 0;
  }
}

size_t varset_path_cat(void * restrict dst, size_t sz, const varset * restrict cfg)
{
  return zcatf(dst, sz, "//varsets/%.*s", cfg->id_len, cfg->id);
}

void varset_stats_build(varset * restrict varset, bool reset, bacon_writer * restrict builder_writer)
{
  stats_build(&varset->stats.record, &descriptor_stats_vertex, builder_writer);
  stats_build(&varset->varset_stats.record, &descriptor_stats_varset, builder_writer);
}

void varset_describe(const varset * restrict varset, struct bacon_writer * restrict builder_writer)
{
  moria_foreach_state st;
  moria_edge *e;

  { MAPPING
    vertex_path_describe(&varset->vertex, builder_writer);
    { SET
      {
        vertex_mappings(&varset->vertex, builder_writer);
        { MAPPING STRING("self")
          BYTES(varset->self_node_abspath, varset->self_node_abspath_len)
        }
        { MAPPING STRING("modules")
          { LIST
            moria_foreach_edge_up(&st, &varset->vertex, e) {
              switch(edge_type_get(e)) {
                case EDGE_VARSET_MOD:
                  vertex_path_describe(e->A, builder_writer);
                default:
              }
            }
          }
        }
        { MAPPING STRING("stats")
          { SET
            varset_stats_build((struct varset*)varset, false, builder_writer);
          }
        }
      }
    }
  }
}

void varset_file_stats_build(fsent * restrict fse, bool reset, bacon_writer * restrict builder_writer)
{
  fsent_stats_build(fse, reset, builder_writer);
}

void varset_file_describe(const fsent * restrict fse, struct bacon_writer * restrict builder_writer)
{
  { MAPPING
    vertex_path_describe(&fse->vertex, builder_writer);
    { SET
      fsent_mappings(fse, builder_writer);
      { MAPPING STRING("varset")
        vertex_path_describe(&fse->self_var->vertex, builder_writer);
      }
      { MAPPING STRING("stats")
        { SET
          varset_file_stats_build((struct fsent *)fse, false, builder_writer);
        }
      }
    }
  }
}

void varset_identity_say(const struct varset * restrict varset, struct narrator * restrict N)
{
  attrs32_say(varset->vertex.attrs, graph_vertex_attrs, N);
  narrator_sayf(N, " id %.*s path %s", varset->id_len, varset->id, varset->self_node_abspath);
}

void varset_file_identity_say(const struct fsent * restrict fse, struct narrator * restrict N)
{
  attrs32_say(fse->vertex.attrs, graph_vertex_attrs, N);
  narrator_sayf(N, " id %.*s path %s", fse->self_var->id_len, fse->self_var->id, fse->self_var->self_node_abspath);
}
