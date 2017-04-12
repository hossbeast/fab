/* Copyright (c) 2012-2017 Todd Freed <todd.freed@gmail.com>

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

#ifndef _ATTRS_H
#define _ATTRS_H

#include <stdint.h>

#include "macros.h"

#define restrict __restrict

struct attrs16 {
  const char * name;
  uint16_t namel;
  uint16_t value;
};

static int attrs16_compare_byname(const void * _A, const void * _B)
{
  const struct attrs16 * A = _A;
  const struct attrs16 * B = _B;
  return memncmp(A->name, A->namel, B->name, B->namel);
}

static uint16_t attrs16_lookup_byname(const struct attrs16 * restrict table, uint16_t table_len, const char * name, size_t namel)
{
  struct attrs16 key = { .name = name, .namel = namel };
  struct attrs16 * attr = bsearch(&key, table, table_len, sizeof(*table), attrs16_compare_byname);
  if(attr)
    return attr->value;

  return 0;
}

static inline const char * attrs16_option_name(const struct attrs16 * restrict table, uint16_t table_len, uint16_t mask, uint16_t attrs)
{
  attrs &= mask;

  uint16_t x;
  for(x = 0; x < table_len; x++)
  {
    if(attrs == table[x].value)
      return table[x].name;
  }

  return 0;
}

#undef restrict
#endif
