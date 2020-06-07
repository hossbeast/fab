/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

   This file is part of fab.

   fab is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   fab is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with fab.  If not, see <http://www.gnu.org/licenses/>. */

#include <stdarg.h>

#include "types.h"
#include "xapi.h"

#include "valyria/set.h"
#include "xunit/type.h"
#include "value/parser.h"
#include "moria/vertex.h"
#include "moria/edge.h"
#include "moria/traverse.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "xlinux/xstdlib.h"
#include "value.h"
#include "valyria/pstring.h"
#include "value/VALUE.errtab.h"
#include "value/writer.h"
#include "valyria/llist.h"

#include "selector.internal.h"
#include "build_thread.internal.h"
#include "module.h"
#include "request.h"
#include "node.h"
#include "path.h"
#include "pattern.h"
#include "lookup.h"
#include "selection.h"

#include "common/attrs.h"
#include "zbuffer.h"

#define TRAVERSE_MIN_DISTANCE_DEFAULT 1
#define TRAVERSE_MAX_DISTANCE_DEFAULT UINT8_MAX

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

static void __attribute__((constructor)) init()
{
  attrs32_init(selector_type_attrs);
  attrs32_init(selector_nodeset_attrs);
}

static xapi node_selected(selector_context * ctx, node * n, uint16_t rank)
{
  enter;

  fatal(selection_add_node, ctx->selection, n, rank);

  finally : coda;
}

static xapi selector_visitor(vertex * v, void * arg, traversal_mode mode, int distance, int * result)
{
  enter;

  selector_context * ctx = arg;
  selector * sel = ctx->sel;
  node * n = vertex_value(v);

  /* terminate an upwards traversal at the root node */
  if(n == g_root)
  {
    *result = MORIA_TRAVERSE_PRUNE;
    goto XAPI_FINALLY;
  }

  if(sel->criteria.edge_travel == EDGE_TYPE_FS)
  {
    if(sel->module_only && node_module_get(n) != ctx->mod)
    {
      *result = MORIA_TRAVERSE_PRUNE;
      goto XAPI_FINALLY;
    }
  }

  if(sel->extension)
  {
    if(n->name->extl < sel->extension_len)
    {
      goto XAPI_FINALLY;
    }
    else if(memcmp(n->name->ext + n->name->extl - sel->extension_len, sel->extension, sel->extension_len) != 0)
    {
      goto XAPI_FINALLY;
    }
  }

  fatal(node_selected, ctx, n, distance);

  finally : coda;
}

static int selector_cmp(selector * A, selector * B)
{
  int d;

  if(!A && !B)
    return 0;

  if(!A)
    return -1;

  if(!B)
    return 1;

  if((d = INTCMP(A->type, B->type)))
    return d;

  if(A->type == SELECTOR_PATTERN)
  {
    return pattern_cmp(A->pattern, B->pattern);
  }
  else if(A->type == SELECTOR_TRAVERSE)
  {
    return 0
      ?: INTCMP(A->min_distance, B->min_distance)
      ?: INTCMP(A->max_distance, B->max_distance)
      ?: memcmp(&A->direction, &B->direction, sizeof(A->direction))
      ?: memcmp(&A->criteria, &B->criteria, sizeof(B->direction))
      ;
  }

  else if(A->type == SELECTOR_NODESET)
  {
    return INTCMP(A->nodeset, B->nodeset);
  }
  else if(A->type == SELECTOR_SEQUENCE || A->type == SELECTOR_UNION)
  {
    selector * a = llist_first(&A->head, selector, lln);
    selector * b = llist_first(&B->head, selector, lln);

    while(a || b)
    {
      if((d = selector_cmp(a, b)))
        return d;

      a = llist_next(&A->head, a, lln);
      b = llist_next(&B->head, b, lln);
    }
  }
  else
  {
    RUNTIME_ABORT();
  }

  return 0;
}

static void selector_unpack(va_list va, xunit_arg * a)
{
  a->p = va_arg(va, void*);
}

static int selector_compare(xunit_arg * A, xunit_arg * B)
{
  return selector_cmp(A->p, B->p);
}

static size_t selector_znload(char * restrict buf, size_t sz, selector * restrict sel);
static size_t selector_znload(char * restrict buf, size_t sz, selector * restrict sel)
{
  return 0;
}

static void selector_info_push(const char * const restrict name, xunit_arg * a)
{
  selector * sel = a->p;

  if(sel == 0)
  {
    xapi_info_pushs(name, "-none-");
  }
  else
  {
    char buf[512];
    size_t z = 0;

    z += selector_znload(buf + z, sizeof(buf) - z, sel);
    xapi_info_pushw(name, buf, z);
  }
}

static xapi selector_ctx_exec(selector_context * restrict ctx)
{
  enter;

  selector * sel;
  vertex_traversal_state * state = 0;
  selector *op;
  selection *in = 0;
  selection *out = 0;
  selection * T;
  int x;
  node * n;
  node_edge_dependency * ne;
  edge * e;
  selected_node *sn;
  uint32_t attrs;

  sel = ctx->sel;

  if(sel->type == SELECTOR_SEQUENCE)
  {
    /* input to each operation is the output from the preceding operation */
    llist_foreach(&sel->head, op, lln) {
      ctx->sel = op;
      fatal(selector_ctx_exec, ctx);
    }
  }
  else if(sel->type == SELECTOR_UNION)
  {
    fatal(selection_create, &in);
    fatal(selection_create, &out);
    fatal(selection_replicate, in, ctx->selection);

    /* independent input to each operation, output is their union */
    x = 0;
    llist_foreach(&sel->head, op, lln) {
      if(x)
      {
        fatal(selection_reset, ctx->selection);
        fatal(selection_replicate, ctx->selection, in);
      }

      ctx->sel = op;
      fatal(selector_ctx_exec, ctx);
      fatal(selection_replicate, out, ctx->selection);
      x++;
    }

    T = ctx->selection;
    ctx->selection = out;
    out = T;
  }
  if(sel->nodeset == SELECTOR_NODESET_TARGET)
  {
    if(ctx->bpe && ctx->bpe->typemark == BPE_NODE)
    {
      n = containerof(ctx->bpe, typeof(*n), self_bpe);
      fatal(node_selected, ctx, n, 0);
    }
    else if(ctx->bpe && ctx->bpe->typemark == BPE_NODE_EDGE_DEPENDENCY)
    {
      ne = containerof(ctx->bpe, typeof(*ne), bpe);
      e = edge_containerof(ne);
      if(!(e->attrs & MORIA_EDGE_HYPER))
      {
        if(ne->dir == EDGE_TGT_SRC)
          n = vertex_value(e->A);
        else
          n = vertex_value(e->B);
        fatal(node_selected, ctx, n, 0);
      }
      else if(ne->dir == EDGE_TGT_SRC && e->Alen)
      {
        n = vertex_value(e->Alist[0].v);
        fatal(node_selected, ctx, n, 0);
      }
      else if(ne->dir == EDGE_SRC_TGT && e->Blen)
      {
        n = vertex_value(e->Blist[0].v);
        fatal(node_selected, ctx, n, 0);
      }
    }
  }
  else if(sel->nodeset == SELECTOR_NODESET_TARGETS)
  {
    if(ctx->bpe && ctx->bpe->typemark == BPE_NODE)
    {
      n = containerof(ctx->bpe, typeof(*n), self_bpe);
      fatal(node_selected, ctx, n, 0);
    }
    else if(ctx->bpe && ctx->bpe->typemark == BPE_NODE_EDGE_DEPENDENCY)
    {
      ne = containerof(ctx->bpe, typeof(*ne), bpe);
      e = edge_containerof(ne);
      if(!(e->attrs & MORIA_EDGE_HYPER))
      {
        if(ne->dir == EDGE_TGT_SRC)
          n = vertex_value(e->A);
        else
          n = vertex_value(e->B);
        fatal(node_selected, ctx, n, 0);
      }
      else if(ne->dir == EDGE_TGT_SRC)
      {
        for(x = 0; x < e->Alen; x++)
        {
          n = vertex_value(e->Alist[x].v);
          fatal(node_selected, ctx, n, 0);
        }
      }
      else if(ne->dir == EDGE_SRC_TGT)
      {
        for(x = 0; x < e->Blen; x++)
        {
          n = vertex_value(e->Blist[x].v);
          fatal(node_selected, ctx, n, 0);
        }
      }
    }
  }
  else if(sel->nodeset == SELECTOR_NODESET_SOURCE)
  {
    if(ctx->bpe && ctx->bpe->typemark == BPE_NODE_EDGE_DEPENDENCY)
    {
      ne = containerof(ctx->bpe, typeof(*ne), bpe);
      e = edge_containerof(ne);
      if(!(e->attrs & MORIA_EDGE_HYPER))
      {
        if(ne->dir == EDGE_TGT_SRC)
          n = vertex_value(e->B);
        else
          n = vertex_value(e->A);
        fatal(node_selected, ctx, n, 0);
      }
      else if(ne->dir == EDGE_TGT_SRC && e->Blen)
      {
        n = vertex_value(e->Blist[0].v);
        fatal(node_selected, ctx, n, 0);
      }
      else if(ne->dir == EDGE_SRC_TGT && e->Alen)
      {
        n = vertex_value(e->Alist[0].v);
        fatal(node_selected, ctx, n, 0);
      }
    }
  }
  else if(sel->nodeset == SELECTOR_NODESET_SOURCES)
  {
    if(ctx->bpe && ctx->bpe->typemark == BPE_NODE_EDGE_DEPENDENCY)
    {
      ne = containerof(ctx->bpe, typeof(*ne), bpe);
      e = edge_containerof(ne);
      if(!(e->attrs & MORIA_EDGE_HYPER))
      {
        if(ne->dir == EDGE_TGT_SRC)
          n = vertex_value(e->B);
        else
          n = vertex_value(e->A);
        fatal(node_selected, ctx, n, 0);
      }
      else if(ne->dir == EDGE_TGT_SRC)
      {
        for(x = 0; x < e->Blen; x++)
        {
          n = vertex_value(e->Blist[x].v);
          fatal(node_selected, ctx, n, 0);
        }
      }
      else if(ne->dir == EDGE_SRC_TGT)
      {
        for(x = 0; x < e->Alen; x++)
        {
          n = vertex_value(e->Alist[x].v);
          fatal(node_selected, ctx, n, 0);
        }
      }
    }
  }
  else if(sel->nodeset == SELECTOR_NODESET_MODULE)
  {
    if(ctx->mod)
    {
      fatal(node_selected, ctx, ctx->mod->dir_node, 0);
    }
  }
  else if(sel->type == SELECTOR_PATTERN)
  {
    fatal(pattern_lookup, sel->pattern, ctx->mod, ctx->base, 0, ctx->selection, 0);
  }
  else if(sel->type == SELECTOR_TRAVERSE)
  {
    fatal(graph_vertex_traversal_begin, g_graph, &state);
    ctx->sel = sel;

    in = ctx->selection;
    fatal(selection_create, &ctx->selection);

    llist_foreach(&in->list, sn, lln) {
      attrs = sel->direction | MORIA_TRAVERSE_PRE | MORIA_TRAVERSE_DEPTH;
      if(sel->exhaustive) {
        attrs |= MORIA_TRAVERSE_EXHAUSTIVE;
      }

      fatal(graph_traverse_vertices
        , g_graph
        , vertex_containerof(sn->n)
        , selector_visitor
        , state
        /* selectors only visit fsent nodes */
        , (traversal_criteria[]) {{
              vertex_travel : sel->criteria.vertex_travel | VERTEX_TYPE_NODE
            , vertex_visit : sel->criteria.vertex_visit | VERTEX_TYPE_NODE
            , edge_travel : sel->criteria.edge_travel
            , edge_visit : sel->criteria.edge_visit
            , min_depth : sel->min_distance
            , max_depth : sel->max_distance
          }}
        , attrs
        , ctx
      );
    }
  }

  fatal(selection_finalize, ctx->selection);

finally:
  graph_vertex_traversal_end(g_graph, state);
  fatal(selection_xfree, in);
  fatal(selection_xfree, out);
coda;
}

static xapi writer_write(selector * const restrict sel, value_writer * const restrict writer, bool top)
{
  enter;

  selector * next;
  const char *name;
  char space[512];
  narrator * N;
  char nstor[NARRATOR_STATIC_SIZE];

  if(sel->type == SELECTOR_SEQUENCE)
  {
    // sequence is the default top level aggregate for a selector
    if(!top || llist_count(&sel->head) != 1)
    {
      fatal(value_writer_push_mapping, writer);
      fatal(value_writer_string, writer, "sequence");
      fatal(value_writer_push_list, writer);
    }
    llist_foreach(&sel->head, next, lln) {
      fatal(writer_write, next, writer, false);
    }
    if(!top || llist_count(&sel->head) != 1)
    {
      fatal(value_writer_pop_list, writer);
      fatal(value_writer_pop_mapping, writer);
    }
  }
  else if(sel->type == SELECTOR_UNION)
  {
    fatal(value_writer_push_mapping, writer);
    fatal(value_writer_string, writer, "union");
    fatal(value_writer_push_list, writer);

    llist_foreach(&sel->head, next, lln) {
      fatal(writer_write, next, writer, false);
    }

    fatal(value_writer_pop_list, writer);
    fatal(value_writer_pop_mapping, writer);
  }
  else if(sel->type == SELECTOR_TRAVERSE)
  {
    fatal(value_writer_push_mapping, writer);
    fatal(value_writer_string, writer, "traverse");
    fatal(value_writer_push_set, writer);

    if(sel->min_distance != TRAVERSE_MIN_DISTANCE_DEFAULT)
      fatal(value_writer_mapping_string_uint, writer, "min-distance", sel->min_distance);

    if(sel->max_distance != TRAVERSE_MAX_DISTANCE_DEFAULT)
      fatal(value_writer_mapping_string_uint, writer, "max-distance", sel->max_distance);

    if(sel->direction)
    {
      name = attrs32_name_byvalue(traverse_type_attrs, TRAVERSE_TYPE_OPT & sel->direction);
      RUNTIME_ASSERT(name);
      fatal(value_writer_mapping_string_string, writer, "direction", name);
    }

    if(sel->criteria.edge_travel)
    {
      name = attrs32_name_byvalue(graph_edge_attrs, EDGE_TYPE_OPT & sel->criteria.edge_travel);
      RUNTIME_ASSERT(name);
      fatal(value_writer_mapping_string_string, writer, "relation", name);
    }

    if(sel->criteria.vertex_travel)
    {
      name = attrs32_name_byvalue(graph_vertex_attrs, VERTEX_FILETYPE_OPT & sel->criteria.vertex_travel);
      RUNTIME_ASSERT(name);
      fatal(value_writer_mapping_string_string, writer, "file-type", name);
    }

    fatal(value_writer_pop_set, writer);
    fatal(value_writer_pop_mapping, writer);
  }
  else if(sel->type == SELECTOR_PATTERN)
  {
    N = narrator_fixed_init(nstor, space, sizeof(space));
    fatal(pattern_say, sel->pattern, N);
    fatal(value_writer_mapping_string_bytes, writer, "pattern", space, narrator_fixed_size(N));
  }
  else if(sel->type == SELECTOR_NODESET)
  {
    name = attrs32_name_byvalue(selector_nodeset_attrs, SELECTOR_NODESET_OPT & sel->nodeset);
    RUNTIME_ASSERT(name);
    fatal(value_writer_string, writer, name);
  }
  else
  {
    RUNTIME_ABORT();
  }

  finally : coda;
}


//
// internal
//

xapi selector_writer_write(selector * const restrict sel, value_writer * const restrict writer)
{
  xproxy(writer_write, sel, writer, true);
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
        llist_delete(op, lln);
        selector_free(op);
      }
    }

    else if(sel->type == SELECTOR_PATTERN)
    {
      pattern_free(sel->pattern);
    }

    else if(sel->type == SELECTOR_TRAVERSE)
    {
      free(sel->extension);
    }
  }

  free(sel);
}

xapi selector_alloc(selector_type type, selector ** restrict rs)
{
  enter;

  selector *s;

  fatal(xmalloc, &s, sizeof(*s));

  llist_init_node(&s->lln);
  s->type = type;
  if(type == SELECTOR_SEQUENCE || type == SELECTOR_UNION)
  {
    llist_init_node(&s->head);
  }
  else if(type == SELECTOR_TRAVERSE)
  {
    s->min_distance = TRAVERSE_MIN_DISTANCE_DEFAULT;
    s->max_distance = TRAVERSE_MAX_DISTANCE_DEFAULT;
    s->module_only = true;
  }

  *rs = s;

  finally : coda;
}

void selector_ifree(selector ** restrict s)
{
  selector_free(*s);
  *s = 0;
}

xapi selector_context_xdestroy(selector_context *ctx)
{
  enter;

  fatal(selection_xfree, ctx->selection);

  finally : coda;
}

xapi selector_exec(selector * restrict sel, selector_context * restrict ctx)
{
  enter;

  ctx->sel = sel;

  if(!ctx->selection)
    fatal(selection_create, &ctx->selection);
  else
    fatal(selection_reset, ctx->selection);

//xsays("selector-exec\n");
//fatal(selector_say, sel, g_narrator_stdout);
//xsays("\n");

  fatal(selector_ctx_exec, ctx);

  finally : coda;
}

xapi selector_say(selector * const restrict sel, narrator * const restrict N)
{
  enter;

  value_writer writer;
  value_writer_init(&writer);
  fatal(value_writer_open, &writer, N);
  fatal(selector_writer_write, sel, &writer);
  fatal(value_writer_close, &writer);

finally:
  fatal(value_writer_destroy, &writer);
coda;
}

xunit_type * xunit_selector = (xunit_type[]) {{
    xu_unpack : selector_unpack
  , xu_compare : selector_compare
  , xu_info_push : selector_info_push
}};

#if 0
  size_t z = 0;

  selector * next;
  llist *T;

  if(sel)
  {
    if(sel->type == SELECTOR_SEQUENCE)
    {
      z += znloads(buf + z, sz - z, "sequence : [");

      llist_foreach(&sel->head, T, next, lln) {
        z += selector_znload(buf + z, sz - z, next);
      }

      z += znloads(buf + z, sz - z, "]");
    }
    else if(sel->type == SELECTOR_UNION)
    {
      z += znloads(buf + z, sz - z, "union : [");

      llist_foreach(&sel->head, T, next, lln) {
        z += selector_znload(buf + z, sz - z, next);
      }

      z += znloads(buf + z, sz - z, "]");
    }
    else if(sel->type == SELECTOR_TRAVERSE)
    {
      z += znloads(buf + z, sz - z, "traverse : {");

      if(sel->min_distance != TRAVERSE_MIN_DISTANCE_DEFAULT)
        z += znloadf(buf + z, sz - z, " min-distance : %hhu", sel->min_distance);

      if(sel->max_distance != TRAVERSE_MAX_DISTANCE_DEFAULT)
        z += znloadf(buf + z, sz - z, " max-distance : %hhu", sel->max_distance);

      const char * direction = attrs32_name_byvalue(traverse_type_attrs, TRAVERSE_TYPE_OPT, sel->direction);
      if(direction)
      {
        z += znloads(buf + z, sz - z, " direction : ");
        z += znloads(buf + z, sz - z, direction);
      }
      
      const char * relation = attrs32_name_byvalue(graph_edge_attrs, EDGE_TYPE_OPT, sel->criteria.edge_travel);
      if(relation)
      {
        z += znloads(buf + z, sz - z, " relation : ");
        z += znloads(buf + z, sz - z, relation);
      }
      
      const char * nodes = attrs32_name_byvalue(graph_edge_attrs, VERTEX_FILETYPE_OPT, sel->criteria.vertex_travel);
      if(nodes)
      {
        z += znloads(buf + z, sz - z, " nodes : ");
        z += znloads(buf + z, sz - z, nodes);
      }

      z += znloads(buf + z, sz - z, " }");
    }
    else if(sel->type == SELECTOR_PATTERN)
    {
      z += znloads(buf + z, sz - z, " pattern : ");
      z += znloads(buf + z, sz - z, sel->pattern);
    }
    else if(sel->type == SELECTOR_NODESET)
    {
      z += znloads(buf + z, sz - z, attrs32_name_byvalue(selector_nodeset_attrs, SELECTOR_NODESET_OPT, sel->nodeset));
    }
  }

  return z;
    /* NOTE : this should be able to return multiple matches, e.g. classes/alternations permitted */
    fatal(node_lookup_pattern, sel->pattern, ctx->base, &n);

    if(n)
      fatal(node_selected, ctx, n);
#endif
