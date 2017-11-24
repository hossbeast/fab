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
#include <stdlib.h>
#include <string.h>

#include "xapi.h"
#include "types.h"
#include "xlinux/xstdlib.h"

#include "internal.h"
#include "map.internal.h"
#include "faults.internal.h"
#include "maputils.internal.h"

#include "macros.h"

/*
 * Default capacity, the minimum number of entries which can be set without rehashing
 */
#define DEFAULT_CAPACITY  100

/*
 * Maximum proportion of used entries to allocated table size
 *
 * valid values : 0 < saturation <= 1
 *
 * smaller values trade space for time because sparser tables mean shorter probe sequences
 */
#define MAX_SATURATION    0.45f      /* for 100-sized table, reallocate at 45 keys */

//
// static
//

/// probe
//
// SUMMARY
//  search for a key by linear probe and get its index
//
// PARAMETERS
//  m    - map
//  key  - pointer to key
//  keyl - size of key, nonzero
//  i    - (returns) table index
//
// RETURNS
//   0 : found
//   1 : not found, and the first deleted slot in the probe sequence is returned in i
//  -1 : not found
//
static int __attribute__((nonnull)) probe(const map * restrict m, const void * restrict key, size_t keyl, size_t * restrict i)
{
  // start at the index which the key hashes to
  *i = maputils_hashkey(key, keyl, m->lm);

  size_t ij = 0;
  size_t * ijp = &ij;

  while(1)
  {
    if(m->tk[*i])
    {
      if(m->tk[*i]->attr & MAP_KEY_DELETED)
      {
        if(ijp)
          *ijp = *i;

        ijp = 0;
      }
      else if(m->tk[*i]->l == keyl)
      {
        if(memcmp(key, m->tk[*i]->p, keyl) == 0)
          return 0;
      }
    }
    else if(ijp == 0)
    {
      *i = ij;
      return 1;
    }
    else
    {
      return -1;
    }

    (*i)++;
    (*i) &= m->lm;
  }
}

//
// protected
//

xapi map_allocate(
    map ** const restrict m
  , uint32_t attr
  , size_t vsz
  , void * free_value
  , void * xfree_value
  , size_t capacity
)
{
  enter;

  fatal(xmalloc, m, sizeof(**m));

  // compute initial table size for 100 keys @ given saturation
  (*m)->table_size = (capacity ?: DEFAULT_CAPACITY) * (1 / MAX_SATURATION);

  // round up to the next highest power of 2
  (*m)->table_size = roundup2((*m)->table_size);
  (*m)->overflow_size = (size_t)((*m)->table_size * MAX_SATURATION);
  (*m)->lm = (*m)->table_size - 1;

  (*m)->free_value = free_value;
  (*m)->xfree_value = xfree_value;
  (*m)->vsz = vsz;
  (*m)->attr = attr;

  fatal(xmalloc, &(*m)->tk, sizeof(*(*m)->tk) * (*m)->table_size);
  fatal(xmalloc, &(*m)->tv, VALUE_SIZE(*m) * (*m)->table_size);

  finally : coda;
}

xapi map_put(
    map * restrict m
  , const void * restrict k
  , size_t kl
  , void * const restrict v
  , void * const * restrict rv
)
{
  enter;

  key ** ks = 0;    // old keytable
  char * vs = 0;    // old value table
  size_t ksl = 0;

  int x;

  size_t i = 0;
  int r = probe(m, k, kl, &i);
  if(r == 0)  // found
  {
    // the value will be overwritten
    if(m->free_value)
      m->free_value(VALUE(m, i));
    else if(m->xfree_value)
      fatal(m->xfree_value, VALUE(m, i));
  }
  else if(r == 1) // not found, and a suitable deleted slot exists
  {
    // no-op
  }
  else if(m->size == m->overflow_size) // not found, and saturation has been reached
  {
    // allocate new tables
    void * tmp = m->tk;
    fatal(xmalloc, &m->tk, sizeof(*m->tk) * (m->table_size << 2));
    ks = tmp;
    ksl = m->table_size;

    tmp = m->tv;
    fatal(xmalloc, &m->tv, VALUE_SIZE(m) * (m->table_size << 2));
    vs = tmp;

    m->table_size <<= 2;
    m->overflow_size = (size_t)(m->table_size * MAX_SATURATION);
    m->lm = m->table_size - 1;

    // reassociate active entries
    for(x = 0; x < ksl; x++)
    {
      if(ks[x] && !(ks[x]->attr & MAP_KEY_DELETED))
      {
        // first empty spot in the linear probe sequence
        size_t j = maputils_hashkey(ks[x]->p, ks[x]->l, m->lm);
        while(m->tk[j])
        {
          j++;
          j &= m->lm;
        }

        m->tk[j] = ks[x];
        ks[x] = 0;
        memcpy(m->tv + (VALUE_SIZE(m) * j), vs + (VALUE_SIZE(m) * x), VALUE_SIZE(m));
      }
    }

    // probe against the new table dimensions
    probe(m, k, kl, &i);
  }

  // copy the key
  if(r)
  {
    if(m->tk[i] == 0 || kl >= m->tk[i]->a)
    {
      fatal(xrealloc, &m->tk[i], 1, sizeof(*m->tk[i]) + kl + 1, (m->tk[i] ? m->tk[i]->a : 0));
      m->tk[i]->a = kl + 1;
    }

    memcpy(m->tk[i]->p, k, kl);
    m->tk[i]->p[kl] = 0;
    m->tk[i]->l = kl;
    m->tk[i]->attr = 0;

    m->size++;
  }
  // copy the value
  if(v)
    ((void**)m->tv)[i] = v;

  // return a pointer to the value
  if(rv)
    *(void**)rv = VALUE(m, i);

finally:
  for(x = 0; x < ksl; x++)
  {
    // free_element was called for these values when they were deleted
    wfree(ks[x]);
  }

  wfree(ks);
  wfree(vs);
coda;
}

//
// api
//

API xapi map_create(map ** restrict m)
{
  xproxy(map_allocate, m, MAP_SECONDARY, 0, 0, 0, 0);
}

API xapi map_createx(
    map ** restrict m
  , void * free_value
  , void * xfree_value
  , size_t capacity
)
{
  xproxy(map_allocate, m, MAP_SECONDARY, 0, free_value, xfree_value, capacity);
}

API xapi map_set(map * restrict m, const void * restrict k, size_t kl, void * restrict v)
{
  xproxy(map_put, m, k, kl, v, 0);
}

API void * map_get(const map * restrict m, const void * restrict k, size_t kl)
{
  size_t i = 0;
  if(probe(m, k, kl, &i) == 0)
    return VALUE(m, i);

  return 0;
}

API size_t map_size(const map * restrict m)
{
  return m->size;
}

API xapi map_recycle(map * restrict m)
{
  enter;

  // reset all lengths; all allocations remain intact
  int x;
  for(x = 0; x < m->table_size; x++)
  {
    if(m->tk[x] && !(m->tk[x]->attr & MAP_KEY_DELETED))
    {
      if(m->free_value)
        m->free_value(VALUE(m, x));
      else if(m->xfree_value)
        fatal(m->xfree_value, VALUE(m, x));
    }

    if(m->tk[x])
      m->tk[x]->attr = 0;
  }

  m->size = 0;

  finally : coda;
}

API xapi map_delete(map * restrict m, const void * restrict k, size_t kl)
{
  enter;

  size_t i;
  if(probe(m, k, kl, &i) == 0)
  {
    if(m->free_value)
      m->free_value(VALUE(m, i));
    else if(m->xfree_value)
      fatal(m->xfree_value, VALUE(m, i));

    m->tk[i]->attr |= MAP_KEY_DELETED;
    m->size--;
  }

  finally : coda;
}

API xapi map_keys(const map * restrict m, const char *** restrict keys, size_t * restrict keysl)
{
  enter;

  fatal(xmalloc, keys, m->size * sizeof(*keys));
  (*keysl) = 0;

  size_t x;
  for(x = 0; x < m->table_size; x++)
  {
    if(m->tk[x] && !(m->tk[x]->attr & MAP_KEY_DELETED))
      (*keys)[(*keysl)++] = m->tk[x]->p;
  }

  finally : coda;
}

API xapi map_values(const map * restrict m, void * restrict _values, size_t * restrict valuesl)
{
  enter;

  void *** values = _values;

  fatal(xmalloc, values, m->size * sizeof(*values));
  (*valuesl) = 0;

  size_t x;
  for(x = 0; x < m->table_size; x++)
  {
    if(m->tk[x] && !(m->tk[x]->attr & MAP_KEY_DELETED))
      (*values)[(*valuesl)++] = VALUE(m, x);
  }

  finally : coda;
}

API xapi map_xfree(map * restrict m)
{
  enter;

  if(m)   // free-like semantics
  {
    fatal(map_recycle, m);

    int x;
    for(x = 0; x < m->table_size; x++)
      wfree(m->tk[x]);

    wfree(m->tk);
    wfree(m->tv);
  }

  wfree(m);

  finally : coda;
}
API xapi map_ixfree(map ** restrict m)
{
  enter;

  fatal(map_xfree, *m);
  *m = 0;

  finally : coda;
}

API size_t map_table_size(const map * restrict m)
{
  return m->table_size;
}

API const void * map_table_key(const map * restrict m, size_t x)
{
  if(m->tk[x] && !(m->tk[x]->attr & MAP_KEY_DELETED))
    return m->tk[x]->p;

  return 0;
}

API void * map_table_value(const map * restrict m, size_t x)
{
  if(m->tk[x] && !(m->tk[x]->attr & MAP_KEY_DELETED))
    return VALUE(m, x);

  return 0;
}
