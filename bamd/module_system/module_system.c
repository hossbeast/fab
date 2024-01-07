/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <string.h>
#include <inttypes.h>
#include <errno.h>

#include "types.h"

#include "moria/edge.h"
#include "moria/foreach.h"
#include "bacon/value.h"
#include "bacon/merge.h"
#include "bacon/parser.h"
#include "bacon/writer.h"
#include "bacon/coerce.h"
#include "bacon/eval.h"
#include "bacon/expr.h"
#include "valyria/llist.h"
#include "valyria/map.h"
#include "valyria/hashtable.h"
#include "xlinux/xfcntl.h"
#include "xlinux/xstdlib.h"

#include "module.h"
#include "exec.h"
#include "exec_builder.h"
#include "exec_render.h"
#include "fsedge.h"
#include "fsent.h"
#include "rules_parser.h"
#include "params.h"
#include "pattern.h"
#include "lookup.h"
#include "generate.h"
#include "render.h"
#include "rule.h"
#include "rule_module_association.h"
#include "module_edge.h"
#include "dir_rma.h"
#include "rule_system.h"
#include "shadow.h"
#include "stats.h"
#include "channel.h"
#include "system_state.h"
#include "events.h"
#include "block.h"
#include "rule/probes.h"
#include "rules.h"
#include "config.h"
#include "vertex.h"
#include "edge.h"
#include "fsent.h"

#include "common/assure.h"
#include "common/snarf.h"
#include "common/hash.h"
#include "marshal.h"
#include "zcat.h"

void module_system_reconcile(rule_run_context * restrict rule_run_ctx)
{
  module *mod = 0;
  rules *rules = 0;
  rules_container *con;
  llist *T;
  graph_invalidation_context *invalidation;
  uint32_t count;
//  bool b;
  moria_foreach_state st;
  moria_edge *edge;
  llist reload_list = LLIST_INITIALIZER(reload_list);
  bool reimport;

  invalidation = &rule_run_ctx->invalidation;

  graph_invalidation_begin(invalidation);

  /* bootstrap modules under cwd */
  moria_traverse_vertices(
      &g_graph
    , &g_base->vertex
    , module_bootstrap_visitor
    , 0
    , (moria_traversal_criteria[]) {{
          edge_travel: EDGE_FSTREE
        , edge_visit : EDGE_FSTREE
        , vertex_travel : VERTEX_FILETYPE_DIR
        , vertex_visit : VERTEX_FILETYPE_DIR
      }}
    , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE
    , invalidation
  );

  /* destroy modules that have been deleted */
  llist_foreach_safe(&module_rebuild_list, con, lln, T) {
    switch(con->attrs & VERTEX_TYPE_OPT) {
      case VERTEX_MODULE:
        mod = containerof(con, typeof(*mod), invalid);
        rules = 0;
        break;
      case VERTEX_RULES:
        mod = 0;
        rules = containerof(con, typeof(*rules), invalid);
        break;
      default:
        RTA;
    }

    if(mod && !vertex_exists_get(&mod->self_node->vertex)) {
      module_release(mod, invalidation);
    } else if(rules && !vertex_exists_get(&rules->self_node->vertex)) {
      rules_release(rules, invalidation);
    }
  }

  /* reload modules and rules which have been invalidated */
  while((con = llist_shift(&module_rebuild_list, typeof(*con), lln))) {
    switch(con->attrs & VERTEX_TYPE_OPT) {
      case VERTEX_MODULE:
        mod = containerof(con, typeof(*mod), invalid);
        rules = 0;
        break;
      case VERTEX_RULES:
        mod = 0;
        rules = containerof(con, typeof(*rules), invalid);
        break;
      default:
        RTA;
    }

    if(con->disconnect) {
      llist_unshift(&module_rebuild_list, &con->lln);
      if(mod) {
        module_disconnect_rules(mod, invalidation);
      } else {
        rules_disconnect(rules, invalidation);
      }
      if(system_error) {
        goto end;
      }
      con->disconnect = false;
    } else if(con->reparse) {
      llist_unshift(&module_rebuild_list, &con->lln);
      if(mod) {
        module_reparse(mod);
      } else {
        rules_reparse(rules);
      }
      if(system_error) {
        goto end;
      }
      con->reparse = false;
      if(mod) {
        vertex_mark_fresh(&mod->self_node->vertex);
      } else {
        vertex_mark_fresh(&rules->self_node->vertex);
      }
    } else if(con->rebuild) {
      llist_unshift(&module_rebuild_list, &con->lln);
      if(mod) {
        module_rebuild(mod, rule_run_ctx);
      } else {
        /* rules files are not rebuilt */
      }
      if(system_error) {
        goto end;
      }
      con->rebuild = false;
    } else if(con->reimport) {
      llist_unshift(&module_rebuild_list, &con->lln);
      if(mod) {
        reimport = module_reimport(mod, rule_run_ctx);
      } else {
        /* rules files are not imported */
        reimport = false;
      }
      if(system_error) {
        goto end;
      }
      con->reimport = reimport;
    } else if(con->prune) {
      llist_unshift(&module_rebuild_list, &con->lln);
      if(mod) {
        module_prune(mod, invalidation);
      } else {
        /* rules files are not pruned */
      }
      con->prune = false;
    } else {
      llist_unshift(&reload_list, &con->lln);
    }
  }

  while((con = llist_shift(&reload_list, typeof(*con), lln))) {
    switch(con->attrs & VERTEX_TYPE_OPT) {
      case VERTEX_MODULE:
        mod = containerof(con, typeof(*mod), invalid);
        rules = 0;
        break;
      case VERTEX_RULES:
        mod = 0;
        rules = containerof(con, typeof(*rules), invalid);
        break;
      default:
        RTA;
    }

    if(mod) {
      module_reload(mod);
      vertex_mark_fresh(&mod->vertex);
    } else {
      /* rules files are not reloaded */
      vertex_mark_fresh(&rules->vertex);
    }
  }

end:

  /* destroy out-of-tree modules which are no longer referenced */
  llist_foreach_safe(&module_list, mod, vertex.owner, T) {
    /* in-tree check */
    if(mod->in_tree) {
      continue;
    }

    /* is-referenced by an in-tree module check */
    count = 0;
    moria_traverse_vertex_edges(&g_graph, &mod->vertex, moria_edge_count, 0
      , (moria_traversal_criteria[]) {{
            edge_travel : EDGE_MODULES
          , edge_visit : EDGE_MODULES
        }}
      , MORIA_TRAVERSE_UP | MORIA_TRAVERSE_PRE
      , &count
    );
    if(count > 0) {
      continue;
    }

    module_release(mod, invalidation);
  }

  /* destroy rules file that are no longer referenced */
  llist_foreach_safe(&rules_list, rules, vertex.owner, T) {
    moria_foreach_edge_up(&st, &rules->vertex, edge) {
      switch(edge_type_get(edge)) {
        /* special edges dont count as a reference */
        case EDGE_RULES_SPECIAL:
          break;
        default:
          goto next;
      }
    }

    rules_release(rules, invalidation);
  next:
  }

  graph_invalidation_end(invalidation);

  /* If the reload list is not empty, then reconciliation did not complete and any modules
   * in the reload list go back onto the rebuild list, so that they will be re-considered
   * for deletion on the next iteration */
  llist_splice_head(&module_rebuild_list, &reload_list);
}
