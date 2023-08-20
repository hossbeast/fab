/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "valyria/list.h"
#include "valyria/pstring.h"
#include "valyria/set.h"

#include "value.h"
#include "common/hash.h"
#include "macros.h"

//
// api
//

int API value_cmp(value * const restrict A, value * const restrict B)
{
  int d;
  int x;
  int r = 0;

  if(A == 0 || B == 0)
  {
    r = (A == 0) - (B == 0);
  }
  else if((d = A->type - B->type))
  {
    r = d;
  }
  else if(A->type == VALUE_TYPE_STRING)
  {
    r = pscmp(A->s, B->s);
  }
  else if(A->type == VALUE_TYPE_FLOAT)
  {
    r = INTCMP(A->f, B->f);
  }
  else if(A->type == VALUE_TYPE_POSINT)
  {
    r = INTCMP(A->u, B->u);
  }
  else if(A->type == VALUE_TYPE_NEGINT)
  {
    r = INTCMP(A->i, B->i);
  }
  else if(A->type == VALUE_TYPE_BOOLEAN)
  {
    r = A->b != B->b;
  }
  else if(A->type == VALUE_TYPE_VARIABLE)
  {
    if((r = INTCMP(A->v.len, B->v.len)))
      r = memcmp(A->v.name, B->v.name, A->v.len);
  }
  else if(A->type == VALUE_TYPE_LIST)
  {
    if((d = A->items->size - B->items->size))
    {
      r = d;
    }
    else
    {
      for(x = 0; x < A->items->size; x++)
      {
        if((d = value_cmp(list_get(A->items, x), list_get(B->items, x))))
        {
          r = d;
          break;
        }
      }
    }
  }
  else if(A->type == VALUE_TYPE_SET)
  {
    r = !set_equal(A->els, B->els);
  }
  else if(A->type == VALUE_TYPE_MAPPING)
  {
    r = value_cmp(A->key, B->key);
  }

  return r;
}

bool API value_equal(value * const restrict A, value * const restrict B)
{
  if(A == 0 || B == 0)
    return !!A == !!B;

  if(A->type != B->type)
    return false;

  if(value_hash(0, A) != value_hash(0, B))
    return false;

  return value_cmp(A, B) == 0;
}

uint32_t API value_hash(uint32_t h, value * const restrict val)
{
  uint32_t hash;
  int x;

  if(!val->hash)
  {
    hash = hash32(0, &val->type, sizeof(val->type));

    if(val->type == VALUE_TYPE_LIST)
    {
      for(x = 0; x < val->items->size; x++)
        hash = value_hash(hash, list_get(val->items, x));
    }
    else if(val->type == VALUE_TYPE_SET)
    {
      hash += val->els->hash;
    }
    else if(val->type == VALUE_TYPE_MAPPING)
    {
      hash += val->key->hash;
    }
    else if(val->type == VALUE_TYPE_STRING)
    {
      hash = hash32(hash, val->s->s, val->s->size);
    }
    else if(val->type == VALUE_TYPE_FLOAT)
    {
      hash = hash32(hash, &val->f, sizeof(val->f));
    }
    else if(val->type == VALUE_TYPE_BOOLEAN)
    {
      hash = hash32(hash, &val->b, sizeof(val->b));
    }
    else if(val->type == VALUE_TYPE_NEGINT)
    {
      hash = hash32(hash, &val->i, sizeof(val->i));
    }
    else if(val->type == VALUE_TYPE_POSINT)
    {
      hash = hash32(hash, &val->u, sizeof(val->u));
    }

    val->hash = hash;
  }

  return h + val->hash;
}
