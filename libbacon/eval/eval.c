/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>

#include "valyria/pstring.h"
#include "valyria/hashtable.h"

#include "common/attrs.h"

#include "value.h"
#include "clone.h"
#include "eval.h"
#include "make.h"
#include "glue.h"
#include "coerce.h"
#include "set.h"
#include "merge.h"

#include "macros.h"

#include "narrator.h"

#define CASE2(x, y) x | (y << 8)
#define CASE3(x, y, z) x | (y << 8) | (z << 16)

static int eval(
    struct bacon_parser * restrict parser
  , bacon_expr * restrict expr
  , const bacon_value * restrict  vars
  , bacon_value ** restrict rv
);

static int eval_binary_logical(
    struct bacon_parser * restrict parser
  , bacon_expr * restrict expr
  , const bacon_value * restrict  vars
  , bacon_value ** restrict rv
)
{
  bacon_value *loper, *T0;
  bacon_value *roper, *T1;
  int r;

  eval(parser, expr->loper, vars, &T0);
  eval(parser, expr->roper, vars, &T1);

  bacon_value_coerce(parser, T0, BACON_TYPE_BOOLEAN, &loper);
  bacon_value_coerce(parser, T1, BACON_TYPE_BOOLEAN, &roper);

  r = 0;
  switch(expr->op) {
    case BACON_LOGAND:
      bacon_boolean_mk(parser, 0, rv, loper->b && roper->b);
      break;
    case BACON_LOGOR:
      bacon_boolean_mk(parser, 0, rv, loper->b || roper->b);
      break;
    default:
      r = -EINVAL;
  }

  bacon_parser_value_free(parser, T0);
  if(loper != T0) {
    bacon_parser_value_free(parser, loper);
  }
  bacon_parser_value_free(parser, T1);
  if(roper != T1) {
    bacon_parser_value_free(parser, roper);
  }

  return r;
}

static int eval_binary_relational(
    struct bacon_parser * restrict parser
  , bacon_expr * restrict expr
  , const bacon_value * restrict  vars
  , bacon_value ** restrict valp
)
{
  bacon_value *loper;
  bacon_value *roper;
  int r;
  int rv;

  eval(parser, expr->loper, vars, &loper);
  eval(parser, expr->roper, vars, &roper);

  r = bacon_value_cmp(loper, roper);
  rv = 0;
  switch(expr->op) {
    case BACON_LT:
      bacon_boolean_mk(parser, 0, valp, r < 0);
      break;
    case BACON_LE:
      bacon_boolean_mk(parser, 0, valp, r <= 0);
      break;
    case BACON_GT:
      bacon_boolean_mk(parser, 0, valp, r > 0);
      break;
    case BACON_GE:
      bacon_boolean_mk(parser, 0, valp, r >= 0);
      break;
    case BACON_EQ:
      bacon_boolean_mk(parser, 0, valp, r == 0);
      break;
    case BACON_NE:
      bacon_boolean_mk(parser, 0, valp, r != 0);
      break;
    default:
      rv = -EINVAL;
  }

  bacon_parser_value_free(parser, loper);
  bacon_parser_value_free(parser, roper);

  return rv;
}

static int eval_binary_arithmetic(
    struct bacon_parser * restrict parser
  , bacon_expr * restrict expr
  , const bacon_value * restrict  vars
  , bacon_value ** restrict rv
)
{
  bacon_value *loper, *T0;
  bacon_value *roper, *T1;
  uint32_t ltype;
  uint32_t rtype;
  int r = 0;

  eval(parser, expr->loper, vars, &T0);
  eval(parser, expr->roper, vars, &T1);

  /* promotions */
  if(T0->type == BACON_TYPE_STRING || T1->type == BACON_TYPE_STRING) {
    bacon_value_coerce(parser, T0, BACON_TYPE_STRING, &loper);
    bacon_value_coerce(parser, T1, BACON_TYPE_STRING, &roper);
  } else if(T0->type == BACON_TYPE_FLOAT || T1->type == BACON_TYPE_FLOAT) {
    bacon_value_coerce(parser, T0, BACON_TYPE_FLOAT, &loper);
    bacon_value_coerce(parser, T1, BACON_TYPE_FLOAT, &roper);
  } else {
    loper = T0;
    roper = T1;
  }

  switch(CASE2(expr->op, loper->type)) {
    /* ADD - numbers */
    case CASE2(BACON_ADD, BACON_TYPE_INTEGER):
      bacon_integer_mk(parser, 0, rv, loper->i + roper->i);
      goto end;
    case CASE2(BACON_ADD, BACON_TYPE_FLOAT):
      bacon_float_mk(parser, 0, rv, loper->f + roper->f);
      goto end;
    /* ADD - strings */
    case CASE2(BACON_ADD, BACON_TYPE_STRING):
      bacon_string_mkv(parser, 0, 0, rv, loper);
      bacon_string_mkv(parser, 0, *rv, rv, roper);
      goto end;
    /* SUB */
    case CASE2(BACON_SUB, BACON_TYPE_INTEGER):
      bacon_integer_mk(parser, 0, rv, loper->i - roper->i);
      goto end;
    case CASE2(BACON_SUB, BACON_TYPE_FLOAT):
      bacon_float_mk(parser, 0, rv, loper->f - roper->f);
      goto end;
    /* DIV */
    case CASE2(BACON_DIV, BACON_TYPE_INTEGER):
      bacon_integer_mk(parser, 0, rv, loper->i / roper->i);
      goto end;
    case CASE2(BACON_DIV, BACON_TYPE_FLOAT):
      bacon_float_mk(parser, 0, rv, loper->f / roper->f);
      goto end;
    /* MOD - integers */
    case CASE2(BACON_MOD, BACON_TYPE_INTEGER):
      bacon_integer_mk(parser, 0, rv, loper->i % roper->i);
      goto end;
    /* MUL */
    case CASE2(BACON_MUL, BACON_TYPE_INTEGER):
      bacon_integer_mk(parser, 0, rv, loper->i * roper->i);
      goto end;
    case CASE2(BACON_MUL, BACON_TYPE_FLOAT):
      bacon_float_mk(parser, 0, rv, loper->f * roper->f);
      goto end;
    /* SHL / SHR */
    case CASE2(BACON_SHL, BACON_TYPE_INTEGER):
      bacon_integer_mk(parser, 0, rv, loper->i << roper->i);
      goto end;
    case CASE2(BACON_SHR, BACON_TYPE_INTEGER):
      bacon_integer_mk(parser, 0, rv, loper->i >> roper->i);
      goto end;
  }

  ltype = loper->type & BACON_TYPE_CLASS;
  rtype = roper->type & BACON_TYPE_CLASS;
  switch(CASE3(expr->op, loper->type, rtype)) {
    case CASE3(BACON_ADD, BACON_TYPE_SET, BACON_TYPE_SCALAR):
      bacon_clone(parser, rv, loper);
      bacon_set_mkv(parser, 0, *rv, 0, bacon_clone(parser, 0, roper));
      goto end;
    case CASE3(BACON_ADD, BACON_TYPE_LIST, BACON_TYPE_SCALAR):
      bacon_clone(parser, rv, loper);
      bacon_list_mkv(parser, 0, *rv, 0, bacon_clone(parser, 0, roper));
      goto end;
  }
  switch(CASE3(expr->op, ltype, roper->type)) {
    case CASE3(BACON_ADD, BACON_TYPE_SCALAR, BACON_TYPE_SET):
      bacon_clone(parser, rv, roper);
      bacon_set_mkv(parser, 0, *rv, 0, bacon_clone(parser, 0, loper));
      goto end;
    case CASE3(BACON_ADD, BACON_TYPE_SCALAR, BACON_TYPE_LIST):
      bacon_clone(parser, rv, roper);
      bacon_list_mkv(parser, 0, *rv, 0, bacon_clone(parser, 0, loper));
      goto end;
  }

  r = -EINVAL;
end:
  bacon_parser_value_free(parser, T0);
  if(loper != T0) {
    bacon_parser_value_free(parser, loper);
  }
  bacon_parser_value_free(parser, T1);
  if(roper != T1) {
    bacon_parser_value_free(parser, roper);
  }

  return r;
}

static void set_bitand(struct bacon_parser * restrict parser, bacon_value * restrict dst, const bacon_value * restrict A, const bacon_value * restrict B)
{
  bacon_value *a;
  rbnode *rbn;

  /* items in A and in B */
  rbtree_foreach(&A->set, a, rbn) {
    if((rbn = rbtree_lookup(&B->set, a, bacon_set_key_cmp)) == 0) {
      continue;
    }

    rbtree_put(&dst->set, &bacon_clone(parser, 0, a)->rbn, bacon_set_rbn_cmp);
  }
}

static void set_bitxor(struct bacon_parser * restrict parser, bacon_value * restrict dst, const bacon_value * restrict A, const bacon_value * restrict B)
{
  bacon_value *v;
  rbnode *rbn;

  /* items in A not in B */
  rbtree_foreach(&A->set, v, rbn) {
    if((rbn = rbtree_lookup(&B->set, v, bacon_set_key_cmp))) {
      continue;
    }

    rbtree_put(&dst->set, &bacon_clone(parser, 0, v)->rbn, bacon_set_rbn_cmp);
  }

  /* items in B not in A */
  rbtree_foreach(&B->set, v, rbn) {
    if((rbn = rbtree_lookup(&B->set, v, bacon_set_key_cmp))) {
      continue;
    }

    rbtree_put(&dst->set, &bacon_clone(parser, 0, v)->rbn, bacon_set_rbn_cmp);
  }
}

static void list_bitand(struct bacon_parser * restrict parser, bacon_value * restrict dst, const bacon_value * restrict A, const bacon_value * restrict B)
{
  bacon_value *a;
  bacon_value *b;

  /* items in both A and B */
  a = llist_first(&A->list, typeof(*a), lln);
  b = llist_first(&B->list, typeof(*b), lln);
  while(a && b) {
    if(bacon_value_cmp(a, b) == 0) {
      llist_push(&dst->list, &bacon_clone(parser, 0, a)->lln);
    }
    a = llist_next(&A->list, a, lln);
    b = llist_next(&B->list, b, lln);
  }
}

static void list_bitxor(struct bacon_parser * restrict parser, bacon_value * restrict dst, const bacon_value * restrict A, const bacon_value * restrict B)
{
  bacon_value *a;
  bacon_value *b;

  /* items in a not in b */
  /* items in b not in a */
  a = llist_first(&A->list, typeof(*a), lln);
  b = llist_first(&B->list, typeof(*b), lln);
  while(a && b) {
    if(bacon_value_cmp(a, b) != 0) {
      llist_push(&dst->list, &bacon_clone(parser, 0, a)->lln);
      llist_push(&dst->list, &bacon_clone(parser, 0, b)->lln);
    }
    a = llist_next(&A->list, a, lln);
    b = llist_next(&B->list, b, lln);
  }
  while(a) {
    llist_push(&dst->list, &bacon_clone(parser, 0, a)->lln);
    a = llist_next(&A->list, a, lln);
  }
  while(b) {
    llist_push(&dst->list, &bacon_clone(parser, 0, b)->lln);
    b = llist_next(&B->list, b, lln);
  }
}

static int eval_binary_bitwise(
    struct bacon_parser * restrict parser
  , bacon_expr * restrict expr
  , const bacon_value * restrict  vars
  , bacon_value ** restrict rv
)
{
  bacon_value *loper;
  bacon_value *roper;
  int r;

  eval(parser, expr->loper, vars, &loper);
  eval(parser, expr->roper, vars, &roper);

  r = 0;
  switch(CASE3(expr->op, loper->type, roper->type))
  {
    /* integers */
    case CASE3(BACON_BITOR, BACON_TYPE_INTEGER, BACON_TYPE_INTEGER):
      bacon_integer_mk(parser, 0, rv, loper->i | roper->i);
      break;
    case CASE3(BACON_BITAND, BACON_TYPE_INTEGER, BACON_TYPE_INTEGER):
      bacon_integer_mk(parser, 0, rv, loper->i & roper->i);
      break;
    case CASE3(BACON_BITXOR, BACON_TYPE_INTEGER, BACON_TYPE_INTEGER):
      bacon_integer_mk(parser, 0, rv, loper->i ^ roper->i);
      break;

    /* sets */
    case CASE3(BACON_BITOR, BACON_TYPE_SET, BACON_TYPE_SET):
      bacon_set_mkv(parser, 0, 0, rv, 0);
      bacon_merge(parser, rv, loper);
      bacon_merge(parser, rv, roper);
      break;
    case CASE3(BACON_BITAND, BACON_TYPE_SET, BACON_TYPE_SET):
      bacon_set_mkv(parser, 0, 0, rv, 0);
      set_bitand(parser, *rv, loper, roper);
      break;
    case CASE3(BACON_BITXOR, BACON_TYPE_SET, BACON_TYPE_SET):
      bacon_set_mkv(parser, 0, 0, rv, 0);
      set_bitxor(parser, *rv, loper, roper);
      break;
    /* lists */
    case CASE3(BACON_BITOR, BACON_TYPE_LIST, BACON_TYPE_LIST):
      bacon_list_mkv(parser, 0, 0, rv, 0);
      bacon_merge(parser, rv, loper);
      bacon_merge(parser, rv, roper);
      break;
    case CASE3(BACON_BITAND, BACON_TYPE_LIST, BACON_TYPE_LIST):
      bacon_list_mkv(parser, 0, 0, rv, 0);
      list_bitand(parser, *rv, loper, roper);
      break;
    case CASE3(BACON_BITXOR, BACON_TYPE_LIST, BACON_TYPE_LIST):
      bacon_list_mkv(parser, 0, 0, rv, 0);
      list_bitxor(parser, *rv, loper, roper);
      break;
    default:
      r = -EINVAL;
  }

  bacon_parser_value_free(parser, loper);
  bacon_parser_value_free(parser, roper);

  return r;
}

static int eval_unary_bitwise(
    struct bacon_parser * restrict parser
  , bacon_expr * restrict expr
  , const bacon_value * restrict  vars
  , bacon_value ** restrict rv
)
{
  bacon_value *oper;
  int r;

  eval(parser, expr->oper, vars, &oper);

  r = 0;
  switch(CASE2(expr->op, oper->type)) {
    case CASE2(BACON_BITINV, BACON_TYPE_INTEGER):
      bacon_integer_mk(parser, 0, rv, ~oper->i);
      break;
    default:
      r = -EINVAL;
  }

  bacon_parser_value_free(parser, oper);
  return r;
}

static int eval_unary_logical(
    struct bacon_parser * restrict parser
  , bacon_expr * restrict expr
  , const bacon_value * restrict  vars
  , bacon_value ** restrict rv
)
{
  bacon_value *oper, *T = 0;

  eval(parser, expr->oper, vars, &T);

  /* promotions */
  bacon_value_coerce(parser, T, BACON_TYPE_BOOLEAN, &oper);
  bacon_boolean_mk(parser, 0, rv, !oper->b);

  bacon_parser_value_free(parser, T);
  if(oper != T) {
    bacon_parser_value_free(parser, oper);
  }

  return 0;
}

/*
 * evaluate an expression, always returns a newly allocated value
 */
static int eval(
    struct bacon_parser * restrict parser
  , bacon_expr * restrict expr
  , const bacon_value * restrict  vars
  , bacon_value ** restrict rv
)
{
  bacon_value *v;
  int r;

  switch(expr->op) {
    case BACON_LOGAND:
    case BACON_LOGOR:
      r = eval_binary_logical(parser, expr, vars, &v);
      break;
    case BACON_LT:
    case BACON_LE:
    case BACON_GT:
    case BACON_GE:
    case BACON_EQ:
    case BACON_NE:
      r = eval_binary_relational(parser, expr, vars, &v);
      break;
    case BACON_DIV:
    case BACON_MOD:
    case BACON_MUL:
    case BACON_ADD:
    case BACON_SUB:
    case BACON_SHL:
    case BACON_SHR:
      r = eval_binary_arithmetic(parser, expr, vars, &v);
      break;
    case BACON_BITOR:
    case BACON_BITAND:
    case BACON_BITXOR:
      r = eval_binary_bitwise(parser, expr, vars, &v);
      break;
    case BACON_BITINV:
      r = eval_unary_bitwise(parser, expr, vars, &v);
      break;
    case BACON_LOGINV:
      r = eval_unary_logical(parser, expr, vars, &v);
      break;
    case BACON_VALUE:
      v = expr->val;
      r = 0;
      if(v && v->type == BACON_TYPE_VARIABLE && vars) {
        v = bacon_set_lookupw(vars, v->s, v->len);
      }
      if(v) {
        v = bacon_clone(parser, 0, v);
      }
      break;
    default:
      r = -EINVAL;
  }

  if(r == 0) {
    *rv = v;
  }

  return r;
}

int bacon_eval(
    bacon_parser * restrict parser
  , bacon_expr * restrict expr
  , const bacon_value * restrict  vars
  , bacon_value ** restrict rv
)
{
  return eval(parser, expr, vars, rv);
}
