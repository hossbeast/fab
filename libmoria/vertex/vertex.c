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
#include "xlinux/xstdlib.h"

#include "valyria/list.h"

#include "internal.h"
#include "vertex.internal.h"

#define restrict __restrict

//
// public
//

xapi vertex_create(vertex ** const restrict v)
{
  enter;

  fatal(xmalloc, v, sizeof(**v));
  fatal(list_create, &(*v)->up);
  fatal(list_create, &(*v)->down);

  finally : coda;
}

xapi vertex_xfree(vertex * const restrict v)
{
  enter;

  if(v)
  {
    fatal(list_xfree, v->up);
    fatal(list_xfree, v->down);
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

//
// api
//

API void vertex_value_set(vertex * const restrict v, VERTEX_VALUE_TYPE * value)
{
  v->value = value;
}

API VERTEX_VALUE_TYPE * vertex_value(vertex * const restrict v)
{
  return v->value;
}
