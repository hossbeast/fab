/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "common/hash.h"
#include "valyria/rbtree.h"
#include "value.h"
#include "set.h"

#include "narrator.h"

int bacon_set_key_cmp(void * restrict key, const rbnode * restrict B)
{
  const bacon_value *a = key;
  const bacon_value *b = containerof(B, typeof(*b), rbn);

  return bacon_value_cmp(a, b);
}

int bacon_set_rbn_cmp(const rbnode * restrict A, const rbnode * restrict B)
{
  const bacon_value *a = containerof(A, typeof(*a), rbn);
  const bacon_value *b = containerof(B, typeof(*b), rbn);

  return bacon_value_cmp(a, b);
}

bacon_value *bacon_set_lookupw(const bacon_value * restrict v, const char * restrict key, uint16_t key_len)
{
  if(v->type != BACON_TYPE_SET) {
    return 0;
  }

  bacon_value keyval;
  bacon_value *mapping;
  rbnode *rbn;

  keyval = (typeof(keyval)) {
      type : BACON_TYPE_MAPPING
    , key : (bacon_value[]) {{
        type : BACON_TYPE_STRING
      , s : (char*)key
      , len : key_len
    }}
  };
  keyval.key->hash = hash32(keyval.key->type, keyval.key->s, keyval.key->len);
  keyval.hash = keyval.key->hash;

  if((rbn = rbtree_lookup(&v->set, &keyval, bacon_set_key_cmp))) {
    mapping = containerof(rbn, typeof(*mapping), rbn);
    return mapping->val;
  }

  return 0;
}
