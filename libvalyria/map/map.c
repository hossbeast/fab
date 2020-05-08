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
#include "map.internal.h"
#include "hashtable.internal.h"

#include "macros.h"
#include "hash.h"

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

static xapi delete_entry(const hashtable_t * ht, void * _ent)
{
  enter;

  const map_t * m = containerof(ht, map_t, ht);
  mapping * ent = _ent;

  if(m->destroy_fn)
    m->destroy_fn(ent->v);
  else if(m->xdestroy_fn)
    fatal(m->xdestroy_fn, ent->v);

  finally : coda;
}

static xapi destroy_entry(const hashtable_t * ht, void * _ent)
{
  enter;

  mapping * ent = _ent;

  wfree(ent->k);

  finally : coda;
}

static xapi store_entry(const hashtable_t * ht, void * _dst, void * _src, bool found)
{
  enter;

  mapping * dst = _dst;
  mapping * src = _src;

  if(found)
  {
    fatal(delete_entry, ht, dst);
  }
  else
  {
    if(dst->k == 0 || src->kl >= dst->ka)
    {
      fatal(xrealloc, &dst->k, 1, src->kl + 1, dst->ka);
      dst->ka = src->kl + 1;
    }
    memcpy(dst->k, src->k, src->kl);
    ((char*)dst->k)[src->kl] = 0;
    dst->kl = src->kl;
  }

  dst->v = src->v;
  dst->vl = src->vl;

  finally : coda;
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

API xapi map_createx(map ** const restrict mx, size_t capacity, void (*free_value)(void*), xapi (*xfree_value)(void*))
{
  enter;

  map_t * m = 0;
  fatal(xmalloc, &m, sizeof(*m));

  fatal(hashtable_init
    , &m->ht
    , sizeof(mapping)
    , capacity
    , &ht_ops
    , 0
    , 0
  );

  m->destroy_fn = free_value;
  m->xdestroy_fn = xfree_value;

  *mx = &m->mx;
  m = 0;

finally:
  if(m)
    fatal(map_xfree, &m->mx);
coda;
}

API xapi map_create(map ** restrict m)
{
  xproxy(map_createx, m, 0, 0, 0);
}

API xapi map_xfree(map * restrict mx)
{
  enter;

  map_t * m = containerof(mx, map_t, mx);

  if(m)
    fatal(hashtable_xdestroy, &m->ht);

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

API xapi map_recycle(map * restrict mx)
{
  enter;

  map_t * m = containerof(mx, map_t, mx);
  fatal(hashtable_recycle, &m->htx);

  finally : coda;
}

API xapi map_put(map * restrict mx, const void * k, uint16_t kl, void * v, size_t vl)
{
  enter;

  map_t * m = containerof(mx, map_t, mx);

  mapping ent = { k : (void*)k, kl : kl, v : v, vl : vl };
  fatal(hashtable_put, &m->htx, &ent);

  finally : coda;
}

API void * map_get(const map * restrict mx, const void * restrict k, uint16_t kl)
{
  map_t * m = containerof(mx, map_t, mx);
  mapping * ent;

  mapping key = { k : (char*)k, kl : kl };
  if((ent = hashtable_get(&m->htx, &key)))
    return ent->v;

  return 0;
}

API bool map_get_mapping(const map * restrict mx, const void * restrict k, uint16_t kl, void * v, size_t * restrict vl)
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

API xapi map_splice(map * restrict dstx, map * restrict srcx)
{
  enter;

  map_t * dst = containerof(dstx, map_t, mx);
  map_t * src = containerof(srcx, map_t, mx);

  fatal(hashtable_splice, &dst->htx, &src->htx);

  finally : coda;
}

API xapi map_replicate(map * restrict dstx, map * restrict srcx)
{
  enter;

  map_t * dst = containerof(dstx, map_t, mx);
  map_t * src = containerof(srcx, map_t, mx);

  fatal(hashtable_replicate, &dst->htx, &src->htx);

  finally : coda;
}

API bool map_equal(map * restrict Ax, map * restrict Bx)
{
  map_t * A = containerof(Ax, map_t, mx);
  map_t * B = containerof(Bx, map_t, mx);

  return hashtable_equal(&A->htx, &B->htx);
}

API xapi map_delete(map * restrict mx, const void * restrict k, uint16_t kl)
{
  enter;

  map_t * m = containerof(mx, map_t, mx);

  mapping key = { k : (char*)k, kl : kl };
  fatal(hashtable_delete, &m->htx, &key);

  finally : coda;
}

API xapi map_keys(const map * restrict mx, void * restrict _keys, uint16_t * restrict keysl)
{
  enter;

  mapping * mg;
  size_t x;
  size_t i;

  void *** keys = _keys;

  const map_t * m = containerof(mx, map_t, mx);

  fatal(xmalloc, keys, sizeof(*keys) * m->size);

  i = 0;
  for(x = 0; x < m->table_size; x++)
  {
    if((mg = hashtable_table_entry(&m->htx, x)))
      (*keys)[i++] = mg->k;
  }
  (*keysl) = m->size;

  finally : coda;
}

API xapi map_values(const map * restrict mx, void * restrict _values, size_t * restrict valuesl)
{
  enter;

  mapping * mg;
  size_t x;
  size_t i;
  void *** values = _values;

  const map_t * m = containerof(mx, map_t, mx);

  fatal(xmalloc, values, sizeof(*values) * m->size);

  i = 0;
  for(x = 0; x < m->table_size; x++)
  {
    if((mg = hashtable_table_entry(&m->htx, x)))
      (*values)[i++] = mg->v;
  }

  (*valuesl) = m->size;

  finally : coda;
}

API const void * map_table_key(const map * restrict mx, size_t x)
{
  const map_t * m = containerof(mx, map_t, mx);

  mapping * ent;
  if((ent = hashtable_table_entry(&m->htx, x)))
    return ent->k;

  return 0;
}

API void * map_table_value(const map * restrict mx, size_t x)
{
  const map_t * m = containerof(mx, map_t, mx);

  mapping * ent;
  if((ent = hashtable_table_entry(&m->htx, x)))
    return ent->v;

  return false;
}

API bool map_table_mapping(const map * restrict mx, size_t x, const void ** restrict k, uint16_t * restrict kl, void * v, size_t * restrict vl)
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
