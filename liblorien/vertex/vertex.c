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

#include "valyria/list.h"

#include "internal.h"
#include "vertex.internal.h"

#define restrict __restrict

//
// public
//

xapi vertex_initialize(vertex * restrict v)
{
  enter;

  fatal(list_create, &v->up, 0);
  fatal(list_create, &v->down, 0);

  finally : coda;
}

void vertex_destroy(vertex * restrict v)
{
  list_free(v->up);
  list_free(v->down);
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
