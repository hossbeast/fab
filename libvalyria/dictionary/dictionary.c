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

#include "xlinux/xstdlib.h"

#include "internal.h"
#include "dictionary.internal.h"

#include "macros.h"

//
// dictionary/hashtable callbacks
//

static uint32_t hash_entry(const hashtable_t * ht, void * _ent)
{
  entry * ent = _ent;
  return ht->hash_fn(0, ent->k, ent->kl);
}

static int compare_entries(const hashtable_t * ht, void * _A, void * _B)
{
  entry * A = _A;
  entry * B = _B;
  return ht->cmp_fn(A->k, A->kl, B->k, B->kl);
}

static xapi delete_entry(const hashtable_t * ht, void * _ent)
{
  enter;

  const dictionary_t * d = containerof(ht, dictionary_t, ht);
  entry * ent = _ent;

  if(d->destroy_fn)
    d->destroy_fn(ent->v);
  else if(d->xdestroy_fn)
    fatal(d->xdestroy_fn, ent->v);

  memset(ent->v, 0, d->vsz);

  finally : coda;
}

static xapi destroy_entry(const hashtable_t * ht, void * _ent)
{
  enter;

  entry * ent = _ent;
  wfree(ent->k);

  finally : coda;
}

static xapi store_entry(const hashtable_t * ht, void * _dst, void * _src, bool found)
{
  enter;

  const dictionary_t * d = containerof(ht, dictionary_t, ht);
  entry * dst = _dst;
  entry * src = _src;

  if(!found)
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
  else if(src->val)
  {
    fatal(delete_entry, ht, dst);
  }

  if(src->val)
  {
    memcpy(dst->v, src->v, d->vsz);
  }

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

API xapi dictionary_createx(
    dictionary ** restrict dictx
  , size_t vsz
  , size_t capacity
  , void (*destroy_value)(void * value)
  , xapi (*xdestroy_value)(void * value)
)
{
  enter;

  dictionary_t * dict = 0;
  fatal(xmalloc, &dict, sizeof(*dict));

  // alignment
  size_t vssz = roundup(vsz, sizeof(entry));

  fatal(hashtable_init
    , &dict->ht
    , sizeof(entry) + vssz
    , capacity
    , &ht_ops
    , 0
    , 0
  );

  dict->destroy_fn = destroy_value;
  dict->xdestroy_fn = xdestroy_value;

  dict->vsz = vsz;

  *dictx = &dict->dx;
  dict = 0;

finally:
  if(dict)
    fatal(dictionary_xfree, &dict->dx);
coda;
}

API xapi dictionary_create(dictionary ** restrict d, size_t vsz)
{
  xproxy(dictionary_createx, d, vsz, 0, 0, 0);
}

API xapi dictionary_put(dictionary * restrict dx, const void * restrict k, uint16_t kl, void * v)
{
  enter;

  dictionary_t * d = containerof(dx, dictionary_t, dx);
  ht_bucket * bp;
  entry * ent;

  char buf[sizeof(entry) + 64];
  entry *entp = (void*)buf;
  entp->k = (void*)k;
  entp->kl = kl;
  memcpy(entp->v, v, d->vsz);
  entp->val = true;

  fatal(hashtable_store, &d->ht, entp, &bp);

  ent = (typeof(ent))bp->p;
  ent->val = true;

  finally : coda;
}

API xapi dictionary_store(dictionary * restrict dx, const void * restrict k, uint16_t kl, void * v)
{
  enter;

  dictionary_t * d = containerof(dx, dictionary_t, dx);
  ht_bucket * bp;
  entry * ent;

  char buf[sizeof(entry) + 64];
  entry *entp = (void*)buf;
  entp->k = (void*)k;
  entp->kl = kl;
  entp->val = false;

  fatal(hashtable_store, &d->ht, entp, &bp);

  ent = (typeof(ent))bp->p;
  ent->val = true;
  *(void**)v = ent->v;

  finally : coda;
}

API void * dictionary_get(dictionary* restrict dx, const void * restrict k, uint16_t kl)
{
  dictionary_t * d = containerof(dx, dictionary_t, dx);
  entry * ent;

  entry key = { k : (void*)k, kl : kl };
  if((ent = hashtable_get(&d->htx, &key)))
    return ent->v;

  return 0;
}

API xapi dictionary_recycle(dictionary* restrict dx)
{
  enter;

  dictionary_t * d = containerof(dx, dictionary_t, dx);
  fatal(hashtable_recycle, &d->htx);

  finally : coda;
}

API xapi dictionary_replicate(dictionary* restrict dstx, dictionary * restrict srcx)
{
  enter;

  dictionary_t * dst = containerof(dstx, dictionary_t, dx);
  dictionary_t * src = containerof(srcx, dictionary_t, dx);

  fatal(hashtable_replicate, &dst->htx, &src->htx);

  finally : coda;
}

API xapi dictionary_splice(dictionary * restrict dstx, dictionary * restrict srcx)
{
  enter;

  dictionary_t * dst = containerof(dstx, dictionary_t, dx);
  dictionary_t * src = containerof(srcx, dictionary_t, dx);

  fatal(hashtable_splice, &dst->htx, &src->htx);

  finally : coda;
}

API xapi dictionary_delete(dictionary * restrict dx, const void * restrict k, uint16_t kl)
{
  enter;

  dictionary_t * d = containerof(dx, dictionary_t, dx);

  entry key = { k : (void*)k, kl : kl };
  fatal(hashtable_delete, &d->htx, &key);

  finally : coda;
}

API xapi dictionary_keys(const dictionary * restrict dx, void * restrict _keys, uint16_t * restrict keysl)
{
  enter;

  size_t x;
  size_t i;

  void *** keys = _keys;

  dictionary_t * d = containerof(dx, dictionary_t, dx);
  entry * ent;

  fatal(xmalloc, keys, sizeof(*keys) * d->size);

  i = 0;
  for(x = 0; x < d->table_size; x++)
  {
    if((ent = hashtable_table_entry(&d->htx, x)))
      (*keys)[i++] = ent->k;
  }
  (*keysl) = d->size;

  finally : coda;
}

API xapi dictionary_values(const dictionary* restrict dx, void * restrict _values, size_t * restrict valuesl)
{
  enter;

  size_t x;
  size_t i;

  void *** values = _values;

  dictionary_t * d = containerof(dx, dictionary_t, dx);
  entry * ent;

  fatal(xmalloc, values, sizeof(*values) * d->size);

  i = 0;
  for(x = 0; x < d->table_size; x++)
  {
    if((ent = hashtable_table_entry(&d->htx, x)))
      (*values)[i++] = ent->v;
  }
  (*valuesl) = d->size;

  finally : coda;
}

API xapi dictionary_xfree(dictionary* restrict dx)
{
  enter;

  dictionary_t * d = containerof(dx, dictionary_t, dx);

  if(d)
    fatal(hashtable_xdestroy, &d->ht);

  wfree(d);

  finally : coda;
}

API xapi dictionary_ixfree(dictionary** restrict d)
{
  enter;

  fatal(dictionary_xfree, *d);
  *d = 0;

  finally : coda;
}

API const void * dictionary_table_key(const dictionary * restrict dx, size_t x)
{
  dictionary_t * d = containerof(dx, dictionary_t, dx);
  entry * ent;
  if((ent = hashtable_table_entry(&d->htx, x)))
    return ent->k;

  return 0;
}

API void * dictionary_table_value(const dictionary * restrict dx, size_t x)
{
  dictionary_t * d = containerof(dx, dictionary_t, dx);
  entry * ent;
  if((ent = hashtable_table_entry(&d->htx, x)))
    return ent->v;

  return 0;
}
