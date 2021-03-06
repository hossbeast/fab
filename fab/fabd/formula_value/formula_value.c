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

#include <inttypes.h>

#include "types.h"
#include "xapi.h"

#include "xlinux/xstdlib.h"
#include "value.h"
#include "value/writer.h"
#include "yyutil/parser.h"
#include "valyria/map.h"
#include "valyria/list.h"
#include "valyria/set.h"

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
#include "path_cache.h"

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

attrs32 * formula_sysvar_attrs = (attrs32[]) {{
#undef DEF
#define DEF(x, s, r, y) + 1
    num : 0
      FORMULA_SYSVAR_TABLE
  , members : (member32[]) {
#undef DEF
#define DEF(x, s, r, y) { name : s, value : UINT32_C(y), range : UINT32_C(r) },
FORMULA_SYSVAR_TABLE
  }
}};

static void __attribute__((constructor)) init()
{
  attrs32_init(formula_value_attrs);
  attrs32_init(formula_sysvar_attrs);
}

//
// static
//

static xapi allocate(formula_value ** restrict v, formula_value_type type)
{
  enter;

  fatal(xmalloc, v, sizeof(**v));
  (*v)->type = type;

  finally : coda;
}

static xapi writer_write(const formula_value * restrict val, value_writer * const restrict writer, bool top)
{
  enter;

  const chain *T;
  formula_value *sv;
  const char *name;

  if(val->type == FORMULA_VALUE_LIST)
  {
    fatal(value_writer_push_list, writer);

    chain_foreach(T, sv, chn, val->list_head) {
      fatal(writer_write, sv, writer, false);
    }

    fatal(value_writer_pop_list, writer);
  }
  else if(val->type == FORMULA_VALUE_SET)
  {
    fatal(value_writer_push_set, writer);

    rbtree_foreach(val->set, sv, rbn) {
      fatal(writer_write, sv, writer, top);
    }

    fatal(value_writer_pop_set, writer);
  }
  else if(val->type == FORMULA_VALUE_MAPPING)
  {
    fatal(value_writer_push_mapping, writer);
      fatal(value_writer_bytes, writer, val->m.name, val->m.name_len);
      fatal(writer_write, val->m.value, writer, top);
    fatal(value_writer_pop_mapping, writer);
  }
  else if(val->type == FORMULA_VALUE_STRING)
  {
    fatal(value_writer_string, writer, val->s);
  }
  else if(val->type == FORMULA_VALUE_FLOAT)
  {
    fatal(value_writer_float, writer, val->f);
  }
  else if(val->type == FORMULA_VALUE_BOOLEAN)
  {
    fatal(value_writer_bool, writer, val->b);
  }
  else if(val->type == FORMULA_VALUE_POSINT)
  {
    fatal(value_writer_uint, writer, val->u);
  }
  else if(val->type == FORMULA_VALUE_NEGINT)
  {
    fatal(value_writer_int, writer, val->i);
  }
  else if(val->type == FORMULA_VALUE_VARIABLE)
  {
    fatal(value_writer_variable, writer, val->v.name, val->v.name_len);
  }
  else if(val->type == FORMULA_VALUE_SYSVAR)
  {
    name = attrs32_name_byvalue(formula_sysvar_attrs, FORMULA_SYSVAR_OPT & val->sv);
    fatal(value_writer_variable, writer, MMS(name));
  }
  else if(val->type == FORMULA_VALUE_SELECT)
  {
    fatal(value_writer_push_mapping, writer);
      fatal(value_writer_string, writer, "select");
      fatal(value_writer_push_list, writer);
      fatal(selector_writer_write, val->op.selector, writer);
      fatal(value_writer_pop_list, writer);
    fatal(value_writer_pop_mapping, writer);
  }
  else if(val->type == FORMULA_VALUE_PROPERTY)
  {
    fatal(value_writer_push_mapping, writer);
      fatal(value_writer_string, writer, "property");
      fatal(value_writer_string, writer, attrs32_name_byvalue(fsent_property_attrs, FSENT_PROPERTY_OPT & val->op.property));
    fatal(value_writer_pop_mapping, writer);
  }
  else if(val->type == FORMULA_VALUE_PREPEND)
  {
    fatal(value_writer_push_mapping, writer);
      fatal(value_writer_string, writer, "prepend");
      fatal(writer_write, val->op.operand, writer, false);
    fatal(value_writer_pop_mapping, writer);
  }
  else if(val->type == FORMULA_VALUE_PATH_SEARCH)
  {
    fatal(value_writer_push_mapping, writer);
      fatal(value_writer_string, writer, "path-search");
      fatal(writer_write, val->op.operand, writer, false);
    fatal(value_writer_pop_mapping, writer);
  }
  else if(val->type == FORMULA_VALUE_SEQUENCE)
  {
    if(!top || chain_count(val->op.list_head, chn) != 1)
    {
      fatal(value_writer_push_mapping, writer);
      fatal(value_writer_string, writer, "sequence");
    }
    fatal(value_writer_push_list, writer);

    chain_foreach(T, sv, chn, val->op.list_head) {
      fatal(writer_write, sv, writer, false);
    }

    fatal(value_writer_pop_list, writer);
    if(!top || chain_count(val->op.list_head, chn) != 1)
    {
      fatal(value_writer_pop_mapping, writer);
    }
  }
  else
  {
    RUNTIME_ABORT();
  }

  finally : coda;
}

//
// tracing
//

xapi formula_value_say(const formula_value * restrict fv, struct narrator * restrict N)
{
  enter;

  value_writer writer;

  value_writer_init(&writer);
  fatal(value_writer_open, &writer, N);
  fatal(writer_write, fv, &writer, true);
  fatal(value_writer_close, &writer);

finally:
  fatal(value_writer_destroy, &writer);
coda;
}

//
// internal
//

xapi formula_value_posint_mk(
    const yyu_location * restrict loc
  , formula_value ** rv
  , uint64_t u
)
{
  enter;

  formula_value *v;

  fatal(allocate, &v, FORMULA_VALUE_POSINT);
  v->u = u;

  *rv = v;

  finally : coda;
}

xapi formula_value_negint_mk(
    const yyu_location * restrict loc
  , formula_value ** rv
  , int64_t i
)
{
  enter;

  formula_value *v;

  fatal(allocate, &v, FORMULA_VALUE_NEGINT);
  v->i = i;

  *rv = v;

  finally : coda;
}

xapi formula_value_float_mk(
    const yyu_location * restrict loc
  , formula_value ** rv
  , float fp
)
{
  enter;

  formula_value *v;

  fatal(allocate, &v, FORMULA_VALUE_FLOAT);
  v->f = fp;

  *rv = v;

  finally : coda;
}

xapi formula_value_boolean_mk(
    const yyu_location * restrict loc
  , formula_value ** rv
  , bool b
)
{
  enter;

  formula_value *v;

  fatal(allocate, &v, FORMULA_VALUE_BOOLEAN);
  v->b = b;

  *rv = v;

  finally : coda;
}

xapi formula_value_string_mk(
    const yyu_location * restrict loc
  , formula_value ** rv
  , char * restrict s
)
{
  enter;

  formula_value *v;

  fatal(allocate, &v, FORMULA_VALUE_STRING);
  v->s = s;

  *rv = v;

  finally : coda;
}

xapi formula_value_variable_mk(
    const yyu_location * restrict loc
  , formula_value ** rv
  , const char * restrict name
  , uint8_t len
)
{
  enter;

  formula_value *v;

  fatal(allocate, &v, FORMULA_VALUE_VARIABLE);
  len = MIN(len, sizeof(v->v.name) - 1);
  memcpy(v->v.name, name, len);
  v->v.name[len] = 0;
  v->v.name_len = len;

  *rv = v;

  finally : coda;
}

xapi formula_value_sysvar_mk(
    const yyu_location * restrict loc
  , formula_value ** rv
  , uint16_t token
)
{
  enter;

  formula_value *v;

  fatal(allocate, &v, FORMULA_VALUE_SYSVAR);

  if(token == formula_BM_SOURCE)
    v->sv = FORMULA_SYSVAR_SOURCE;
  else if(token == formula_BM_SOURCES)
    v->sv = FORMULA_SYSVAR_SOURCES;
  else if(token == formula_BM_TARGET)
    v->sv = FORMULA_SYSVAR_TARGET;
  else if(token == formula_BM_TARGETS)
    v->sv = FORMULA_SYSVAR_TARGETS;
  else if(token == formula_BM_VARIANT)
    v->sv = FORMULA_SYSVAR_VARIANT;

  *rv = v;

  finally : coda;
}

xapi formula_value_mapping_mk(
    const yyu_location * restrict loc
  , formula_value ** rv
  , const char * name
  , uint8_t len
  , formula_value * restrict value
)
{
  enter;

  formula_value *v;

  fatal(allocate, &v, FORMULA_VALUE_MAPPING);
  len = MIN(len, sizeof(v->m.name) - 1);
  memcpy(v->m.name, name, len);
  v->m.name[len] = 0;
  v->m.name_len = len;
  v->m.value = value;

  *rv = v;

  finally : coda;
}

xapi formula_value_list_mk(
    const yyu_location * restrict loc
  , formula_value ** restrict rv
  , formula_value * restrict list_head
)
{
  enter;

  formula_value *v;

  fatal(allocate, &v, FORMULA_VALUE_LIST);
  v->list_head = list_head;

  *rv = v;

  finally : coda;
}

xapi formula_value_set_mk(
    const yyu_location * restrict loc
  , formula_value ** restrict rv
  , rbtree * restrict rbt
)
{
  enter;

  formula_value *v;

  fatal(allocate, &v, FORMULA_VALUE_SET);
  v->set = rbt;

  *rv = v;

  finally : coda;
}

xapi formula_value_property_mk(
    const yyu_location * restrict loc
  , formula_value ** rv
  , fsent_property property
)
{
  enter;

  formula_value *v;

  fatal(allocate, &v, FORMULA_VALUE_PROPERTY);
  v->op.property = property;

  *rv = v;

  finally : coda;
}

xapi formula_value_select_mk(
    const yyu_location * restrict loc
  , formula_value ** rv
  , selector * restrict select
)
{
  enter;

  formula_value *v;

  fatal(allocate, &v, FORMULA_VALUE_SELECT);
  v->op.selector = select;

  *rv = v;

  finally : coda;
}

xapi formula_value_prepend_mk(
    const yyu_location * restrict loc
  , formula_value ** rv
  , formula_value * restrict val
)
{
  enter;

  formula_value *v;

  fatal(allocate, &v, FORMULA_VALUE_PREPEND);
  v->op.operand = val;

  *rv = v;

  finally : coda;
}

xapi formula_value_path_search_mk(
    const yyu_location * restrict loc
  , formula_value ** rv
  , formula_value * restrict val
)
{
  enter;

  formula_value *v;

  fatal(allocate, &v, FORMULA_VALUE_PATH_SEARCH);
  v->op.operand = val;

  *rv = v;

  finally : coda;
}

xapi formula_value_sequence_mk(
    const yyu_location * restrict loc
  , formula_value ** restrict rv
  , formula_value * restrict list_head
)
{
  enter;

  formula_value *v;

  fatal(allocate, &v, FORMULA_VALUE_SEQUENCE);
  v->op.list_head = list_head;

  *rv = v;

  finally : coda;
}

//
// public
//

void formula_value_set_free(rbtree * restrict rbt)
{
  formula_value *sv;
  llist list, *tmp;

  if(rbt)
  {
    llist_init_node(&list);
    rbtree_foreach(rbt, sv, rbn) {
      llist_append(&list, sv, lln);
    }

    llist_foreach_safe(&list, sv, lln, tmp) {
      formula_value_free(sv);
    }
  }

  wfree(rbt);
}

void formula_value_free(formula_value * restrict v)
{
  const chain *T[2];
  formula_value *sv;

  if(!v)
    return;

  if(v->type == FORMULA_VALUE_LIST)
  {
    chain_foreach_safe(T, sv, chn, v->list_head) {
      formula_value_free(sv);
    }
  }
  else if(v->type == FORMULA_VALUE_SET)
  {
    formula_value_set_free(v->set);
  }
  else if(v->type == FORMULA_VALUE_STRING)
  {
    wfree(v->s);
  }
  else if(v->type == FORMULA_VALUE_MAPPING)
  {
    formula_value_free(v->m.value);
  }
  else if(v->type == FORMULA_VALUE_SELECT)
  {
    selector_free(v->op.selector);
  }
  else if(v->type == FORMULA_VALUE_PREPEND)
  {
    formula_value_free(v->op.operand);
  }
  else if(v->type == FORMULA_VALUE_PATH_SEARCH)
  {
    formula_value_free(v->op.operand);
  }
  else if(v->type == FORMULA_VALUE_SEQUENCE)
  {
    chain_foreach_safe(T, sv, chn, v->op.list_head) {
      formula_value_free(sv);
    }
  }

  wfree(v);
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

xapi formula_value_render(const formula_value * restrict v, struct narrator * restrict N)
{
  enter;

  const chain *T;
  formula_value * sv;

  if(v->type == FORMULA_VALUE_LIST)
  {
    fatal(narrator_xsays, N, "[");

    chain_foreach(T, sv, chn, v->list_head) {
      fatal(narrator_xsays, N, " ");
      fatal(formula_value_render, sv, N);
    }

    if(v->list_head)
      fatal(narrator_xsays, N, " ");

    fatal(narrator_xsays, N, "]");
  }
  else if(v->type == FORMULA_VALUE_SET)
  {
    fatal(narrator_xsays, N, "{");

    rbtree_foreach(v->set, sv, rbn) {
      fatal(narrator_xsays, N, " ");
      fatal(formula_value_render, sv, N);
    }

    if(!rbtree_empty(v->set))
      fatal(narrator_xsays, N, " ");

    fatal(narrator_xsays, N, "}");
  }
  else if(v->type == FORMULA_VALUE_MAPPING)
  {
    fatal(narrator_xsayw, N, v->m.name, v->m.name_len);
    fatal(narrator_xsays, N, " : ");
    fatal(formula_value_render, v->m.value, N);
  }
  else if(v->type == FORMULA_VALUE_STRING)
  {
    fatal(narrator_xsays, N, v->s);
  }
  else if(v->type == FORMULA_VALUE_FLOAT)
  {
    fatal(narrator_xsayf, N, "%.2lf", v->f);
  }
  else if(v->type == FORMULA_VALUE_BOOLEAN)
  {
    fatal(narrator_xsayf, N, "%s", v->b ? "true" : "false");
  }
  else if(v->type == FORMULA_VALUE_POSINT)
  {
    fatal(narrator_xsayf, N, "%"PRIu64, v->u);
  }
  else if(v->type == FORMULA_VALUE_NEGINT)
  {
    fatal(narrator_xsayf, N, "%"PRId64, v->i);
  }

  finally : coda;
}
