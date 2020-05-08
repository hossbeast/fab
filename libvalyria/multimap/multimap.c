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
#include <stdio.h>

#include "xapi.h"
#include "types.h"
#include "xlinux/xstdlib.h"

#include "internal.h"
#include "multimap.internal.h"
#include "faults.internal.h"

#include "macros.h"
#include "hash.h"

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
#define MAX_SATURATION  0.45f      /* for 100-sized table, reallocate at 45 keys */

#define VALUE_SIZE(m) sizeof(void*)
#define MAP_KEY_DELETED 0x01    /* this key has been deleted */

#define MULTIMAP_CONTIGUOUS   0x04  /* entries in the set are stored contiguously */
#define MULTIMAP_FIRST        0x08  /* first is set */
#define MULTIMAP_NEXT         0x10  /* next is set */
#define MULTIMAP_LAST         0x20  /* last is set */

#define VALUE(m, x) ({                                \
  void * val;                                         \
  val = *(char**)((m)->tv + ((x) * VALUE_SIZE(m)));   \
  val;                                                \
})

static uint32_t hash_reduce(uint32_t h, const char * restrict k, size_t kl, uint32_t lm)
{
  size_t x;

  if(fault(ALL_INDEX_BOUNDARY))
  {
    return lm;
  }
  else if(fault(KEY_INDEX_BOUNDARY))
  {
    for(x = 0; x < fault_state.key_index_boundary.len; x++)
    {
      typeof(*fault_state.key_index_boundary.keys) faultkey = fault_state.key_index_boundary.keys[x];

      if(kl == faultkey.len && memcmp(k, faultkey.key, kl) == 0)
        return lm;
    }
  }

  return h & lm;
}

static uint32_t hash(const void * restrict key, size_t keyl)
{
  if(keyl == 0)
    return DATA_ZERO_LEN_HASH;

  return hash32(0, key, keyl);
}

/// probe
//
// SUMMARY
//  search for a key by linear probe and get the index of the head entry
//
// RETURN
//  boolean indicating whether the key was found
//
static bool __attribute__((nonnull)) probe(const multimap_t * restrict m, uint32_t h, const void * restrict key, size_t keyl, size_t * restrict index)
{
  size_t i = hash_reduce(h, key, keyl, m->lm);
  multimap_key * k = m->tk[i];

  if(k && !(k->attr & MAP_KEY_DELETED) && (k->attr & MULTIMAP_FIRST))
    i = k->first;

  while(1)
  {
    k = m->tk[i];
    if(k)
    {
      if(k->attr & MAP_KEY_DELETED)
      {
        // skip
      }
      else if((k->attr & MULTIMAP_LAST) && k->l == keyl && memcmp(key, k->p, keyl) == 0)
      {
        *index = i;
        return 1;
      }

      if((k->attr & MULTIMAP_LAST) && (k->attr & MULTIMAP_CONTIGUOUS))
      {
        i = k->last;
      }
    }
    else
    {
      *index = i;
      return 0;
    }

    // ring increment
    i++;
    i &= m->lm;
  }
}

xapi multimap_rehash(multimap_t * restrict m)
{
  enter;

  multimap_key ** ks = 0; // old keytable
  char * vs = 0;          // old value table
  size_t ksl = 0;
  void * tmp;
  size_t i;

  // allocate new tables
  tmp = m->tk;
  fatal(xmalloc, &m->tk, sizeof(*m->tk) * (m->table_size << 2));
  ks = tmp;

  tmp = m->tv;
  fatal(xmalloc, &m->tv, sizeof(void*) * (m->table_size << 2));
  vs = tmp;

  ksl = m->table_size;
  m->table_size <<= 2;
  m->overflow_size = (size_t)(m->table_size * MAX_SATURATION);
  m->lm = m->table_size - 1;

  // reassociate non-deleted entries
  for(i = 0; i < ksl; i++)
  {
    if(ks[i] && (ks[i]->attr & MULTIMAP_LAST) && !(ks[i]->attr & MAP_KEY_DELETED))
    {
      // start at the first empty spot in the linear probe sequence
      size_t head = hash_reduce(ks[i]->h, ks[i]->p, ks[i]->l, m->lm);

      while(m->tk[head])
      {
        head++;
        head &= m->lm;
      }

      size_t js[2] = { [1] = head };

      size_t n = 1;
      size_t x = i;
      bool contiguous = true;
      while(1)
      {
        js[0] = js[1];

        while(m->tk[js[1]])
        {
          js[1]++;
          js[1] &= m->lm;
        }

        multimap_key tk = *ks[x];
        m->tk[js[1]] = ks[x];
        ks[x] = 0;
        m->tk[js[1]]->attr = 0;

        // copy the value
        memcpy(m->tv + (sizeof(void*) * js[1]), vs + (sizeof(void*) * x), sizeof(void*));

        if(n > 1)
        {
          m->tk[js[1]]->attr |= MULTIMAP_FIRST;
          m->tk[js[1]]->first = head;
          m->tk[js[0]]->attr |= MULTIMAP_NEXT;
          m->tk[js[0]]->next = js[1];

          if(js[1] != (js[0] + 1))
            contiguous = false;
        }

        if(!(tk.attr & MULTIMAP_NEXT))
          break;

        x = tk.next;
        n++;
      }

      m->tk[head]->attr |= MULTIMAP_LAST;
      m->tk[head]->last = js[1];
      m->tk[head]->n = n;
      if(contiguous)
        m->tk[head]->attr |= MULTIMAP_CONTIGUOUS;
    }
  }

finally:
  for(i = 0; i < ksl; i++)
    wfree(ks[i]);

  wfree(ks);
  wfree(vs);
coda;
}

//
// api
//

API xapi multimap_create(multimap ** const restrict m)
{
  xproxy(multimap_createx, m, 0, 0, 0);
}

API xapi multimap_createx(
    multimap ** const restrict mx
  , void * free_value
  , void * xfree_value
  , size_t capacity
)
{
  enter;

  multimap_t * m = 0;

  fatal(xmalloc, &m, sizeof(*m));

  // compute initial table size for 100 keys @ given saturation
  m->table_size = (capacity ?: DEFAULT_CAPACITY) * (1 / MAX_SATURATION);

  // round up to the next highest power of 2
  m->table_size = roundup2(m->table_size);
  m->overflow_size = (size_t)(m->table_size * MAX_SATURATION);
  m->lm = m->table_size - 1;

  m->free_value = free_value;
  m->xfree_value = xfree_value;

  fatal(xmalloc, &m->tk, sizeof(*m->tk) * m->table_size);
  fatal(xmalloc, &m->tv, VALUE_SIZE(m) * m->table_size);

  *mx = &m->mx;
  m = 0;

finally:
  if(m)
    fatal(multimap_xfree, &m->mx);
coda;
}

API xapi multimap_set(multimap * restrict mx, const void * restrict key, size_t keyl, void * restrict value)
{
  enter;

  multimap_t * m = containerof(mx, multimap_t, mx);

  // grow the table and rehash the entryset
  if(m->size == m->overflow_size)
    fatal(multimap_rehash, m);

  uint32_t h = hash(key, keyl);

  size_t i;
  size_t last;
  bool found = probe(m, h, key, keyl, &i);
  multimap_key * k = m->tk[i];
  bool contiguous = true;
  if(!found)
  {
    if(k == 0 || keyl >= k->a)
    {
      fatal(xrealloc, &m->tk[i], 1, sizeof(multimap_key) + keyl + 1, (k ? k->a : 0));
      k = m->tk[i];
      k->a = keyl + 1;
    }

    memcpy(k->p, key, keyl);
    k->p[keyl] = 0;
    k->l = keyl;
    k->attr = MULTIMAP_CONTIGUOUS;
    k->n = 0;
    last = i;
  }
  else
  {
    // seek from the tail
    size_t next = k->last + 1;
    next &= m->lm;
    if(next != (k->last + 1))
      contiguous = false;

    while(m->tk[next] && !(m->tk[next]->attr & MAP_KEY_DELETED))
    {
      next++;
      next &= m->lm;
      contiguous = false;
    }

    if(m->tk[next] == 0)
    {
      fatal(xmalloc, &m->tk[next], sizeof(multimap_key));
      m->tk[next]->a = 0;
    }

    m->tk[next]->attr |= MULTIMAP_FIRST;
    m->tk[next]->first = i;

    m->tk[m->tk[i]->last]->attr |= MULTIMAP_NEXT;
    m->tk[m->tk[i]->last]->next = next;
    last = next;
  }

  m->tk[i]->attr |= MULTIMAP_LAST;
  m->tk[i]->last = last;
  m->tk[i]->n++;
  if(!contiguous)
    m->tk[i]->attr &= ~MULTIMAP_CONTIGUOUS;
  m->tk[i]->h = h;
  m->size++;

  // copy the value
  if(value)
    ((void**)m->tv)[m->tk[i]->last] = value;

  finally : coda;
}

API xapi multimap_get(
    const multimap * restrict mx
  , const void * restrict key
  , size_t keyl
  , void * _tmp
  , void * restrict vals
  , size_t * restrict valsl
)
{
  enter;

  struct {
    uint16_t sz;
    char v[];
  } ** tmp = _tmp;

  uint32_t h = hash(key, keyl);

  const multimap_t * m = containerof(mx, multimap_t, mx);

  size_t i;
  if(probe(m, h, key, keyl, &i) == 0)
  {
    *valsl = 0;
  }
  else if(m->tk[i]->attr & MULTIMAP_CONTIGUOUS)
  {
    if(vals)
      *(void**)vals = m->tv + (i * sizeof(void*));
    *valsl = m->tk[i]->n;
  }
  else
  {
    if(tmp && vals)
    {
      size_t oldsz = 0;
      if(*tmp)
        oldsz = (*tmp)->sz;
      fatal(xrealloc, tmp, 1, sizeof(**tmp) + (sizeof(void*) * m->tk[i]->n), oldsz);
      (*tmp)->sz = m->tk[i]->n;

      size_t x = 0;
      while(1)
      {
        (*(void***)tmp)[x++] = VALUE(m, i);

        if(!(m->tk[i]->attr & MULTIMAP_NEXT))
          break;

        i = m->tk[i]->next;
      }

      *(void**)vals = *(void**)tmp;
    }

    *valsl = m->tk[m->tk[i]->first]->n;
  }

  finally : coda;
}

API xapi multimap_xfree(multimap * const restrict mx)
{
  enter;

  multimap_t * m = containerof(mx, multimap_t, mx);

  if(m)   // free-like semantics
  {
    fatal(multimap_recycle, &m->mx);

    int x;
    for(x = 0; x < m->table_size; x++)
      wfree(m->tk[x]);

    wfree(m->tk);
    wfree(m->tv);
  }

  wfree(m);

  finally : coda;
}

API xapi multimap_ixfree(multimap ** const restrict m)
{
  enter;

  fatal(multimap_xfree, *m);
  *m = 0;

  finally : coda;
}

API const void * multimap_table_key(const multimap * restrict mx, size_t x)
{
  multimap_t * m = containerof(mx, multimap_t, mx);
  multimap_key * k = m->tk[x];

  if(!k)
    return 0;

  if(k->attr & MAP_KEY_DELETED)
    return 0;

  if(k->attr & MULTIMAP_FIRST)
    k = m->tk[k->first];

  return k->p;
}

API void * multimap_table_value(const multimap * restrict mx, size_t x)
{
  multimap_t * m = containerof(mx, multimap_t, mx);
  multimap_key * k = m->tk[x];

  if(!k)
    return 0;

  if(k->attr & MAP_KEY_DELETED)
    return 0;

  return VALUE(m, x);
}

API xapi multimap_delete(multimap * const restrict mx, const void * restrict key, size_t keyl)
{
  enter;

  multimap_t * m = containerof(mx, multimap_t, mx);
  uint32_t h = hash(key, keyl);

  size_t i = 0;
  if(probe(m, h, key, keyl, &i))
  {
    size_t * next = &i;
    size_t x;
    do
    {
      x = *next;
      next = 0;
      if(m->tk[x]->attr & MULTIMAP_NEXT)
        next = &m->tk[x]->next;

      if(m->free_value)
        m->free_value(VALUE(m, x));
      else if(m->xfree_value)
        fatal(m->xfree_value, VALUE(m, x));

      m->tk[x]->attr = MAP_KEY_DELETED;
    } while(next);

    m->tk[i]->attr = MAP_KEY_DELETED;
    m->size -= m->tk[i]->n;
  }

  finally : coda;
}

API xapi multimap_recycle(multimap * const restrict mx)
{
  enter;

  multimap_t * m = containerof(mx, multimap_t, mx);

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

API xapi multimap_keys(const multimap * restrict mx, const char *** restrict keys, size_t * restrict keysl)
{
  enter;

  const multimap_t * m = containerof(mx, multimap_t, mx);

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
