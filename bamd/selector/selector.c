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
#include "system.h"
#include "vertex.h"
#include "events.h"
#include "buildplan_entity.h"
#include "edge.h"
#include "shadow.h"
#include "fstree.h"

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

static struct selector_traverse criteria_fs;
static struct selector_traverse criteria_depends;
static struct selector_traverse criteria_special;
static struct selector_traverse criteria_uses;
static struct selector_traverse criteria_requires;

static struct selector_traverse visit_module;
static struct selector_traverse visit_rules;
static struct selector_traverse visit_formula;
static struct selector_traverse visit_var;
static struct selector_traverse visit_config;
static struct selector_traverse visit_plugin;
static struct selector_traverse visit_directory;
static struct selector_traverse visit_module_dir;
static struct selector_traverse visit_rule;
static struct selector_traverse visit_fs;

static void __attribute__((constructor)) init()
{
  attrs32_init(selector_type_attrs);
  attrs32_init(selector_nodeset_attrs);

  /* pre-configured traversals */
  selector_graph_fs(&criteria_fs);
  selector_graph_depends(&criteria_depends);
  selector_graph_special(&criteria_special);
  selector_graph_uses(&criteria_uses);
  selector_graph_requires(&criteria_requires);

  /* pre-configured visits */
  selector_visit_special_module(&visit_module);
  selector_visit_special_formula(&visit_formula);
  selector_visit_special_plugin(&visit_plugin);
  selector_visit_special_ruleset(&visit_rules);
  selector_visit_special_varset(&visit_var);
  selector_visit_special_configset(&visit_config);
  selector_visit_directory(&visit_directory);
  selector_visit_module_dir(&visit_module_dir);
  selector_visit_rule(&visit_rule);
  selector_visit_fs(&visit_fs);
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
      if(sel->traverse.direction == MORIA_TRAVERSE_UP) {
        result->flags = MORIA_TRAVERSE_PRUNE;
      }
    }
    else if(sel->traverse.criteria.edge_travel & EDGE_KIND_FSTREE)
    {
      mod = vertex_module_get(v);
      if(!sel->traverse.cross_module && mod && ctx->mod && mod != ctx->mod)
      {
        result->flags = MORIA_TRAVERSE_PRUNE;
        return;
      }
    }
  }

  selection_add_vertex(ctx->selection, v, distance);
}

static void
__attribute__((nonnull(2)))
__attribute__((format(printf, 2, 3)))
selector_error(channel * restrict chan, const char * restrict fmt, ...)
{
  bamipc_message *msg;
  event_bacon *pb;
  events_context ectx;
  va_list va;

  va_start(va, fmt);
  if(chan)
  {
    msg = channel_produce(chan);
    msg->code = ENOENT;
    msg->type = BAMIPC_MSG_RESULT;
    msg->size = vsnprintf(msg->text, sizeof(msg->text), fmt, va);
    channel_post(chan, msg);
  }
  else
  {
    pb = system_error_event(&ectx);
    pb->code = ENOENT;
    pb->size = vsnprintf(pb->text, sizeof(pb->text), fmt, va);
    events_publish(&ectx);
  }
  va_end(va);
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
  fsent *n;
  moria_vertex *v;
  selected *sn;
  module *mod;
  const fstree *fst;
  const char *name;
  bool *errp;
  uint32_t attrs;
  struct bacon_value *vars;
  moria_foreach_state st;
  moria_edge *edge;
  entlist *eln;
  moria_traversal_criteria criteria;

  if(ctx->chan) {
    errp = &ctx->chan->error;
  } else {
    errp = &system_has_error;
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
      selector_error(ctx->chan, "not found %s", name);
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
    n = fsent_path_lookup(ctx->cwd, ctx->mod, sel->lookup.path, sel->lookup.path_len, sel->lookup.nofollow);
    if(n && !sel->lookup.hidden && !vertex_exists_get(&n->vertex)) {
      n = 0;
    }

    if(n) {
      selection_add_vertex(ctx->selection, &n->vertex, 0);
    } else {
      selector_error(ctx->chan, "not found %.*s", sel->lookup.path_len, sel->lookup.path);
    }
  }
  else if(sel->type == SELECTOR_MODULE_OF)
  {
    in = ctx->selection;
    selection_create(&ctx->selection, ctx->iteration_type);

    llist_foreach(&in->list, sn, lln) {
      v = sn->v;
      if(vertex_type_get(v) == VERTEX_CONTEXT_MODULE) {
        mod = (module *)ctx->mod;
      } else {
        mod = vertex_module_get(v);
      }
      if(mod) {
        selection_add_vertex(ctx->selection, &mod->vertex, 0);
      } else {
        selector_error(ctx->chan, "module not found for %.*s", v->label_len, v->label);
      }
    }
  }
  else if(sel->type == SELECTOR_FSTREE_OF)
  {
    in = ctx->selection;
    selection_create(&ctx->selection, ctx->iteration_type);

    llist_foreach(&in->list, sn, lln) {
      v = sn->v;
      if((fst = vertex_fstree_get(v))) {
        selection_add_vertex(ctx->selection, &fst->vertex, 0);
      } else {
        selector_error(ctx->chan, "fstree not found for %.*s", v->label_len, v->label);
      }
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
      if(vertex_type_get(v) == VERTEX_MODULE)
      {
        if(!(sel->traverse.criteria.edge_travel & EDGE_MODULES)) {
          mod = containerof(v, module, vertex);
          v = &mod->dir_node->vertex;
        }
      }
      /* sidestep from the fsent, to the owning module */
      else if(vertex_type_get(v) != VERTEX_MODULE)
      {
        if(sel->traverse.criteria.edge_travel & EDGE_MODULES) {
          mod = vertex_module_get(v);
          v = &mod->vertex;
        }
      }

      memcpy(&criteria, &sel->traverse.criteria, sizeof(criteria));

      attrs = 0;
      if(sel->traverse.exhaustive) {
        attrs |= MORIA_TRAVERSE_EXHAUSTIVE;
      }
      if(sel->traverse.nofollow) {
        attrs |= MORIA_TRAVERSE_NOFOLLOW;
      }
      if(sel->traverse.hidden) {
        criteria.vertex_visit_mask &= ~VERTEX_EXISTS_BIT;
        criteria.vertex_visit &= ~VERTEX_EXISTS_BIT;
      }
      criteria.vertex_visit_mask |= sel->traverse.visit_mask;
      criteria.vertex_visit |= sel->traverse.visit_bits;

      moria_traverse_vertices(
          &g_graph
        , v
        , selector_visitor
        , state
        /* selectors can only visit fsent nodes */
        , &criteria
        , sel->traverse.direction | sel->traverse.mode | MORIA_TRAVERSE_DEPTH | attrs
        , ctx
      );
    }

    moria_vertex_traversal_end(&g_graph, state);
  }

  selection_finalize(ctx->selection);
  selection_free(in);
  selection_free(out);
}

static int traverse_criteria_cmp(struct selector_traverse * restrict A, struct selector_traverse * restrict B)
{
  return memcmp_region(&A->criteria, &B->criteria, vertex_travel_mask, edge_visit);
}

static int traverse_visit_cmp(struct selector_traverse * restrict A, struct selector_traverse * restrict B)
{
  return 0
    ?: INTCMP(A->visit_mask, B->visit_mask)
    ?: INTCMP(A->visit_bits, B->visit_bits)
    ;
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
          if(sel->traverse.criteria.min_depth != SELECTOR_TRAVERSE_DEFAULT_MIN_DEPTH) {
            MAPPING
            STRING("min-distance")
            UINT(sel->traverse.criteria.min_depth)
          }
          if(sel->traverse.criteria.max_depth != SELECTOR_TRAVERSE_DEFAULT_MAX_DEPTH) {
            MAPPING
            STRING("max-distance")
            UINT(sel->traverse.criteria.max_depth)
          }
          { MAPPING
            STRING("direction")
            STRING(attrs32_name_byvalue(traverse_type_attrs, TRAVERSE_TYPE_OPT & sel->traverse.direction))
          }
          { MAPPING
            STRING("order")
            STRING(attrs32_name_byvalue(traverse_order_attrs, TRAVERSE_ORDER_OPT & sel->traverse.mode))
          }
          { MAPPING
            STRING("graph")
            if(traverse_criteria_cmp(&sel->traverse, &criteria_fs) == 0) {
              STRING("fs")
            } else if(traverse_criteria_cmp(&sel->traverse, &criteria_special) == 0) {
              STRING("special")
            } else if(traverse_criteria_cmp(&sel->traverse, &criteria_depends) == 0) {
              STRING("depends")
            } else if(traverse_criteria_cmp(&sel->traverse, &criteria_uses) == 0) {
              STRING("uses")
            } else if(traverse_criteria_cmp(&sel->traverse, &criteria_requires) == 0) {
              STRING("requires")
            } else {
              RTA;
            }
          }
          if(sel->traverse.visit_mask) {
            MAPPING
            STRING("visit")
            if(traverse_visit_cmp(&sel->traverse, &visit_module) == 0) {
              STRING("module")
            } else if(traverse_visit_cmp(&sel->traverse, &visit_rules) == 0) {
              STRING("rules")
            } else if(traverse_visit_cmp(&sel->traverse, &visit_formula) == 0) {
              STRING("formula")
            } else if(traverse_visit_cmp(&sel->traverse, &visit_var) == 0) {
              STRING("var")
            } else if(traverse_visit_cmp(&sel->traverse, &visit_config) == 0) {
              STRING("config")
            } else if(traverse_visit_cmp(&sel->traverse, &visit_plugin) == 0) {
              STRING("plugin")
            } else if(traverse_visit_cmp(&sel->traverse, &visit_directory) == 0) {
              STRING("directory")
            } else if(traverse_visit_cmp(&sel->traverse, &visit_module_dir) == 0) {
              STRING("module-dir")
            } else if(traverse_visit_cmp(&sel->traverse, &visit_rule) == 0) {
              STRING("rule")
            } else if(traverse_visit_cmp(&sel->traverse, &visit_fs) == 0) {
              STRING("fs")
            } else {
              RTA;
            }
          }
          { MAPPING
            STRING("exhaustive")
            BOOL(sel->traverse.exhaustive)
          }
          { MAPPING
            STRING("cross-module")
            BOOL(sel->traverse.cross_module)
          }
          { MAPPING
            STRING("hidden")
            BOOL(sel->traverse.hidden)
          }
          { MAPPING
            STRING("nofollow")
            BOOL(sel->traverse.nofollow)
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
            BYTES(sel->lookup.path, sel->lookup.path_len)
          }
          { MAPPING
            STRING("nofollow")
            BOOL(sel->lookup.nofollow)
          }
          { MAPPING
            STRING("hidden")
            BOOL(sel->lookup.hidden)
          }
        }
      }
      break;
    case SELECTOR_MODULE_OF:
      STRING("module-of")
      break;
    case SELECTOR_FSTREE_OF:
      STRING("fstree-of")
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
      free(sel->lookup.path);
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

void selector_exec(selector * restrict sel, selector_context * restrict ctx)
{
  if(!ctx->selection)
    selection_create(&ctx->selection, ctx->iteration_type);
  else
    selection_reset(ctx->selection, ctx->iteration_type);

  ctx->sel = sel;

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

void selector_graph_fs(struct selector_traverse *traverse)
{
  traverse->criteria.vertex_travel_mask = 0;
  traverse->criteria.vertex_travel      = 0;
  traverse->criteria.vertex_visit_mask  = VERTEX_EXISTS_BIT;
  traverse->criteria.vertex_visit       = VERTEX_EXISTS_BIT;
  traverse->criteria.edge_travel_mask   = EDGE_TYPE_OPT;
  traverse->criteria.edge_travel        = EDGE_FSTREE;
  traverse->criteria.edge_visit_mask    = 0;
  traverse->criteria.edge_visit         = 0;
}

void selector_graph_depends(struct selector_traverse *traverse)
{
  traverse->criteria.vertex_travel_mask = 0;
  traverse->criteria.vertex_travel      = 0;
  traverse->criteria.vertex_visit_mask  = VERTEX_KIND_OPT;
  traverse->criteria.vertex_visit       = VERTEX_KIND_FSENT;
  traverse->criteria.edge_travel_mask   = EDGE_DEPENDS;
  traverse->criteria.edge_travel        = EDGE_DEPENDS;
  traverse->criteria.edge_visit_mask    = 0;
  traverse->criteria.edge_visit         = 0;
}

void selector_graph_special(struct selector_traverse *traverse)
{
  traverse->criteria.vertex_travel_mask = 0;
  traverse->criteria.vertex_travel      = 0;
  traverse->criteria.vertex_visit_mask  = 0;
  traverse->criteria.vertex_visit       = 0;
  traverse->criteria.edge_travel_mask   = EDGE_KIND_OPT;
  traverse->criteria.edge_travel        = EDGE_KIND_SPECIAL;
  traverse->criteria.edge_visit_mask    = 0;
  traverse->criteria.edge_visit         = 0;
}

void selector_graph_uses(struct selector_traverse *traverse)
{
  traverse->criteria.vertex_travel_mask = VERTEX_KIND_OPT;
  traverse->criteria.vertex_travel      = VERTEX_KIND_FSENT;
  traverse->criteria.vertex_visit_mask  = VERTEX_KIND_OPT;
  traverse->criteria.vertex_visit       = VERTEX_KIND_FSENT;
  traverse->criteria.edge_travel_mask   = EDGE_TYPE_OPT;
  traverse->criteria.edge_travel        = EDGE_RULESET_MOD;
  traverse->criteria.edge_visit_mask    = 0;
  traverse->criteria.edge_visit         = 0;
}

void selector_graph_requires(struct selector_traverse *traverse)
{
  traverse->criteria.vertex_travel_mask = VERTEX_KIND_OPT;
  traverse->criteria.vertex_travel      = VERTEX_KIND_FSENT;
  traverse->criteria.vertex_visit_mask  = VERTEX_KIND_OPT;
  traverse->criteria.vertex_visit       = VERTEX_KIND_FSENT;
  traverse->criteria.edge_travel_mask   = EDGE_TYPE_OPT;
  traverse->criteria.edge_travel        = EDGE_REQUIRES;
  traverse->criteria.edge_visit_mask    = 0;
  traverse->criteria.edge_visit         = 0;
}

void selector_visit_special_module(struct selector_traverse * const restrict traverse)
{
  traverse->visit_mask = VERTEX_SPECIAL_OPT;
  traverse->visit_bits = VERTEX_SPECIAL_MODULE;
}

void selector_visit_special_ruleset(struct selector_traverse * const restrict traverse)
{
  traverse->visit_mask = VERTEX_SPECIAL_OPT;
  traverse->visit_bits = VERTEX_SPECIAL_RULESET;
}

void selector_visit_special_formula(struct selector_traverse * const restrict traverse)
{
  traverse->visit_mask = VERTEX_SPECIAL_OPT;
  traverse->visit_bits = VERTEX_SPECIAL_FORMULA;
}

void selector_visit_special_varset(struct selector_traverse * const restrict traverse)
{
  traverse->visit_mask = VERTEX_SPECIAL_OPT;
  traverse->visit_bits = VERTEX_SPECIAL_VARSET;
}

void selector_visit_special_configset(struct selector_traverse * const restrict traverse)
{
  traverse->visit_mask = VERTEX_SPECIAL_OPT;
  traverse->visit_bits = VERTEX_SPECIAL_CONFIGSET;
}

void selector_visit_special_plugin(struct selector_traverse * const restrict traverse)
{
  traverse->visit_mask = VERTEX_SPECIAL_OPT;
  traverse->visit_bits = VERTEX_SPECIAL_PLUGIN;
}

void selector_visit_module_dir(struct selector_traverse * const restrict traverse)
{
  traverse->visit_mask = VERTEX_TYPE_OPT;
  traverse->visit_bits = VERTEX_MODULE_DIR;
}

void selector_visit_directory(struct selector_traverse * const restrict traverse)
{
  traverse->visit_mask = VERTEX_FILETYPE_OPT;
  traverse->visit_bits = VERTEX_FILETYPE_DIR;
}

void selector_visit_rule(struct selector_traverse * const restrict traverse)
{
  traverse->visit_mask = VERTEX_TYPE_OPT;
  traverse->visit_bits = VERTEX_RULE;
}

void selector_visit_fs(struct selector_traverse * const restrict traverse)
{
  traverse->visit_mask = VERTEX_TYPE_OPT;
  traverse->visit_bits = VERTEX_FSTREE;
}

void selector_visit_rma(struct selector_traverse * const restrict traverse)
{
  traverse->visit_mask = VERTEX_TYPE_OPT;
  traverse->visit_bits = VERTEX_RMA;
}

void selector_visit_bpe(struct selector_traverse * const restrict traverse)
{
  traverse->visit_mask = VERTEX_TYPE_OPT;
  traverse->visit_bits = VERTEX_BPE;
}
