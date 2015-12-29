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
#include "xlinux.h"

#include "list.h"

#include "grow.h"

#define restrict __restrict

xapi list_append(list * const restrict s, void * const restrict el)
{
  enter;

  fatal(grow, &s->list_v, s->list_es, 1, &s->list_l, &s->list_a);
  memcpy(((char*)s->list_v) + ((s->list_l - 1) * s->list_es), el, s->list_es);

  finally : coda;
}

xapi list_append_range(list * const restrict s, void * const restrict el, size_t len)
{
  enter;

  size_t list_l = s->list_l;
  fatal(grow, &s->list_v, s->list_es, len, &s->list_l, &s->list_a);
  memcpy(((char*)s->list_v) + (list_l * s->list_es), el, s->list_es * len);

  finally : coda;
}

xapi list_grow(list * const restrict s, size_t len)
{
  xproxy(grow, &s->list_v, s->list_es, len, &s->list_l, &s->list_a);
}

void list_clear(list * const restrict s)
{
  s->list_l = 0;
}
