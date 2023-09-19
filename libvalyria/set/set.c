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

#include "types.h"

#include "xlinux/xstdlib.h"

#include "hashtable.internal.h"
#include "set.internal.h"

#include "macros.h"
#include "common/hash.h"

//
// static
//

static uint32_t set_default_hash_fn(uint32_t h, const void * data, size_t len)
{
  return hash32(hash32(h, &data, sizeof(data)), &len, sizeof(len));
}

static int set_default_cmp_fn(const void * A, size_t Asz, const void * B, size_t Bsz)
{
  return
       INTCMP(A, B)
    ?: INTCMP(Asz, Bsz)
    ;
}

//
// set/hashtable callbacks
//

static uint32_t hash_entry(const hashtable_t * ht, void * entry)
{
  element * el = entry;
  return ht->hash_fn(0, el->p, el->l);
}

static int compare_entries(const hashtable_t * ht, void * _A, void * _B)
{
  element * A = _A;
  element * B = _B;

  return ht->cmp_fn(A->p, A->l, B->p, B->l);
}

static void delete_entry(const hashtable_t * ht, void * entry)
{
  const set_t * s = (typeof(s))ht;
  element * el = entry;

  if(s->destroy_fn)
    s->destroy_fn(el->p);
}

static void destroy_entry(const hashtable_t * ht, void * entry)
{
}

static void store_entry(const hashtable_t * ht, void * dst, void * entry, bool found)
{
  if(found)
  {
    delete_entry(ht, dst);
  }

  memcpy(dst, entry, ht->esz);
}

static ht_operations ht_ops = {
    hash_entry : hash_entry
  , compare_entries : compare_entries
  , delete_entry : delete_entry
  , destroy_entry : destroy_entry
  , store_entry : store_entry
};

static int probe(const set_t* const restrict s, uint32_t h, void * ent, size_t ent_sz, set_key_cmp cmp, size_t * restrict i)
{
  ht_bucket *b;
  element * el;
  size_t li;
  if(!i)
    i = &li;

  *i = h & s->ht.lm;

  size_t ij = 0;
  size_t * ijp = &ij;

  while(1)
  {
    b = hashtable_bucket_at(&s->ht, s->ht.tab, *i);
    RUNTIME_ASSERT((b->attr & ~(HT_OCCUPIED | HT_DELETED)) == 0);
    el = (void*)b->p;
    if(b->attr & HT_OCCUPIED)
    {
      if(b->attr & HT_DELETED)
      {
        if(ijp)
          *ijp = *i;

        ijp = 0;
      }
      else if(cmp(el->p, el->l, ent, ent_sz) == 0)
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
    (*i) &= s->ht.lm;
  }
}

//
// api
//

void API set_createx(
    set ** const restrict rv
  , size_t capacity
  , uint32_t (*hash_fn)(uint32_t h, const void * element, size_t sz)
  , int (*cmp_fn)(const void * A, size_t Asz, const void * B, size_t Bsz)
  , void (*destroy_fn)(void * element)
)
{
  set_t * s = 0;

  xmalloc(&s, sizeof(*s));

  if(!hash_fn)
    hash_fn = set_default_hash_fn;

  if(!cmp_fn)
    cmp_fn = set_default_cmp_fn;

  hashtable_init(
    &s->ht
    , sizeof(element)
    , capacity
    , &ht_ops
    , hash_fn
    , cmp_fn
  );

  s->destroy_fn = destroy_fn;

  *rv = &s->sx;
  s = 0;

  if(s) {
    set_xfree(&s->sx);
  }
}

void API set_create(set ** restrict sx)
{
  set_createx(sx, 0, 0, 0, 0);
}

void API set_xfree(set * restrict sx)
{
  set_t * s = containerof(sx, set_t, sx);

  if(s)   // free-like semantics
    hashtable_xdestroy(&s->ht);

  wfree(s);
}

void API set_ixfree(set ** restrict sx)
{
  set_xfree(*sx);
  *sx = 0;
}

void API set_recycle(set * restrict sx)
{
  set_t * s = containerof(sx, set_t, sx);
  hashtable_recycle(&s->htx);
}

void API set_put(set * restrict sx, void * e, size_t len)
{
  set_t * s = containerof(sx, set_t, sx);

  element el = { p : e, l : len };
  hashtable_put(&s->htx, &el);
}

void set_store(set * sx, void * e, size_t len, void ** restrict elementp)
{
  set_t *s = containerof(sx, set_t, sx);
  ht_bucket * bp;

  element el = { p : e, l : len };
  hashtable_store(&s->ht, &el, &bp);

  element * elp = (typeof(elp))bp->p;
  *(void**)elementp = elp->p;
}

void API set_splice(set * restrict dstx, set * restrict srcx)
{
  set_t * src = containerof(srcx, set_t, sx);
  set_t * dst = containerof(dstx, set_t, sx);

  hashtable_splice(&dst->htx, &src->htx);
}

void API set_replicate(set * restrict dstx, set * restrict srcx)
{
  set_t * src = containerof(srcx, set_t, sx);
  set_t * dst = containerof(dstx, set_t, sx);

  hashtable_replicate(&dst->htx, &src->htx);
}

bool API set_contains(const set * sx, const void * e, size_t len)
{
  const set_t * s = containerof(sx, set_t, sx);

  element key = { .p = (void*)e, .l = len };
  return !!hashtable_get(&s->htx, &key);
}

void * API set_get(const set * sx, const void * e, size_t len)
{
  const set_t * s = containerof(sx, set_t, sx);

  element key = { .p = (void*)e, .l = len };
  element * el = hashtable_get(&s->htx, &key);
  if(el)
    return el->p;

  return 0;
}

void * API set_search(const set * sx, void *key, size_t key_len, set_key_hash hash, set_key_cmp cmp)
{
  const set_t * s = containerof(sx, set_t, sx);
  uint32_t h;
  element * el;
  ht_bucket * b;
  size_t i;

  h = hash(0, key, key_len);
  if(probe(s, h, key, key_len, cmp, &i) == 0)
  {
    b = hashtable_bucket_at(&s->ht, s->ht.tab, i);
    el = (void*)b->p;
    return el->p;
  }

  return 0;
}

bool API set_get_element(const set * sx, const void * e, size_t len, void * elp, size_t * restrict ell)
{
  const set_t * s = containerof(sx, set_t, sx);

  element key = { .p = (void*)e, .l = len };
  element * el = hashtable_get(&s->htx, &key);
  if(el)
  {
    if(elp)
      *(void**)elp = el->p;
    if(ell)
      *ell = el->l;

    return true;
  }

  return false;
}

bool API set_equal(set * const Ax, set * const Bx)
{
  set_t * A = containerof(Ax, set_t, sx);
  set_t * B = containerof(Bx, set_t, sx);

  return hashtable_equal(&A->htx, &B->htx);
}

void API set_delete(set * sx, const void * e, size_t len)
{
  set_t * s = containerof(sx, set_t, sx);

  element el = { .p = (void*)e, .l = len };
  hashtable_delete(&s->htx, &el);
}

void API set_elements(const set * restrict sx, void * restrict _els, size_t ** restrict elsls, size_t * restrict elsl)
{
  element * el;
  size_t x;
  size_t i;

  const set_t * s = containerof(sx, set_t, sx);
  void *** els = _els;

  xmalloc(els, sizeof(**els) * s->size);
  xmalloc(elsls, sizeof(**elsls) * s->size);

  i = 0;
  for(x = 0; x < s->table_size; x++)
  {
    if((el = hashtable_table_entry(&s->htx, x)))
    {
      (*els)[i] = el->p;
      (*elsls)[i] = el->l;
      i++;
    }
  }
  (*elsl) = s->size;
}

bool API set_table_element(const set * restrict sx, size_t x, void * elp, size_t * elenp)
{
  ht_bucket * htb;
  element * el;

  const set_t * s = containerof(sx, set_t, sx);
  if((htb = hashtable_table_bucket(&s->ht, x)))
  {
    el = (typeof(el))htb->p;

    if(elp)
      *(void**)elp = el->p;
    if(elenp)
      *elenp = el->l;

    return true;
  }

  return false;
}

void * API set_table_get(const set * restrict sx, size_t x)
{
  ht_bucket * htb;
  element * el;

  const set_t * s = containerof(sx, set_t, sx);
  if((htb = hashtable_table_bucket(&s->ht, x)))
  {
    el = (typeof(el))htb->p;
    return el->p;
  }

  return 0;
}

void API set_table_delete(set * restrict sx, size_t x)
{
  set_t * s = containerof(sx, set_t, sx);

  hashtable_table_delete(&s->htx, x);
}
