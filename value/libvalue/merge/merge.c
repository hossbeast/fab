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

#include <stdio.h>

#include "xapi.h"
#include "valyria/list.h"
#include "valyria/hashtable.h"
#include "valyria/pstring.h"

#include "internal.h"
#include "VALUE.errtab.h"
#include "merge.internal.h"

#include "logger.h"
#include "narrator.h"

#include "zbuffer.h"

static xapi merge(
    value * const restrict dst
  , const value * const restrict src
  , uint16_t outer_attr
  , const char ** restrict parts
  , size_t partsz
  , size_t * restrict partsl
)
  __attribute__((nonnull));

static xapi merge(
    value * const restrict dst
  , const value * const restrict src
  , uint16_t outer_attr
  , const char ** restrict parts
  , size_t partsz
  , size_t * restrict partsl
)
{
  enter;

  int x, y;

  if(src->type != dst->type)
  {
    xapi_info_pushf("types", "%s -> %s", VALUE_TYPE_STRING(dst->type), VALUE_TYPE_STRING(src->type));
    xapi_info_pushf("location"
      , "[%d,%d - %d,%d]"
      , dst->loc.f_lin
      , dst->loc.f_col
      , dst->loc.l_lin
      , dst->loc.l_col
    );
    if(dst->loc.fname)
      xapi_info_pushs("file", dst->loc.fname);

    fail(VALUE_DIFFTYPE);
  }
  else if(dst->type == VALUE_TYPE_MAP)
  {
    if((outer_attr & MERGE_OPT) == VALUE_MERGE_SET)
    {
      dst->keys = src->keys;
      dst->vals = src->vals;
    }
    else // VALUE_MERGE_ADD
    {
      x = dst->keys->l - 1;
      y = src->keys->l - 1;
      for(; x >= 0 && y >= 0;)
      {
        value * src_key = list_get(src->keys, y);
        value * dst_key = list_get(dst->keys, x);
        int d = pscmp(src_key->s, dst_key->s);
        if(d < 0)
        {
          x--;
        }
        else if(d > 0)
        {
          fatal(list_replicate, dst->keys, x + 1, src->keys, y, 1);
          fatal(list_replicate, dst->vals, x + 1, src->vals, y, 1);
          y--;
        }
        else
        {
          uint16_t dt = ((value*)list_get(dst->vals, x))->type;
          uint16_t st = ((value*)list_get(src->vals, y))->type;

          if((src_key->attr & MERGE_OPT) == VALUE_MERGE_SET)
          {
            fatal(list_splice, dst->vals, x, src->vals, y, 1);
          }
          else if(dt & st & VALUE_TYPE_SCALAR)
          {
            fatal(list_splice, dst->vals, x, src->vals, y, 1);
          }
          else
          {
            if(invoke(merge, list_get(dst->vals, x), list_get(src->vals, y), src_key->attr, parts, partsz, partsl))
            {
              if(*partsl < partsz)
                parts[(*partsl)++] = src_key->s->s;

              fail(0);
            }
          }

          x--;
          y--;
        }
      }

      if(x == -1 && y >= 0)
      {
        fatal(list_replicate, dst->keys, 0, src->keys, 0, y + 1);
        fatal(list_replicate, dst->vals, 0, src->vals, 0, y + 1);
      }
    }
  }
  else if(dst->type == VALUE_TYPE_LIST)
  {
    if((outer_attr & MERGE_OPT) == VALUE_MERGE_SET)
    {
      fatal(list_splice, dst->items, 0, src->items, 0, src->items->l);
      fatal(list_truncate, dst->items, src->items->l);
    }
    else // VALUE_MERGE_ADD
    {
      fatal(list_replicate, dst->items, dst->items->l, src->items, 0, src->items->l);
    }
  }
  else if(dst->type == VALUE_TYPE_SET)
  {
    if((outer_attr & MERGE_OPT) == VALUE_MERGE_SET)
    {
      dst->els = src->els;
    }
    else // VALUE_MERGE_ADD
    {
      for(x = 0; x < src->els->table_size; x++)
      {
        value ** ent;
        if((ent = hashtable_table_entry(src->els, x)))
          fatal(hashtable_put, dst->els, ent);
      }
    }
  }

  finally : coda;
}

API xapi value_merge(value * const restrict dst, const value * const restrict src)
{
  enter;

  char space[128];
  const char * parts[16];
  size_t partsl = 0;

  fatal(merge, dst, src, 0, parts, sizeof(parts) / sizeof(parts[0]), &partsl);

finally:
  if(XAPI_UNWINDING)
  {
    char * s = space;
    size_t sz = sizeof(space);
    size_t l = 0;

    int x;
    for(x = partsl - 1; x >= 0; x--)
    {
      if(x != partsl - 1)
        l += znloadf(s + l, sz - l, ".");
      l += znloadf(s + l, sz - l, "%s", parts[x]);
    }

    xapi_infos("path", space);
  }
coda;
}
