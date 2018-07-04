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
#include "types.h"

#include "xlinux/xstdlib.h"

#include "internal.h"
#include "hashtable.internal.h"

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
#define MAX_SATURATION    0.45f      /* for 100-sized table, reallocate at 45 keys */

static uint32_t entry_hash(hashtable * ht, uint32_t h, void * ent, size_t sz)
{
  return hash32(h, ent, sz);
}

static int entry_cmp(hashtable * ht, void * a, size_t asz, void * b, size_t bsz)
{
  return memncmp(a, asz, b, bsz);
}

static xapi delete_entry(hashtable * restrict ht, ht_bucket * b)
{
  enter;

  if(ht->free_fn)
    ht->free_fn(ht, b->p);
  else if(ht->xfree_fn)
    fatal(ht->xfree_fn, ht, b->p);

  b->attr |= HT_DELETED;

  finally : coda;
}

static ht_bucket * table_bucket(const hashtable * restrict ht, size_t x)
{
  ht_bucket * b = hashtable_bucket_at(ht, ht->tab, x);
  if(b->attr == HT_OCCUPIED)
    return b;

  return 0;
}

//
// internal
//

void hashtable_set_entry(hashtable * ht, ht_bucket * restrict b, uint32_t h, void * entry)
{
  memcpy(b->p, entry, ht->esz);
  b->h = h;
  b->attr = HT_OCCUPIED;
  ht->count++;
  ht->hash += h;
}

ht_bucket * hashtable_bucket_at(const hashtable * restrict ht, ht_bucket * restrict tab, size_t x)
{
  return (ht_bucket*)(((char*)tab) + ((sizeof(*tab) + ht->esz) * x));
}

int hashtable_probe(hashtable * const restrict ht, uint32_t h, void * restrict entry, size_t * restrict i)
{
  size_t li;
  if(!i)
    i = &li;

  *i = h & ht->lm;

  size_t ij = 0;
  size_t * ijp = &ij;

  while(1)
  {
    ht_bucket * b = hashtable_bucket_at(ht, ht->tab, *i);
    if(b->attr & HT_OCCUPIED)
    {
      if(b->attr & HT_DELETED)
      {
        if(ijp)
          *ijp = *i;

        ijp = 0;
      }
      else if(ht->cmp_fn(ht, entry, ht->esz, b->p, ht->esz) == 0)
      {
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
    (*i) &= ht->lm;
  }
}

xapi hashtable_init(
    hashtable * restrict ht
  , size_t esz
  , size_t capacity
  , ht_hash_fn hash_fn
  , ht_cmp_fn cmp_fn
  , ht_free_fn free_fn
  , ht_xfree_fn xfree_fn
)
{
  enter;

  ht->esz = esz;

  // compute initial table size for 100 keys @ given saturation
  ht->table_size = (capacity ?: DEFAULT_CAPACITY) * (1 / MAX_SATURATION);

  // round up to the next highest power of 2
  ht->table_size = roundup2(ht->table_size);
  ht->overflow_count = (size_t)(ht->table_size * MAX_SATURATION);
  ht->lm = ht->table_size - 1;

  ht->free_fn = free_fn;
  ht->xfree_fn = xfree_fn;
  if(!(ht->hash_fn = hash_fn))
    ht->hash_fn = entry_hash;
  if(!(ht->cmp_fn = cmp_fn))
    ht->cmp_fn = entry_cmp;

  fatal(xmalloc, &ht->tab, ht->table_size * (sizeof(*ht->tab) + ht->esz));

  finally : coda;
}

xapi hashtable_grow(hashtable * restrict ht)
{
  enter;

  ht_bucket * tab = 0;
  size_t tabl = 0;
  int x;

  // new table
  void * tmp = ht->tab;
  fatal(xmalloc, &ht->tab, (ht->table_size << 2) * (sizeof(*ht->tab) + ht->esz));
  tab = tmp;
  tabl = ht->table_size;

  ht->table_size <<= 2;
  ht->overflow_count = (size_t)(ht->table_size * MAX_SATURATION);
  ht->lm = ht->table_size - 1;

  // reseat entries
  for(x = 0; x < tabl; x++)
  {
    ht_bucket * b = hashtable_bucket_at(ht, tab, x);

    size_t j = b->h & ht->lm;
    while(hashtable_bucket_at(ht, ht->tab, j)->attr)
    {
      j++;
      j &= ht->lm;
    }

    ht_bucket * nb = hashtable_bucket_at(ht, ht->tab, j);
    memcpy(nb, b, sizeof(*b) + ht->esz);
  }

finally:
  wfree(tab);
coda;
}

//
// API
//

API xapi hashtable_create(hashtable ** restrict ht, size_t esz)
{
  enter;

  fatal(hashtable_createx, ht, esz, 0, 0, 0, 0, 0);

  finally : coda;
}

API xapi hashtable_createx(
    hashtable ** restrict ht
  , size_t esz
  , size_t capacity
  , ht_hash_fn hash_fn
  , ht_cmp_fn cmp_fn
  , ht_free_fn free_fn
  , ht_xfree_fn xfree_fn
)
{
  enter;

  fatal(xmalloc, ht, sizeof(**ht));
  fatal(hashtable_init, *ht, esz, capacity, hash_fn, cmp_fn, free_fn, xfree_fn);

  finally : coda;
}

API xapi hashtable_xdestroy(hashtable * restrict ht)
{
  enter;

  fatal(hashtable_recycle, ht);
  wfree(ht->tab);

  finally : coda;
}

API xapi hashtable_xfree(hashtable * restrict ht)
{
  enter;

  if(ht)
  {
    fatal(hashtable_xdestroy, ht);
  }
  wfree(ht);

  finally : coda;
}

API xapi hashtable_ixfree(hashtable ** restrict ht)
{
  enter;

  hashtable_xfree(*ht);
  *ht = 0;

  finally : coda;
}

API xapi hashtable_put(hashtable * restrict ht, void * const restrict entry)
{
  enter;

  ht_bucket * b;
  size_t i;
  uint32_t h;

  h = ht->hash_fn(ht, 0, entry, ht->esz);
  int r = hashtable_probe(ht, h, entry, &i);
  if(r == 0)
  {
    // found
  }
  else if(r == 1)
  {
    // not found, found a deleted slot
  }
  else if(ht->count == ht->overflow_count)
  {
    fatal(hashtable_grow, ht);
    hashtable_probe(ht, h, entry, &i);
  }

  if(r)
  {
    b = hashtable_bucket_at(ht, ht->tab, i);
    hashtable_set_entry(ht, b, h, entry);
  }

  finally : coda;
}

API bool hashtable_contains(hashtable * restrict ht, void * const restrict entry)
{
  uint32_t h = ht->hash_fn(ht, 0, entry, ht->esz);
  return hashtable_probe(ht, h, entry, 0) == 0;
}

API xapi hashtable_delete(hashtable * restrict ht, void * restrict entry)
{
  enter;

  size_t i;
  uint32_t h;
  ht_bucket * b;

  h = ht->hash_fn(ht, 0, entry, ht->esz);
  if(hashtable_probe(ht, h, entry, &i) == 0)
  {
    b = hashtable_bucket_at(ht, ht->tab, i);
    fatal(delete_entry, ht, b);
    ht->count--;
    ht->hash -= b->h;
  }

  finally : coda;
}

API bool hashtable_equal(hashtable * const A, hashtable * const B)
{
  ht_bucket * b;

  if(A->count != B->count)
    return false;

  if(A->hash != B->hash)
    return false;

  size_t x;
  for(x = 0; x < A->table_size; x++)
  {
    if((b = table_bucket(A, x)))
    {
      if(hashtable_probe(B, b->h, b->p, 0) != 0)
        return false;
    }
  }

  return true;
}

API xapi hashtable_recycle(hashtable * const restrict ht)
{
  enter;

  int x;

  if(ht->count != 0)
  {
    for(x = 0; x < ht->table_size; x++)
    {
      ht_bucket * b = hashtable_bucket_at(ht, ht->tab, x);
      if(b->attr == HT_OCCUPIED)
        fatal(delete_entry, ht, b);
    }

    ht->count = 0;
    ht->hash = 0;
  }

  finally : coda;
}

API xapi hashtable_entries(const hashtable * restrict ht, void * restrict _entries, size_t * restrict entriesl)
{
  enter;

  size_t x;
  size_t i;
  void *** entries = _entries;

  fatal(xmalloc, entries, sizeof(*entries) * ht->count);

  i = 0;
  for(x = 0; x < ht->table_size; x++)
  {
    ht_bucket * b = hashtable_bucket_at(ht, ht->tab, x);
    if(b->attr == HT_OCCUPIED)
      (*entries)[i++] = b->p;
  }
  (*entriesl) = ht->count;

  finally : coda;
}

API void * hashtable_table_entry(const hashtable * restrict ht, size_t x)
{
  ht_bucket * b = table_bucket(ht, x);
  if(b)
    return b->p;

  return NULL;
}
