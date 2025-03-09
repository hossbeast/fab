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
#include "resolve.h"
#include "module.h"
#include "pattern.h"
#include "request.h"
#include "command.h"
#include "channel.h"
#include "zcat.h"
#include "system.h"
#include "vertex.h"
#include "events/events.h"
#include "bpe.h"
#include "edge.h"
#include "shadow.h"
#include "fstree.h"
#include "namespace.h"
#include "fsname.h"
#include "request_handler.h"
#include "formula_record.h"
#include "times.h"
#include "search.internal.h"

attrs32 *selector_type_attrs = (attrs32[]) {{
#define SELECTOR_DEF(x, s, r, y) + 1
    num : 0 SELECTOR_TYPE_TABLE
  , members : (member32[]) {
#undef SELECTOR_DEF
#define SELECTOR_DEF(x, s, r, y) { name : s, value : UINT32_C(y), range : UINT32_C(r) },
SELECTOR_TYPE_TABLE
  }
}};

attrs32 *selector_nodeset_attrs = (attrs32[]) {{
#undef SELECTOR_DEF
#define SELECTOR_DEF(x, s, r, y) + 1
    num : 0 SELECTOR_NODESET_TABLE
  , members : (member32[]) {
#undef SELECTOR_DEF
#define SELECTOR_DEF(x, s, r, y) { name : s, value : UINT32_C(y), range : UINT32_C(r) },
SELECTOR_NODESET_TABLE
  }
}};

attrs32 *traverse_type_attrs = (attrs32[]) {{
#undef TRAVERSE_ATTR_DEF
#define TRAVERSE_ATTR_DEF(x, s, r, y) + 1
    num : 0 TRAVERSE_TYPE_TABLE
  , members : (member32[]) {
#undef TRAVERSE_ATTR_DEF
#define TRAVERSE_ATTR_DEF(x, s, r, y) { name : s, value : y, range : UINT32_C(r) },
TRAVERSE_TYPE_TABLE
  }
}};

attrs32 *traverse_order_attrs = (attrs32[]) {{
#undef TRAVERSE_ATTR_DEF
#define TRAVERSE_ATTR_DEF(x, s, r, y) + 1
    num : 0 TRAVERSE_ORDER_TABLE
  , members : (member32[]) {
#undef TRAVERSE_ATTR_DEF
#define TRAVERSE_ATTR_DEF(x, s, r, y) { name : s, value : y, range : UINT32_C(r) },
TRAVERSE_ORDER_TABLE
  }
}};

attrs32 *selector_expect_attrs = (attrs32[]) {{
#undef DEF
#define DEF(x, s, r, y) + 1
    num : 0 SELECTOR_EXPECT_TABLE
  , members : (member32[]) {
#undef DEF
#define DEF(x, s, r, y) { name : s, value : y, range : UINT32_C(r) },
SELECTOR_EXPECT_TABLE
  }
}};

static struct selector_traverse criteria_fs;
static struct selector_traverse criteria_depends;
static struct selector_traverse criteria_special;
static struct selector_traverse criteria_uses;
static struct selector_traverse criteria_records;

static struct selector_traverse visit_module;
static struct selector_traverse visit_rules;
static struct selector_traverse visit_formula;
static struct selector_traverse visit_var;
static struct selector_traverse visit_config;
static struct selector_traverse visit_plugin;
static struct selector_traverse visit_directory;
static struct selector_traverse visit_rule;
static struct selector_traverse visit_fstree;
static struct selector_traverse visit_record;

static void __attribute__((constructor)) init()
{
  attrs32_init(selector_type_attrs);
  attrs32_init(selector_nodeset_attrs);
  attrs32_init(traverse_type_attrs);
  attrs32_init(traverse_order_attrs);
  attrs32_init(selector_expect_attrs);

  /* pre-configured traversals */
  selector_graph_fs(&criteria_fs);
  selector_graph_depends(&criteria_depends);
  selector_graph_special(&criteria_special);
  selector_graph_uses(&criteria_uses);
  selector_graph_records(&criteria_records);

  /* pre-configured visits */
  selector_visit_special_module(&visit_module);
  selector_visit_special_formula(&visit_formula);
  selector_visit_special_plugin(&visit_plugin);
  selector_visit_special_ruleset(&visit_rules);
  selector_visit_special_varset(&visit_var);
  selector_visit_special_configset(&visit_config);
  selector_visit_directory(&visit_directory);
  selector_visit_rule(&visit_rule);
  selector_visit_fstree(&visit_fstree);
  selector_visit_record(&visit_record);
}

static void selector_visitor(moria_vertex *v, void *arg, moria_traversal_mode mode, int distance, moria_traverse_result * restrict result)
{
  selector_context *ctx = arg;
  selector *sel = ctx->sel;
  const module *mod = 0;
  const fstree *fst = 0;
  fsent *fse;

  /* crossing restrictions cannot be enforced in a postorder traversal */
  if(mode == MORIA_TRAVERSE_POST) {
    goto end;
  }

  if(vertex_kind_get(v) != VERTEX_KIND_FSENT) {
    goto end;
  }

  if(!sel->traverse.cross_module) {
    mod = vertex_module_of(v);
  }
  if(!sel->traverse.cross_fstree) {
    fst = vertex_fstree_of(v);
  }

  /* terminate an upwards traversal at the root(s) */
  if(v == &g_root->vertex || v == &g_shadow->vertex) {
    if(sel->traverse.direction == MORIA_TRAVERSE_UP) {
      result->flags = MORIA_TRAVERSE_PRUNE;
      return;
    }
  }

  /* restrict crossings */
  if(!sel->traverse.cross_module) {
    if(ctx->mod && mod != ctx->mod)
    {
      result->flags = MORIA_TRAVERSE_PRUNE;
      return;
    }
  }

  if(!sel->traverse.cross_fstree) {
    if(ctx->fst && fst != ctx->fst) {
      result->flags = MORIA_TRAVERSE_PRUNE;
      return;
    }
  }

  /* visit filters */
  if(sel->traverse.primary) {
    fse = containerof(v, typeof(*fse), vertex);
    if(!fsent_is_primary(fse)) {
      return;
    }
  }
  if(sel->traverse.invalid) {
    if(!vertex_invalid_get(v)) {
      return;
    }
  }

end:
  selection_add_vertex(ctx->selection, v, distance);
  if(mod && !ctx->mod) {
    ctx->mod = mod;
  }
  if(fst && !ctx->fst) {
    ctx->fst = fst;
  }
}

static void
__attribute__((nonnull(1, 3)))
__attribute__((format(printf, 3, 4)))
selector_error(selector_context * restrict ctx, int code, const char * restrict fmt, ...)
{
  bamipc_message *msg;
  events_context ectx;
  narrator_fixed fixed;
  bacon_writer writer, *builder_writer;
  va_list va;

  msg = system_error_event(&ectx);
  narrator_fixed_init(&fixed, msg->text, msg->size);
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, &fixed.base);

  { MAPPING STRING("selector error")

    bacon_writer_push_string(builder_writer);
    va_start(va, fmt);
    bacon_writevf(builder_writer, fmt, va);
    va_end(va);
    bacon_writer_pop_string(builder_writer);
  }
  bacon_writer_close(&writer);

  msg->code = code;
  msg->len = fixed.l;
  events_publish(&ectx);
}

static void namespace_lookup_error(selector_context * restrict ctx,  struct selector * restrict sel)
{
  narrator_fixed fixed;
  bacon_writer writer, *builder_writer;
  events_context ectx;
  bamipc_message *msg;
  const yyu_location *loc;

  loc = &sel->lookup.pattern->pattern.loc;

  msg = system_error_event(&ectx);
  narrator_fixed_init(&fixed, msg->text, msg->size);
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, &fixed.base);

  { MAPPING
    STRING("unresolved namespace ref");
    { SET
      { MAPPING STRING("location")
        LOADF("%s [%d,%d - %d,%d]", loc->fname, loc->f_lin + 1, loc->f_col + 1, loc->l_lin + 1, loc->l_col + 1);
      }
      { MAPPING STRING("namespace")
        BYTES(sel->lookup.namespace, sel->lookup.namespace_len)
      }
    }
  }
  bacon_writer_close(&writer);

  msg->code = EINVAL;
  msg->len = fixed.l;
  events_publish(&ectx);
}

static void namespace_resolve_error(selector_context * restrict sel_ctx, struct lookup_context * restrict ctx)
{
  narrator_fixed fixed;
  bacon_writer writer, *builder_writer;
  events_context ectx;
  bamipc_message *msg;

  msg = system_error_event(&ectx);
  narrator_fixed_init(&fixed, msg->text, msg->size);
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, &fixed.base);

  { MAPPING
    if(ctx->rctx.r == 0) {
      STRING("unresolved lookup")
    } else {
      STRING("ambiguous lookup")
    }
    lookup_namespace_error_build(builder_writer, ctx);
  }

  bacon_writer_close(&writer);
  msg->code = EINVAL;
  msg->len = fixed.l;
  events_publish(&ectx);
}

static void pattern_resolve_bound_error(selector_context * restrict ctx, const selector * restrict sel)
{
  narrator_fixed fixed;
  bacon_writer writer, *builder_writer;
  events_context ectx;
  bamipc_message *msg;

  if(ctx->handler) {
    msg = request_handler_tx(ctx->handler, BAMIPC_MSG_RESULT, BAMIPC_MSG_BACON, ENOENT);
  } else {
    msg = system_error_event(&ectx);
  }

  narrator_fixed_init(&fixed, msg->text, msg->size);
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, &fixed.base);

  { MAPPING STRING("selector error")
    { SET
      { MAPPING STRING("expected")
        STRING(attrs32_name_byvalue(selector_expect_attrs, sel->expect))
      }
      { MAPPING STRING("actual")
        UINT(ctx->lctx.rctx.r)
      }
      { MAPPING STRING("selector")
        { SET
          selector_describe((void*)sel, builder_writer);
        }
      }
    }
  }
  bacon_writer_close(&writer);

  msg->code = ENOENT;
  msg->len = fixed.l;
  if(ctx->handler) {
    msg->type = BAMIPC_MSG_RESULT;
    request_handler_post(ctx->handler, msg);
  } else {
    events_publish(&ectx);
  }
}

struct sort_context {

};

static struct timespec ts_field(moria_vertex * restrict v, selector_sortkey key)
{
  switch(CASE2(vertex_type_get(v), key)) {
    case CASE2(VERTEX_FORMULA_RECORD, SELECTOR_SORTKEY_FORKTIME):
      return containerof(v, formula_record, vertex)->forked_time;
  }

  return (struct timespec) { };
}

static uint64_t u64_field(moria_vertex * restrict v, selector_sortkey key)
{
  formula_record *fmlrec;

  switch(CASE2(vertex_type_get(v), key)) {
    case CASE2(VERTEX_FORMULA_RECORD, SELECTOR_SORTKEY_RUNTIME):
      fmlrec = containerof(v, formula_record, vertex);
      return timespec_delta(&fmlrec->waited_time, &fmlrec->forked_time);
  }

  return 0;
}

static int cmp(void *_ctx, llist * restrict Aln, llist * restrict Bln)
{
  selector_context *ctx = _ctx;
  struct selector *selector = ctx->sel;
  typeof(selector->subset.sort) *sort = &selector->subset.sort;
  typeof(*sort->ranks) *rank;
  selected *A, *B;
  int x;
  int d;
  struct timespec at, bt;
  uint64_t au64, bu64;

  A = containerof(Aln, typeof(*A), lln);
  B = containerof(Bln, typeof(*B), lln);

  d = 0;
  for(x = 0; x < sort->ranks_len; x++) {
    rank = &sort->ranks[x];
    switch(rank->key) {
      case SELECTOR_SORTKEY_FORKTIME:
        at = ts_field(A->v, rank->key);
        bt = ts_field(B->v, rank->key);
        if(rank->order == SELECTOR_ORDER_ASCENDING) {
          d = INTCMP(at.tv_sec, bt.tv_sec) ?: INTCMP(at.tv_nsec, bt.tv_nsec);
        } else {
          d = INTCMP(bt.tv_sec, at.tv_sec) ?: INTCMP(bt.tv_nsec, at.tv_nsec);
        }
        break;
      case SELECTOR_SORTKEY_RUNTIME:
        au64 = u64_field(A->v, rank->key);
        bu64 = u64_field(B->v, rank->key);
        if(rank->order == SELECTOR_ORDER_ASCENDING) {
          d = INTCMP(au64, bu64);
        } else {
          d = INTCMP(bu64, au64);
        }
        break;
    }
    if(d != 0) {
      return d;
    }
  }

  return d;
}

static void subset(selector_context * restrict ctx)
{
  selected *sn;
  selection *out;
  int x;

  /* sort the active selection */
  selection_finalize(ctx->selection);
  llist_sort(&ctx->selection->list, cmp, ctx);

  /* output selection chosen from the input selection */
  selection_create(&out, ctx->iteration_type);
  x = 0;
  llist_foreach(&ctx->selection->list, sn, lln) {
    selection_add_vertex(out, sn->v, sn->distance);
    if(++x == ctx->sel->subset.limit) {
      break;
    }
  }

  selection_free(ctx->selection);
  ctx->selection = out;
}

static void search_hit(void *ctxp, const pattern_search_hit *hit)
{
  selector_context *ctx = ctxp;

  selection_add_vertex(ctx->selection, &hit->node->vertex, 0);
}

static void selector_ctx_exec(selector_context * restrict ctx)
{
  selector *sel;
  moria_vertex_traversal_state * state;
  selector *op;
  selection *in = 0;
  selection *out = 0;
  selection * T;
  moria_vertex *v;
  selected *sn;
  module *mod;
  const fstree *fst;
  const char *name;
  bool *errp;
  bool err = false;
  uint32_t attrs;
  struct bacon_value *vars;
  moria_foreach_state st;
  moria_edge *edge;
  moria_traversal_criteria criteria;
  namespace *ns;
  int x;
  int r;

  if(ctx->handler) {
    errp = &err;
  } else {
    errp = &system_has_error;
  }

  sel = ctx->sel;
  switch(sel->type) {
    case SELECTOR_SEQUENCE:
      /* input to each operation is the output from the preceding operation */
      llist_foreach(&sel->head, op, lln) {
        ctx->sel = op;
        selector_ctx_exec(ctx);
        if(*errp) {
          break;
        }
      }
      break;
    case SELECTOR_UNION:
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
      break;
    case SELECTOR_SUBSET:
      subset(ctx);
      break;
    case SELECTOR_NODESET:
      if(sel->nodeset == SELECTOR_NODESET_MODULES)
      {
        llist_foreach(&module_list, mod, vertex.owner) {
          selection_add_vertex(ctx->selection, &mod->vertex, 0);
        }
      } else if(sel->nodeset == SELECTOR_NODESET_MODULE) {
        if(ctx->mod) {
          selection_add_vertex(ctx->selection, &ctx->mod->vertex, 0);
        }
      } else if(sel->nodeset == SELECTOR_NODESET_ANCHOR) {
        if(ctx->mod) {
          selection_add_vertex(ctx->selection, &ctx->mod->anchor_dir->vertex, 0);
        }
      } else if(!ctx->bpe) {
        name = attrs32_name_byvalue(selector_nodeset_attrs, SELECTOR_NODESET_OPT & sel->nodeset);
        selector_error(ctx, ENOENT, "not found %s", name);
      } else if(sel->nodeset == SELECTOR_NODESET_TARGET || sel->nodeset == SELECTOR_NODESET_TARGETS) {
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
      } else if(sel->nodeset == SELECTOR_NODESET_SOURCE || sel->nodeset == SELECTOR_NODESET_SOURCES) {
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
      break;
  case SELECTOR_PATH:
    vars = 0;
    if(ctx->mod) {
      vars = ctx->mod->vars;
    }

    memset(&ctx->lctx.rctx, 0, sizeof(ctx->lctx.rctx));
    ctx->lctx.rctx.mod = ctx->mod;
    ctx->lctx.rctx.vars = vars;
    ctx->lctx.rctx.base = ctx->cwd;
    ctx->lctx.rctx.nofollow = sel->nofollow;
    ctx->lctx.rctx.hidden = sel->hidden;
    r = pattern_resolve_path(&ctx->lctx.rctx, sel->path.pattern);
    switch(sel->expect) {
      case SELECTOR_EXPECT_ZERO:
        if(r != 0) {
          pattern_resolve_bound_error(ctx, sel);
        }
        break;
      case SELECTOR_EXPECT_ONE:
      case SELECTOR_EXPECT_NONZERO:
        if(r != 1) {
          pattern_resolve_bound_error(ctx, sel);
        }
        break;
      case SELECTOR_EXPECT_ANY:
        break;
    }
    if(r == 1) {
      selection_add_vertex(ctx->selection, ctx->lctx.rctx.vertices[0], 0);
    }
    break;
  case SELECTOR_LOOKUP:
    vars = 0;
    if(ctx->mod) {
      vars = ctx->mod->vars;
    }

    ns = 0;
    if(sel->lookup.namespace_len) {
      if((ns = namespace_lookup(sel->lookup.namespace, sel->lookup.namespace_len)) == NULL) {
        namespace_lookup_error(ctx, sel);
      }
    }

    memset(&ctx->lctx, 0, sizeof(ctx->lctx));
    if(sel->lookup.pattern->pattern.bound) {
      ctx->lctx.rctx.mod = ctx->mod;
      ctx->lctx.rctx.vars = vars;
      ctx->lctx.rctx.base = ctx->cwd;
      ctx->lctx.rctx.nofollow = sel->nofollow;
      ctx->lctx.rctx.hidden = sel->hidden;
      ctx->lctx.loc = &sel->lookup.pattern->pattern.loc;
      r = pattern_resolve_lookup(&ctx->lctx.rctx, sel->lookup.pattern);
      switch(sel->expect) {
        case SELECTOR_EXPECT_ZERO:
          if(r != 0) {
            pattern_resolve_bound_error(ctx, sel);
          }
          break;
        case SELECTOR_EXPECT_ONE:
        case SELECTOR_EXPECT_NONZERO:
          if(r != 1) {
            pattern_resolve_bound_error(ctx, sel);
          }
          break;
        case SELECTOR_EXPECT_ANY:
          break;
      }
    } else {
      ctx->lctx.ns = ns;
      ctx->lctx.ns_name = ns->name;
      ctx->lctx.ns_name_len = ns->name_len;
      ctx->lctx.rctx.vars = vars;
      ctx->lctx.loc = &sel->lookup.pattern->pattern.loc;
      r = namespace_resolve(&ctx->lctx, sel->lookup.pattern);
      switch(sel->expect) {
        case SELECTOR_EXPECT_ZERO:
          if(r != 0) {
            namespace_resolve_error(ctx, &ctx->lctx);
          }
          break;
        case SELECTOR_EXPECT_ONE:
          if(r != 1) {
            namespace_resolve_error(ctx, &ctx->lctx);
          }
          break;
        case SELECTOR_EXPECT_NONZERO:
          if(r == 0) {
            namespace_resolve_error(ctx, &ctx->lctx);
          }
          break;
        case SELECTOR_EXPECT_ANY:
          break;
      }
    }
    if(r == 1) {
      selection_add_vertex(ctx->selection, ctx->lctx.rctx.vertices[0], 0);
    }
    break;
  case SELECTOR_SEARCH:
    pattern_search(sel->search.pattern, ctx->mod, search_hit, ctx, 0, 0);
    break;
  case SELECTOR_MODULE_OF:
    in = ctx->selection;
    selection_create(&ctx->selection, ctx->iteration_type);

    llist_foreach(&in->list, sn, lln) {
      v = sn->v;
      if(vertex_type_get(v) == VERTEX_CONTEXT_MODULE) {
        mod = (module *)ctx->mod;
      } else {
        mod = vertex_module_of(v);
      }
      if(mod) {
        selection_add_vertex(ctx->selection, &mod->vertex, 0);
      }
    }
    break;
  case SELECTOR_FSTREE_OF:
    in = ctx->selection;
    selection_create(&ctx->selection, ctx->iteration_type);

    llist_foreach(&in->list, sn, lln) {
      v = sn->v;
      if((fst = vertex_fstree_of(v))) {
        selection_add_vertex(ctx->selection, &fst->vertex, 0);
      }
    }
    break;
  case SELECTOR_TRAVERSE:
    moria_vertex_traversal_begin(&g_graph, &state);
    ctx->sel = sel;

    in = ctx->selection;
    selection_create(&ctx->selection, ctx->iteration_type);

    llist_foreach(&in->list, sn, lln) {
      v = sn->v;

      /* sidestep from the module node, to the module build dir */
      if(vertex_type_get(v) == VERTEX_MODULE)
      {
        if(!(sel->traverse.criteria.edge_travel & EDGE_MODULES)) {
          mod = containerof(v, module, vertex);
          v = &mod->build_dir->vertex;
        }
      }
      /* sidestep from the fsent, to the owning module */
      else if(vertex_type_get(v) != VERTEX_MODULE)
      {
        if(sel->traverse.criteria.edge_travel & EDGE_MODULES) {
          mod = vertex_module_of(v);
          v = &mod->vertex;
        }
      }

      memcpy(&criteria, &sel->traverse.criteria, sizeof(criteria));

      attrs = 0;
      if(sel->traverse.exhaustive) {
        attrs |= MORIA_TRAVERSE_EXHAUSTIVE;
      }
      if(sel->nofollow) {
        attrs |= MORIA_TRAVERSE_NOFOLLOW;
      }
      if(sel->hidden) {
        criteria.vertex_visit_mask &= ~VERTEX_EXISTS_BIT;
        criteria.vertex_visit &= ~VERTEX_EXISTS_BIT;
      }
      criteria.vertex_visit_mask |= sel->traverse.visit_mask;
      criteria.vertex_visit |= sel->traverse.visit_bits;

      ctx->build_mod = 0;
      ctx->anchor_mod = 0;

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
    break;
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
  selector *next;
  const char *name;

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
          { MAPPING STRING("direction")
            STRING(attrs32_name_byvalue(traverse_type_attrs, TRAVERSE_TYPE_OPT & sel->traverse.direction))
          }
          { MAPPING STRING("order")
            STRING(attrs32_name_byvalue(traverse_order_attrs, TRAVERSE_ORDER_OPT & sel->traverse.mode))
          }
          { MAPPING STRING("graph")
            if(traverse_criteria_cmp(&sel->traverse, &criteria_fs) == 0) {
              STRING("fs")
            } else if(traverse_criteria_cmp(&sel->traverse, &criteria_special) == 0) {
              STRING("special")
            } else if(traverse_criteria_cmp(&sel->traverse, &criteria_depends) == 0) {
              STRING("depends")
            } else if(traverse_criteria_cmp(&sel->traverse, &criteria_uses) == 0) {
              STRING("uses")
            } else if(traverse_criteria_cmp(&sel->traverse, &criteria_records) == 0) {
              STRING("records")
            } else {
              RTA;
            }
          }
          if(sel->traverse.visit_mask) {
            MAPPING
            STRING("visit")
            if(sel->traverse.primary) {
              STRING("primary")
            } else if(sel->traverse.invalid) {
              STRING("invalid")
            } else if(traverse_visit_cmp(&sel->traverse, &visit_module) == 0) {
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
            } else if(traverse_visit_cmp(&sel->traverse, &visit_rule) == 0) {
              STRING("rule")
            } else if(traverse_visit_cmp(&sel->traverse, &visit_fstree) == 0) {
              STRING("fstree")
            } else {
              RTA;
            }
          }
          { MAPPING STRING("exhaustive")
            BOOL(sel->traverse.exhaustive)
          }
          { MAPPING STRING("cross-module")
            BOOL(sel->traverse.cross_module)
          }
          { MAPPING STRING("cross-fstree")
            BOOL(sel->traverse.cross_fstree)
          }
          { MAPPING STRING("hidden")
            BOOL(sel->hidden)
          }
          { MAPPING STRING("nofollow")
            BOOL(sel->nofollow)
          }
        }
      }
      break;
    case SELECTOR_LOOKUP:
      { MAPPING
        STRING("lookup")
        { SET
          { MAPPING STRING("pattern")
            bacon_writer_push_bytes(builder_writer);
            pattern_say(&sel->lookup.pattern->pattern, builder_writer->N);
            bacon_writer_pop_bytes(builder_writer);
          }
          { MAPPING STRING("namespace")
            BYTES(sel->lookup.namespace, sel->lookup.namespace_len)
          }
          { MAPPING STRING("expect")
            STRING(attrs32_name_byvalue(selector_expect_attrs, sel->expect))
          }
        }
      }
      break;
    case SELECTOR_PATH:
      { MAPPING
        STRING("path")
        { SET
          { MAPPING STRING("pattern")
            bacon_writer_push_bytes(builder_writer);
            pattern_say(&sel->path.pattern->pattern, builder_writer->N);
            bacon_writer_pop_bytes(builder_writer);
          }
          { MAPPING
            STRING("nofollow")
            BOOL(sel->nofollow)
          }
          { MAPPING
            STRING("hidden")
            BOOL(sel->hidden)
          }
          { MAPPING STRING("expect")
            STRING(attrs32_name_byvalue(selector_expect_attrs, sel->expect))
          }
        }
      }
      break;
    case SELECTOR_SEARCH:
      { MAPPING
        STRING("search")
        { SET
          { MAPPING STRING("pattern")
            bacon_writer_push_bytes(builder_writer);
            pattern_say(&sel->path.pattern->pattern, builder_writer->N);
            bacon_writer_pop_bytes(builder_writer);
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

void selector_describe(selector * const restrict sel, bacon_writer * const restrict writer)
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
    switch(sel->type) {
      case SELECTOR_SEQUENCE:
      case SELECTOR_UNION:
        llist_foreach_safe(&sel->head, op, lln, T) {
          llist_delete(&op->lln);
          selector_free(op);
        }
        break;
      case SELECTOR_LOOKUP:
        pattern_free(&sel->lookup.pattern->pattern);
        free(sel->lookup.namespace);
        break;
      case SELECTOR_PATH:
        pattern_free(&sel->path.pattern->pattern);
        break;
      case SELECTOR_SEARCH:
        pattern_free(&sel->search.pattern->pattern);
        break;
      case SELECTOR_SUBSET:
      case SELECTOR_NODESET:
      case SELECTOR_TRAVERSE:
      case SELECTOR_MODULE_OF:
      case SELECTOR_FSTREE_OF:
        break;
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
  if(type == SELECTOR_SEQUENCE || type == SELECTOR_UNION) {
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
  memset(ctx, 0, sizeof(*ctx));
}

void selector_context_destroy(selector_context *ctx)
{
  selection_free(ctx->selection);
}

void selector_exec(selector * restrict sel, selector_context * restrict ctx)
{
  if(!ctx->selection) {
    selection_create(&ctx->selection, ctx->iteration_type);
  } else {
    selection_reset(ctx->selection, ctx->iteration_type);
  }

  ctx->sel = sel;

  RUNTIME_ASSERT(ctx->cwd);

  selector_ctx_exec(ctx);
}

void selector_say(selector * const restrict sel, narrator * const restrict N)
{
  bacon_writer writer;
  bacon_writer_init(&writer);
  bacon_writer_open(&writer, N);

  selector_describe(sel, &writer);

  bacon_writer_close(&writer);
  bacon_writer_destroy(&writer);
}

void selector_show(selector * const restrict sel)
{
  selector_say(sel, g_narrator_stdout);
  printf("\n");
}

void selector_graph_records(struct selector_traverse *traverse)
{
  traverse->criteria.vertex_travel_mask = 0;
  traverse->criteria.vertex_travel      = 0;
  traverse->criteria.vertex_visit_mask  = VERTEX_KIND_OPT;
  traverse->criteria.vertex_visit       = VERTEX_KIND_RECORD;
  traverse->criteria.edge_travel_mask   = EDGE_RECORDS;
  traverse->criteria.edge_travel        = EDGE_RECORDS;
  traverse->criteria.edge_visit_mask    = 0;
  traverse->criteria.edge_visit         = 0;
}

void selector_graph_fs(struct selector_traverse *traverse)
{
  traverse->criteria.vertex_travel_mask = 0;
  traverse->criteria.vertex_travel      = 0;
  traverse->criteria.vertex_visit_mask  = VERTEX_EXISTS_BIT;
  traverse->criteria.vertex_visit       = VERTEX_EXISTS_BIT;
  traverse->criteria.edge_travel_mask   = EDGE_TYPE_OPT;
  traverse->criteria.edge_travel        = EDGE_FS;
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
  traverse->criteria.edge_travel_mask   = EDGE_SPECIALS;
  traverse->criteria.edge_travel        = EDGE_SPECIALS;
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
  traverse->criteria.edge_travel        = EDGE_RULESET_USES_MOD;
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

void selector_visit_directory(struct selector_traverse * const restrict traverse)
{
  traverse->visit_mask = VERTEX_FILETYPE_OPT;
  traverse->visit_bits = VERTEX_FILETYPE_DIR;
}

void selector_visit_primary(struct selector_traverse * const restrict traverse)
{
  traverse->visit_mask = VERTEX_KIND_OPT;
  traverse->visit_bits = VERTEX_KIND_FSENT;
  traverse->primary = true;
}

void selector_visit_invalid(struct selector_traverse * const restrict traverse)
{
  traverse->visit_mask = VERTEX_KIND_OPT;
  traverse->visit_bits = VERTEX_KIND_FSENT;
  traverse->invalid = true;
}

void selector_visit_rule(struct selector_traverse * const restrict traverse)
{
  traverse->visit_mask = VERTEX_TYPE_OPT;
  traverse->visit_bits = VERTEX_RULE;
}

void selector_visit_fstree(struct selector_traverse * const restrict traverse)
{
  traverse->visit_mask = VERTEX_TYPE_OPT;
  traverse->visit_bits = VERTEX_FSTREE;
}

void selector_visit_record(struct selector_traverse * const restrict traverse)
{
  traverse->visit_mask = VERTEX_KIND_OPT;
  traverse->visit_bits = VERTEX_KIND_RECORD;
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

void selector_visit_client(struct selector_traverse * const restrict traverse)
{
  traverse->visit_mask = VERTEX_TYPE_OPT;
  traverse->visit_bits = VERTEX_CLIENT;
}
