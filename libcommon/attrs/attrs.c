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

#include <stdlib.h>

#include "attrs.h"

#include "macros.h"
#include "hash.h"
#include "zbuffer.h"

//
// static 16
//

static int member16_cmp_name(const void * _A, const void * _B)
{
  const member16 * A = _A;
  const member16 * B = _B;

  return memncmp(A->name, A->namel, B->name, B->namel);
}

static const member16 * attrs16_member_bynamew(const attrs16 * restrict e, const char * restrict name, uint8_t namel)
{
  member16 key = { .name = name, .namel = namel };
  member16 * member = bsearch(&key, e->members, e->num, sizeof(*e->members), member16_cmp_name);

  return member;
}

static const member16 * attrs16_member_byvalue(const attrs16 * restrict e, uint16_t attrs)
{
  uint16_t x;
  for(x = 0; x < e->num; x++)
  {
    if((attrs & e->members[x].range) == e->members[x].value)
      return &e->members[x];
  }

  return 0;
}

//
// static 32
//

static int member32_cmp_name(const void * _A, const void * _B)
{
  const member32 * A = _A;
  const member32 * B = _B;

  return memncmp(A->name, A->namel, B->name, B->namel);
}

static const member32 * attrs32_member_bynamew(const attrs32 * restrict e, const char * restrict name, uint8_t namel)
{
  member32 key = { .name = name, .namel = namel };
  member32 * member = bsearch(&key, e->members, e->num, sizeof(*e->members), member32_cmp_name);

  return member;
}

static const member32 * attrs32_member_byvalue(const attrs32 * restrict e, uint32_t attrs)
{
  uint16_t x;
  for(x = 0; x < e->num; x++)
  {
    if((attrs & e->members[x].range) == e->members[x].value)
      return &e->members[x];
  }

  return 0;
}

//
// public 16
//

void API attrs16_init(attrs16 * e)
{
  int x;
  for(x = 0; x < e->num; x++)
    e->members[x].namel = strlen(e->members[x].name);

  qsort(e->members, e->num, sizeof(*e->members), member16_cmp_name);
}

uint16_t API attrs16_value_bynamew(const attrs16 * restrict e, const char * restrict name, uint8_t namel)
{
  const member16 * member;

  if((member = attrs16_member_bynamew(e, name, namel)))
    return member->value;

  return 0;
}

uint16_t API attrs16_value_byname(const attrs16 * restrict e, const char * restrict name)
{
  return attrs16_value_bynamew(e, name, strlen(name));
}

const char * API attrs16_name_byvalue(const attrs16 * restrict e, uint16_t attrs)
{
  const member16 * member;

  if((member = attrs16_member_byvalue(e, attrs)))
    return member->name;

  return 0;
}

size_t API znload_attrs16(void * restrict dst, size_t sz, const attrs16 * restrict e, uint16_t attrs)
{
  uint16_t erange;
  uint16_t evalue;
  size_t z;
  uint16_t bits = 0;
  uint16_t rem;
  int x;

  z = 0;

  for(x = 0; x < e->num; x++)
  {
    erange = e->members[x].range;
    evalue = e->members[x].value;

    if((erange & attrs) == evalue)
    {
      if(z) {
        z += znloadf(dst + z, sz - z, "|");
      }

      z += znloadf(dst + z, sz - z, "%.*s", e->members[x].namel, e->members[x].name);
      bits |= e->members[x].range;
    }
  }

  if((rem = (attrs & ~bits)))
  {
    if(z) {
      z += znloadf(dst + z, sz - z, "|");
    }
    z += znloadf(dst + z, sz - z, "0x%x", rem);
  }

  return z;
}

//
// public 32
//

void API attrs32_init(attrs32 * e)
{
  int x;
  for(x = 0; x < e->num; x++)
    e->members[x].namel = strlen(e->members[x].name);

  qsort(e->members, e->num, sizeof(*e->members), member32_cmp_name);
}

uint32_t API attrs32_value_bynamew(const attrs32 * restrict e, const char * restrict name, uint8_t namel)
{
  const member32 * member;

  if((member = attrs32_member_bynamew(e, name, namel)))
    return member->value;

  return 0;
}

uint32_t API attrs32_value_byname(const attrs32 * restrict e, const char * restrict name)
{
  return attrs32_value_bynamew(e, name, strlen(name));
}

const char * API attrs32_name_byvalue(const attrs32 * restrict e, uint32_t attrs)
{
  const member32 * member;

  if((member = attrs32_member_byvalue(e, attrs)))
    return member->name;

  return 0;
}

size_t API znload_attrs32(void * restrict dst, size_t sz, const attrs32 * restrict e, uint32_t attrs)
{
  uint32_t erange;
  uint32_t evalue;
  size_t z = 0;
  uint32_t bits = 0;
  uint32_t rem;
  int x;

  z = 0;
  for(x = 0; x < e->num; x++)
  {
    erange = e->members[x].range;
    evalue = e->members[x].value;

    if((erange & attrs) == evalue)
    {
      if(z) {
        z += znloadf(dst + z, sz - z, "|");
      }

      z += znloadf(dst + z, sz - z, "%.*s", e->members[x].namel, e->members[x].name);
      bits |= e->members[x].range;
    }
  }

  if((rem = (attrs & ~bits)))
  {
    if(z) {
      z += znloadf(dst + z, sz - z, "|");
    }
    z += znloadf(dst + z, sz - z, "0x%x", rem);
  }

  return z;
}
