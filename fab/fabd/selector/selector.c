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

#include "xapi.h"

#include "common/attrs.h"
#include "moria/traverse.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "value/writer.h"
#include "xlinux/xstdlib.h"
#include "xunit/type.h"

#include "selector.internal.h"
#include "dependency.h"
#include "fsent.h"
#include "lookup.h"
#include "module.h"
#include "pattern.h"
#include "request.h"

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

static xapi selector_visitor(moria_vertex * v, void * arg, moria_traversal_mode mode, int distance, int * result)
{
  enter;

  selector_context * ctx = arg;
  selector * sel = ctx->sel;
  fsent *n;

//printf("%2d %s\n", distance, v->label);

  if((v->attrs & VERTEX_TYPE_OPT) == VERTEX_TYPE_FSENT)
  {
    n = containerof(v, fsent, vertex);

    /* terminate an upwards traversal at the root fsent */
    if(n == g_root)
    {
      *result = MORIA_TRAVERSE_PRUNE;
      goto XAPI_FINALLY;
    }

    if(sel->criteria.edge_travel & EDGE_TYPE_FSTREE)
    {
      if(!sel->cross_module && fsent_module_get(n) != ctx->mod)
      {
printf("SELECTOR no-cross node %p %s mod %p vs %p\n", n, n->name.name, fsent_module_get(n), ctx->mod);
        *result = MORIA_TRAVERSE_PRUNE;
        goto XAPI_FINALLY;
      }
    }

    if(sel->extension)
    {
      if(n->name.extl < sel->extension_len)
      {
        goto XAPI_FINALLY;
      }
      else if(memcmp(n->name.ext + n->name.extl - sel->extension_len, sel->extension, sel->extension_len) != 0)
      {
        goto XAPI_FINALLY;
      }
    }
  }
  else
  {
    RUNTIME_ASSERT((v->attrs & VERTEX_TYPE_OPT) == VERTEX_TYPE_MODULE);
    RUNTIME_ASSERT(!sel->extension);
  }

  fatal(selection_add_vertex, ctx->selection, v, distance);

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
  else if(A->type == SELECTOR_PATH)
  {
    return memncmp(A->path, A->path_len, B->path, B->path_len);
  }
  else if(A->type == SELECTOR_TRAVERSE)
  {
    return 0
//      ?: INTCMP(A->min_distance, B->min_distance)
//      ?: INTCMP(A->max_distance, B->max_distance)
      ?: memcmp(&A->direction, &B->direction, sizeof(A->direction))
      ?: memcmp(&A->criteria, &B->criteria, sizeof(A->criteria))
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

static void selector_info_push(const char * const restrict name, xunit_arg * a)
{
  selector * sel = a->p;

  if(sel == 0)
  {
    xapi_info_pushs(name, "-none-");
  }
  else
  {
    xapi_info_pushs(name, "-not-none-");
  }
}

static xapi selector_ctx_exec(selector_context * restrict ctx)
{
  enter;

  selector * sel;
  moria_vertex_traversal_state * state = 0;
  selector *op;
  selection *in = 0;
  selection *out = 0;
  selection * T;
  int x;
  fsent * n;
  moria_vertex *v;
  moria_edge * e;
  selected *sn;
  module *mod;
  const char *name;

  sel = ctx->sel;
  if(sel->type == SELECTOR_SEQUENCE)
  {
    /* input to each operation is the output from the preceding operation */
    llist_foreach(&sel->head, op, lln) {
      ctx->sel = op;
      fatal(selector_ctx_exec, ctx);
      if(ctx->errlen) {
        break;
      }
    }
  }
  else if(sel->type == SELECTOR_UNION)
  {
    fatal(selection_create, &in, ctx->iteration_type);
    fatal(selection_create, &out, ctx->iteration_type);
    fatal(selection_replicate, in, ctx->selection);

    /* independent input to each operation, output is their union */
    x = 0;
    llist_foreach(&sel->head, op, lln) {
      if(x)
      {
        fatal(selection_reset, ctx->selection, ctx->iteration_type);
        fatal(selection_replicate, ctx->selection, in);
      }

      ctx->sel = op;
      fatal(selector_ctx_exec, ctx);
      fatal(selection_replicate, out, ctx->selection);
      x++;

      if(ctx->errlen) {
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
      fatal(selection_add_vertex, ctx->selection, &ctx->mod->vertex, 0);
    }
    else if(!ctx->bpe)
    {
      name = attrs32_name_byvalue(selector_nodeset_attrs, SELECTOR_NODESET_OPT & sel->nodeset);
      ctx->errlen = snprintf(ctx->err, sizeof(ctx->err), "ENOENT %s", name);
    }
    else if(sel->nodeset == SELECTOR_NODESET_TARGET)
    {
      e = &ctx->bpe->edge;
      if(!(e->attrs & MORIA_EDGE_HYPER))
      {
        v = e->A;
        fatal(selection_add_vertex, ctx->selection, v, 0);
      }
      else if(e->Alen)
      {
        v = e->Alist[0].v;
        fatal(selection_add_vertex, ctx->selection, v, 0);
      }
    }
    else if(sel->nodeset == SELECTOR_NODESET_TARGETS)
    {
      e = &ctx->bpe->edge;
      if(!(e->attrs & MORIA_EDGE_HYPER))
      {
        v = e->A;
        fatal(selection_add_vertex, ctx->selection, v, 0);
      }
      else
      {
        for(x = 0; x < e->Alen; x++)
        {
          v = e->Alist[x].v;
          fatal(selection_add_vertex, ctx->selection, v, 0);
        }
      }
    }
    else if(sel->nodeset == SELECTOR_NODESET_SOURCE)
    {
      e = &ctx->bpe->edge;
      if(!(e->attrs & MORIA_EDGE_HYPER))
      {
        v = e->B;
        fatal(selection_add_vertex, ctx->selection, v, 0);
      }
      else if(e->Blen)
      {
        v = e->Blist[0].v;
        fatal(selection_add_vertex, ctx->selection, v, 0);
      }
    }
    else if(sel->nodeset == SELECTOR_NODESET_SOURCES)
    {
      e = &ctx->bpe->edge;
      if(!(e->attrs & MORIA_EDGE_HYPER))
      {
        v = e->B;
        fatal(selection_add_vertex, ctx->selection, v, 0);
      }
      else
      {
        for(x = 0; x < e->Blen; x++)
        {
          v = e->Blist[x].v;
          fatal(selection_add_vertex, ctx->selection, v, 0);
        }
      }
    }
  }
  else if(sel->type == SELECTOR_PATTERN)
  {
    fatal(pattern_lookup, sel->pattern, 0, ctx->selection, ctx->err, sizeof(ctx->err), &ctx->errlen);
  }
  else if(sel->type == SELECTOR_PATH)
  {
    if((n = fsent_path_lookup(ctx->mod, sel->path, sel->path_len)))
    {
      fatal(selection_add_vertex, ctx->selection, &n->vertex, 0);
    }
    else
    {
      ctx->errlen = snprintf(ctx->err, sizeof(ctx->err), "ENOENT path : %.*s", (int)sel->path_len, sel->path);
    }
  }
  else if(sel->type == SELECTOR_TRAVERSE)
  {
    fatal(moria_vertex_traversal_begin, &g_graph, &state);
    ctx->sel = sel;

    in = ctx->selection;
    fatal(selection_create, &ctx->selection, ctx->iteration_type);

    llist_foreach(&in->list, sn, lln) {
//extern int goats;
//goats = 1;
//printf("SELECTOR TRAVERSE start\n");
//fatal(selector_say, sel, g_narrator_stdout);
//printf("\n");

      v = sn->v;

      /* special handling for stuff */
      if((v->attrs & VERTEX_TYPE_OPT) == VERTEX_TYPE_MODULE) {
        if(!(sel->criteria.edge_travel & EDGE_MODULES)) {
          mod = containerof(v, module, vertex);
          v = &mod->dir_node->vertex;
        }
      }

      /* special handling for stuff */
      else if((v->attrs & VERTEX_TYPE_OPT) == VERTEX_TYPE_FSENT) {
        if(sel->criteria.edge_travel & EDGE_MODULES) {
          mod = fsent_module_get(containerof(v, fsent, vertex));
          v = &mod->vertex;
        }
      }

      fatal(moria_traverse_vertices
        , &g_graph
        , v
        , selector_visitor
        , state
        /* selectors can only visit fsent nodes */
        , &sel->criteria
//        , (moria_traversal_criteria[]) {{
//              vertex_travel : sel->criteria.vertex_travel // | VERTEX_TYPE_FSENT
//            , vertex_visit : sel->criteria.vertex_visit // | VERTEX_TYPE_FSENT
//            , edge_travel : sel->criteria.edge_travel
//            , edge_visit : sel->criteria.edge_visit
//            , min_depth : sel->min_distance
//            , max_depth : sel->max_distance
//          }}
        , sel->direction | sel->mode | sel->exhaustive | MORIA_TRAVERSE_DEPTH // | MORIA_TRAVERSE_NOFOLLOW
        , ctx
      );
//printf("SELECTOR TRAVERSE done\n");
//goats = 0;
    }
  }

  fatal(selection_finalize, ctx->selection);

finally:
  moria_vertex_traversal_end(&g_graph, state);
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
  narrator_fixed nstor;

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

    if(sel->criteria.min_depth != SELECTOR_TRAVERSE_DEFAULT_CRITERIA.min_depth) // TRAVERSE_MIN_DISTANCE_DEFAULT)
      fatal(value_writer_mapping_string_uint, writer, "min-distance", sel->criteria.min_depth);

    if(sel->criteria.max_depth != SELECTOR_TRAVERSE_DEFAULT_CRITERIA.max_depth) // TRAVERSE_MAX_DISTANCE_DEFAULT)
      fatal(value_writer_mapping_string_uint, writer, "max-distance", sel->criteria.max_depth);

    name = attrs32_name_byvalue(traverse_type_attrs, TRAVERSE_TYPE_OPT & sel->direction);
    RUNTIME_ASSERT(name);
    fatal(value_writer_mapping_string_string, writer, "direction", name);

    RUNTIME_ASSERT(sel->criteria.edge_travel);
    if(sel->criteria.edge_travel == EDGE_FSTREE)
    {
      if(sel->criteria.vertex_visit == VERTEX_MODULE_DIR)
      {
        fatal(value_writer_mapping_string_string, writer, "graph", "modtree");
      }
      else if(sel->criteria.vertex_visit == VERTEX_FILETYPE_DIR)
      {
        fatal(value_writer_mapping_string_string, writer, "graph", "dirtree");
      }
      else
      {
        name = attrs32_name_byvalue(graph_edge_attrs, EDGE_KIND_OPT & sel->criteria.edge_travel);
        RUNTIME_ASSERT(name);
        fatal(value_writer_mapping_string_string, writer, "graph", name);
      }
    }
    else
    {
      name = attrs32_name_byvalue(graph_edge_attrs, EDGE_KIND_OPT & sel->criteria.edge_travel);
      RUNTIME_ASSERT(name);
      fatal(value_writer_mapping_string_string, writer, "graph", name);
    }

    fatal(value_writer_pop_set, writer);
    fatal(value_writer_pop_mapping, writer);
  }
  else if(sel->type == SELECTOR_PATTERN)
  {
    N = narrator_fixed_init(&nstor, space, sizeof(space));
    fatal(pattern_say, sel->pattern, N);
    fatal(value_writer_mapping_string_bytes, writer, "pattern", space, nstor.l);
  }
  else if(sel->type == SELECTOR_PATH)
  {
    fatal(value_writer_mapping_string_bytes, writer, "path", sel->path, sel->path_len);
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
    else if(sel->type == SELECTOR_PATH)
    {
      wfree(sel->path);
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

xapi selector_exec(selector * restrict sel, selector_context * restrict ctx, selection_iteration_type iteration_type)
{
  enter;

  if(!ctx->selection)
    fatal(selection_create, &ctx->selection, iteration_type);
  else
    fatal(selection_reset, ctx->selection, iteration_type);

  ctx->sel = sel;
  ctx->iteration_type = iteration_type;
  ctx->errlen = 0;

//xsays("selector-exec\n");
//fatal(selector_say, sel, g_narrator_stdout);
//xsays("\n");

  RUNTIME_ASSERT(ctx->mod);

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
    if(sel->criteria.vertex_travel)
    {
      name = attrs32_name_byvalue(graph_vertex_attrs, VERTEX_FILETYPE_OPT & sel->criteria.vertex_travel);
      RUNTIME_ASSERT(name);
      fatal(value_writer_mapping_string_string, writer, "file-type", name);
    }
#endif
