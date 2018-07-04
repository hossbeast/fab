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
#include "map.internal.h"

#include "macros.h"
#include "hash.h"

static uint32_t mapping_hash(hashtable * ht, uint32_t h, void * _A, size_t sz)
{
  mapping * A = *(mapping **)_A;
  return hash32(h, A->k, A->kl);
}

static int mapping_cmp(hashtable * ht, void * restrict _A, size_t Asz, void * restrict _B, size_t Bsz)
{
  mapping * A = *(mapping **)_A;
  mapping * B = *(mapping **)_B;

  return memncmp(A->k, A->kl, B->k, B->kl);
}

static xapi mapping_destroy(hashtable * ht, void * _A)
{
  enter;

  map * m = (map*)ht;
  mapping * mg = *(mapping **)_A;

  if(m->free_value)
    m->free_value(mg->v);
  if(m->xfree_value)
    fatal(m->xfree_value, mg->v);

  finally : coda; 
}

static void mapping_free(mapping * mg)
{
  if(mg)
  {
    wfree(mg->k);
  }
  wfree(mg);
}

static mapping * table_mapping(map * restrict m, size_t i)
{
  ht_bucket * b = hashtable_bucket_at(m, m->tab, i);
  mapping ** mgp = (void*)b->p;
  return *mgp;
}

//
// internal
//

xapi map_put(map * restrict m, const void * k, size_t kl, void * restrict v, size_t vsz, void * const * const restrict rv)
{
  enter;

  uint32_t h;
  size_t i;
  mapping * lmg;
  mapping * mg = 0;
  mapping * mgp = 0;

  if(kl == 0)
    goto XAPI_FINALIZE;

  lmg = (typeof(*lmg)[]){{ .k = (void*)k, .kl = kl }};

  h = mapping_hash(m, 0, &lmg, 0);
  int r = hashtable_probe(m, h, &lmg, &i);
  if(r == 0)  // found
  {
    mg = table_mapping(m, i);

    // the value will be overwritten
    if(m->free_value)
      m->free_value(mg->v);
    else if(m->xfree_value)
      fatal(m->xfree_value, mg->v);
  }
  // not found, and a suitable deleted slot exists
  else if(r == 1)
  {
    mg = table_mapping(m, i);
  }
  else
  {
    // not found, and saturation has been reached
    if(m->count == m->overflow_count)
    {
      fatal(hashtable_grow, m);
      hashtable_probe(m, h, &lmg, &i);
    }
    fatal(xmalloc, &mgp, sizeof(*mgp));
    mg = mgp;
  }

  if(r)
  {
    if(mg->k == 0 || kl >= mg->ka)
    {
      fatal(xrealloc, &mg->k, 1, sizeof(*mg->k) + kl + 1, mg->ka);
      mg->ka = kl + 1;
    }

    memcpy(mg->k, k, kl);
    mg->k[kl] = 0;
    mg->kl = kl;

    ht_bucket *b = hashtable_bucket_at(m, m->tab, i);
    hashtable_set_entry(m, b, h, &mg);
    mgp = 0;
  }

  // copy the value
  if(v)
    mg->v = v;

  // return pointer to the value
  if(rv)
  {
    if(mg->v == 0)
      fatal(xmalloc, &mg->v, vsz);

    *(void**)rv = mg->v;
  }

finally:
  m->free_mapping(mgp);
coda;
}

xapi map_init(map * const restrict m, size_t capacity, void * free_value, void * xfree_value)
{
  enter;

  fatal(hashtable_init, m, sizeof(mapping*), capacity, mapping_hash, mapping_cmp, 0, mapping_destroy);

  m->free_value = free_value;
  m->xfree_value = xfree_value;

  finally : coda;
}

//
// api
//

API xapi map_createx(map ** const restrict m, size_t capacity, void * free_value, void * xfree_value)
{
  enter;

  fatal(xmalloc, m, sizeof(**m));
  fatal(hashtable_init, *m, sizeof(mapping*), capacity, mapping_hash, mapping_cmp, 0, mapping_destroy);

  (*m)->free_value = free_value;
  (*m)->xfree_value = xfree_value;
  (*m)->free_mapping = mapping_free;

  finally : coda;
}

API xapi map_create(map ** restrict m)
{
  xproxy(map_createx, m, 0, 0, 0);
}

API xapi map_xfree(map * restrict m)
{
  enter;

  if(m)   // free-like semantics
  {
    fatal(hashtable_recycle, m);

    size_t x;
    for(x = 0; x < m->table_size; x++)
    {
      mapping * mg = table_mapping(m, x);
      m->free_mapping(mg);
    }

    wfree(m->tab);
  }

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

API xapi map_recycle(map * restrict m)
{
  xproxy(hashtable_recycle, m);
}

API xapi map_set(map * restrict m, const void * k, size_t kl, void * restrict v)
{
  xproxy(map_put, m, k, kl, v, 0, 0);
}

API void * map_get(const map * restrict m, const void * restrict k, size_t kl)
{
  mapping * lmg;
  uint32_t h;
  size_t i;

  lmg = (typeof(*lmg)[]){{ .k = (void*)k, .kl = kl }};
  h = mapping_hash(m, 0, &lmg, 0);

  if(hashtable_probe(m, h, &lmg, &i) == 0)
    return table_mapping((void*)m, i)->v;

  return 0;
}

API xapi map_delete(map * restrict m, const void * restrict k, size_t kl)
{
  enter;

  mapping * lmg;

  lmg = (typeof(*lmg)[]){{ .k = (void*)k, .kl = kl }};
  fatal(hashtable_delete, m, &lmg);

  finally : coda;
}

API xapi map_keys(const map * restrict m, const char *** restrict keys, size_t * restrict keysl)
{
  enter;

  mapping ** mg;
  size_t x;
  size_t i;

  fatal(xmalloc, keys, sizeof(*keys) * m->count);

  i = 0;
  for(x = 0; x < m->table_size; x++)
  {
    if((mg = hashtable_table_entry(m, x)))
      (*keys)[i++] = (*mg)->k;
  }
  (*keysl) = m->count;

  finally : coda;
}

API xapi map_values(const map * restrict m, void * restrict _values, size_t * restrict valuesl)
{
  enter;

  mapping ** mg;
  size_t x;
  size_t i;
  void *** values = _values;

  fatal(xmalloc, values, sizeof(*values) * m->count);

  i = 0;
  for(x = 0; x < m->table_size; x++)
  {
    if((mg = hashtable_table_entry(m, x)))
      (*values)[i++] = (*mg)->v;
  }

  (*valuesl) = m->count;

  finally : coda;
}

API const void * map_table_key(const map * restrict m, size_t x)
{
  mapping ** mg;
  if((mg = hashtable_table_entry(m, x)))
    return (*mg)->k;

  return 0;
}

API void * map_table_value(const map * restrict m, size_t x)
{
  mapping ** mg;
  if((mg = hashtable_table_entry(m, x)))
    return (*mg)->v;

  return 0;
}
