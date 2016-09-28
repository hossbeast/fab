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
#include "xlinux/xstdlib.h"

#include "internal.h"
#include "map.h"
#include "map.def.h"

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
#define SATURATION        0.45f      /* for 100-sized table, reallocate at 45 keys */

#define VALUE_SIZE(m) ({          \
  size_t valz = sizeof(void*);    \
  if((m)->attr & MAP_PRIMARY)     \
  {                               \
    valz = (m)->vsz;              \
  }                               \
  valz;                           \
})

#define VALUE(m, tv, x) ({                         \
  void * val;                                      \
  if((m)->attr & MAP_PRIMARY)                      \
  {                                                \
    val = tv + ((x) * VALUE_SIZE(m));              \
  }                                                \
  else                                             \
  {                                                \
    val = *(char**)(tv + ((x) * VALUE_SIZE(m)));   \
  }                                                \
  val;                                             \
})

#define restrict __restrict

//
// static
//

/// hashkey
//
// SUMMARY
//  compute a hash for the key
//
// REMARKS
//  the jenkins hash function : http://en.wikipedia.org/wiki/Jenkins_hash_function
// 
static size_t __attribute__((nonnull)) hashkey(const char * const restrict k, const size_t kl, size_t lm)
{
  size_t x;
  size_t h = 0;
  for(x = 0; x < kl; x++)
  {
    h += k[x];
    h += (h << 10);
    h ^= (h >> 6);
  }
  h += (h << 3);
  h ^= (h >> 11);
  h += (h << 15);

  return h & lm;
}

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
static int __attribute__((nonnull)) probe(const map * const restrict m, const char * const restrict key, const size_t keyl, size_t * const restrict i)
{
  // start at the index which the key hashes to
  *i = hashkey(key, keyl, m->lm);

  size_t ij = 0;
  size_t * ijp = &ij;

  while(1)
  {
    if(m->tk[*i] && (m->tk[*i]->l || m->tk[*i]->d))
    {
      if(m->tk[*i]->d)
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

    // ring increment
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
  , void (*free_value)(MAP_VALUE_TYPE *)
  , xapi (*xfree_value)(MAP_VALUE_TYPE *)
  , size_t capacity
)
{
  enter;

  fatal(xmalloc, m, sizeof(*m[0]));

  // compute initial table size for 100 keys @ given saturation
  (*m)->table_size = (capacity ?: DEFAULT_CAPACITY) * (1 / SATURATION);

  // round up to the next highest power of 2
  (*m)->table_size--;
  (*m)->table_size |= (*m)->table_size >> 1;
  (*m)->table_size |= (*m)->table_size >> 2;
  (*m)->table_size |= (*m)->table_size >> 4;
  (*m)->table_size |= (*m)->table_size >> 8;
  (*m)->table_size |= (*m)->table_size >> 16;
  (*m)->table_size++;

  (*m)->overflow_size = (size_t)((*m)->table_size * SATURATION);
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
    map * const restrict m
  , const char * const restrict k
  , size_t kl
  , MAP_VALUE_TYPE * const restrict v
  , MAP_VALUE_TYPE * const * const restrict rv
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
      m->free_value(VALUE(m, m->tv, i));
    else if(m->xfree_value)
      fatal(m->xfree_value, VALUE(m, m->tv, i));
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
    m->overflow_size = (size_t)(m->table_size * SATURATION);
    m->lm = m->table_size - 1;

    // reassociate active entries
    for(x = 0; x < ksl; x++)
    {
      if(ks[x] && ks[x]->l && !ks[x]->d)
      {
        // first empty spot in the linear probe sequence
        size_t j = hashkey(ks[x]->p, ks[x]->l, m->lm);
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
      fatal(xrealloc, &m->tk[i], sizeof(*m->tk[i]), kl + 1, (m->tk[i] ? m->tk[i]->a : 0));
      m->tk[i]->a = kl + 1;
    }

    memcpy(m->tk[i]->p, k, kl);
    m->tk[i]->p[kl] = 0;
    m->tk[i]->l = kl;
    m->tk[i]->d = 0;

    m->size++;
  }

  // copy the value
  if(v)
    ((void**)m->tv)[i] = v;

  // return a pointer to the value
  if(rv)
    *(void**)rv = VALUE(m, m->tv, i);

finally:
  for(x = 0; x < ksl; x++)
  {
    // free_element was called for these values when they were deleted
    xfree(ks[x]);
  }

  xfree(ks);
  xfree(vs);
coda;
}


//
// api
//

API xapi map_create(map ** const restrict m)
{
  enter;

  fatal(map_allocate, m, MAP_SECONDARY, 0, 0, 0, 0);

  finally : coda;
}

API xapi map_createx(
    map ** const restrict m
  , void (*free_value)(MAP_VALUE_TYPE *)
  , xapi (*xfree_value)(MAP_VALUE_TYPE *)
  , size_t capacity
)
{
  enter;

  fatal(map_allocate, m, MAP_SECONDARY, 0, free_value, xfree_value, capacity);

  finally : coda;
}

API xapi map_set(map* const restrict m, const char * const restrict k, size_t kl, MAP_VALUE_TYPE * const restrict v)
{
  enter;

  fatal(map_put, m, k, kl, v, 0);

  finally : coda;
}

API MAP_VALUE_TYPE * map_get(const map* const restrict m, const char * const restrict k, size_t kl)
{
  // perform probe
  size_t i = 0;
  if(probe(m, k, kl, &i) == 0)
    return VALUE(m, m->tv, i);

  return 0;
}

API size_t map_size(const map * const restrict m)
{
  return m->size;
}

API xapi map_recycle(map* const restrict m)
{
  enter;

  // reset all lengths; all allocations remain intact
  int x;
  for(x = 0; x < m->table_size; x++)
  {
    if(m->tk[x] && m->tk[x]->l && !m->tk[x]->d)
    {
      if(m->free_value)
        m->free_value(VALUE(m, m->tv, x));
      else if(m->xfree_value)
        fatal(m->xfree_value, VALUE(m, m->tv, x));

      m->tk[x]->l = 0;
      m->tk[x]->d = 0;
    }
  }

  m->size = 0;

  finally : coda;
}

API xapi map_delete(map* const restrict m, const char * const restrict k, size_t kl)
{
  enter;

  size_t i = 0;
  if(probe(m, k, kl, &i) == 0)
  {
    if(m->free_value)
      m->free_value(VALUE(m, m->tv, i));
    else if(m->xfree_value)
      fatal(m->xfree_value, VALUE(m, m->tv, i));

    m->tk[i]->l = 0;
    m->tk[i]->d = 1;

    m->size--;
  }

  finally : coda;
}

API xapi map_keys(const map * const restrict m, const char *** const restrict keys, size_t * const restrict keysl)
{
  enter;

  fatal(xmalloc, keys, m->size * sizeof(*keys));
  (*keysl) = 0;

  size_t x;
  for(x = 0; x < m->table_size; x++)
  {
    if(m->tk[x] && m->tk[x]->l && !m->tk[x]->d)
    {
      (*keys)[(*keysl)++] = m->tk[x]->p;
    }
  }

  finally : coda;
}

API xapi map_values(const map* const restrict m, MAP_VALUE_TYPE *** restrict values, size_t * const restrict valuesl)
{
  enter;

  fatal(xmalloc, values, m->size * sizeof(*values));
  (*valuesl) = 0;

  size_t x;
  for(x = 0; x < m->table_size; x++)
  {
    if(m->tk[x] && m->tk[x]->l && !m->tk[x]->d)
    {
      (*values)[(*valuesl)++] = VALUE(m, m->tv, x);
    }
  }

  finally : coda;
}

API xapi map_xfree(map* const restrict m)
{
  enter;

  if(m)   // free-like semantics
  {
    int x;
    for(x = 0; x < m->table_size; x++)
    {
      if(m->tk[x] && m->tk[x]->l && !m->tk[x]->d)
      {
        if(m->free_value)
          m->free_value(VALUE(m, m->tv, x));
        else if(m->xfree_value)
          fatal(m->xfree_value, VALUE(m, m->tv, x));
      }

      xfree(m->tk[x]);
    }

    xfree(m->tk);
    xfree(m->tv);
  }

  xfree(m);

  finally : coda;
}

API xapi map_ixfree(map** const restrict m)
{
  enter;

  fatal(map_xfree, *m);
  *m = 0;

  finally : coda;
}

API size_t map_table_size(const map * const restrict m)
{
  return m->table_size;
}

API const char * map_table_key(const map * const restrict m, size_t x)
{
  if(m->tk[x] && m->tk[x]->l && !m->tk[x]->d)
    return m->tk[x]->p;

  return 0;
}

API MAP_VALUE_TYPE * map_table_value(const map * const restrict m, size_t x)
{
  if(m->tk[x] && m->tk[x]->l && !m->tk[x]->d)
    return VALUE(m, m->tv, x);

  return 0;
}
