/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <inttypes.h>

#include "types.h"

#include "xlinux/xstdlib.h"
#include "bacon/value.h"
#include "bacon/writer.h"
#include "bacon/builder.h"
#include "yyutil/parser.h"
#include "valyria/hashtable.h"
#include "valyria/pstring.h"

#include "formula_value.internal.h"
#include "formula.tokens.h"
#include "exec.h"
#include "exec_builder.h"
#include "exec_render.h"
#include "selector.h"
#include "sysvar.h"
#include "build_slot.h"
#include "module.h"
#include "selection.h"
#include "formula.h"

#include "common/attrs.h"

attrs32 * formula_value_attrs = (attrs32[]) {{
#undef DEF
#define DEF(x, s, r, y) + 1
    num : 0
      FORMULA_VALUE_TABLE
  , members : (member32[]) {
#undef DEF
#define DEF(x, s, r, y) { name : s, value : UINT32_C(y), range : UINT32_C(r) },
FORMULA_VALUE_TABLE
  }
}};

static void __attribute__((constructor)) init()
{
  attrs32_init(formula_value_attrs);
}

//
// static
//

static void allocate(formula_value ** restrict pv, formula_value_type type)
{
  formula_value *v;

  xmalloc(&v, sizeof(*v));
  v->type = type;

  llist_init(&v->lln);
  rbnode_init(&v->rbn);

  *pv = v;
}

static void writer_write(const formula_value * restrict val, bacon_writer * const restrict builder_writer, bool top)
{
  formula_value *sv;

  switch(val->type) {
    case FORMULA_VALUE_LIST:
      { LIST
        llist_foreach(&val->list, sv, lln) {
          writer_write(sv, builder_writer, false);
        }
      }
      break;
    case FORMULA_VALUE_SET:
      { SET
        rbtree_foreach(&val->set, sv, rbn) {
          writer_write(sv, builder_writer, top);
        }
      }
      break;
    case FORMULA_VALUE_MAPPING:
      { MAPPING
        BYTES(val->m.name, val->m.name_len);
        writer_write(val->m.value, builder_writer, top);
      }
      break;
    case FORMULA_VALUE_STRING:
      STRING(val->s);
      break;
    case FORMULA_VALUE_FLOAT:
      FLOAT(val->f);
      break;
    case FORMULA_VALUE_BOOLEAN:
      BOOL(val->b);
      break;
    case FORMULA_VALUE_POSINT:
      UINT(val->u);
      break;
    case FORMULA_VALUE_NEGINT:
      INT(val->i);
      break;
    case FORMULA_VALUE_VARIABLE:
      VARIABLEW(val->v.name, val->v.name_len);
      break;
    case FORMULA_VALUE_SELECT:
      { MAPPING
        STRING("select");
        { LIST
          selector_writer_write(val->op.selector, builder_writer);
        }
      }
      break;
    case FORMULA_VALUE_PROPERTY:
      { MAPPING
        STRING("property");
        STRING(attrs32_name_byvalue(fsent_property_attrs, FSENT_PROPERTY_OPT & val->op.property));
      }
      break;
    case FORMULA_VALUE_PREPEND:
      { MAPPING
        STRING("prepend");
        writer_write(val->op.operand, builder_writer, false);
      }
      break;
    case FORMULA_VALUE_PATH_SEARCH:
      { MAPPING
        STRING("path-search");
        writer_write(val->op.operand, builder_writer, false);
      }
      break;
    case FORMULA_VALUE_SEQUENCE:
      if(!top || llist_count(&val->op.list) != 1)
      {
        bacon_writer_push_mapping(builder_writer);
        bacon_writer_string(builder_writer, "sequence");
      }

      { LIST
        llist_foreach(&val->op.list, sv, lln) {
          writer_write(sv, builder_writer, false);
        }
      }

      if(!top || llist_count(&val->op.list) != 1)
      {
        bacon_writer_pop_mapping(builder_writer);
      }
      break;
    default:
      RTA;
  }
}

//
// tracing
//

void formula_value_say(const formula_value * restrict fv, struct narrator * restrict N)
{
  bacon_writer writer;

  bacon_writer_init(&writer);
  bacon_writer_open(&writer, N);
  writer_write(fv, &writer, true);
  bacon_writer_close(&writer);

  bacon_writer_destroy(&writer);
}

//
// internal
//

void formula_value_posint_mk(
    const yyu_location * restrict loc
  , formula_value ** rv
  , uint64_t u
)
{
  formula_value *v;

  allocate(&v, FORMULA_VALUE_POSINT);
  v->u = u;

  *rv = v;
}

void formula_value_negint_mk(
    const yyu_location * restrict loc
  , formula_value ** rv
  , int64_t i
)
{
  formula_value *v;

  allocate(&v, FORMULA_VALUE_NEGINT);
  v->i = i;

  *rv = v;
}

void formula_value_float_mk(
    const yyu_location * restrict loc
  , formula_value ** rv
  , float fp
)
{
  formula_value *v;

  allocate(&v, FORMULA_VALUE_FLOAT);
  v->f = fp;

  *rv = v;
}

void formula_value_boolean_mk(
    const yyu_location * restrict loc
  , formula_value ** rv
  , bool b
)
{
  formula_value *v;

  allocate(&v, FORMULA_VALUE_BOOLEAN);
  v->b = b;

  *rv = v;
}

void formula_value_string_mkc(
    const struct yyu_location * restrict loc
  , struct formula_value * e
  , struct formula_value ** rv
  , char c
)
{
  if(!e) {
    allocate(&e, FORMULA_VALUE_STRING);
  }

  pscatc(&e->s, &e->len, &e->sa, c);

  if(rv) {
    *rv = e;
  }
}

void formula_value_string_mkv(
    const struct yyu_location * restrict loc
  , struct formula_value * e
  , struct formula_value ** rv
  , struct formula_value * el
)
{
  if(!e) {
    allocate(&e, FORMULA_VALUE_STRING);
  }

  pscatw(&e->s, &e->len, &e->sa, el->s, el->len);

  if(rv) {
    *rv = e;
  }
}

void formula_value_string_mkw(
    const yyu_location * restrict loc
  , formula_value * e
  , formula_value ** rv
  , const char * restrict s
  , uint16_t l
)
{
  if(!e) {
    allocate(&e, FORMULA_VALUE_STRING);
  }
  pscatw(&e->s, &e->len, &e->sa, s, l);

  if(rv) {
    *rv = e;
  }
}

void formula_value_variable_mk(
    const yyu_location * restrict loc
  , formula_value ** rv
  , const char * restrict name
  , uint8_t len
)
{
  formula_value *v;

  allocate(&v, FORMULA_VALUE_VARIABLE);
  len = MIN(len, sizeof(v->v.name) - 1);
  memcpy(v->v.name, name, len);
  v->v.name[len] = 0;
  v->v.name_len = len;

  *rv = v;
}

void formula_value_mapping_mk(
    const yyu_location * restrict loc
  , formula_value ** rv
  , const char * name
  , uint8_t len
  , formula_value * restrict value
)
{
  formula_value *v;

  allocate(&v, FORMULA_VALUE_MAPPING);
  len = MIN(len, sizeof(v->m.name) - 1);
  memcpy(v->m.name, name, len);
  v->m.name[len] = 0;
  v->m.name_len = len;
  v->m.value = value;

  *rv = v;
}

void formula_value_list_mkv(
    const yyu_location * restrict loc
  , formula_value * e
  , formula_value ** rv
  , formula_value * el
)
{
  if(!e) {
    allocate(&e, FORMULA_VALUE_LIST);
    llist_init(&e->list);
  }
  if(el) {
    llist_push(&e->list, &el->lln);
  }
  if(rv) {
    *rv = e;
  }
}

void formula_value_set_mkv(
    const yyu_location * restrict loc
  , formula_value * e
  , formula_value ** rv
  , formula_value * el
)
{
  rbnode *rbn;
  formula_value *old;

  if(!e) {
    allocate(&e, FORMULA_VALUE_SET);
    rbtree_init(&e->set);
  }
  if(el) {
    if((rbn = rbtree_put(&e->set, &el->rbn, fmlval_rbn_cmp))) {
      /* overwrite existing item */
      old = containerof(rbn, typeof(*old), rbn);
      rbtree_replace(&e->set, &old->rbn, &el->rbn);
      formula_value_free(old);
    }
  }
  if(rv) {
    *rv = e;
  }
}

void formula_value_property_mk(
    const yyu_location * restrict loc
  , formula_value ** rv
  , fsent_property property
)
{
  formula_value *v;

  allocate(&v, FORMULA_VALUE_PROPERTY);
  v->op.property = property;

  *rv = v;
}

void formula_value_select_mk(
    const yyu_location * restrict loc
  , formula_value ** rv
  , selector * restrict select
)
{
  formula_value *v;

  allocate(&v, FORMULA_VALUE_SELECT);
  v->op.selector = select;

  *rv = v;
}

void formula_value_prepend_mk(
    const yyu_location * restrict loc
  , formula_value ** rv
  , formula_value * restrict val
)
{
  formula_value *v;

  allocate(&v, FORMULA_VALUE_PREPEND);
  v->op.operand = val;

  *rv = v;
}

void formula_value_path_search_mk(
    const yyu_location * restrict loc
  , formula_value ** rv
  , formula_value * restrict val
)
{
  formula_value *v;

  allocate(&v, FORMULA_VALUE_PATH_SEARCH);
  v->op.operand = val;

  *rv = v;
}

void formula_value_sequence_mkv(
    const yyu_location * restrict loc
  , formula_value * e
  , formula_value ** rv
  , formula_value * el
)
{
  if(!e) {
    allocate(&e, FORMULA_VALUE_SEQUENCE);
    llist_init(&e->op.list);
  }
  if(el) {
    llist_push(&e->op.list, &el->lln);
  }
  if(rv) {
    *rv = e;
  }
}

//
// public
//

static void set_free(rbtree * restrict rbt)
{
  formula_value *sv;
  llist list, *tmp;

  if(!rbt) {
    return;
  }

  llist_init(&list);
  rbtree_foreach(rbt, sv, rbn) {
    llist_append(&list, &sv->lln);
  }

  llist_foreach_safe(&list, sv, lln, tmp) {
    formula_value_free(sv);
  }
}

void formula_value_free(formula_value * restrict v)
{
  llist *T;
  formula_value *sv;

  if(!v) {
    return;
  }

  switch(v->type)
  {
    case FORMULA_VALUE_LIST:
      llist_foreach_safe(&v->list, sv, lln, T) {
        formula_value_free(sv);
      }
      break;
    case FORMULA_VALUE_SET:
      set_free(&v->set);
      break;
    case FORMULA_VALUE_STRING:
      free(v->s);
      break;
    case FORMULA_VALUE_MAPPING:
      formula_value_free(v->m.value);
      break;
    case FORMULA_VALUE_SELECT:
      selector_free(v->op.selector);
      break;
    case FORMULA_VALUE_PREPEND:
      formula_value_free(v->op.operand);
      break;
    case FORMULA_VALUE_PATH_SEARCH:
      formula_value_free(v->op.operand);
      break;
    case FORMULA_VALUE_SEQUENCE:
      llist_foreach_safe(&v->op.list, sv, lln, T) {
        formula_value_free(sv);
      }
      break;
    case FORMULA_VALUE_FLOAT:
    case FORMULA_VALUE_BOOLEAN:
    case FORMULA_VALUE_POSINT:
    case FORMULA_VALUE_NEGINT:
    case FORMULA_VALUE_VARIABLE:
    case FORMULA_VALUE_PROPERTY:
      break;
      RTA;
  }

  free(v);
}

void formula_value_ifree(formula_value ** restrict v)
{
  formula_value_free(*v);
  *v = 0;
}

int fmlval_rbn_cmp(const rbnode * restrict a, const rbnode * restrict b)
{
  formula_value *A = containerof(a, formula_value, rbn);
  formula_value *B = containerof(b, formula_value, rbn);

  RUNTIME_ASSERT(A->type == FORMULA_VALUE_MAPPING);
  RUNTIME_ASSERT(B->type == FORMULA_VALUE_MAPPING);

  return memncmp(A->m.name, A->m.name_len, B->m.name, B->m.name_len);
}

void formula_value_render(const formula_value * restrict v, struct narrator * restrict N)
{
  formula_value * sv;

  if(v->type == FORMULA_VALUE_LIST)
  {
    narrator_says(N, "[");

    llist_foreach(&v->list, sv, lln) {
      narrator_says(N, " ");
      formula_value_render(sv, N);
    }

    if(llist_count(&v->list) != 0) {
      narrator_says(N, " ");
    }

    narrator_says(N, "]");
  }
  else if(v->type == FORMULA_VALUE_SET)
  {
    narrator_says(N, "{");

    rbtree_foreach(&v->set, sv, rbn) {
      narrator_says(N, " ");
      formula_value_render(sv, N);
    }

    if(!rbtree_empty(&v->set)) {
      narrator_says(N, " ");
    }

    narrator_says(N, "}");
  }
  else if(v->type == FORMULA_VALUE_MAPPING)
  {
    narrator_sayw(N, v->m.name, v->m.name_len);
    narrator_says(N, " : ");
    formula_value_render(v->m.value, N);
  }
  else if(v->type == FORMULA_VALUE_STRING)
  {
    narrator_says(N, v->s);
  }
  else if(v->type == FORMULA_VALUE_FLOAT)
  {
    narrator_sayf(N, "%.2lf", v->f);
  }
  else if(v->type == FORMULA_VALUE_BOOLEAN)
  {
    narrator_sayf(N, "%s", v->b ? "true" : "false");
  }
  else if(v->type == FORMULA_VALUE_POSINT)
  {
    narrator_sayf(N, "%"PRIu64, v->u);
  }
  else if(v->type == FORMULA_VALUE_NEGINT)
  {
    narrator_sayf(N, "%"PRId64, v->i);
  }
}
