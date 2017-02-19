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

#include <string.h>

#include "xapi.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xstring.h"

#include "valyria/list.h"

#include "internal.h"
#include "vertex.internal.h"
#include "edge.internal.h"

#include "strutil.h"

#define restrict __restrict

//
// public
//

xapi vertex_createw(vertex ** const restrict v, size_t vsz, const char * const restrict label, size_t label_len, uint32_t attrs)
{
  enter;

  fatal(xmalloc, v, sizeof(**v) + vsz);
  fatal(list_create, &(*v)->up);
  fatal(list_create, &(*v)->down);
  fatal(ixstrndup, &(*v)->label, label, label_len);
  (*v)->label_len = label_len;
  (*v)->attrs = attrs;

  finally : coda;
}

xapi vertex_xfree(vertex * const restrict v)
{
  enter;

  if(v)
  {
    fatal(list_xfree, v->up);
    fatal(list_xfree, v->down);
    wfree(v->label);
  }

  wfree(v);

  finally : coda;
}

xapi vertex_ixfree(vertex ** const restrict v)
{
  enter;

  fatal(vertex_xfree, *v);
  *v = 0;

  finally : coda;
}

int vertex_compare(void * _ctx, const void * _e, size_t idx)
{
  struct vertex_cmp_context * ctx = _ctx;
  const edge * e = _e;

  if(ctx->A)
    ctx->lc = estrcmp(ctx->A, ctx->len, e->A->label, e->A->label_len, 0);
  else
    ctx->lc = estrcmp(ctx->B, ctx->len, e->B->label, e->B->label_len, 0);

  ctx->lx = idx;
  return ctx->lc;
};

//
// api
//

API void vertex_value_set(vertex * const restrict v, void * value, size_t vsz)
{
  memcpy(v->value, value, vsz);
}

API void * vertex_value(vertex * const restrict v)
{
  return v->value;
}

API vertex * vertex_ascendw(vertex * const restrict v, const char * const restrict label, size_t label_len)
{
  edge * e;
  if((e = vertex_ascend_edgew(v, label, label_len)))
    return e->A;
  return 0;
}

API edge * vertex_ascend_edgew(vertex * const restrict v, const char * const restrict label, size_t label_len)
{
  struct vertex_cmp_context ctx = { A : label, len : label_len };
  return list_search(v->up, &ctx, vertex_compare);
}

API vertex * vertex_descendw(vertex * const restrict v, const char * const restrict label, size_t label_len)
{
  edge * e;
  if((e = vertex_descend_edgew(v, label, label_len)))
    return e->B;
  return 0;
}

API edge * vertex_descend_edgew(vertex * const restrict v, const char * const restrict label, size_t label_len)
{
  struct vertex_cmp_context ctx = { B : label, len : label_len };
  return list_search(v->down, &ctx, vertex_compare);
}
