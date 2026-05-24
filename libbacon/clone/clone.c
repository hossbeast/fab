/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "valyria/pstring.h"

#include "clone.h"
#include "glue.h"
#include "set.h"

//
// static
//

static bacon_value *clone_set(bacon_parser * restrict parser, const bacon_value * src)
{
  bacon_value * v;
  bacon_value * c;
  bacon_value *dst;

  dst = bacon_set_alloc(parser);

  rbtree_foreach(&src->set, v, rbn) {
    bacon_clone(parser, &c, v);
    rbtree_put(&dst->set, &c->rbn, bacon_set_rbn_cmp);
  }

  return dst;
}

static bacon_value *clone_list(bacon_parser * restrict parser, const bacon_value * src)
{
  bacon_value *dst;
  bacon_value *v;
  bacon_value *c;

  dst = bacon_list_alloc(parser);

  llist_foreach(&src->list, v, lln) {
    bacon_clone(parser, &c, v);
    llist_push(&dst->list, &c->lln);
  }

  return dst;
}

static bacon_value *clone_mapping(bacon_parser * restrict parser, const bacon_value * src)
{
  bacon_value *dst;
  bacon_value * c;

  dst = bacon_mapping_alloc(parser);

  bacon_clone(parser, &c, src->key);
  dst->key = c;

  bacon_clone(parser, &c, src->val);
  dst->val = c;

  return dst;
}

static bacon_value *clone_string(bacon_parser * restrict parser, const bacon_value * src)
{
  bacon_value *dst;

  dst = bacon_string_alloc(parser);
  pscatw(&dst->s, &dst->len, &dst->sa, src->s, src->len);

  return dst;
}

static bacon_value *clone_float(bacon_parser * restrict parser, const bacon_value * src)
{
  bacon_value *dst;

  dst = bacon_float_alloc(parser);
  dst->f = src->f;

  return dst;
}

static bacon_value *clone_bool(bacon_parser * restrict parser, const bacon_value * src)
{
  bacon_value *dst;

  dst = bacon_boolean_alloc(parser);
  dst->b = src->b;

  return dst;
}

static bacon_value *clone_integer(bacon_parser * restrict parser, const bacon_value * src)
{
  bacon_value *dst;

  dst = bacon_integer_alloc(parser);
  dst->i = src->i;

  return dst;
}

static bacon_value *clone_variable(bacon_parser * restrict parser, const bacon_value * src)
{
  bacon_value *dst;

  dst = bacon_variable_alloc(parser);
  pscatw(&dst->s, &dst->len, &dst->sa, src->s, src->len);

  return dst;
}

//
// api
//

bacon_value *bacon_clone(
    bacon_parser * restrict parser
  , bacon_value ** restrict dst
  , const bacon_value * restrict src
)
{
  bacon_value *v = 0;

  switch(src->type)
  {
    case BACON_TYPE_SET:
      v = clone_set(parser, src);
      break;
    case BACON_TYPE_LIST:
      v = clone_list(parser, src);
      break;
    case BACON_TYPE_MAPPING:
      v = clone_mapping(parser, src);
      break;
    case BACON_TYPE_STRING:
      v = clone_string(parser, src);
      break;
    case BACON_TYPE_FLOAT:
      v = clone_float(parser, src);
      break;
    case BACON_TYPE_BOOLEAN:
      v = clone_bool(parser, src);
      break;
    case BACON_TYPE_INTEGER:
      v = clone_integer(parser, src);
      break;
    case BACON_TYPE_VARIABLE:
      v = clone_variable(parser, src);
      break;
    default:
      RTA;
  }

  v->hash = src->hash;

  if(dst) {
    *dst = v;
  }

  return v;
}
