/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "bacon/builder.h"
#include "narrator.h"
#include "moria/foreach.h"

#include "edge.h"
#include "edges.h"
#include "graph.h"
#include "disintegrate.h"
#include "fsent.h"
#include "symlink.h"

#include "fsent_symlink.h"

bool fsent_symlink_reap(struct fsent * restrict fse, struct graph_disintegrate_context * restrict ctx)
{
  moria_foreach_state st;
  moria_edge *edge;

  moria_foreach_edge(&st, &fse->vertex, edge) {
    switch(edge_type_get(edge)) {
      case EDGE_REF_SYMLINK:
        break;
      default:
        return false;
    }
  }

  /* reap if the only edge is the ref */
  return true;
}

void fsent_symlink_unlink(struct fsent * restrict fse, struct graph_disintegrate_context * restrict ctx)
{
  moria_foreach_state st;
  moria_edge *edge;

  symlink_readlink_cancel(fse);

  moria_foreach_edge(&st, &fse->vertex, edge) {
    switch(edge_type_get(edge)) {
      case EDGE_REF_SYMLINK:
        edges_push(ctx->edges, edge);
      default:
        break;
    }
  }
}

void fsent_symlink_describe(fsent * restrict fse, bacon_writer * restrict builder_writer)
{
  moria_edge *e;

  { MAPPING
    vertex_path_describe(&fse->vertex, builder_writer);
    { SET
      vertex_mappings(&fse->vertex, builder_writer);
//      { MAPPING
//        STRING("readlink-pending")
//        BOOL(llist_attached(&fse->fs_readlink_lln));
//      }
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
        bacon_writer_push_string(builder_writer);
        if((e = edges_check_down(&fse->vertex, EDGE_REF_SYMLINK))) {
          vertex_path_say(e->B, builder_writer->N);
        } else {
          narrator_says(builder_writer->N, "_");
        }
        bacon_writer_pop_string(builder_writer);
      }
      { MAPPING STRING("stats")
        { SET
          vertex_stats_describe(&fse->vertex, false, builder_writer);
        }
      }
      vertex_relations_describe(&fse->vertex, builder_writer);
    }
  }
}

moria_vertex *fsent_symlink_follow(const fsent * restrict fse)
{
  const struct moria_vertex *ref;
  const struct moria_edge *edge;

  ref = &fse->vertex;
  while(vertex_type_get(ref) == VERTEX_SYMLINK) {
    if((edge = edges_check_down(ref, EDGE_REF_SYMLINK)) == 0) {
      return 0; /* unresolved link */
    }
    ref = edge->B;
  }

  return (struct moria_vertex*)ref;
}
