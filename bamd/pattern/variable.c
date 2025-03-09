/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "narrator.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xstring.h"
#include "bacon/value.h"
#include "bacon/set.h"
#include "bacon/builder.h"
#include "bacon/coerce.h"
#include "common/attrs.h"

#include "variable.h"
#include "generate.internal.h"
#include "search.internal.h"
#include "pattern.internal.h"
#include "fsent.h"
#include "module.h"

#include "zcat.h"

//
// static
//

static void variable_describe(const pattern_segment * restrict fn, bacon_writer * restrict builder_writer)
{
  const pattern_variable *variable = &fn->variable;

  { MAPPING
    STRING("type")
    STRING(attrs16_name_byvalue(pattern_variable_type_attrs, variable->variable_type))
  }
  switch(variable->variable_type) {
    case PATTERN_VARIABLE_TYPE_NAME:
      { MAPPING
        STRING("name")
        BYTES(variable->name, variable->name_len)
      }
      break;
    case PATTERN_VARIABLE_TYPE_NUM:
      { MAPPING
        STRING("num")
        UINT(variable->num)
      }
      break;
  }
}

static bool should_brace(const char *s, uint16_t l)
{
  int x;

  for(x = 0; x < l; x++)
  {
    if(s[x] >= 'a' && s[x] <= 'z')
      continue;
    if(s[x] >= 'A' && s[x] <= 'Z')
      continue;
    if(s[x] == '_')
      continue;
    if(x > 0 && (s[x] >= '0' && s[x] <= '9'))
      continue;

    break;
  }

  return x < l;
}

static void variable_say(const pattern_segment * restrict fn, const pattern_segment * restrict next, narrator * restrict N)
{
  const pattern_variable * n = &fn->variable;
  bool brace;

  xsayc('$');

  brace = false;
  if(next) {
    switch(next->type) {
      case PATTERN_QUALIFIER:
        brace = true;
        break;
      case PATTERN_WORD:
        if(next->word.len > 0 && next->word.text[0] != '.') {
          brace = true;
        }
        break;
      case PATTERN_CHARACTER:
        if(next->byte.code != '.') {
          brace = true;
        }
      default:
        break;
    }
  }

  if(n->variable_type == PATTERN_VARIABLE_TYPE_NAME) {
    brace |= should_brace(n->name, n->name_len);
  }

  if(brace) {
    xsayc('{');
  }

  if(n->variable_type == PATTERN_VARIABLE_TYPE_NAME) {
    xsayw(n->name, n->name_len);
  } else if(n->variable_type == PATTERN_VARIABLE_TYPE_NUM) {
    xsayf("%hhu", n->num);
  }

  if(brace) {
    xsayc('}');
  }
}

static void variable_destroy(pattern_segment * restrict fn)
{
  pattern_variable * n = &fn->variable;

  if(n->variable_type == PATTERN_VARIABLE_TYPE_NAME) {
    free(n->name);
  }
}

static size_t variable_cat(void * restrict dst, size_t sz, const bacon_value * restrict vars, const union pattern_segment * restrict seg)
{
  const pattern_variable * variable;
  const bacon_value *val;
  size_t z;

  variable = &seg->variable;

  if(vars == NULL) {
    return 0;
  }

  RUNTIME_ASSERT(vars->type == BACON_TYPE_SET);
  if((val = bacon_set_lookupw(vars, variable->name, variable->name_len)) == 0) {
    return 0;
  }
  RUNTIME_ASSERT(val->type == BACON_TYPE_STRING);

  z = 0;
  z += zcatw(dst + z, sz - z, val->s, val->len);

  return z;
}

static void variable_search(const pattern_segment * restrict segment, pattern_search_context * restrict ctx)
{
  const pattern_variable * variable;
  const char * restrict name;
  uint16_t namel;
  const bacon_value *val;

  variable = &segment->variable;
  name = ctx->node->name.name;
  namel = ctx->node->name.namel;

  if(ctx->vars == NULL) {
    ctx->traversal->unsatisfied = true;
    return;
  }

  RUNTIME_ASSERT(ctx->vars->type == BACON_TYPE_SET);
  if((val = bacon_set_lookupw(ctx->vars, variable->name, variable->name_len)) == 0) {
    ctx->traversal->unsatisfied = true;
    return;
  }

//bacon_value_say(ctx->vars, g_narrator_stdout);
//printf("\n");

  RUNTIME_ASSERT(val->type == BACON_TYPE_STRING);
  if(     (namel - ctx->traversal->offset) >= val->len
       && memcmp(name + ctx->traversal->offset, val->s, val->len) == 0) {
    ctx->traversal->offset += val->len;
  } else {
    ctx->traversal->unsatisfied = true;
  }
}

static void variable_generate(const pattern_segment * restrict seg, pattern_generate_context * restrict ctx)
{
  const pattern_variable * n = &seg->variable;
  const bacon_value *val0;
  bacon_value *val;

  /* references to matching groups */
  if(n->variable_type == PATTERN_VARIABLE_TYPE_NUM)
  {
    if(ctx->match && ctx->match->group_max >= n->num)
    {
      narrator_sayw(ctx->section_narrator
        , ctx->match->groups[n->num].start
        , ctx->match->groups[n->num].len
      );
    }
    goto end;
  }

  /* named vars */
  RUNTIME_ASSERT(n->variable_type == PATTERN_VARIABLE_TYPE_NAME);
  val0 = 0;
  if(ctx->vars) {
    RUNTIME_ASSERT(ctx->vars->type == BACON_TYPE_SET);
    val0 = bacon_set_lookupw(ctx->vars, n->name, n->name_len);
  }

  if(!val0 && ctx->mod->vars) {
    RUNTIME_ASSERT(ctx->mod->vars->type == BACON_TYPE_SET);
    val0 = bacon_set_lookupw(ctx->mod->vars, n->name, n->name_len);
  }

  if(val0) {
    bacon_value_coerce(ctx->parser, val0, BACON_TYPE_STRING, &val);
    narrator_sayw(ctx->section_narrator, val->s, val->len);
    if(val0 != val) {
      bacon_value_free(ctx->parser, val);
    }
  }

end:
  pattern_segment_generate(ctx);
}

static int variable_cmp(const pattern_segment * A, const pattern_segment *B)
{
  int d;

  if((d = INTCMP(A->variable.num, B->variable.num)))
    return d;

  return memncmp(A->variable.name, A->variable.name_len, B->variable.name, B->variable.name_len);
}

static pattern_segment_vtable vtable = PATTERN_SEGMENT_VTABLE(variable, PATTERN_VARIABLE, 0);

void pattern_variable_mk(
    pattern_segment ** restrict rv
  , const yyu_location * restrict loc
  , pattern_variable_type variable_type
  , uint16_t num
  , const char * restrict name
  , uint16_t name_len
)
{
  pattern_segment * n = 0;

  xmalloc(&n, sizeof(*n));
  pattern_segment_init(n, &vtable, loc);

  n->variable.variable_type = variable_type;
  if(n->variable.variable_type == PATTERN_VARIABLE_TYPE_NUM)
  {
    n->variable.num = num;
  }
  else if(variable_type == PATTERN_VARIABLE_TYPE_NAME)
  {
    ixstrndup(&n->variable.name, name, name_len);
    n->variable.name_len = name_len;
  }

  *rv = n;
}
