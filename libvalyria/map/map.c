/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "types.h"

#include "xlinux/xstdlib.h"
#include "common/hash.h"

#include "map.h"
#include "macros.h"

/*
 * Default capacity, the minimum number of entries which can be put without rehashing
 */
#define DEFAULT_CAPACITY  100

/*
 * Maximum proportion of used entries to allocated table size
 *
 * valid values : 0 < saturation <= 1
 *
 * smaller values trade space for time because sparser tables mean shorter probe sequences
 */
#define MAX_SATURATION   0.45f  /* for 100-sized table, reallocate at 45 keys */

#define OCCUPIED 0x01   /* this bucket contains an entry */
#define DELETED  0x02   /* this bucket has been deleted */

/* delete the stored value - retain the key allocation */
static void entry_delete(map * restrict m, map_entry * restrict b)
{
  if(b->attr & DELETED) {
    return;
  }
  b->attr |= DELETED;

  if(m->delete_fn) {
    m->delete_fn(b->val);
  }
}

static void entry_destroy(map * restrict m, map_entry * restrict b)
{
  entry_delete(m, b);
  free(b->key);
}

static map_entry * entry_get(const map * restrict m, size_t x)
{
  map_entry * b;

  b = &m->tab[x & m->lm];
  if(b->attr == OCCUPIED) {
    return b;
  }

  return 0;
}

/* reallocate the table to the next-larger power-of-two */
static void table_grow(map * restrict m)
{
  map_entry * tab = 0;
  size_t tabl = 0;
  int x;
  map_entry *b;
  map_entry *nb;
  size_t j;

  // new table
  tab = m->tab;
  xmalloc(&m->tab, (m->table_size << 2) * (sizeof(*m->tab)));
  tabl = m->table_size;

  m->table_size <<= 1;
  m->overflow_size = (size_t)(m->table_size * MAX_SATURATION);
  m->lm = m->table_size - 1;

  /* reseat occupied entries in the new allocation and discard deleted entries
   * thus, after this operation, dirty size == size */
  for(x = 0; x < tabl; x++)
  {
    b = &tab[x];
    if(!(b->attr & OCCUPIED)) {
      continue;
    }

    if(b->attr & DELETED) {
      free(b->key);
      continue;
    }

    j = b->hash - 1;
    do
    {
      j++;
      nb = &m->tab[j & m->lm];
    } while(nb->attr & OCCUPIED);

    /* claims the key allocation */
    *nb = *b;
  }

  m->dirty_size = m->size;
  free(tab);
}

/*
 * search for an entry by linear probe
 *
 * m       - map
 * h       - key hash
 * key     - pointer to the key
 * key_len - key length
 * [i]     - (returns) table index
 *
 * returns
 *   0 : found
 *   1 : not found, and the first deleted slot in the probe sequence is returned in i
 *  -1 : not found
 */
static int __attribute__((nonnull)) probe(
    const map * const restrict m
  , uint32_t h
  , const void * key
  , size_t key_len
  , size_t * restrict i
)
{
  size_t ij;
  size_t *ijp;
  map_entry *b;

  *i = h;
  ij = 0;
  ijp = &ij;

  while(1)
  {
    b = &m->tab[*i & m->lm];
    RUNTIME_ASSERT((b->attr & ~(OCCUPIED | DELETED)) == 0);
    if(b->attr & OCCUPIED)
    {
      if(b->attr & DELETED)
      {
        if(ijp) {
          *ijp = *i;
        }

        ijp = 0;
      }
      else if(memncmp(key, key_len, b->key, b->key_len) == 0)
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
  }
}

//
// api
//

void map_initx(map * restrict m, size_t capacity, void (*delete_fn)(void*))
{
  memset(m, 0, sizeof(*m));

  // compute initial table size for 100 keys @ given saturation
  m->table_size = (capacity ?: DEFAULT_CAPACITY) * (1 / MAX_SATURATION);

  // round up to the next highest power of 2
  m->table_size = roundup2(m->table_size);
  m->lm = m->table_size - 1;

  m->overflow_size = (size_t)(m->table_size * MAX_SATURATION);

  xmalloc(&m->tab, m->table_size * (sizeof(*m->tab)));

  m->delete_fn = delete_fn;
}

void map_init(map * restrict m)
{
  map_initx(m, 0, 0);
}

void map_createx(map ** const restrict mp, size_t capacity, void (*delete_fn)(void*))
{
  map * m;

  xmalloc(&m, sizeof(*m));
  map_initx(m, capacity, delete_fn);

  *mp = m;
}

void map_create(map ** restrict m)
{
  map_createx(m, 0, 0);
}

void map_destroy(map * restrict m)
{
  map_entry *b;
  size_t x;

  for(x = 0; x < m->table_size; x++)
  {
    b = &m->tab[x];
    entry_destroy(m, b);
  }

  free(m->tab);
}

void map_free(map * restrict m)
{
  if(m) {
    map_destroy(m);
  }

  free(m);
}

void map_ifree(map ** restrict m)
{
  map_free(*m);
  *m = 0;
}

void map_recycle(map * restrict m)
{
  map_entry *b;
  size_t x;

  if(m->size == 0) {
    return;
  }

  for(x = 0; x < m->table_size; x++)
  {
    b = &m->tab[x];
    if(b->attr == OCCUPIED) {
      entry_delete(m, b);
    }
  }

  m->size = 0;
  m->hash = 0;
}

bool map_search(const map * restrict m, map_search_context * restrict ctx)
{
  ctx->hash = hash32(0, ctx->key, ctx->key_len);
  ctx->probe_result = probe(m, ctx->hash, ctx->key, ctx->key_len, &ctx->index);
  return ctx->probe_result == 0;
}

void map_store(map * restrict m, map_search_context * restrict ctx, void * val, size_t val_len)
{
  map_entry *b;

  if(ctx->probe_result == -1 && m->dirty_size == m->overflow_size)
  {
    table_grow(m);
    probe(m, ctx->hash, ctx->key, ctx->key_len, &ctx->index);
  }

  b = &m->tab[ctx->index & m->lm];
  if(ctx->probe_result == 0)
  {
    if(m->delete_fn) {
      m->delete_fn(b->val);
    }
  }
  else
  {
    if(b->key == 0 || ctx->key_len >= b->key_a)
    {
      xrealloc(&b->key, 1, ctx->key_len + 1, b->key_a);
      b->key_a = ctx->key_len + 1;
    }
    memcpy(b->key, ctx->key, ctx->key_len);
    ((char*)b->key)[ctx->key_len] = 0;
    b->key_len = ctx->key_len;
  }
  b->val = val;
  b->val_len = val_len;

  if(ctx->probe_result)
  {
    b->hash = ctx->hash;
    if(!b->attr) {
      m->dirty_size++;
    }
    b->attr = OCCUPIED;
    m->size++;
    m->hash += ctx->hash;
  }
}

void map_put(map * restrict m, const void * k, uint16_t kl, void * v, size_t vl)
{
  map_search_context ctx;

  ctx = (typeof(ctx)) {
    key : k,
    key_len : kl,
  };

  map_search(m, &ctx);
  map_store(m, &ctx, v, vl);
}

void * map_get(const map * restrict m, const void * restrict k, uint16_t kl)
{
  map_search_context ctx;
  map_entry *b;

  ctx = (typeof(ctx)) {
      key : k,
      key_len : kl,
  };
  if(map_search(m, &ctx)) {
    b = &m->tab[ctx.index & m->lm];
    return b->val;
  }

  return 0;
}

bool map_get_mapping(const map * restrict m, const void * restrict k, uint16_t kl, void * v, size_t * restrict vl)
{
  map_search_context ctx;
  map_entry *b;

  ctx = (typeof(ctx)) {
      key : k,
      key_len : kl,
  };
  if(map_search(m, &ctx)) {
    b = &m->tab[ctx.index & m->lm];
    *(void**)v = b->val;
    *vl = b->val_len;
    return true;
  }

  return false;
}

void map_splice(map * restrict dst, map * restrict src)
{
  map_entry * b;
  size_t x;

  for(x = 0; x < src->table_size; x++)
  {
    if((b = entry_get(src, x)))
    {
      map_put(dst, b->key, b->key_len, b->val, b->val_len);

      // ownership transfer
      b->attr |= DELETED;
    }
  }
  src->size = 0;
  src->hash = 0;
}

void map_replicate(map * restrict dst, map * restrict src)
{
  map_entry * b;
  size_t x;

  for(x = 0; x < src->table_size; x++)
  {
    if((b = entry_get(src, x)))
    {
      map_put(dst, b->key, b->key_len, b->val, b->val_len);
    }
  }
}

bool map_equal(map * restrict A, map * restrict B)
{
  map_entry * b;
  size_t x;
  size_t i;

  if(A->size != B->size) {
    return false;
  }

  if(A->hash != B->hash) {
    return false;
  }

  for(x = 0; x < A->table_size; x++)
  {
    if((b = entry_get(A, x)))
    {
      if(probe(B, b->hash, b->key, b->key_len, &i) != 0) {
        return false;
      }
    }
  }

  return true;
}

void map_delete(map * restrict m, const void * restrict k, uint16_t kl)
{
  map_search_context ctx;
  map_entry *b;

  ctx = (typeof(ctx)) {
    key : k,
    key_len : kl
  };
  if(!map_search(m, &ctx)) {
    return;
  }

  b = &m->tab[ctx.index & m->lm];
  entry_delete(m, b);
  m->size--;
  m->hash -= b->hash;
}

void * map_table_key(const map * restrict m, size_t x)
{
  map_entry * b;

  if((b = entry_get(m, x))) {
    return b->key;
  }

  return 0;
}

void * map_table_value(const map * restrict m, size_t x)
{
  map_entry * b;

  if((b = entry_get(m, x))) {
    return b->val;
  }

  return 0;
}

bool map_table_mapping(const map * restrict m, size_t x, void ** restrict k, uint16_t * restrict kl, void * v, size_t * restrict vl)
{
  map_entry * b;
  if((b = entry_get(m, x)))
  {
    if(k) {
      *k = b->key;
    }
    if(kl) {
      *kl = b->key_len;
    }
    if(v) {
      *(void**)v = b->val;
    }
    if(vl) {
      *vl = b->val_len;
    }

    return true;
  }

  return false;
}
