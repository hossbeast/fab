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
#include "valyria/pstring.h"
#include "valyria/list.h"
#include "valyria/set.h"

#include "value.h"
#include "clone.h"
#include "store.internal.h"
#include "parser.internal.h"

//
// static
//

static xapi clone_set(value_parser * restrict parser, value ** restrict dst, const value * src)
{
  enter;

  int x;
  value * v;
  value * c;

  fatal(store_set, parser, dst);

  for(x = 0; x < src->els->table_size; x++)
  {
    if(!((v = set_table_get(src->els, x))))
      continue;

    fatal(value_clone, parser, &c, v);
    fatal(set_put, (*dst)->els, c, 0);
  }

  finally : coda;
}

static xapi clone_list(value_parser * restrict parser, value ** restrict dst, const value * src)
{
  enter;

  int x;
  value * c;

  fatal(store_list, parser, dst);

  for(x = 0; x < src->items->size; x++)
  {
    fatal(value_clone, parser, &c, list_get(src->items, x));
    fatal(list_push, (*dst)->items, c, 0);
  }

  finally : coda;
}

static xapi clone_mapping(value_parser * restrict parser, value ** restrict dst, const value * src)
{
  enter;

  value * c;

  fatal(store_mapping, parser, dst);

  fatal(value_clone, parser, &c, src->key);
  (*dst)->key = c;

  fatal(value_clone, parser, &c, src->val);
  (*dst)->val = c;

  finally : coda;
}

static xapi clone_string(value_parser * restrict parser, value ** restrict dst, const value * src)
{
  enter;

  fatal(store_string, parser, dst);
  fatal(pscats, (*dst)->s, src->s->s);

  finally : coda;
}

static xapi clone_float(value_parser * restrict parser, value ** restrict dst, const value * src)
{
  enter;

  fatal(store_float, parser, dst);
  (*dst)->f = src->f;

  finally : coda;
}

static xapi clone_bool(value_parser * restrict parser, value ** restrict dst, const value * src)
{
  enter;

  fatal(store_boolean, parser, dst);
  (*dst)->b = src->b;

  finally : coda;
}

static xapi clone_posint(value_parser * restrict parser, value ** restrict dst, const value * src)
{
  enter;

  fatal(store_posint, parser, dst);
  (*dst)->u = src->u;

  finally : coda;
}

static xapi clone_negint(value_parser * restrict parser, value ** restrict dst, const value * src)
{
  enter;

  fatal(store_negint, parser, dst);
  (*dst)->i = src->i;

  finally : coda;
}

static xapi clone_variable(value_parser * restrict parser, value ** restrict dst, const value * src)
{
  enter;

  fatal(store_variable, parser, dst);
  (*dst)->v = src->v;

  finally : coda;
}

//
// api
//

xapi API value_clone(
    value_parser * const restrict parser
  , value ** restrict dst
  , const value * restrict src
)
{
  enter;

  if(src->type == VALUE_TYPE_SET)
    fatal(clone_set, parser, dst, src);
  else if(src->type == VALUE_TYPE_LIST)
    fatal(clone_list, parser, dst, src);
  else if(src->type == VALUE_TYPE_MAPPING)
    fatal(clone_mapping, parser, dst, src);

  else if(src->type == VALUE_TYPE_STRING)
    fatal(clone_string, parser, dst, src);
  else if(src->type == VALUE_TYPE_FLOAT)
    fatal(clone_float, parser, dst, src);
  else if(src->type == VALUE_TYPE_BOOLEAN)
    fatal(clone_bool, parser, dst, src);
  else if(src->type == VALUE_TYPE_POSINT)
    fatal(clone_posint, parser, dst, src);
  else if(src->type == VALUE_TYPE_NEGINT)
    fatal(clone_negint, parser, dst, src);
  else if(src->type == VALUE_TYPE_VARIABLE)
    fatal(clone_variable, parser, dst, src);

  finally : coda;
}
