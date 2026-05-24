/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "value.h"
#include "set.h"
#include "registry.internal.h"

#include "macros.h"

//
// api
//

static int cmp(const bacon_value * const restrict A, const bacon_value * const restrict B)
{
  int d;
  bacon_value *a;
  bacon_value *b;
  rbnode *rbn;
  const bacon_object *obj;

  if(A == 0 || B == 0)
  {
    return (A == 0) - (B == 0);
  }
  if((d = INTCMP(A->type, B->type)))
  {
    return d;
  }
  switch(A->type)
  {
    case BACON_TYPE_STRING:
      return memncmp(A->s, A->len, B->s, B->len);
    case BACON_TYPE_FLOAT:
      return INTCMP(A->f, B->f);
    case BACON_TYPE_INTEGER:
      return INTCMP(A->i, B->i);
    case BACON_TYPE_BOOLEAN:
      return INTCMP(A->b, B->b);
    case BACON_TYPE_VARIABLE:
      return memncmp(A->s, A->len, B->s, B->len);
    case BACON_TYPE_LIST:
      a = llist_first(&A->list, typeof(*a), lln);
      b = llist_first(&B->list, typeof(*b), lln);
      while(a || b) {
        if((d = bacon_value_cmp(a, b))) {
          return d;
        }
        a = llist_next(&A->list, a, lln);
        b = llist_next(&B->list, b, lln);
      }
      if(!!a ^ !!b) {
        if(a) {
          return 1;
        } else {
          return -1;
        }
      }
      break;
    case BACON_TYPE_SET:
      rbtree_foreach(&A->set, a, rbn) {
        if((rbn = rbtree_lookup(&B->set, a, bacon_set_key_cmp)) == 0) {
          return 1;
        } else {
          b = containerof(rbn, typeof(*b), rbn);
          if((d = bacon_value_cmp(a, b))) {
            if(d > 0) {
              return 1;
            } else if(d < 0) {
              return -1;
            }
          }
        }
      }
      rbtree_foreach(&B->set, b, rbn) {
        if((rbn = rbtree_lookup(&A->set, b, bacon_set_key_cmp)) == 0) {
          return -1;
        } else {
          a = containerof(rbn, typeof(*a), rbn);
          if((d = bacon_value_cmp(a, b))) {
            if(d > 0) {
              return -1;
            } else if(d < 0) {
              return 1;
            }
          }
        }
      }
      break;
    case BACON_TYPE_MAPPING:
      return bacon_value_cmp(A->key, B->key);
      if((d = bacon_value_cmp(A->key, B->key)) != 0) {
        return d;
      }
      return bacon_value_cmp(A->val, B->val);
    default:
      obj = registry_get(A->type);
      RUNTIME_ASSERT(obj != NULL);
      return obj->cmp(A, B);
  }

  return 0;
}

int bacon_value_cmp(const bacon_value * const restrict A, const bacon_value * const restrict B)
{
  return cmp(A, B);
}

bool bacon_value_equal(const bacon_value * const restrict A, const bacon_value * const restrict B)
{
  if(A == 0 || B == 0) {
    return !!A == !!B;
  } else if(A->type != B->type) {
    return false;
  } else if(A->hash != B->hash) {
    return false;
  }

  return bacon_value_cmp(A, B) == 0;
}
