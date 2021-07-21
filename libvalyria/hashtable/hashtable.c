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

#include "types.h"

#include "xlinux/xstdlib.h"

#include "hashtable.internal.h"
#include "faults.internal.h"

#include "macros.h"
#include "common/hash.h"

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

static uint32_t hash_entry(const hashtable_t * ht, void * ent)
{
  return ht->hash_fn(0, ent, ht->esz);
}

static int compare_entries(const hashtable_t * ht, void * A, void * B)
{
  return ht->cmp_fn(A, ht->esz, B, ht->esz);
}

static void delete_entry(const hashtable_t * ht, void * ent)
{
  if(ht->destroy_fn)
    ht->destroy_fn(ent);
}

static void destroy_entry(const hashtable_t * ht, void * ent)
{
}

static void store_entry(const hashtable_t * ht, void * dst, void * entry, bool found)
{
  if(found)
    ht->ops->delete_entry(ht, entry);

  memcpy(dst, entry, ht->esz);
}

static ht_operations ht_cbs = {
    hash_entry : hash_entry
  , compare_entries : compare_entries
  , delete_entry : delete_entry
  , destroy_entry : destroy_entry
  , store_entry: store_entry
};

static void bucket_delete(hashtable_t * restrict ht, ht_bucket * b)
{
  ht->ops->delete_entry(ht, b->p);
  b->attr |= HT_DELETED;
}

static void bucket_destroy(hashtable_t * restrict ht, ht_bucket * b)
{
  if((b->attr & HT_DELETED) == 0)
    ht->ops->delete_entry(ht, b->p);

  ht->ops->destroy_entry(ht, b->p);
}

static uint32_t hash_index(uint32_t h, uint32_t lm)
{
  if(fault(HASH_INDEX))
  {
    int x;
    for(x = 0; x < fault_state.hash_index.len; x++)
    {
      typeof(*fault_state.hash_index.hashes) faultentry = fault_state.hash_index.hashes[x];
      if(h == faultentry.hash)
        return faultentry.index;
    }
  }

  return h & lm;
}

#if DEVEL
static uint32_t devel_hash_fn(uint32_t h, const void * const restrict v, size_t l)
{
  if(fault(KEY_HASH))
  {
    int x;
    for(x = 0; x < fault_state.key_hash.len; x++)
    {
      typeof(*fault_state.key_hash.keys) faultentry = fault_state.key_hash.keys[x];
      if(memncmp(v, l, faultentry.key, faultentry.len) == 0)
        return faultentry.hash;
    }
  }

  return hash32(h, v, l);
}
#endif

//
// internal
//

ht_bucket * hashtable_table_bucket(const hashtable_t * restrict ht, size_t x)
{
  ht_bucket * b = hashtable_bucket_at(ht, ht->tab, x);
  if(b->attr == HT_OCCUPIED)
    return b;

  return 0;
}

ht_bucket * hashtable_bucket_at(const hashtable_t * restrict ht, ht_bucket * restrict tab, size_t x)
{
  return (ht_bucket*)(((char*)tab) + ((sizeof(*tab) + ht->essz) * x));
}

int hashtable_probe(const hashtable_t * const restrict ht, uint32_t h, void * ent, size_t ent_sz, hashtable_cmp cmp, size_t * restrict i)
{
  size_t li;
  if(!i)
    i = &li;

  *i = hash_index(h, ht->lm);

  size_t ij = 0;
  size_t * ijp = &ij;

  while(1)
  {
    ht_bucket * b = hashtable_bucket_at(ht, ht->tab, *i);
    RUNTIME_ASSERT((b->attr & ~(HT_OCCUPIED | HT_DELETED)) == 0);
    if(b->attr & HT_OCCUPIED)
    {
      if(b->attr & HT_DELETED)
      {
        if(ijp)
          *ijp = *i;

        ijp = 0;
      }
      else if(cmp && cmp(b->p, ent, ent_sz) == 0)
      {
        return 0;
      }
      else if(!cmp && ht->ops->compare_entries(ht, ent, b->p) == 0)
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

void hashtable_grow(hashtable_t * restrict ht)
{
  ht_bucket * tab = 0;
  size_t tabl = 0;
  int x;
  ht_bucket *b;
  ht_bucket *nb;
  size_t j;


  // new table
  tab = ht->tab;
  xmalloc(&ht->tab, (ht->table_size << 2) * (sizeof(*ht->tab) + ht->essz));
  tabl = ht->table_size;

  ht->table_size <<= 2;
  ht->overflow_size = (size_t)(ht->table_size * MAX_SATURATION);
  ht->lm = ht->table_size - 1;

  /* reseat occupied entries in the new allocation and discard deleted entries
   * thus, after this operation, dirty size == size
   */
  for(x = 0; x < tabl; x++)
  {
    b = hashtable_bucket_at(ht, tab, x);

    if(!(b->attr & HT_OCCUPIED))
      continue;

    if(b->attr & HT_DELETED)
    {
      ht->ops->destroy_entry(ht, b->p);
      continue;
    }

    j = b->h & ht->lm;
    nb = hashtable_bucket_at(ht, ht->tab, j);
    while(nb->attr & HT_OCCUPIED)
    {
      j++;
      j &= ht->lm;
      nb = hashtable_bucket_at(ht, ht->tab, j);
    }

    nb->attr = HT_OCCUPIED;
    nb->h = b->h;
    memcpy(nb->p, b->p, ht->esz);
  }

  ht->dirty_size = ht->size;

  wfree(tab);
}

void hashtable_init(
    hashtable_t * restrict ht
  , size_t esz
  , size_t capacity
  , ht_operations * restrict ops
  , uint32_t (*hash_fn)(uint32_t h, const void * entry, size_t sz)
  , int (*cmp_fn)(const void * A, size_t Asz, const void * B, size_t Bsz)
)
{
  // compute initial table size for 100 keys @ given saturation
  ht->table_size = (capacity ?: DEFAULT_CAPACITY) * (1 / MAX_SATURATION);

  // round up to the next highest power of 2
  ht->table_size = roundup2(ht->table_size);

  ht->overflow_size = (size_t)(ht->table_size * MAX_SATURATION);
  ht->lm = ht->table_size - 1;

  ht->ops = ops;
  ht->esz = esz;
  ht->essz = roundup(ht->esz, sizeof(ht_bucket));

  xmalloc(&ht->tab, ht->table_size * (sizeof(*ht->tab) + ht->essz));

  // default hash fn
  if(hash_fn)
    ht->hash_fn = hash_fn;
  else
  {
#if DEVEL
    ht->hash_fn = devel_hash_fn;
#else
    ht->hash_fn = hash32;
#endif
  }

  // default cmp fn
  ht->cmp_fn = cmp_fn ?: memncmp;
}

void hashtable_xdestroy(hashtable_t * restrict ht)
{
  int x;
  for(x = 0; x < ht->table_size; x++)
  {
    ht_bucket * b = hashtable_bucket_at(ht, ht->tab, x);
    if(b->attr & HT_OCCUPIED)
      bucket_destroy(ht, b);
  }

  wfree(ht->tab);
}

void hashtable_store(hashtable_t * restrict ht, void * entry, ht_bucket ** bp)
{
  ht_bucket *b;
  size_t i;
  uint32_t h;
  int r;

  h = ht->ops->hash_entry(ht, entry);
  r = hashtable_probe(ht, h, entry, 0, 0, &i);
  if(r == -1 && ht->dirty_size == ht->overflow_size)
  {
    hashtable_grow(ht);
    hashtable_probe(ht, h, entry, 0, 0, &i);
  }

  b = hashtable_bucket_at(ht, ht->tab, i);
  ht->ops->store_entry(ht, b->p, entry, r == 0);

  // update accounting on new entry
  if(r)
  {
    b->h = h;
    if(b->attr == 0)
      ht->dirty_size++;
    b->attr = HT_OCCUPIED;
    ht->size++;
    ht->hash += h;
  }

  *bp = b;
}

//
// api
//

void API hashtable_create(hashtable ** restrict ht, size_t esz)
{
  hashtable_createx(ht, esz, 0, 0, 0, 0);
}

void API hashtable_createx(
    hashtable ** restrict htx
  , size_t esz
  , size_t capacity
  , uint32_t (*hash_fn)(uint32_t h, const void * entry, size_t sz)
  , int (*cmp_fn)(const void * A, size_t Asz, const void * B, size_t Bsz)
  , void (*destroy_fn)(void * entry)
)
{
  hashtable_t * ht = 0;

  xmalloc(&ht, sizeof(*ht));
  hashtable_init(ht, esz, capacity, &ht_cbs, hash_fn, cmp_fn);

  ht->destroy_fn = destroy_fn;

  *htx = &ht->htx;
  ht = 0;

  if(ht)
    hashtable_xfree(&ht->htx);
}

void API hashtable_xfree(hashtable * restrict htx)
{
  hashtable_t * ht = containerof(htx, hashtable_t, htx);

  if(ht)
    hashtable_xdestroy(ht);

  wfree(ht);
}

void API hashtable_ixfree(hashtable ** restrict htx)
{
  hashtable_xfree(*htx);
  *htx = 0;
}

void API hashtable_put(hashtable * restrict htx, void * entry)
{
  ht_bucket *b;
  hashtable_t * ht = containerof(htx, hashtable_t, htx);

  hashtable_store(ht, entry, &b);
}

void * API hashtable_get(const hashtable * restrict htx, void * entry)
{
  size_t i;
  uint32_t h;
  ht_bucket * b;

  hashtable_t * ht = containerof(htx, hashtable_t, htx);

  h = ht->ops->hash_entry(ht, entry);
  if(hashtable_probe(ht, h, entry, 0, 0, &i) == 0)
  {
    b = hashtable_bucket_at(ht, ht->tab, i);
    return b->p;
  }

  return 0;
}

void * API hashtable_search(const hashtable * restrict htx, void * key, size_t key_len, hashtable_hash hash, hashtable_cmp cmp)
{
  size_t i;
  uint32_t h;
  ht_bucket * b;

  hashtable_t * ht = containerof(htx, hashtable_t, htx);

  h = hash(0, key, key_len);
  if(hashtable_probe(ht, h, key, key_len, cmp, &i) == 0)
  {
    b = hashtable_bucket_at(ht, ht->tab, i);
    return b->p;
  }

  return 0;
}

void API hashtable_delete(hashtable * restrict htx, void * restrict entry)
{
  size_t i;
  uint32_t h;
  ht_bucket * b;

  hashtable_t * ht = containerof(htx, hashtable_t, htx);

  h = ht->ops->hash_entry(ht, entry);
  if(hashtable_probe(ht, h, entry, 0, 0, &i) == 0)
  {
    b = hashtable_bucket_at(ht, ht->tab, i);
    bucket_delete(ht, b);
    ht->size--;
    ht->hash -= b->h;
  }
}

void API hashtable_splice(hashtable * dstx, hashtable * srcx)
{
  ht_bucket * b;

  hashtable_t * dst = containerof(dstx, hashtable_t, htx);
  hashtable_t * src = containerof(srcx, hashtable_t, htx);

  int x;
  for(x = 0; x < src->table_size; x++)
  {
    if((b = hashtable_table_bucket(src, x)))
    {
      hashtable_put(&dst->htx, b->p);

      // ownership transfer
      b->attr |= HT_DELETED;
    }
  }
  src->size = 0;
  src->hash = 0;
}

void API hashtable_replicate(hashtable * dstx, hashtable * srcx)
{
  ht_bucket * b;

  hashtable_t * dst = containerof(dstx, hashtable_t, htx);
  hashtable_t * src = containerof(srcx, hashtable_t, htx);

  int x;
  for(x = 0; x < src->table_size; x++)
  {
    if((b = hashtable_table_bucket(src, x)))
    {
      hashtable_put(&dst->htx, b->p);
    }
  }
}

bool API hashtable_equal(hashtable * const Ax, hashtable * const Bx)
{
  ht_bucket * b;

  hashtable_t * A = containerof(Ax, hashtable_t, htx);
  hashtable_t * B = containerof(Bx, hashtable_t, htx);

  if(A->size != B->size)
    return false;

  if(A->hash != B->hash)
    return false;

  size_t x;
  for(x = 0; x < A->table_size; x++)
  {
    if((b = hashtable_table_bucket(A, x)))
    {
      if(hashtable_probe(B, b->h, b->p, 0, 0, 0) != 0)
        return false;
    }
  }

  return true;
}

void API hashtable_recycle(hashtable * const restrict htx)
{
  hashtable_t * ht = containerof(htx, hashtable_t, htx);

  if(ht->size != 0)
  {
    int x;
    for(x = 0; x < ht->table_size; x++)
    {
      ht_bucket * b = hashtable_bucket_at(ht, ht->tab, x);
      if(b->attr == HT_OCCUPIED)
        bucket_delete(ht, b);
    }

    ht->size = 0;
    ht->hash = 0;
  }
}

void API hashtable_entries(const hashtable * restrict htx, void * restrict _entries, size_t * restrict entriesl)
{
  size_t x;
  size_t i;
  void *** entries = _entries;

  const hashtable_t * ht = containerof(htx, hashtable_t, htx);

  xmalloc(entries, sizeof(**entries) * ht->size);

  i = 0;
  for(x = 0; x < ht->table_size; x++)
  {
    void * ent;
    if((ent = hashtable_table_entry(&ht->htx, x)))
      (*entries)[i++] = ent;
  }
  (*entriesl) = ht->size;
}

void * API hashtable_table_entry(const hashtable * restrict htx, size_t x)
{
  const hashtable_t * ht = containerof(htx, hashtable_t, htx);

  ht_bucket * b = hashtable_table_bucket(ht, x);
  if(b)
    return b->p;

  return NULL;
}

void API hashtable_table_delete(hashtable * restrict htx, size_t x)
{
  hashtable_t * ht = containerof(htx, hashtable_t, htx);
  ht_bucket * b = hashtable_table_bucket(ht, x);
  bucket_delete(ht, b);
  ht->size--;
  ht->hash -= b->h;
}
