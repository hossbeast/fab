/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>

#include "valyria/hashtable.h"
#include "narrator.h"
#include "bacon/writer.h"
#include "bacon/builder.h"
#include "narrator/fixed.h"

#include "buildplan.h"
#include "dependency.h"
#include "formula.h"
#include "buildplan_entity.h"
#include "fsent.h"
#include "vertex.h"
#include "channel.h"
#include "zcat.h"
#include "common/attrs.h"
#include "events.h"
#include "system_state.h"
#include "rule_module_association.h"

selection buildplan_selection;
uint16_t buildplan_id;
enum buildplan_state buildplan_state;

typedef struct traverse_context {
  moria_vertex *stack[32];
  uint16_t stack_len;
} traverse_context;

/*
 * Add a node to the plan
 *
 * @distance - length of the path to this node from the target
 */
static void buildplan_visitor_direct(
    moria_vertex * v
  , void * arg
  , moria_traversal_mode mode
  , int distance
  , moria_traverse_result * restrict result
)
{
  fsent *fse;
  channel *chan;
  fabipc_message *msg;
  bacon_writer writer, *builder_writer;
  narrator_fixed fixed;
  traverse_context *ctx;
  uint16_t stack_index;
  int x;

  /* tracking the path by which this node was visited */
  ctx = arg;
  stack_index = distance / 2;
  if(stack_index < ARRAY_LEN(ctx->stack)) {
    ctx->stack[stack_index] = v;
  }

  if(!vertex_invalid_get(v)) {
    return;
  }
  fse = containerof(v, fsent, vertex);

  if(!fse->bpe && vertex_shadowtype_get(v)) {
    /* special case ; shadow nodes can act as placeholders, even with no attached formula */
    selection_add_vertex(&buildplan_selection, &fse->vertex, distance);
    return;
  }

  /* this is an error case - node is not up to date, and theres no way to update it */
  if(!fse->bpe)
  {
    buildplan_state = UNSATISFIED;

    if(fse->bp_plan_id == buildplan_id) {
      /* already issued the error for this node */
      return;
    }
    fse->bp_plan_id = buildplan_id;

    msg = system_error_event(&chan);
    narrator_fixed_init(&fixed, msg->text, sizeof(msg->text));
    bacon_writer_init(&writer);
    builder_writer = bacon_writer_open(&writer, &fixed.base);
    { MAPPING
      STRING("cannot update")
      { SET
        { MAPPING
          STRING("trail")
          { LIST
            for(x = 0; x <= stack_index; x++) {
              vertex_describe(ctx->stack[x], builder_writer);
            }
          }
        }
      }
    }
    bacon_writer_close(builder_writer);
    msg->size = fixed.l;
    msg->code = -1;
    events_publish(chan, msg);
    return;
  }

  selection_add_bpe(&buildplan_selection, fse->bpe, distance);
}

void buildplan_setup()
{
  selection_init(&buildplan_selection);
  buildplan_id = 1;
}

void buildplan_cleanup()
{
  selection_destroy(&buildplan_selection);
}

void buildplan_reset()
{
  selection_reset(&buildplan_selection, SELECTION_ITERATION_TYPE_RANK);
  buildplan_id++;
  buildplan_state = NOOP;
}

void buildplan_finalize()
{
  selection_finalize(&buildplan_selection);
  buildplan_report();
}

void buildplan_report()
{
  selected *sn;
  const buildplan_entity *bpe;

  printf("%3zu executions in %2hu stages\n", buildplan_selection.selected_entities.size, buildplan_selection.numranks);

return;

  llist_foreach(&buildplan_selection.list, sn, lln) {
    printf(" %-3d ", sn->rank);
    switch(sn->type) {
      case SELECTED_BPE:
        bpe = sn->bpe;
        printf("bpe %p %s", bpe, bpe->fml->self_node->name.name);
        printf(" -> ");
        buildplan_entity_say_targets(bpe, g_narrator_stdout);
        break;
      case SELECTED_VERTEX:
        fsent_absolute_path_say(containerof(sn->v, fsent, vertex), g_narrator_stdout);
        break;
      default:
        RTA;
    }
    printf("\n");
  }
}

void buildplan_add_transitive(moria_vertex * restrict v)
{
  traverse_context ctx;

  moria_traverse_vertices(
      &g_graph
    , v
    , buildplan_visitor_direct
    , 0
    , (moria_traversal_criteria[]) {{
          edge_travel : EDGE_DEPENDS
        , vertex_visit_mask : VERTEX_KIND_OPT
        , vertex_visit : VERTEX_KIND_FSENT
      }}
    , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE | MORIA_TRAVERSE_EXHAUSTIVE
    , &ctx
  );
}

void buildplan_select_transitive(llist * restrict selection)
{
  selected *sn;

  llist_foreach(selection, sn, lln) {
    buildplan_add_transitive(sn->v);
  }
}

void buildplan_select_direct(llist * restrict selection)
{
  selected *sn;

  llist_foreach(selection, sn, lln) {
    buildplan_visitor_direct((void*)sn->v, 0, 0, 0, 0);
  }
}
