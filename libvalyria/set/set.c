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

#include "xapi.h"
#include "types.h"

#include "xlinux/xstdlib.h"

#include "internal.h"
#include "hashtable.internal.h"
#include "set.internal.h"

#include "macros.h"
#include "hash.h"

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

static xapi delete_entry(const hashtable_t * ht, void * entry)
{
  enter;

  const set_t * s = (typeof(s))ht;
  element * el = entry;

  if(s->destroy_fn)
    s->destroy_fn(el->p);
  else if(s->xdestroy_fn)
    fatal(s->xdestroy_fn, el->p);

  finally : coda;
}

static xapi destroy_entry(const hashtable_t * ht, void * entry)
{
  enter;

  finally : coda;
}

static xapi store_entry(const hashtable_t * ht, void * dst, void * entry, bool found)
{
  enter;

  if(found)
  {
    fatal(delete_entry, ht, dst);
  }

  memcpy(dst, entry, ht->esz);

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

API xapi set_createx(
    set ** const restrict rv
  , size_t capacity
  , uint32_t (*hash_fn)(uint32_t h, const void * element, size_t sz)
  , int (*cmp_fn)(const void * A, size_t Asz, const void * B, size_t Bsz)
  , void (*destroy_fn)(void * element)
  , xapi (*xdestroy_fn)(void * element)
)
{
  enter;

  set_t * s = 0;

  fatal(xmalloc, &s, sizeof(*s));

  if(!hash_fn)
    hash_fn = set_default_hash_fn;

  if(!cmp_fn)
    cmp_fn = set_default_cmp_fn;

  fatal(hashtable_init
    , &s->ht
    , sizeof(element)
    , capacity
    , &ht_ops
    , hash_fn
    , cmp_fn
  );

  s->destroy_fn = destroy_fn;
  s->xdestroy_fn = xdestroy_fn;

  *rv = &s->sx;
  s = 0;

finally:
  if(s)
    fatal(set_xfree, &s->sx);
coda;
}

API xapi set_create(set ** restrict sx)
{
  xproxy(set_createx, sx, 0, 0, 0, 0, 0);
}

API xapi set_xfree(set * restrict sx)
{
  enter;

  set_t * s = containerof(sx, set_t, sx);

  if(s)   // free-like semantics
    fatal(hashtable_xdestroy, &s->ht);

  wfree(s);

  finally : coda;
}

API xapi set_ixfree(set ** restrict sx)
{
  enter;

  fatal(set_xfree, *sx);
  *sx = 0;

  finally : coda;
}

API xapi set_recycle(set * restrict sx)
{
  enter;

  set_t * s = containerof(sx, set_t, sx);
  fatal(hashtable_recycle, &s->htx);

  finally : coda;
}

API xapi set_put(set * restrict sx, void * e, size_t len)
{
  enter;

  set_t * s = containerof(sx, set_t, sx);

  element el = { p : e, l : len };
  fatal(hashtable_put, &s->htx, &el);

  finally : coda;
}

xapi set_store(set * sx, void * e, size_t len, void ** restrict elementp)
{
  enter;

  set_t *s = containerof(sx, set_t, sx);
  ht_bucket * bp;

  element el = { p : e, l : len };
  fatal(hashtable_store, &s->ht, &el, &bp);

  element * elp = (typeof(elp))bp->p;
  *(void**)elementp = elp->p;

  finally : coda;
}

API xapi set_splice(set * restrict dstx, set * restrict srcx)
{
  enter;

  set_t * src = containerof(srcx, set_t, sx);
  set_t * dst = containerof(dstx, set_t, sx);

  fatal(hashtable_splice, &dst->htx, &src->htx);

  finally : coda;
}

API xapi set_replicate(set * restrict dstx, set * restrict srcx)
{
  enter;

  set_t * src = containerof(srcx, set_t, sx);
  set_t * dst = containerof(dstx, set_t, sx);

  fatal(hashtable_replicate, &dst->htx, &src->htx);

  finally : coda;
}

API bool set_contains(const set * sx, const void * e, size_t len)
{
  const set_t * s = containerof(sx, set_t, sx);

  element key = { .p = (void*)e, .l = len };
  return !!hashtable_get(&s->htx, &key);
}

API void * set_get(const set * sx, const void * e, size_t len)
{
  const set_t * s = containerof(sx, set_t, sx);

  element key = { .p = (void*)e, .l = len };
  element * el = hashtable_get(&s->htx, &key);
  if(el)
    return el->p;

  return 0;
}

API bool set_get_element(const set * sx, const void * e, size_t len, void * elp, size_t * restrict ell)
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

API bool set_equal(set * const Ax, set * const Bx)
{
  set_t * A = containerof(Ax, set_t, sx);
  set_t * B = containerof(Bx, set_t, sx);

  return hashtable_equal(&A->htx, &B->htx);
}

API xapi set_delete(set * sx, const void * e, size_t len)
{
  enter;

  set_t * s = containerof(sx, set_t, sx);

  element el = { .p = (void*)e, .l = len };
  fatal(hashtable_delete, &s->htx, &el);

  finally : coda;
}

API xapi set_elements(const set * restrict sx, void * restrict _els, size_t ** restrict elsls, size_t * restrict elsl)
{
  enter;

  element * el;
  size_t x;
  size_t i;

  const set_t * s = containerof(sx, set_t, sx);
  void *** els = _els;

  fatal(xmalloc, els, sizeof(**els) * s->size);
  fatal(xmalloc, elsls, sizeof(**elsls) * s->size);

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

  finally : coda;
}

API bool set_table_element(const set * restrict sx, size_t x, void * elp, size_t * elenp)
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

API void * set_table_get(const set * restrict sx, size_t x)
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

API xapi set_table_delete(set * restrict sx, size_t x)
{
  enter;

  set_t * s = containerof(sx, set_t, sx);

  fatal(hashtable_table_delete, &s->htx, x);

  finally : coda;
}
