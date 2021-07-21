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

#include "map.internal.h"
#include "hashtable.internal.h"

#include "macros.h"
#include "common/hash.h"

//
// map/hashtable callbacks
//

static uint32_t hash_entry(const hashtable_t * ht, void * _entry)
{
  mapping * ent = _entry;
  return ht->hash_fn(0, ent->k, ent->kl);
}

static int compare_entries(const hashtable_t * ht, void * _A, void * _B)
{
  mapping * A = _A;
  mapping * B = _B;
  return ht->cmp_fn(A->k, A->kl, B->k, B->kl);
}

static void delete_entry(const hashtable_t * ht, void * _ent)
{
  const map_t * m = containerof(ht, map_t, ht);
  mapping * ent = _ent;

  if(m->destroy_fn)
    m->destroy_fn(ent->v);
}

static void destroy_entry(const hashtable_t * ht, void * _ent)
{
  mapping * ent = _ent;

  wfree(ent->k);
}

static void store_entry(const hashtable_t * ht, void * _dst, void * _src, bool found)
{
  mapping * dst = _dst;
  mapping * src = _src;

  if(found)
  {
    delete_entry(ht, dst);
  }
  else
  {
    if(dst->k == 0 || src->kl >= dst->ka)
    {
      xrealloc(&dst->k, 1, src->kl + 1, dst->ka);
      dst->ka = src->kl + 1;
    }
    memcpy(dst->k, src->k, src->kl);
    ((char*)dst->k)[src->kl] = 0;
    dst->kl = src->kl;
  }

  dst->v = src->v;
  dst->vl = src->vl;
}

static ht_operations ht_ops = {
    hash_entry : hash_entry
  , compare_entries : compare_entries
  , delete_entry : delete_entry
  , destroy_entry : destroy_entry
  , store_entry : store_entry
};

//
// api
//

void API map_createx(map ** const restrict mx, size_t capacity, void (*free_value)(void*))
{
  map_t * m = 0;
  xmalloc(&m, sizeof(*m));

  hashtable_init(
    &m->ht
    , sizeof(mapping)
    , capacity
    , &ht_ops
    , 0
    , 0
  );

  m->destroy_fn = free_value;

  *mx = &m->mx;
  m = 0;

  if(m)
    map_xfree(&m->mx);
}

void API map_create(map ** restrict m)
{
  map_createx(m, 0, 0);
}

void API map_xfree(map * restrict mx)
{
  map_t * m = containerof(mx, map_t, mx);

  if(m)
    hashtable_xdestroy(&m->ht);

  wfree(m);
}

void API map_ixfree(map ** restrict m)
{
  map_xfree(*m);
  *m = 0;
}

void API map_recycle(map * restrict mx)
{
  map_t * m = containerof(mx, map_t, mx);
  hashtable_recycle(&m->htx);
}

void API map_put(map * restrict mx, const void * k, uint16_t kl, void * v, size_t vl)
{
  map_t * m = containerof(mx, map_t, mx);

  mapping ent = { k : (void*)k, kl : kl, v : v, vl : vl };
  hashtable_put(&m->htx, &ent);
}

void * API map_get(const map * restrict mx, const void * restrict k, uint16_t kl)
{
  map_t * m = containerof(mx, map_t, mx);
  mapping * ent;

  mapping key = { k : (char*)k, kl : kl };
  if((ent = hashtable_get(&m->htx, &key)))
    return ent->v;

  return 0;
}

bool API map_get_mapping(const map * restrict mx, const void * restrict k, uint16_t kl, void * v, size_t * restrict vl)
{
  map_t * m = containerof(mx, map_t, mx);
  mapping * ent;

  mapping key = { k : (char*)k, kl : kl };
  if((ent = hashtable_get(&m->htx, &key)))
  {
    if(v)
      *(void**)v = ent->v;
    if(vl)
      *vl = ent->vl;

    return true;
  }

  return false;
}

void API map_splice(map * restrict dstx, map * restrict srcx)
{
  map_t * dst = containerof(dstx, map_t, mx);
  map_t * src = containerof(srcx, map_t, mx);

  hashtable_splice(&dst->htx, &src->htx);
}

void API map_replicate(map * restrict dstx, map * restrict srcx)
{
  map_t * dst = containerof(dstx, map_t, mx);
  map_t * src = containerof(srcx, map_t, mx);

  hashtable_replicate(&dst->htx, &src->htx);
}

bool API map_equal(map * restrict Ax, map * restrict Bx)
{
  map_t * A = containerof(Ax, map_t, mx);
  map_t * B = containerof(Bx, map_t, mx);

  return hashtable_equal(&A->htx, &B->htx);
}

void API map_delete(map * restrict mx, const void * restrict k, uint16_t kl)
{
  map_t * m = containerof(mx, map_t, mx);

  mapping key = { k : (char*)k, kl : kl };
  hashtable_delete(&m->htx, &key);
}

void API map_keys(const map * restrict mx, void * restrict _keys, uint16_t * restrict keysl)
{
  mapping * mg;
  size_t x;
  size_t i;

  void *** keys = _keys;

  const map_t * m = containerof(mx, map_t, mx);

  xmalloc(keys, sizeof(*keys) * m->size);

  i = 0;
  for(x = 0; x < m->table_size; x++)
  {
    if((mg = hashtable_table_entry(&m->htx, x)))
      (*keys)[i++] = mg->k;
  }
  (*keysl) = m->size;
}

void API map_values(const map * restrict mx, void * restrict _values, size_t * restrict valuesl)
{
  mapping * mg;
  size_t x;
  size_t i;
  void *** values = _values;

  const map_t * m = containerof(mx, map_t, mx);

  xmalloc(values, sizeof(*values) * m->size);

  i = 0;
  for(x = 0; x < m->table_size; x++)
  {
    if((mg = hashtable_table_entry(&m->htx, x)))
      (*values)[i++] = mg->v;
  }

  (*valuesl) = m->size;
}

const void * API map_table_key(const map * restrict mx, size_t x)
{
  const map_t * m = containerof(mx, map_t, mx);

  mapping * ent;
  if((ent = hashtable_table_entry(&m->htx, x)))
    return ent->k;

  return 0;
}

void * API map_table_value(const map * restrict mx, size_t x)
{
  const map_t * m = containerof(mx, map_t, mx);

  mapping * ent;
  if((ent = hashtable_table_entry(&m->htx, x)))
    return ent->v;

  return false;
}

bool API map_table_mapping(const map * restrict mx, size_t x, const void ** restrict k, uint16_t * restrict kl, void * v, size_t * restrict vl)
{
  const map_t * m = containerof(mx, map_t, mx);

  mapping * ent;
  if((ent = hashtable_table_entry(&m->htx, x)))
  {
    if(k)
      *k = ent->k;
    if(kl)
      *kl = ent->kl;
    if(v)
      *(void**)v = ent->v;
    if(vl)
      *vl = ent->vl;

    return true;
  }

  return false;
}
