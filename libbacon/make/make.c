/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "valyria/pstring.h"
#include "common/hash.h"

#include "make.h"
#include "glue.h"
#include "set.h"
#include "location.h"


bacon_value * bacon_float_mk(
    bacon_parser * const restrict parser
  , const yyu_location * const restrict loc
  , struct bacon_value ** rv
  , double fp
)
{
  bacon_value * e = 0;
  e = bacon_float_alloc(parser);

  if(loc) {
    bacon_location_clone(&e->loc, loc, parser);
  }

  e->f = fp;
  e->hash = hash32(e->type, &e->f, sizeof(e->f));
  if(rv) {
    *rv = e;
  }
  return e;
}

bacon_value *bacon_integer_mk(
    bacon_parser * const restrict parser
  , const yyu_location * const restrict loc
  , struct bacon_value ** rv
  , int128_t i
)
{
  bacon_value * e = 0;
  e = bacon_integer_alloc(parser);

  if(loc) {
    bacon_location_clone(&e->loc, loc, parser);
  }

  e->i = i;
  e->hash = hash32(e->type, &e->i, sizeof(e->i));
  if(rv) {
    *rv = e;
  }
  return e;
}

struct bacon_value *bacon_boolean_mk(
    bacon_parser * const restrict parser
  , const yyu_location * const restrict loc
  , struct bacon_value ** rv
  , int b
)
{
  bacon_value * e;

  e = bacon_boolean_alloc(parser);
  e->hash = e->type;

  if(loc) {
    bacon_location_clone(&e->loc, loc, parser);
  }

  e->b = !!b;
  e->hash = hash32(e->hash, &e->b, sizeof(e->b));
  if(rv) {
    *rv = e;
  }

  return e;
}

bacon_value * bacon_string_mk(
    bacon_parser * const restrict parser
  , const yyu_location * const restrict loc
  , struct bacon_value * e
  , struct bacon_value ** rv
)
{
  if(!e) {
    e = bacon_string_alloc(parser);
    e->hash = e->type;
  }
  if(loc) {
    bacon_location_clone(&e->loc, loc, parser);
  }

  if(rv) {
    *rv = e;
  }
  return e;
}

bacon_value * bacon_string_mks(
    bacon_parser * const restrict parser
  , const yyu_location * const restrict loc
  , struct bacon_value * e
  , struct bacon_value ** rv
  , const char * const restrict s
)
{
  size_t sl;

  if(!e) {
    e = bacon_string_alloc(parser);
    e->hash = e->type;
  }
  if(loc) {
    bacon_location_clone(&e->loc, loc, parser);
  }

  sl = strlen(s);
  pscatw(&e->s, &e->len, &e->sa, s, sl);
  e->hash = hash32(e->hash, s, sl);

  if(rv) {
    *rv = e;
  }
  return e;
}

bacon_value * bacon_string_mkw(
    bacon_parser * const restrict parser
  , const yyu_location * const restrict loc
  , struct bacon_value * e
  , struct bacon_value ** rv
  , const char * const restrict b
  , size_t l
)
{
  if(!e) {
    e = bacon_string_alloc(parser);
    e->hash = e->type;
  }
  if(loc) {
    bacon_location_clone(&e->loc, loc, parser);
  }

  pscatw(&e->s, &e->len, &e->sa, b, l);
  e->hash = hash32(e->hash, b, l);

  if(rv) {
    *rv = e;
  }
  return e;
}

bacon_value * bacon_string_mkc(
    bacon_parser * const restrict parser
  , const yyu_location * const restrict loc
  , struct bacon_value * e
  , struct bacon_value ** rv
  , uint8_t c
)
{
  if(!e) {
    e = bacon_string_alloc(parser);
    e->hash = e->type;
  }
  if(loc) {
    bacon_location_clone(&e->loc, loc, parser);
  }

  pscatc(&e->s, &e->len, &e->sa, c);
  e->hash = hash32(e->hash, &c, sizeof(c));

  if(rv) {
    *rv = e;
  }
  return e;
}

bacon_value * bacon_string_mkv(
    bacon_parser * const restrict parser
  , const yyu_location * const restrict loc
  , struct bacon_value * e
  , struct bacon_value ** rv
  , struct bacon_value * v
)
{
  if(!e) {
    e = bacon_string_alloc(parser);
    e->hash = e->type;
  }
  if(loc) {
    bacon_location_clone(&e->loc, loc, parser);
  }

  pscatw(&e->s, &e->len, &e->sa, v->s, v->len);
  e->hash = hash32(e->type, v->s, v->len);

  if(rv) {
    *rv = e;
  }
  return e;
}

bacon_value * bacon_string_mkf(
    bacon_parser * const restrict parser
  , const yyu_location * const restrict loc
  , struct bacon_value * e
  , struct bacon_value ** rv
  , const char * restrict fmt
  , ...
)
{
  va_list va;
  size_t prev;

  if(!e) {
    e = bacon_string_alloc(parser);
    e->hash = e->type;
  }
  if(loc) {
    bacon_location_clone(&e->loc, loc, parser);
  }

  prev = e->len;

  va_start(va, fmt);
  pscatvf(&e->s, &e->len, &e->sa, fmt, va);
  va_end(va);
  e->hash = hash32(e->hash, &e->s[prev], e->len - prev);

  if(rv) {
    *rv = e;
  }
  return e;
}

bacon_value * bacon_mapping_mk(
    bacon_parser * const restrict parser
  , const yyu_location * const restrict loc
  , struct bacon_value ** rv
  , struct bacon_value * k
  , struct bacon_value * v
)
{
  bacon_value * e = 0;

  e = bacon_mapping_alloc(parser);
  if(loc) {
    bacon_location_clone(&e->loc, loc, parser);
  }

  e->key = k;
  e->val = v;

  e->hash = 0;
  e->hash = hash32(e->hash, &e->key->hash, sizeof(e->key->hash));
  e->hash = hash32(e->hash, &e->val->hash, sizeof(e->val->hash));

  if(rv) {
    *rv = e;
  }
  return e;
}

bacon_value * bacon_list_mkv(
    bacon_parser * const restrict parser
  , const yyu_location * const restrict loc
  , struct bacon_value * e
  , struct bacon_value ** rv
  , struct bacon_value * el
)
{
  if(!e) {
    e = bacon_list_alloc(parser);
    e->hash = e->type;
  }
  if(loc) {
    bacon_location_clone(&e->loc, loc, parser);
  }

  if(el) {
    llist_push(&e->list, &el->lln);
    e->hash += el->hash;
  }

  if(rv) {
    *rv = e;
  }
  return e;
}

bacon_value * bacon_set_mkv(
    bacon_parser * const restrict parser
  , const yyu_location * const restrict loc
  , struct bacon_value * e
  , struct bacon_value ** rv
  , struct bacon_value * v
)
{
  rbnode *rbn;
  bacon_value *old;

  if(!e) {
    e = bacon_set_alloc(parser);
    e->hash = e->type;
  }
  if(loc) {
    bacon_location_clone(&e->loc, loc, parser);
  }

  if(v) {
    if((rbn = rbtree_put(&e->set, &v->rbn, bacon_set_rbn_cmp))) {
      /* overwrite the existing item */
      old = containerof(rbn, typeof(*v), rbn);
      rbtree_replace(&e->set, &old->rbn, &v->rbn);
      e->hash -= old->hash;
      bacon_parser_value_free(parser, old);
    }
    e->hash += v->hash;
  }

  if(rv) {
    *rv = e;
  }
  return e;
}

bacon_value * bacon_variable_mkw(
    bacon_parser * const restrict parser
  , const yyu_location * const restrict loc
  , struct bacon_value ** rv
  , const char * const restrict name
  , uint16_t len
)
{
  bacon_value *e = 0;

  e = bacon_variable_alloc(parser);
  e->hash = e->type;

  if(loc) {
    bacon_location_clone(&e->loc, loc, parser);
  }

  psloadw(&e->s, &e->len, &e->sa, name, len);
  e->hash = hash32(e->hash, e->s, e->len);

  if(rv) {
    *rv = e;
  }
  return e;
}

bacon_expr *bacon_expr_mk(
    struct bacon_parser * const restrict parser
  , const struct yyu_location * const restrict loc
  , bacon_expr **rv
  , bacon_operation op
  , bacon_expr *loper
  , bacon_expr *roper
)
{
  bacon_expr *e = 0;

  e = bacon_expr_alloc(parser);

  if(loc) {
    bacon_location_clone(&e->loc, loc, parser);
  }

  e->op = op;
  e->loper = loper;
  e->roper = roper;

  if(rv) {
    *rv = e;
  }
  return e;
}

bacon_expr *bacon_expr_wrap(
    struct bacon_parser * const restrict parser
  , bacon_value *v
  , bacon_expr ** rv
)
{
  bacon_expr *e = 0;

  e = bacon_expr_alloc(parser);
  e->op = BACON_VALUE;
  e->val = v;

  if(rv) {
    *rv = e;
  }
  return e;
}
