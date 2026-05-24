/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "xlinux/xstdlib.h"
#include "common/attrs.h"

#include "parser.h"
#include "expr.h"
#include "location.h"

attrs32 bacon_operation_attrs = (attrs32){
#undef DEF
#define DEF(x, y) + 1
    num : 0 BACON_OPERATION_TABLE
  , members : (member32[]) {
#undef DEF
#define DEF(x, y) { name : #x, value : UINT32_C(y), range : BACON_OPERATION_OPT },
BACON_OPERATION_TABLE
  }
};

static void __attribute__((constructor)) init()
{
  attrs32_init(&bacon_operation_attrs);
}

bacon_expr *bacon_expr_alloc(bacon_parser * restrict parser)
{
  bacon_expr * expr = 0;

  if(!(expr = llist_shift(&parser->expr_freelist, typeof(*expr), lln))) {
    xmalloc(&expr, sizeof(*expr));
    llist_init(&expr->lln);
  } else {
    expr->loc = 0;
  }

  return expr;
}

void bacon_expr_free(bacon_parser * restrict parser, struct bacon_expr * restrict e)
{
  if(!e) {
    return;
  }

  switch(e->op) {
    case BACON_LOGAND:
    case BACON_LOGOR:
    case BACON_LT:
    case BACON_LE:
    case BACON_GT:
    case BACON_GE:
    case BACON_EQ:
    case BACON_NE:
    case BACON_DIV:
    case BACON_MOD:
    case BACON_MUL:
    case BACON_ADD:
    case BACON_SUB:
    case BACON_SHL:
    case BACON_SHR:
    case BACON_BITOR:
    case BACON_BITAND:
    case BACON_BITXOR:
      bacon_expr_free(parser, e->loper);
      bacon_expr_free(parser, e->roper);
      break;
    case BACON_BITINV:
    case BACON_LOGINV:
      bacon_expr_free(parser, e->oper);
      break;
    case BACON_VALUE:
      bacon_value_free(parser, e->val);
      break;
  }

  if(e->loc) {
    bacon_location_free(parser, e->loc);
  }
  llist_push(&parser->expr_freelist, &e->lln);
}
