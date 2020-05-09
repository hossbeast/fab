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
#include "types.h"

#include "valyria/list.h"
#include "xlinux/xstdlib.h"
#include "yyutil/parser.h"

#include "box.h"

#include "hash.h"
#include "macros.h"

//
// static
//

static void box_init(box * bx, box_type type, const struct yyu_location * const restrict loc)
{
  memcpy(&bx->loc, loc, sizeof(*loc));
  bx->type = type;
}

//
// box_mk
//

xapi box_int_mk(
    const struct yyu_location * const restrict loc
  , box_int ** rv
  , int v
)
{
  enter;

  box_int * val = 0;

  fatal(xmalloc, &val, sizeof(*val));
  box_init(&val->bx, BOX_INT, loc);
  val->v = v;

  *rv = val;
  val = 0;

finally:
  wfree(val);
coda;
}

xapi box_int8_mk(
    const struct yyu_location * const restrict loc
  , box_int8 ** rv
  , int8_t v
)
{
  enter;

  box_int8 * val = 0;

  fatal(xmalloc, &val, sizeof(*val));
  box_init(&val->bx, BOX_INT8, loc);
  val->v = v;

  *rv = val;
  val = 0;

finally:
  wfree(val);
coda;
}

xapi box_int16_mk(
    const struct yyu_location * const restrict loc
  , box_int16 ** rv
  , int16_t v
)
{
  enter;

  box_int16 * val = 0;

  fatal(xmalloc, &val, sizeof(*val));
  box_init(&val->bx, BOX_INT16, loc);
  val->v = v;

  *rv = val;
  val = 0;

finally:
  wfree(val);
coda;
}

xapi box_int32_mk(
    const struct yyu_location * const restrict loc
  , box_int32 ** rv
  , int32_t v
)
{
  enter;

  box_int32 * val = 0;

  fatal(xmalloc, &val, sizeof(*val));
  box_init(&val->bx, BOX_INT32, loc);
  val->v = v;

  *rv = val;
  val = 0;

finally:
  wfree(val);
coda;
}

xapi box_int64_mk(
    const struct yyu_location * const restrict loc
  , box_int64 ** rv
  , int64_t v
)
{
  enter;

  box_int64 * val = 0;

  fatal(xmalloc, &val, sizeof(*val));
  box_init(&val->bx, BOX_INT64, loc);
  val->v = v;

  *rv = val;
  val = 0;

finally:
  wfree(val);
coda;
}

xapi box_uint8_mk(
    const struct yyu_location * const restrict loc
  , box_uint8 ** rv
  , uint8_t v
)
{
  enter;

  box_uint8 * val = 0;

  fatal(xmalloc, &val, sizeof(*val));
  box_init(&val->bx, BOX_UINT8, loc);
  val->v = v;

  *rv = val;
  val = 0;

finally:
  wfree(val);
coda;
}

xapi box_uint16_mk(
    const struct yyu_location * const restrict loc
  , box_uint16 ** rv
  , uint16_t v
)
{
  enter;

  box_uint16 * val = 0;

  fatal(xmalloc, &val, sizeof(*val));
  box_init(&val->bx, BOX_UINT16, loc);
  val->v = v;

  *rv = val;
  val = 0;

finally:
  wfree(val);
coda;
}

xapi box_uint32_mk(
    const struct yyu_location * const restrict loc
  , box_uint32 ** rv
  , uint32_t v
)
{
  enter;

  box_uint32 * val = 0;

  fatal(xmalloc, &val, sizeof(*val));
  box_init(&val->bx, BOX_UINT32, loc);
  val->v = v;

  *rv = val;
  val = 0;

finally:
  wfree(val);
coda;
}

xapi box_uint64_mk(
    const struct yyu_location * const restrict loc
  , box_uint64 ** rv
  , uint64_t v
)
{
  enter;

  box_uint64 * val = 0;

  fatal(xmalloc, &val, sizeof(*val));
  box_init(&val->bx, BOX_UINT64, loc);
  val->v = v;

  *rv = val;
  val = 0;

finally:
  wfree(val);
coda;
}

xapi box_bool_mk(
    const struct yyu_location * const restrict loc
  , box_bool ** rv
  , bool v
)
{
  enter;

  box_bool * val = 0;

  fatal(xmalloc, &val, sizeof(*val));
  box_init(&val->bx, BOX_BOOL, loc);
  val->v = v;

  *rv = val;
  val = 0;

finally:
  wfree(val);
coda;
}

xapi box_string_mk(
    const struct yyu_location * const restrict loc
  , box_string ** rv
  , char * restrict v
  , uint16_t len
)
{
  enter;

  box_string * val = 0;

  fatal(xmalloc, &val, sizeof(*val));
  box_init(&val->bx, BOX_STRING, loc);
  val->v = v;
  val->l = len;

  *rv = val;
  val = 0;

finally:
  wfree(val);
coda;
}

//
// box_setif
//

void box_string_setif(box_string * restrict b, char ** v)
{
  if(b)
    *v = b->v;
}

void box_bool_setif(box_bool * restrict b, bool * v)
{
  if(b)
    *v = b->v;
}

void box_int16_setif(box_int16 * restrict b, int16_t * v)
{
  if(b)
    *v = b->v;
}

void box_uint16_setif(box_uint16 * restrict b, uint16_t * v)
{
  if(b)
    *v = b->v;
}

void box_int_setif(box_int * restrict b, int * v)
{
  if(b)
    *v = b->v;
}

//
// box
//

uint32_t box_hash(uint32_t h, box * restrict bx)
{
  if(!bx->hash)
  {
    uint32_t hash = hash32(0, &bx->type, sizeof(bx->type));

    if(bx->type == BOX_INT)
    {
      box_int * b = containerof(bx, box_int, bx);
      hash = hash32(hash, &b->v, sizeof(b->v));
    }
    else if(bx->type == BOX_BOOL)
    {
      box_bool * b = containerof(bx, box_bool, bx);
      hash = hash32(hash, &b->v, sizeof(b->v));
    }
    else if(bx->type == BOX_STRING)
    {
      box_string * b = containerof(bx, box_string, bx);
      hash = hash32(hash, b->v, b->l);
    }
    else if(bx->type == BOX_INT8)
    {
      box_int8 * b = containerof(bx, box_int8, bx);
      hash = hash32(hash, &b->v, sizeof(b->v));
    }
    else if(bx->type == BOX_INT16)
    {
      box_int16 * b = containerof(bx, box_int16, bx);
      hash = hash32(hash, &b->v, sizeof(b->v));
    }
    else if(bx->type == BOX_INT32)
    {
      box_int32 * b = containerof(bx, box_int32, bx);
      hash = hash32(hash, &b->v, sizeof(b->v));
    }
    else if(bx->type == BOX_INT64)
    {
      box_int64 * b = containerof(bx, box_int64, bx);
      hash = hash32(hash, &b->v, sizeof(b->v));
    }
    else if(bx->type == BOX_UINT8)
    {
      box_uint8 * b = containerof(bx, box_uint8, bx);
      hash = hash32(hash, &b->v, sizeof(b->v));
    }
    else if(bx->type == BOX_UINT16)
    {
      box_uint16 * b = containerof(bx, box_uint16, bx);
      hash = hash32(hash, &b->v, sizeof(b->v));
    }
    else if(bx->type == BOX_UINT32)
    {
      box_uint32 * b = containerof(bx, box_uint32, bx);
      hash = hash32(hash, &b->v, sizeof(b->v));
    }
    else if(bx->type == BOX_UINT64)
    {
      box_uint64 * b = containerof(bx, box_uint64, bx);
      hash = hash32(hash, &b->v, sizeof(b->v));
    }

    bx->hash = hash;
  }

  return h + bx->hash;
}

int box_cmp(const box * restrict Ax, const box * restrict Bx)
{
  int r = 0;
  int d;
  if(Ax == 0 || Bx == 0)
  {
    r = (Ax == 0) - (Bx == 0);
  }
  else if((d = Ax->type - Bx->type))
  {
    r = d;
  }
  else if(Ax->type == BOX_INT)
  {
    const box_int * A = containerof(Ax, box_int, bx);
    const box_int * B = containerof(Bx, box_int, bx);
    r = INTCMP(A->v, B->v);
  }
  else if(Ax->type == BOX_BOOL)
  {
    const box_bool * A = containerof(Ax, box_bool, bx);
    const box_bool * B = containerof(Bx, box_bool, bx);
    r = A->v != B->v;
  }
  else if(Ax->type == BOX_STRING)
  {
    const box_string * A = containerof(Ax, box_string, bx);
    const box_string * B = containerof(Bx, box_string, bx);
    r = memncmp(A->v, A->l, B->v, B->l);
  }
  else if(Ax->type == BOX_INT8)
  {
    const box_int8 * A = containerof(Ax, box_int8, bx);
    const box_int8 * B = containerof(Bx, box_int8, bx);
    r = INTCMP(A->v, B->v);
  }
  else if(Ax->type == BOX_INT16)
  {
    const box_int16 * A = containerof(Ax, box_int16, bx);
    const box_int16 * B = containerof(Bx, box_int16, bx);
    r = INTCMP(A->v, B->v);
  }
  else if(Ax->type == BOX_INT32)
  {
    const box_int32 * A = containerof(Ax, box_int32, bx);
    const box_int32 * B = containerof(Bx, box_int32, bx);
    r = INTCMP(A->v, B->v);
  }
  else if(Ax->type == BOX_INT64)
  {
    const box_int64 * A = containerof(Ax, box_int64, bx);
    const box_int64 * B = containerof(Bx, box_int64, bx);
    r = INTCMP(A->v, B->v);
  }
  else if(Ax->type == BOX_UINT8)
  {
    const box_uint8 * A = containerof(Ax, box_uint8, bx);
    const box_uint8 * B = containerof(Bx, box_uint8, bx);
    r = INTCMP(A->v, B->v);
  }
  else if(Ax->type == BOX_UINT16)
  {
    const box_uint16 * A = containerof(Ax, box_uint16, bx);
    const box_uint16 * B = containerof(Bx, box_uint16, bx);
    r = INTCMP(A->v, B->v);
  }
  else if(Ax->type == BOX_UINT32)
  {
    const box_uint32 * A = containerof(Ax, box_uint32, bx);
    const box_uint32 * B = containerof(Bx, box_uint32, bx);
    r = INTCMP(A->v, B->v);
  }
  else if(Ax->type == BOX_UINT64)
  {
    const box_uint64 * A = containerof(Ax, box_uint64, bx);
    const box_uint64 * B = containerof(Bx, box_uint64, bx);
    r = INTCMP(A->v, B->v);
  }

  return r;
}

bool box_equal(box * restrict A, box * restrict B)
{
  if(A == 0 || B == 0)
    return !!A == !!B;

  if(A->type != B->type)
    return false;

  if(box_hash(0, A) != box_hash(0, B))
    return false;

  return box_cmp(A, B) == 0;
}

void box_copyif(box ** restrict dst, box * restrict src)
{
  if(src)
    *dst = src;
}

void box_free(box * bx)
{
  if(bx)
  {
    if(bx->type == BOX_STRING)
    {
      box_string * b = containerof(bx, box_string, bx);
      wfree(b->v);
    }
  }

  wfree(bx);
}
