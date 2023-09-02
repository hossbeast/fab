/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "valyria/hashtable.h"
#include "valyria/pstring.h"
#include "valyria/hashtable.h"
#include "xlinux/xstdlib.h"

#include "parser.h"
#include "eval.h"
#include "expr.h"
#include "value.h"
#include "location.h"

#include "common/attrs.h"

attrs32 value_type_attrs = (attrs32){
#undef DEF
#define DEF(x, y) + 1
    num : 0 BACON_TYPE_TABLE
  , members : (member32[]) {
#undef DEF
#define DEF(x, y) { name : #x, value : UINT32_C(y), range : BACON_TYPE_OPT },
BACON_TYPE_TABLE
  }
};

static void __attribute__((constructor)) init()
{
  attrs32_init(&value_type_attrs);
}

static bacon_value * value_alloc(llist * restrict freelist, bacon_type type)
{
  bacon_value * v = 0;

  if((v = llist_shift(freelist, typeof(*v), lln)) == 0) {
    xmalloc(&v, sizeof(*v));
    llist_init(&v->lln);
    rbnode_init(&v->rbn);
  } else {
    v->loc = 0;
  }

  v->type = type;

  return v;
}

static void value_list_destroy(bacon_parser * restrict parser, llist * restrict list)
{
  bacon_value *v;
  llist *T;

  llist_foreach_safe(list, v, lln, T) {
    bacon_value_free(parser, v);
  }
}

static void value_set_destroy(bacon_parser * restrict parser, rbtree * restrict set)
{
  bacon_value *v;
  llist list;

  llist_init(&list);
  rbtree_foreach(set, v, rbn) {
    llist_push(&list, &v->lln);
  }

  value_list_destroy(parser, &list);
}

void bacon_value_free(bacon_parser * restrict parser, struct bacon_value * restrict v)
{
  if(!v) {
    return;
  }

  switch(v->type) {
    case BACON_TYPE_SET:
      value_set_destroy(parser, &v->set);
      break;
    case BACON_TYPE_LIST:
      value_list_destroy(parser, &v->list);
      break;
    case BACON_TYPE_MAPPING:
      bacon_value_free(parser, v->key);
      bacon_value_free(parser, v->val);
      break;
    case BACON_TYPE_STRING:
    case BACON_TYPE_VARIABLE:
      free(v->s);
      break;
    case BACON_TYPE_FLOAT:
    case BACON_TYPE_BOOLEAN:
    case BACON_TYPE_INTEGER:
      break;
    default:
      RTA;
  }

  if(v->loc) {
    bacon_location_free(parser, v->loc);
  }
  llist_push(&parser->value_freelist, &v->lln);
}

bacon_value *bacon_string_alloc(bacon_parser * restrict parser)
{
  bacon_value * v;

  v = value_alloc(&parser->value_freelist, BACON_TYPE_STRING);
  psinit(&v->s, &v->len, &v->sa);

  return v;
}

bacon_value *bacon_float_alloc(bacon_parser * restrict parser)
{
  return value_alloc(&parser->value_freelist, BACON_TYPE_FLOAT);
}

bacon_value *bacon_boolean_alloc(bacon_parser * restrict parser)
{
  return value_alloc(&parser->value_freelist, BACON_TYPE_BOOLEAN);
}

bacon_value *bacon_integer_alloc(bacon_parser * restrict parser)
{
  return value_alloc(&parser->value_freelist, BACON_TYPE_INTEGER);
}

bacon_value *bacon_variable_alloc(bacon_parser * restrict parser)
{
  return value_alloc(&parser->value_freelist, BACON_TYPE_VARIABLE);
}

bacon_value *bacon_mapping_alloc(bacon_parser * restrict parser)
{
  return value_alloc(&parser->value_freelist, BACON_TYPE_MAPPING);
}

bacon_value *bacon_list_alloc(bacon_parser * restrict parser)
{
  bacon_value * v = 0;

  v = value_alloc(&parser->value_freelist, BACON_TYPE_LIST);
  llist_init(&v->list);

  return v;
}

bacon_value *bacon_set_alloc(bacon_parser * restrict parser)
{
  bacon_value * v = 0;

  v = value_alloc(&parser->value_freelist, BACON_TYPE_SET);
  rbtree_init(&v->set);

  return v;
}
