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

#include "xlinux/xstdlib.h"

#include "box.h"

#include "common/hash.h"
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

xapi API box_int_mk(
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

xapi API box_int8_mk(
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

xapi API box_int16_mk(
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

xapi API box_int32_mk(
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

xapi API box_int64_mk(
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

xapi API box_uint8_mk(
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

xapi API box_uint16_mk(
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

xapi API box_uint32_mk(
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

xapi API box_uint64_mk(
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

xapi API box_bool_mk(
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

xapi API box_string_mk(
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

void API box_string_setif(box_string * restrict b, char ** v)
{
  if(b)
    *v = b->v;
}

void API box_bool_setif(box_bool * restrict b, bool * v)
{
  if(b)
    *v = b->v;
}

void API box_int16_setif(box_int16 * restrict b, int16_t * v)
{
  if(b)
    *v = b->v;
}

void API box_uint16_setif(box_uint16 * restrict b, uint16_t * v)
{
  if(b)
    *v = b->v;
}

void API box_int_setif(box_int * restrict b, int * v)
{
  if(b)
    *v = b->v;
}

//
// box
//

int API box_int_cmp(const box_int *a, const box_int *b)
{
  if(a == 0 || b == 0)
  {
    return !!a - !!b;
  }
  return INTCMP(a->v, b->v);
}

int API box_bool_cmp(const box_bool *a, const box_bool *b)
{
  if(a == 0 || b == 0)
  {
    return !!a - !!b;
  }
  return INTCMP(a->v, b->v);
}

int API box_string_cmp(const box_string *a, const box_string *b)
{
  if(a == 0 || b == 0)
  {
    return !!a - !!b;
  }
  return memncmp(a->v, a->l, b->v, b->l);
}

int API box_int8_cmp(const box_int8 *a, const box_int8 *b)
{
  if(a == 0 || b == 0)
  {
    return !!a - !!b;
  }
  return INTCMP(a->v, b->v);
}

int API box_int16_cmp(const box_int16 *a, const box_int16 *b)
{
  if(a == 0 || b == 0)
  {
    return !!a - !!b;
  }
  return INTCMP(a->v, b->v);
}

int API box_int32_cmp(const box_int32 *a, const box_int32 *b)
{
  if(a == 0 || b == 0)
  {
    return !!a - !!b;
  }
  return INTCMP(a->v, b->v);
}

int API box_int64_cmp(const box_int64 *a, const box_int64 *b)
{
  if(a == 0 || b == 0)
  {
    return !!a - !!b;
  }
  return INTCMP(a->v, b->v);
}

int API box_uint8_cmp(const box_uint8 *a, const box_uint8 *b)
{
  if(a == 0 || b == 0)
  {
    return !!a - !!b;
  }
  return INTCMP(a->v, b->v);
}

int API box_uint16_cmp(const box_uint16 *a, const box_uint16 *b)
{
  if(a == 0 || b == 0)
  {
    return !!a - !!b;
  }
  return INTCMP(a->v, b->v);
}

int API box_uint32_cmp(const box_uint32 *a, const box_uint32 *b)
{
  if(a == 0 || b == 0)
  {
    return !!a - !!b;
  }
  return INTCMP(a->v, b->v);
}

int API box_uint64_cmp(const box_uint64 *a, const box_uint64 *b)
{
  if(a == 0 || b == 0)
  {
    return !!a - !!b;
  }
  return INTCMP(a->v, b->v);
}

void API box_free(box * bx)
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
