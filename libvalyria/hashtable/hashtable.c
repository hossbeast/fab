/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "types.h"

#include "xlinux/xstdlib.h"

#include "hashtable.h"
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

#define OCCUPIED 0x01   /* this bucket contains an entry */
#define DELETED  0x02   /* this bucket has been deleted */

static void bucket_delete(hashtable * restrict ht, hashtable_bucket * b)
{
  if(b->attr & DELETED) {
    return;
  }

  b->attr |= DELETED;

  if(!ht->delete_fn) {
    return;
  }

  ht->delete_fn(b->p);
}

static uint32_t hash_index(uint32_t h)
{
  size_t x;

  if(fault(HASH_INDEX))
  {
    for(x = 0; x < fault_state.hash_index.len; x++)
    {
      typeof(*fault_state.hash_index.hashes) faultentry = fault_state.hash_index.hashes[x];
      if(h == faultentry.hash) {
        return faultentry.index;
      }
    }
  }

  return h;
}

#if CONFIG_DEVEL
static uint32_t hash_entry(uint32_t h, const void * entry)
{
  int x;

  if(fault(KEY_HASH))
  {
    for(x = 0; x < fault_state.key_hash.len; x++)
    {
      typeof(*fault_state.key_hash.keys) faultentry = fault_state.key_hash.keys[x];
      if(entry == faultentry.key) {
        return faultentry.hash;
      }
    }
  }

  return hash32(h, &entry, sizeof(entry));
}
#else
static uint32_t hash_entry(uint32_t h, const void * entry)
{
  return hash32(h, &entry, sizeof(entry));
}
#endif

static int cmp_entries(const void *A, const void *B)
{
  return INTCMP(A, B);
}

static hashtable_bucket * bucket_get(const hashtable * restrict ht, size_t x)
{
  hashtable_bucket * b;

  b = &ht->tab[x & ht->lm];
  if(!(b->attr == OCCUPIED)) {
    return 0;
  }

  return b;
}

/* reallocate the table to the next-larger power-of-two */
static void rehash(hashtable * restrict ht)
{
  hashtable_bucket * tab = 0;
  size_t tabl = 0;
  int x;
  hashtable_bucket *b;
  hashtable_bucket *nb;
  size_t j;

  // new table
  tab = ht->tab;
  xmalloc(&ht->tab, (ht->table_size << 2) * sizeof(*ht->tab));
  tabl = ht->table_size;

  ht->table_size <<= 1;
  ht->overflow_size = (size_t)(ht->table_size * MAX_SATURATION);
  ht->lm = ht->table_size - 1;

  /* reseat occupied entries in the new allocation and discard deleted entries
   * thus, after this operation, dirty size == size */
  for(x = 0; x < tabl; x++)
  {
    b = &tab[x];
    if(!(b->attr & OCCUPIED)) {
      continue;
    }

    if(b->attr & DELETED) {
      continue;
    }

    j = b->hash;
    nb = &ht->tab[j & ht->lm];
    while(nb->attr & OCCUPIED)
    {
      j++;
      nb = &ht->tab[j & ht->lm];
    }

    nb->attr = OCCUPIED;
    nb->hash = b->hash;
    nb->p = b->p; // memcpy(nb->p, b->p, ht->esz);
  }

  ht->dirty_size = ht->size;

  free(tab);
}

/*
 * search for an entry by linear probe
 *
 * ht    - hashtable
 * h     - entry hash
 * entry - pointer to the entry
 * [i]   - (returns) table index
 *
 * returns
 *   0 : found
 *   1 : not found, and the first deleted slot in the probe sequence is returned in i
 *  -1 : not found
 */
static int __attribute__((nonnull)) probe(const hashtable * restrict ht, hashtable_search_context * restrict ctx)
{
  size_t ij;
  size_t *ijp;
  hashtable_bucket *b;

  ctx->index = hash_index(ctx->hash);
  ij = 0;
  ijp = &ij;

  while(1)
  {
    b = &ht->tab[ctx->index & ht->lm];
    RUNTIME_ASSERT((b->attr & ~(OCCUPIED | DELETED)) == 0);
    if(b->attr & OCCUPIED)
    {
      if(b->attr & DELETED)
      {
        if(ijp) {
          *ijp = ctx->index;
        }

        ijp = 0;
      }
      else if(ctx->cmp_fn(b->p, ctx->entry) == 0)
      {
        return 0;
      }
    }
    else if(ijp == 0)
    {
      ctx->index = ij;
      return 1;
    }
    else
    {
      return -1;
    }

    ctx->index++;
  }
}

//
// internal
//

void hashtable_initx(
    hashtable * restrict ht
  , size_t capacity
  , uint32_t (*hash_fn)(uint32_t h, const void * entry)
  , int (*cmp_fn)(const void * A, const void * B)
  , void (*delete_fn)(void * entry)
)
{
  memset(ht, 0, sizeof(*ht));

  // compute initial table size for 100 keys @ given saturation
  ht->table_size = (capacity ?: DEFAULT_CAPACITY) * (1 / MAX_SATURATION);

  // round up to the next highest power of 2
  ht->table_size = roundup2(ht->table_size);
  ht->lm = ht->table_size - 1;

  ht->overflow_size = (size_t)(ht->table_size * MAX_SATURATION);

  xmalloc(&ht->tab, ht->table_size * sizeof(*ht->tab));

  if(hash_fn)
  {
    ht->hash_fn = hash_fn;
  }
  else
  {
    ht->hash_fn = hash_entry;
  }

  ht->cmp_fn = cmp_fn ?: cmp_entries;
  ht->delete_fn = delete_fn;
}

void hashtable_init(hashtable * restrict ht)
{
  hashtable_initx(ht, 0, 0, 0, 0);
}

void hashtable_destroy(hashtable * restrict ht)
{
  hashtable_recycle(ht);
  free(ht->tab);
}

//
// api
//

void hashtable_create(hashtable ** restrict ht)
{
  hashtable_createx(ht, 0, 0, 0, 0);
}

void hashtable_createx(
    hashtable ** restrict htp
  , size_t capacity
  , uint32_t (*hash_fn)(uint32_t h, const void * entry)
  , int (*cmp_fn)(const void * A, const void * B)
  , void (*delete_fn)(void * entry)
)
{
  hashtable * ht = 0;

  xmalloc(&ht, sizeof(*ht));
  hashtable_initx(ht, capacity, hash_fn, cmp_fn, delete_fn);

  *htp = ht;
}

void hashtable_free(hashtable * restrict ht)
{
  if(ht) {
    hashtable_destroy(ht);
  }

  free(ht);
}

void hashtable_ifree(hashtable ** restrict htp)
{
  hashtable_free(*htp);
  *htp = 0;
}

bool hashtable_search(const hashtable * restrict ht, hashtable_search_context * restrict ctx)
{
  if(ctx->hash_fn == 0) {
    ctx->hash_fn = ht->hash_fn;
  }
  if(ctx->cmp_fn == 0) {
    ctx->cmp_fn = ht->cmp_fn;
  }

  ctx->hash = ctx->hash_fn(0, ctx->entry);
  ctx->probe_result = probe(ht, ctx);
  return ctx->probe_result == 0;
}

void * hashtable_fetch(const hashtable * restrict ht, hashtable_search_context * restrict ctx)
{
  hashtable_bucket *b;

  b = &ht->tab[ctx->index & ht->lm];
  return b->p;
}

void * hashtable_set(hashtable * restrict ht, hashtable_search_context * restrict ctx)
{
  hashtable_bucket *b;

  if(ctx->probe_result == -1 && ht->dirty_size == ht->overflow_size)
  {
    rehash(ht);
    probe(ht, ctx);//ctx->hash, ctx->entry, &ctx->index);
  }

  b = &ht->tab[ctx->index & ht->lm];
  if(ctx->probe_result == 0 && ht->delete_fn)
  {
    ht->delete_fn(b->p);
  }
  b->p = ctx->entry;

  if(ctx->probe_result)
  {
    b->hash = ctx->hash;
    if(b->attr == 0) {
      ht->dirty_size++;
    }
    b->attr = OCCUPIED;
    ht->size++;
    ht->hash += ctx->hash;
  }

  return b->p;
}

void hashtable_remove(hashtable * restrict ht, hashtable_search_context * restrict ctx)
{
  hashtable_bucket *b;

  b = &ht->tab[ctx->index & ht->lm];
  bucket_delete(ht, b);
  ht->size--;
  ht->hash -= b->hash;
}

bool hashtable_contains(const hashtable * restrict ht, const void * entry)
{
  hashtable_search_context ctx;

  ctx = (typeof(ctx)) {
    entry : (void*)entry
  };

  return hashtable_search(ht, &ctx);
}

void * hashtable_get(const hashtable * restrict ht, const void * entry)
{
  hashtable_search_context ctx;

  ctx = (typeof(ctx)) {
    entry : (void*)entry
  };

  if(hashtable_search(ht, &ctx)) {
    return hashtable_fetch(ht, &ctx);
  }
  return 0;
}

void * hashtable_put(hashtable * restrict ht, void * entry)
{
  hashtable_search_context ctx;

  ctx = (typeof(ctx)) {
    entry : entry
  };
  hashtable_search(ht, &ctx);
  return hashtable_set(ht, &ctx);
}

bool hashtable_delete(hashtable * restrict ht, void * entry)
{
  hashtable_search_context ctx;

  ctx = (typeof(ctx)) {
    entry : entry
  };
  if(!hashtable_search(ht, &ctx)) {
    return false;
  }

  hashtable_remove(ht, &ctx);
  return true;
}

void * hashtable_table_entry(const hashtable * restrict ht, size_t x)
{
  hashtable_bucket *b;

  if((b = bucket_get(ht, x))) {
    return b->p;
  }

  return 0;
}

void hashtable_splice(hashtable * dst, hashtable * src)
{
  hashtable_bucket * b;
  size_t x;

  for(x = 0; x < src->table_size; x++)
  {
    if((b = bucket_get(src, x)))
    {
      hashtable_put(dst, b->p);

      // ownership transfer
      b->attr |= DELETED;
    }
  }
  src->size = 0;
  src->hash = 0;
}

void hashtable_replicate(hashtable * dst, hashtable * src)
{
  hashtable_bucket * b;
  size_t x;

  for(x = 0; x < src->table_size; x++)
  {
    if((b = bucket_get(src, x)))
    {
      hashtable_put(dst, b->p);
    }
  }
}

bool hashtable_equal(hashtable * A, hashtable * B)
{
  hashtable_bucket * b;
  size_t x;
  hashtable_search_context ctx;

  if(A->size != B->size) {
    return false;
  }

  if(A->hash != B->hash) {
    return false;
  }

  ctx.hash_fn = B->hash_fn;
  ctx.cmp_fn = B->cmp_fn;

  for(x = 0; x < A->table_size; x++)
  {
    if(!(b = bucket_get(A, x))) {
      continue;
    }

    ctx.hash = b->hash;
    ctx.entry = b->p;
    if(probe(B, &ctx) != 0) {
      return false;
    }
  }

  return true;
}

void hashtable_recycle(hashtable * restrict ht)
{
  hashtable_bucket *b;
  size_t x;

  if(ht->size == 0) {
    return;
  }

  for(x = 0; x < ht->table_size; x++)
  {
    b = &ht->tab[x];
    if(b->attr == OCCUPIED) {
      bucket_delete(ht, b);
    }
  }

  ht->size = 0;
  ht->hash = 0;
}

void hashtable_enumerate(hashtable * restrict ht, void * restrict _entries, size_t * restrict entriesl)
{
  void *** entries;
  hashtable_bucket *b;
  size_t x;
  size_t i;

  entries = _entries;
  xmalloc(entries, sizeof(**entries) * ht->size);

  i = 0;
  for(x = 0; x < ht->table_size; x++)
  {
    if(!(b = bucket_get(ht, x))) {
      continue;
    }

    (*entries)[i++] = b->p;
  }

  *entriesl = i;
}
