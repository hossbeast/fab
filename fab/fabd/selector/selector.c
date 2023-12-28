/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>

#include "common/attrs.h"
#include "moria/traverse.h"
#include "moria/foreach.h"
#include "narrator/growing.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "bacon/writer.h"
#include "bacon/builder.h"
#include "xlinux/xstdlib.h"

#include "selector.internal.h"
#include "dependency.h"
#include "fsent.h"
#include "lookup.h"
#include "module.h"
#include "pattern.h"
#include "request.h"
#include "channel.h"
#include "zcat.h"
#include "system_state.h"
#include "vertex.h"
#include "events.h"
#include "buildplan_entity.h"
#include "edge.h"
#include "shadow.h"

attrs32 * selector_type_attrs = (attrs32[]) {{
#define SELECTOR_DEF(x, s, r, y) + 1
    num : 0 SELECTOR_TYPE_TABLE
  , members : (member32[]) {
#undef SELECTOR_DEF
#define SELECTOR_DEF(x, s, r, y) { name : s, value : UINT32_C(y), range : UINT32_C(r) },
SELECTOR_TYPE_TABLE
  }
}};

attrs32 * selector_nodeset_attrs = (attrs32[]) {{
#undef SELECTOR_DEF
#define SELECTOR_DEF(x, s, r, y) + 1
    num : 0 SELECTOR_NODESET_TABLE
  , members : (member32[]) {
#undef SELECTOR_DEF
#define SELECTOR_DEF(x, s, r, y) { name : s, value : UINT32_C(y), range : UINT32_C(r) },
SELECTOR_NODESET_TABLE
  }
}};

static moria_traversal_criteria criteria_fstree;
static moria_traversal_criteria criteria_fstree_hidden;
static moria_traversal_criteria criteria_modtree;
static moria_traversal_criteria criteria_dirtree;
static moria_traversal_criteria criteria_depends;
static moria_traversal_criteria criteria_imports;
static moria_traversal_criteria criteria_uses;
static moria_traversal_criteria criteria_requires;

static void __attribute__((constructor)) init()
{
  attrs32_init(selector_type_attrs);
  attrs32_init(selector_nodeset_attrs);

  /* pre-configured traversals */
  selector_graph_fstree(&criteria_fstree);
  selector_graph_fstree_hidden(&criteria_fstree_hidden);
  selector_graph_modtree(&criteria_modtree);
  selector_graph_dirtree(&criteria_dirtree);
  selector_graph_depends(&criteria_depends);
  selector_graph_imports(&criteria_imports);
  selector_graph_uses(&criteria_uses);
  selector_graph_requires(&criteria_requires);
}

static void selector_visitor(moria_vertex * v, void * arg, moria_traversal_mode mode, int distance, moria_traverse_result * restrict result)
{
  selector_context * ctx = arg;
  selector * sel = ctx->sel;
  fsent *n;
  module *mod;

  if(vertex_kind_get(v) == VERTEX_KIND_FSENT)
  {
    n = containerof(v, fsent, vertex);

    /* terminate an upwards traversal at the root(s) */
    if(n == g_root || n == g_shadow)
    {
      if(sel->direction == MORIA_TRAVERSE_UP) {
        result->flags = MORIA_TRAVERSE_PRUNE;
      }
    }
    else if(sel->criteria.edge_travel & EDGE_KIND_FSTREE)
    {
      mod = fsent_module_get(n);
      if(!sel->cross_module && mod && ctx->mod && mod != ctx->mod)
      {
        result->flags = MORIA_TRAVERSE_PRUNE;
        return;
      }
    }
  }
  else if(vertex_kind_get(v) == VERTEX_KIND_BPE)
  {
    return;
  }
  else
  {
    RUNTIME_ASSERT(vertex_special_get(v) == VERTEX_SPECIAL_MODULE);
  }

  selection_add_vertex(ctx->selection, v, distance);
}

static void enoent_error(channel * restrict chan, const char * restrict name, uint16_t name_len)
{
  bamipc_message *msg;

  if(chan)
  {
    msg = channel_produce(chan);
    msg->code = ENOENT;
    msg->type = BAMIPC_MSG_RESPONSE;
    msg->size = zcatf(msg->text, sizeof(msg->text), "not found %.*s", (int)name_len, name);
    channel_post(chan, msg);
  }
  else
  {
    msg = system_error_event(&chan);
    msg->code = ENOENT;
    msg->size = zcatf(msg->text, sizeof(msg->text), "not found %.*s", (int)name_len, name);
    events_publish(chan, msg);
  }
}

static void selector_ctx_exec(selector_context * restrict ctx)
{
  selector * sel;
  moria_vertex_traversal_state * state;
  selector *op;
  selection *in = 0;
  selection *out = 0;
  selection * T;
  int x;
  fsent * n;
  moria_vertex *v;
  selected *sn;
  module *mod;
  const char *name;
  bool *errp;
  uint32_t attrs;
  struct bacon_value *vars;
  moria_foreach_state st;
  moria_edge *edge;
  entlist *eln;

  if(ctx->chan) {
    errp = &ctx->chan->error;
  } else {
    errp = &system_error;
  }

  sel = ctx->sel;
  if(sel->type == SELECTOR_SEQUENCE)
  {
    /* input to each operation is the output from the preceding operation */
    llist_foreach(&sel->head, op, lln) {
      ctx->sel = op;
      selector_ctx_exec(ctx);
      if(*errp) {
        break;
      }
    }
  }
  else if(sel->type == SELECTOR_UNION)
  {
    selection_create(&in, ctx->iteration_type);
    selection_create(&out, ctx->iteration_type);
    selection_replicate(in, ctx->selection);

    /* independent input to each operation, output is their union */
    x = 0;
    llist_foreach(&sel->head, op, lln) {
      if(x)
      {
        selection_reset(ctx->selection, ctx->iteration_type);
        selection_replicate(ctx->selection, in);
      }

      ctx->sel = op;
      selector_ctx_exec(ctx);
      selection_replicate(out, ctx->selection);
      x++;

      if(*errp) {
        /* should probably break here only if all selectors in the union have an error */
        break;
      }
    }

    T = ctx->selection;
    ctx->selection = out;
    out = T;
  }
  else if(sel->type == SELECTOR_NODESET)
  {
    if(sel->nodeset == SELECTOR_NODESET_MODULE)
    {
      if(ctx->mod)
      {
        selection_add_vertex(ctx->selection, &ctx->mod->vertex, 0);
      }
    }
    else if(sel->nodeset == SELECTOR_NODESET_ANCHOR)
    {
      if(ctx->mod)
      {
        selection_add_vertex(ctx->selection, &ctx->mod->anchor_node->vertex, 0);
      }
    }
    else if(!ctx->bpe)
    {
      name = attrs32_name_byvalue(selector_nodeset_attrs, SELECTOR_NODESET_OPT & sel->nodeset);
      enoent_error(ctx->chan, MMS(name));
    }
    else if(sel->nodeset == SELECTOR_NODESET_TARGET || sel->nodeset == SELECTOR_NODESET_TARGETS)
    {
      moria_foreach_edge_up(&st, &ctx->bpe->vertex, edge) {
        if(edge_type_get(edge) != EDGE_BPE_TGT) {
          continue;
        }

        if(!(edge->attrs & MORIA_EDGE_HYPER))
        {
          selection_add_vertex(ctx->selection, edge->A, 0);
        }
        else
        {
          for(x = 0; x < edge->Alen; x++)
          {
            selection_add_vertex(ctx->selection, edge->Alist[x].v, 0);
            if(sel->nodeset == SELECTOR_NODESET_TARGET) {
              break;
            }
          }
        }
        if(sel->nodeset == SELECTOR_NODESET_TARGET) {
          break;
        }
      }
    }
    else if(sel->nodeset == SELECTOR_NODESET_SOURCE || sel->nodeset == SELECTOR_NODESET_SOURCES)
    {
      moria_foreach_edge_down(&st, &ctx->bpe->vertex, edge) {
        if(edge_type_get(edge) != EDGE_SRC_BPE) {
          continue;
        }
        if(!(edge->attrs & MORIA_EDGE_HYPER))
        {
          selection_add_vertex(ctx->selection, edge->B, 0);
        }
        else
        {
          for(x = 0; x < edge->Blen; x++)
          {
            selection_add_vertex(ctx->selection, edge->Blist[x].v, 0);
            if(sel->nodeset == SELECTOR_NODESET_SOURCE) {
              break;
            }
          }
        }
        if(sel->nodeset == SELECTOR_NODESET_SOURCE) {
          break;
        }
      }
    }
  }
  else if(sel->type == SELECTOR_PATTERN)
  {
    vars = 0;
    if(ctx->mod) {
      vars = ctx->mod->vars;
    }
    entlist_destroy(&ctx->lookup_hits);
    pattern_lookup(sel->pattern, vars, ctx->cwd, &ctx->lookup_hits);
    entlist_foreach(&ctx->lookup_hits, v, eln) {
      selection_add_vertex(ctx->selection, v, 0);
    }
  }
  else if(sel->type == SELECTOR_LOOKUP)
  {
    n = fsent_path_lookup(ctx->cwd, ctx->mod, sel->path, sel->path_len, sel->nofollow);
    if(n && !sel->hidden && !vertex_exists_get(&n->vertex)) {
      n = 0;
    }

    if(n) {
      selection_add_vertex(ctx->selection, &n->vertex, 0);
    } else {
      enoent_error(ctx->chan, sel->path, sel->path_len);
    }
  }
  else if(sel->type == SELECTOR_TRAVERSE)
  {
    moria_vertex_traversal_begin(&g_graph, &state);
    ctx->sel = sel;

    in = ctx->selection;
    selection_create(&ctx->selection, ctx->iteration_type);

    llist_foreach(&in->list, sn, lln) {
      v = sn->v;

      /* sidestep from the module node, to the module directory node */
      if(vertex_type_get(v) == VERTEX_MODULE) {
        if(!(sel->criteria.edge_travel & EDGE_MODULES)) {
          mod = containerof(v, module, vertex);
          v = &mod->dir_node->vertex;
        }
      }

      /* sidestep from the fsent, to the owning module */
      else if(vertex_special_get(v) != VERTEX_SPECIAL_MODULE) {
        if(sel->criteria.edge_travel & EDGE_MODULES) {
          mod = fsent_module_get(containerof(v, fsent, vertex));
          v = &mod->vertex;
        }
      }

      attrs = 0;
      if(sel->exhaustive) {
        attrs |= MORIA_TRAVERSE_EXHAUSTIVE;
      }
      if(sel->nofollow) {
        attrs |= MORIA_TRAVERSE_NOFOLLOW;
      }
//      if(sel->criteria.min_depth || sel->criteria.max_depth) {
        attrs |= MORIA_TRAVERSE_DEPTH;
//      }

      moria_traverse_vertices(
          &g_graph
        , v
        , selector_visitor
        , state
        /* selectors can only visit fsent nodes */
        , &sel->criteria
        , sel->direction | sel->mode | attrs
        , ctx
      );
    }

    moria_vertex_traversal_end(&g_graph, state);
  }

  selection_finalize(ctx->selection);
  selection_free(in);
  selection_free(out);
}

static int criteria_cmp(moria_traversal_criteria * restrict A, moria_traversal_criteria * restrict B)
{
  return memcmp_region(A, B, vertex_travel_mask, edge_visit);
}

static void writer_write(selector * const restrict sel, bacon_writer * const restrict builder_writer, bool top)
{
  selector * next;
  const char *name;
  char space[512];
  narrator * N;
  narrator_fixed nstor;

  switch(sel->type) {
    case SELECTOR_SEQUENCE:
      // sequence is the default top level aggregate for a selector
      if(!top || llist_count(&sel->head) != 1)
      {
        bacon_writer_push_mapping(builder_writer);
        bacon_writer_string(builder_writer, "sequence");
        bacon_writer_push_list(builder_writer);
      }
      llist_foreach(&sel->head, next, lln) {
        writer_write(next, builder_writer, false);
      }
      if(!top || llist_count(&sel->head) != 1)
      {
        bacon_writer_pop_list(builder_writer);
        bacon_writer_pop_mapping(builder_writer);
      }
      break;
    case SELECTOR_UNION:
      { MAPPING
        STRING("union")
        { LIST
          llist_foreach(&sel->head, next, lln) {
            writer_write(next, builder_writer, false);
          }
        }
      }
      break;
    case SELECTOR_TRAVERSE:
      { MAPPING
        STRING("traverse")
        { SET
          if(sel->criteria.min_depth != SELECTOR_TRAVERSE_DEFAULT_MIN_DEPTH) {
            MAPPING
            STRING("min-distance")
            UINT(sel->criteria.min_depth)
          }
          if(sel->criteria.max_depth != SELECTOR_TRAVERSE_DEFAULT_MAX_DEPTH) {
            MAPPING
            STRING("max-distance")
            UINT(sel->criteria.max_depth)
          }
          { MAPPING
            STRING("direction")
            STRING(attrs32_name_byvalue(traverse_type_attrs, TRAVERSE_TYPE_OPT & sel->direction))
          }
          { MAPPING
            STRING("order")
            STRING(attrs32_name_byvalue(traverse_order_attrs, TRAVERSE_ORDER_OPT & sel->mode))
          }
          { MAPPING
            STRING("graph")
            if(criteria_cmp(&sel->criteria, &criteria_fstree) == 0) {
              STRING("fs")
            } else if(criteria_cmp(&sel->criteria, &criteria_fstree_hidden) == 0) {
              STRING("fs-hidden")
            } else if(criteria_cmp(&sel->criteria, &criteria_modtree) == 0) {
              STRING("modtree")
            } else if(criteria_cmp(&sel->criteria, &criteria_dirtree) == 0) {
              STRING("dirtree")
            } else if(criteria_cmp(&sel->criteria, &criteria_depends) == 0) {
              STRING("depends")
            } else if(criteria_cmp(&sel->criteria, &criteria_imports) == 0) {
              STRING("imports")
            } else if(criteria_cmp(&sel->criteria, &criteria_uses) == 0) {
              STRING("uses")
            } else if(criteria_cmp(&sel->criteria, &criteria_requires) == 0) {
              STRING("requires")
            } else {
              RTA;
            }
          }
          { MAPPING
            STRING("exhaustive")
            BOOL(sel->exhaustive)
          }
          { MAPPING
            STRING("cross-module")
            BOOL(sel->exhaustive)
          }
          { MAPPING
            STRING("nofollow")
            BOOL(sel->exhaustive)
          }
        }
      }
      break;
    case SELECTOR_PATTERN:
      N = narrator_fixed_init(&nstor, space, sizeof(space));
      pattern_say(sel->pattern, N);
      { MAPPING
        STRING("pattern")
        BYTES(space, nstor.l)
      }
      break;
    case SELECTOR_LOOKUP:
      { MAPPING
        STRING("lookup")
        { SET
          { MAPPING
            STRING("path")
            BYTES(sel->path, sel->path_len)
          }
          { MAPPING
            STRING("nofollow")
            BOOL(sel->nofollow)
          }
        }
      }
      break;
    case SELECTOR_NODESET:
      name = attrs32_name_byvalue(selector_nodeset_attrs, SELECTOR_NODESET_OPT & sel->nodeset);
      RUNTIME_ASSERT(name);
      STRING(name);
      break;
    default:
      RTA;
  }
}


//
// internal
//

void selector_writer_write(selector * const restrict sel, bacon_writer * const restrict writer)
{
  writer_write(sel, writer, true);
}

//
// public
//

void selector_free(selector * restrict sel)
{
  llist *T;
  selector *op;

  if(sel)
  {
    if(sel->type == SELECTOR_SEQUENCE || sel->type == SELECTOR_UNION)
    {
      llist_foreach_safe(&sel->head, op, lln, T) {
        llist_delete(&op->lln);
        selector_free(op);
      }
    }
    else if(sel->type == SELECTOR_PATTERN)
    {
      pattern_free(sel->pattern);
    }
    else if(sel->type == SELECTOR_LOOKUP)
    {
      free(sel->path);
    }
  }

  free(sel);
}

void selector_alloc(selector_type type, selector ** restrict rs)
{
  selector *s;

  xmalloc(&s, sizeof(*s));

  llist_init(&s->lln);
  s->type = type;
  if(type == SELECTOR_SEQUENCE || type == SELECTOR_UNION)
  {
    llist_init(&s->head);
  }

  *rs = s;
}

void selector_ifree(selector ** restrict s)
{
  selector_free(*s);
  *s = 0;
}

void selector_context_init(selector_context *ctx)
{
  entlist_init(&ctx->lookup_hits);
}

void selector_context_destroy(selector_context *ctx)
{
  selection_free(ctx->selection);
  entlist_destroy(&ctx->lookup_hits);
}

void selector_exec(selector * restrict sel, selector_context * restrict ctx, selection_iteration_type iteration_type)
{
  if(!ctx->selection)
    selection_create(&ctx->selection, iteration_type);
  else
    selection_reset(ctx->selection, iteration_type);

  ctx->sel = sel;
  ctx->iteration_type = iteration_type;

  RUNTIME_ASSERT(ctx->cwd);

  selector_ctx_exec(ctx);
}

void selector_say(selector * const restrict sel, narrator * const restrict N)
{
  bacon_writer writer;
  bacon_writer_init(&writer);
  bacon_writer_open(&writer, N);

  selector_writer_write(sel, &writer);

  bacon_writer_close(&writer);
  bacon_writer_destroy(&writer);
}

void selector_graph_fstree(moria_traversal_criteria *criteria)
{
  criteria->vertex_travel_mask = VERTEX_KIND_OPT;
  criteria->vertex_travel      = VERTEX_KIND_FSENT;
  criteria->vertex_visit_mask  = VERTEX_KIND_OPT | VERTEX_EXISTS_BIT;
  criteria->vertex_visit       = VERTEX_KIND_FSENT | VERTEX_EXISTS_BIT;
  criteria->edge_travel_mask   = EDGE_TYPE_OPT;
  criteria->edge_travel        = EDGE_FSTREE;
  criteria->edge_visit_mask    = 0;
  criteria->edge_visit         = 0;
}

void selector_graph_fstree_hidden(moria_traversal_criteria *criteria)
{
  criteria->vertex_travel_mask = VERTEX_KIND_OPT;
  criteria->vertex_travel      = VERTEX_KIND_FSENT;
  criteria->vertex_visit_mask  = VERTEX_KIND_OPT;
  criteria->vertex_visit       = VERTEX_KIND_FSENT;
  criteria->edge_travel_mask   = EDGE_TYPE_OPT;
  criteria->edge_travel        = EDGE_FSTREE;
  criteria->edge_visit_mask    = 0;
  criteria->edge_visit         = 0;
}

void selector_graph_modtree(moria_traversal_criteria *criteria)
{
  criteria->vertex_travel_mask = VERTEX_KIND_OPT;
  criteria->vertex_travel      = VERTEX_KIND_FSENT;
  criteria->vertex_visit_mask  = VERTEX_KIND_OPT;
  criteria->vertex_visit       = VERTEX_KIND_FSENT | VERTEX_MODULE_DIR;
  criteria->edge_travel_mask   = EDGE_TYPE_OPT;
  criteria->edge_travel        = EDGE_FSTREE;
  criteria->edge_visit_mask    = 0;
  criteria->edge_visit         = 0;
}

void selector_graph_dirtree(moria_traversal_criteria *criteria)
{
  criteria->vertex_travel_mask = VERTEX_KIND_OPT;
  criteria->vertex_travel      = VERTEX_KIND_FSENT;
  criteria->vertex_visit_mask  = VERTEX_KIND_OPT | VERTEX_FILETYPE_OPT;
  criteria->vertex_visit       = VERTEX_KIND_FSENT | VERTEX_FILETYPE_DIR;
  criteria->edge_travel_mask   = EDGE_TYPE_OPT;
  criteria->edge_travel        = EDGE_FSTREE;
  criteria->edge_visit_mask    = 0;
  criteria->edge_visit         = 0;
}

void selector_graph_depends(moria_traversal_criteria *criteria)
{
  criteria->vertex_travel_mask = VERTEX_FSE_BPE;
  criteria->vertex_travel      = VERTEX_FSE_BPE;
  criteria->vertex_visit_mask  = VERTEX_KIND_OPT;
  criteria->vertex_visit       = VERTEX_KIND_FSENT;
  criteria->edge_travel_mask   = EDGE_DEPENDS;
  criteria->edge_travel        = EDGE_DEPENDS;
  criteria->edge_visit_mask    = 0;
  criteria->edge_visit         = 0;
}

void selector_graph_imports(moria_traversal_criteria *criteria)
{
  criteria->vertex_travel_mask = 0;
  criteria->vertex_travel      = 0;
  criteria->vertex_visit_mask  = VERTEX_KIND_OPT;
  criteria->vertex_visit       = VERTEX_KIND_FSENT;
  criteria->edge_travel_mask   = EDGE_TYPE_OPT;
  criteria->edge_travel        = -1;
  criteria->edge_visit_mask    = 0;
  criteria->edge_visit         = 0;
}

void selector_graph_uses(moria_traversal_criteria *criteria)
{
  criteria->vertex_travel_mask = VERTEX_KIND_OPT;
  criteria->vertex_travel      = VERTEX_KIND_FSENT;
  criteria->vertex_visit_mask  = VERTEX_KIND_OPT;
  criteria->vertex_visit       = VERTEX_KIND_FSENT;
  criteria->edge_travel_mask   = EDGE_TYPE_OPT;
  criteria->edge_travel        = EDGE_RULES_MOD;
  criteria->edge_visit_mask    = 0;
  criteria->edge_visit         = 0;
}

void selector_graph_requires(moria_traversal_criteria *criteria)
{
  criteria->vertex_travel_mask = VERTEX_KIND_OPT;
  criteria->vertex_travel      = VERTEX_KIND_FSENT;
  criteria->vertex_visit_mask  = VERTEX_KIND_OPT;
  criteria->vertex_visit       = VERTEX_KIND_FSENT;
  criteria->edge_travel_mask   = EDGE_TYPE_OPT;
  criteria->edge_travel        = EDGE_REQUIRES;
  criteria->edge_visit_mask    = 0;
  criteria->edge_visit         = 0;
}
