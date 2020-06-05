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
#include "valyria/set.h"
#include "valyria/pstring.h"
#include "logger.h"
#include "narrator.h"

#include "value.h"
#include "merge.internal.h"
#include "VALUE.errtab.h"
#include "parser.h"
#include "make.h"
#include "clone.h"
#include "value.h"

#include "zbuffer.h"
#include "common/attrs.h"

xapi API value_merge(value_parser * restrict parser, value ** restrict dstp, const value * restrict src, uint16_t attrs)
{
  enter;

  int x;
  value *el;
  value *dst;
  value *c;

  dst = *dstp;

  if(!src) { }
  else if(!dst || (attrs & MERGE_OPT) == VALUE_MERGE_SET)
  {
    fatal(value_clone, parser, dstp, src);
  }

  /* scalar / set promotion */
  else if(dst->type == VALUE_TYPE_SET && (src->type & VALUE_TYPE_SCALAR))
  {
    fatal(value_clone, parser, &c, src);
    fatal(set_put, dst->els, c, 0);
  }
  else if(dst->type & VALUE_TYPE_SCALAR && (src->type == VALUE_TYPE_SET))
  {
    fatal(value_clone, parser, dstp, src);
    fatal(set_put, (*dstp)->els, dst, 0);
  }

  /* scalar / list promotion */
  else if(dst->type == VALUE_TYPE_LIST && (src->type & VALUE_TYPE_SCALAR))
  {
    fatal(value_clone, parser, &c, src);
    fatal(list_push, dst->items, c, 0);
  }
  else if(dst->type & VALUE_TYPE_SCALAR && (src->type == VALUE_TYPE_LIST))
  {
    fatal(value_clone, parser, dstp, src);
    fatal(list_push, (*dstp)->items, dst, 0);
  }

  /* scalar / scalar : list vivification */
  else if((dst->type & VALUE_TYPE_SCALAR) && (src->type & VALUE_TYPE_SCALAR))
  {
    fatal(value_clone, parser, &c, src);
    fatal(value_list_mkv, parser, 0, 0, dstp, c);
    fatal(value_list_mkv, parser, 0, *dstp, dstp, dst);
  }

  /* scalar / mapping promotion */
  else if(dst->type == VALUE_TYPE_MAPPING && (src->type & VALUE_TYPE_SCALAR))
  {
    fatal(value_merge, parser, &dst->val, src, 0);
  }
  else if(dst->type & VALUE_TYPE_SCALAR && (src->type == VALUE_TYPE_MAPPING))
  {
    fatal(value_clone, parser, dstp, src);
    fatal(value_merge, parser, &(*dstp)->val, dst, 0);
  }

  /* mapping -> mapping */
  else if(dst->type == VALUE_TYPE_MAPPING && src->type == VALUE_TYPE_MAPPING)
  {
    fatal(value_merge, parser, &dst->val, src->val, src->attr);
  }
  else if(dst->type == VALUE_TYPE_MAPPING && src->type == VALUE_TYPE_LIST)
  {
    fatal(value_clone, parser, dstp, src);
    fatal(list_push, (*dstp)->items, dst, 0);
  }
  else if(dst->type == VALUE_TYPE_MAPPING && src->type == VALUE_TYPE_SET)
  {
    fatal(value_clone, parser, dstp, src);
    fatal(set_put, (*dstp)->els, dst, 0);
  }

  /* list -> list */
  else if(dst->type == VALUE_TYPE_LIST && src->type == VALUE_TYPE_LIST)
  {
    fatal(value_clone, parser, dstp, src);
    fatal(list_replicate, (*dstp)->items, 0, dst->items, 0, dst->items->size);
  }
  /* set -> list */
  else if(dst->type == VALUE_TYPE_LIST && src->type == VALUE_TYPE_SET)
  {
    for(x = 0; x < src->els->table_size; x++)
    {
      if((el = set_table_get(src->els, x)) == 0)
        continue;

      fatal(value_clone, parser, &c, el);
      fatal(list_push, dst->items, c, 0);
    }
  }
  /* mapping -> list */
  else if(dst->type == VALUE_TYPE_LIST && src->type == VALUE_TYPE_MAPPING)
  {
    fatal(value_clone, parser, &c, src);
    fatal(list_push, dst->items, c, 0);
  }

  /* set -> set */
  else if(dst->type == VALUE_TYPE_SET && src->type == VALUE_TYPE_SET)
  {
    for(x = 0; x < src->els->table_size; x++)
    {
      value * src_el;
      if((src_el = set_table_get(src->els, x)) == 0)
        continue;

      // mapping-equality is key only
      value * old_el = 0;
      if((el = set_get(dst->els, src_el, 0)))
      {
        old_el = el;
        fatal(value_merge, parser, &el, src_el, 0);
      }
      else
      {
        fatal(value_clone, parser, &el, src_el);
      }

      if(el != old_el)
        fatal(set_put, dst->els, el, 0);
    }
  }
  /* list -> set */
  else if(dst->type == VALUE_TYPE_SET && src->type == VALUE_TYPE_LIST)
  {
    for(x = 0; x < src->items->size; x++)
    {
      fatal(value_clone, parser, &c, list_get(src->items, x));
      fatal(set_put, dst->els, c, 0);
    }
  }
  /* mapping -> set */
  else if(dst->type == VALUE_TYPE_SET && src->type == VALUE_TYPE_MAPPING)
  {
    /* mapping-equality is key only */
    value * old_el = 0;
    if((el = set_get(dst->els, src, 0)))
    {
      old_el = el;
      fatal(value_merge, parser, &el, src, src->attr);
    }
    else
    {
      fatal(value_clone, parser, &el, src);
    }

    if(el != old_el)
      fatal(set_put, dst->els, el, 0);
  }

  else
  {
    RUNTIME_ABORT();
  }

  finally : coda;
}
