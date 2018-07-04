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
#include "set.internal.h"

#include "macros.h"
#include "hash.h"

typedef struct element
{
  char * e;
  uint16_t ea; // allocated size
  uint16_t el; // length
} element;

static uint32_t element_hash(hashtable * ht, uint32_t h, void * _A, size_t sz)
{
  element * A = *(element **)_A;
  return hash32(h, A->e, A->el);
}

static int element_cmp(hashtable * ht, void * restrict _A, size_t Asz, void * restrict _B, size_t Bsz)
{
  element * A = *(element **)_A;
  element * B = *(element **)_B;

  return memncmp(A->e, A->el, B->e, B->el);
}

static xapi element_destroy(hashtable * ht, void * _A)
{
  enter;

  set * s = (set*)ht;
  element * mg = *(element **)_A;

  if(s->free_element)
    s->free_element(mg->e, mg->el);
  if(s->xfree_element)
    fatal(s->xfree_element, mg->e, mg->el);

  finally : coda; 
}

static element * table_element(set * restrict s, size_t i)
{
  ht_bucket * b = hashtable_bucket_at(s, s->tab, i);
  element ** mgp = (void*)b->p;
  return *mgp;
}

//
// api
//

API xapi set_createx(set ** const restrict s, size_t capacity, void * free_element, void * xfree_element)
{
  enter;

  fatal(xmalloc, s, sizeof(**s));
  fatal(hashtable_init, *s, sizeof(element*), capacity, element_hash, element_cmp, 0, element_destroy);

  (*s)->free_element = free_element;
  (*s)->xfree_element = xfree_element;

  finally : coda;
}

API xapi set_create(set ** restrict s)
{
  xproxy(set_createx, s, 0, 0, 0);
}

API xapi set_xfree(set * restrict s)
{
  enter;

  if(s)   // free-like semantics
  {
    fatal(hashtable_recycle, s);

    size_t x;
    for(x = 0; x < s->table_size; x++)
    {
      element * mg = table_element(s, x);
      if(mg)
      {
        wfree(mg->e);
      }
      wfree(mg);
    }

    wfree(s->tab);
  }

  wfree(s);

  finally : coda;
}

API xapi set_ixfree(set ** restrict s)
{
  enter;

  fatal(set_xfree, *s);
  *s = 0;

  finally : coda;
}

API xapi set_recycle(set * restrict s)
{
  xproxy(hashtable_recycle, s);
}

API xapi set_put(set * restrict s, const void * e, uint16_t el)
{
  enter;

  uint32_t h;
  size_t i;
  element * lmg;
  element * mg = 0;
  element * mgp = 0;

  if(el == 0)
    goto XAPI_FINALIZE;

  lmg = (typeof(*lmg)[]){{ .e = (void*)e, .el = el }};

  h = element_hash(s, 0, &lmg, 0);
  int r = hashtable_probe(s, h, &lmg, &i);
  if(r == 0)  // found
  {
    mg = table_element(s, i);

    // the element will be overwritten
    if(s->free_element)
      s->free_element(mg->e, mg->el);
    else if(s->xfree_element)
      fatal(s->xfree_element, mg->e, mg->el);
  }
  // not found, and a suitable deleted slot exists
  else if(r == 1)
  {
    mg = table_element(s, i);
  }
  else
  {
    // not found, and saturation has been reached
    if(s->count == s->overflow_count)
    {
      fatal(hashtable_grow, s);
      hashtable_probe(s, h, &lmg, &i);
    }
    fatal(xmalloc, &mgp, sizeof(*mgp));
    mg = mgp;
  }

  if(r)
  {
    if(mg->e == 0 || el >= mg->ea)
    {
      fatal(xrealloc, &mg->e, 1, sizeof(*mg->e) + el + 1, mg->ea);
      mg->ea = el + 1;
    }

    memcpy(mg->e, e, el);
    mg->e[el] = 0;
    mg->el = el;

    ht_bucket *b = hashtable_bucket_at(s, s->tab, i);
    hashtable_set_entry(s, b, h, &mg);
    mgp = 0;
  }

finally:
  if(mgp)
  {
    wfree(mgp->e);
  }
  wfree(mgp);
coda;
}

API bool set_contains(const set * s, const void * e, uint16_t el)
{
  element * lmg;
  uint32_t h;
  size_t i;

  lmg = (typeof(*lmg)[]){{ .e = (void*)e, .el = el }};
  h = element_hash(s, 0, &lmg, 0);

  if(hashtable_probe(s, h, &lmg, &i) == 0)
    return true;

  return false;
}

API bool set_equal(set * const A, set * const B)
{
  return hashtable_equal(A, B);
}

API xapi set_delete(set * s, const void * e, uint16_t el)
{
  enter;

  element * lmg;

  lmg = (typeof(*lmg)[]){{ .e = (void*)e, .el = el }};
  fatal(hashtable_delete, s, &lmg);

  finally : coda;
}

API xapi set_elements(const set * restrict s, const void *** restrict es, uint16_t * restrict esls, size_t * restrict esl)
{
  enter;

  element ** mg;
  size_t x;
  size_t i;

  fatal(xmalloc, es, sizeof(*es) * s->count);
  fatal(xmalloc, esls, sizeof(*esls) * s->count);

  i = 0;
  for(x = 0; x < s->table_size; x++)
  {
    if((mg = hashtable_table_entry(s, x)))
    {
      (*es)[i] = (*mg)->e;
      esls[i] = (*mg)->el;
      i++;
    }
  }
  (*esl) = s->count;

  finally : coda;
}

API bool set_table_element(const set * restrict s, size_t x, void * e, uint16_t * el)
{
  element ** mg;
  if((mg = hashtable_table_entry(s, x)))
  {
    if(e)
      *(void**)e = (*mg)->e;
    if(el)
      *el = (*mg)->el;
    return true;
  }

  return false;
}
