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

struct value;
#define LIST_ELEMENT_TYPE struct value
#include "valyria/list.h"
#include "valyria/pstring.h"

#include "internal.h"
#include "VALUE.errtab.h"
#include "merge.internal.h"

#include "logger.h"
#include "narrator.h"

static xapi merge(value * const restrict dst, const value * const restrict src, uint16_t attr);
static xapi merge(value * const restrict dst, const value * const restrict src, uint16_t attr)
{
  enter;

  if(src->type != dst->type)
    fail(VALUE_DIFFTYPE);

  else if(dst->type == VALUE_TYPE_MAP)
  {
    if((attr & MERGE_OPT) == VALUE_MERGE_SET)
    {
      dst->keys = src->keys;
      dst->vals = src->vals;
    }
    else // VALUE_MERGE_ADD
    {
      int x = dst->keys->l - 1;
      int y = src->keys->l - 1;
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

          if((dt & 0xF0) ^ (st & 0xF0))
            fail(VALUE_DIFFTYPE);
          else if(dt & 0xF00)
            fatal(list_splice, dst->vals, x, src->vals, y, 1);
          else
            fatal(merge, list_get(dst->vals, x), list_get(src->vals, y), src_key->attr);

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
    if((attr & MERGE_OPT) == VALUE_MERGE_SET)
    {
      fatal(list_splice, dst->els, 0, src->els, 0, src->els->l);
      fatal(list_truncate, dst->els, src->els->l);
    }
    else // VALUE_MERGE_ADD
    {
      fatal(list_replicate, dst->els, dst->els->l, src->els, 0, src->els->l);
    }
  }

  finally : coda;
}

API xapi value_merge(value * const restrict dst, const value * const restrict src)
{
  enter;

  fatal(merge, dst, src, 0);

  finally : coda;
}
