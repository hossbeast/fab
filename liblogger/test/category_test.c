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

#include <stdio.h>

#include "xapi.h"
#include "xapi/trace.h"
#include "xapi/calltree.h"
#include "valyria/list.h"

#include "logger.h"
#include "internal.h"
#include "category.internal.h"
#include "attr.internal.h"
#include "LOGGER.errtab.h"

#include "test_util.h"
#include "macros.h"

/// assert_ascending
//
// SUMMARY
//  verify that a sequence of category definitions are sorted by id in
//  ascending order
//
static xapi assert_ascending(logger_category * logsp)
{
  enter;

  logger_category * logs[2] = { [1] = logsp };
  do
  {
    if(logs[0])
    {
      if(strcmp(logs[0]->name, logs[1]->name) == 0)
      {
        assert_eq_u64(logs[0]->id, logs[1]->id);
      }
      else
      {
        assert_gt_u64(logs[0]->id, logs[1]->id);
      }
    }

    logs[0] = logs[1];
    logs[1]++;
  } while(logs[1]->name);

  finally : coda;
}

/// assert_together
//
// SUMMARY
//  assert that categories with the same name are contiguous in the list
//
static xapi assert_together(list * C)
{
  enter;

  int x;
  int y;
  int i;

  logger_category * rep;
  logger_category * other = 0;

  for(x = 0; x < C->l; x++)
  {
    rep = list_get(C, x);
    for(y = x + 1; y < C->l; y++)
    {
      logger_category * last = list_get(C, y);

      if(strcmp(rep->name, last->name))
        break;
    }

    for(i = y + 1; i < C->l; i++)
    {
      logger_category * other = list_get(C, i);

      if(strcmp(rep->name, other->name) == 0)
        break;
    }

    if(i < C->l)
      break;

    x = y - 1;
  }

  assert_null(other);

  finally : coda;
}

/// assert_before
//
// SUMMARY
//  assert that categories with name A appear before categories with name B
//
static xapi assert_before(list * restrict C, const char * restrict A, const char * restrict B)
{
  enter;

  logger_category * cat;

  int x;
  for(x = 0; x < C->l; x++)
  {
    cat = list_get(C, x);
    if(strcmp(cat->name, A) == 0)
      break;
  }

  int y;
  for(y = 0; y < C->l; y++)
  {
    cat = list_get(C, y);
    if(strcmp(cat->name, B) == 0)
      break;
  }
  
  assert_infos("before", A);
  assert_infos("after", B);
  int before_index = x;
  int after_index = y;
  assert_gt_d(before_index, after_index);
  assert_info_unstage();

  finally : coda;
}

static xapi test_category_list_merge_success()
{
  enter;

  logger_category * logs_a = (logger_category[]) {
      { name : "A" }
    , { name : "B" }
    , { name : "C" }
    , { name : "D" }
    , {}
  };

  fatal(logger_category_register, logs_a);
  fatal(categories_activate);

  fatal(assert_ascending, logs_a);

  finally : coda;
}

static xapi test_category_list_merge_success_two()
{
  enter;

  logger_category * logs_a = (logger_category[]) {
      { name : "A" }
    , { name : "B" }
    , { name : "C" }
    , { name : "D" }
    , {}
  };

  logger_category * logs_b = (logger_category[]) {
      { name : "A" }
    , { name : "D" }
    , { name : "E" }
    , { name : "F" }
    , {}
  };

  fatal(logger_category_register, logs_a);
  fatal(logger_category_register, logs_b);
  fatal(categories_activate);

  fatal(assert_ascending, logs_a);
  fatal(assert_ascending, logs_b);

  finally : coda;
}

static xapi test_category_list_merge_success_two_activate()
{
  enter;

  logger_category * logs_a = (logger_category[]) {
      { name : "A" }
    , { name : "B" }
    , { name : "C" }
    , { name : "D" }
    , {}
  };

  logger_category * logs_b = (logger_category[]) {
      { name : "A" }
    , { name : "D" }
    , { name : "E" }
    , { name : "F" }
    , {}
  };

  fatal(logger_category_register, logs_a);
  fatal(categories_activate);
  fatal(logger_category_register, logs_b);
  fatal(categories_activate);

  fatal(assert_ascending, logs_a);
  fatal(assert_ascending, logs_b);

  finally : coda;
}

static xapi test_category_list_merge_success_nonunique()
{
  enter;

  logger_category * logs_a = (logger_category[]) {
      { name : "A" }
    , { name : "A" }
    , { name : "B" }
    , { name : "B" }
    , { name : "C" }
    , { name : "D" }
    , {}
  };

  fatal(logger_category_register, logs_a);
  fatal(categories_activate);

  fatal(assert_ascending, logs_a);

  finally : coda;
}

static xapi test_category_list_merge_success_max()
{
  enter;

  logger_category * logs_a = (logger_category[]) {
      { name : "A00" }
    , { name : "A01" }
    , { name : "A02" }
    , { name : "A03" }
    , { name : "A04" }
    , { name : "A05" }
    , { name : "A06" }
    , { name : "A07" }
    , { name : "A08" }
    , { name : "A09" }
    , { name : "A10" }
    , { name : "A11" }
    , { name : "A12" }
    , { name : "A13" }
    , { name : "A14" }
    , { name : "A15" }
    , { name : "A16" }
    , { name : "A17" }
    , { name : "A18" }
    , { name : "A19" }
    , { name : "A20" }
    , { name : "A21" }
    , { name : "A22" }
    , { name : "A23" }
    , { name : "A24" }
    , { name : "A25" }
    , { name : "A26" }
    , { name : "A27" }
    , { name : "A28" }
    , { name : "A29" }
    , { name : "A30" }
    , { name : "A31" }
    , { name : "A32" }
    , { name : "A33" }
    , { name : "A34" }
    , { name : "A35" }
    , { name : "A36" }
    , { name : "A37" }
    , { name : "A38" }
    , { name : "A39" }
    , { name : "A40" }
    , { name : "A41" }
    , { name : "A42" }
    , { name : "A43" }
    , { name : "A44" }
    , { name : "A45" }
    , { name : "A46" }
    , { name : "A47" }
    , { name : "A48" }
    , { name : "A49" }
    , { name : "A50" }
    , { name : "A51" }
    , { name : "A52" }
    , { name : "A53" }
    , { name : "A54" }
    , { name : "A55" }
    , { name : "A56" }
    , { name : "A57" }
    , { name : "A58" }
    , { name : "A59" }
    , { name : "A60" }
    , { name : "A61" }
    , { name : "A62" }
    , { name : "A63" }
    , {}
  };

  fatal(logger_category_register, logs_a);
  fatal(categories_activate);

  finally : coda;
}

static xapi test_category_list_merge_failure_order()
{
  enter;

  logger_category * logs_a = (logger_category[]) {
      { name : "A" }
    , { name : "B" }
    , {}
  };

  logger_category * logs_b = (logger_category[]) {
      { name : "B" }
    , { name : "A" }
    , {}
  };

  fatal(logger_category_register, logs_a);
  fatal(logger_category_register, logs_b);
  fatal(categories_activate);

  finally : coda;
}

static xapi test_category_list_merge_failure_order_single()
{
  enter;

  logger_category * logs_a = (logger_category[]) {
      { name : "A" }
    , { name : "B" }
    , { name : "A" }
    , {}
  };

  fatal(logger_category_register, logs_a);
  fatal(categories_activate);

  finally : coda;
}

static xapi test_category_list_merge_failure_toomany()
{
  enter;

  logger_category * logs_a = (logger_category[]) {
      { name : "A00" }
    , { name : "A01" }
    , { name : "A02" }
    , { name : "A03" }
    , { name : "A04" }
    , { name : "A05" }
    , { name : "A06" }
    , { name : "A07" }
    , { name : "A08" }
    , { name : "A09" }
    , { name : "A10" }
    , { name : "A11" }
    , { name : "A12" }
    , { name : "A13" }
    , { name : "A14" }
    , { name : "A15" }
    , { name : "A16" }
    , { name : "A17" }
    , { name : "A18" }
    , { name : "A19" }
    , { name : "A20" }
    , { name : "A21" }
    , { name : "A22" }
    , { name : "A23" }
    , { name : "A24" }
    , { name : "A25" }
    , { name : "A26" }
    , { name : "A27" }
    , { name : "A28" }
    , { name : "A29" }
    , { name : "A30" }
    , { name : "A31" }
    , { name : "A32" }
    , { name : "A33" }
    , { name : "A34" }
    , { name : "A35" }
    , { name : "A36" }
    , { name : "A37" }
    , { name : "A38" }
    , { name : "A39" }
    , { name : "A40" }
    , { name : "A41" }
    , { name : "A42" }
    , { name : "A43" }
    , { name : "A44" }
    , { name : "A45" }
    , { name : "A46" }
    , { name : "A47" }
    , { name : "A48" }
    , { name : "A49" }
    , { name : "A50" }
    , { name : "A51" }
    , { name : "A52" }
    , { name : "A53" }
    , { name : "A54" }
    , { name : "A55" }
    , { name : "A56" }
    , { name : "A57" }
    , { name : "A58" }
    , { name : "A59" }
    , { name : "A60" }
    , { name : "A61" }
    , { name : "A62" }
    , { name : "A63" }
    , { name : "A64" }
    , {}
  };

  fatal(logger_category_register, logs_a);
  fatal(categories_activate);

  finally : coda;
}

static xapi test_category_list_merge_attr_rank()
{
  enter;

  logger_category * logs_a = (logger_category[]) {
      { name : "A" , attr : L_RED }
    , { name : "A" , attr : L_BLUE , rank : 10 }
    , { name : "B" , attr : L_RED | L_TRACE, rank : 10 }
    , { name : "B" , attr : L_BLUE }
    , {}
  };

  fatal(logger_category_register, logs_a);
  fatal(categories_activate);

#define assert_eq_color(exp, act) _asserts((act & COLOR_OPT) == exp, QUOTE(act), color_option_name(act), color_option_name(exp))
#define assert_eq_trace(exp, act) _asserts((act & TRACE_OPT) == exp, QUOTE(act), trace_option_name(act), trace_option_name(exp))

  fatal(assert_ascending, logs_a);
  assert_eq_color(L_BLUE, logs_a[0].attr);
  assert_eq_color(L_BLUE, logs_a[1].attr);
  assert_eq_color(L_RED, logs_a[2].attr);
  assert_eq_trace(L_TRACE, logs_a[2].attr);
  assert_eq_color(L_RED, logs_a[3].attr);
  assert_eq_trace(L_TRACE, logs_a[3].attr);

  finally : coda;
}

static xapi test_category_list_merge_common_A()
{
  enter;

  list * A = 0;
  list * B = 0;
  list * C = 0;

  fatal(list_create, &A);
  fatal(list_create, &B);
  fatal(list_create, &C);

  fatal(list_push, A, (logger_category[]) {{ name : "B" }});
  fatal(list_push, A, (logger_category[]) {{ name : "C" }});

  fatal(list_push, B, (logger_category[]) {{ name : "C" }});
  fatal(list_push, B, (logger_category[]) {{ name : "D" }});

  fatal(category_list_merge, A, B, C);

  assert_eq_zu(4, C->l);
  fatal(assert_together, C);
  fatal(assert_before, C, "B", "C");
  fatal(assert_before, C, "C", "D");

finally:
  fatal(list_xfree, A);
  fatal(list_xfree, B);
  fatal(list_xfree, C);
coda;
}

static xapi test_category_list_merge_common_B()
{
  enter;

  list * A = 0;
  list * B = 0;
  list * C = 0;

  fatal(list_create, &A);
  fatal(list_create, &B);
  fatal(list_create, &C);

  fatal(list_push, A, (logger_category[]) {{ name : "B" }});
  fatal(list_push, A, (logger_category[]) {{ name : "C" }});

  fatal(list_push, B, (logger_category[]) {{ name : "A" }});
  fatal(list_push, B, (logger_category[]) {{ name : "B" }});

  fatal(category_list_merge, A, B, C);

  assert_eq_zu(4, C->l);
  fatal(assert_together, C);
  fatal(assert_before, C, "B", "C");
  fatal(assert_before, C, "A", "B");

finally:
  fatal(list_xfree, A);
  fatal(list_xfree, B);
  fatal(list_xfree, C);
coda;
}

static xapi test_category_list_merge_nocommon()
{
  enter;

  list * A = 0;
  list * B = 0;
  list * C = 0;

  fatal(list_create, &A);
  fatal(list_create, &B);
  fatal(list_create, &C);

  fatal(list_push, A, (logger_category[]) {{ name : "B" }});
  fatal(list_push, A, (logger_category[]) {{ name : "B" }});
  fatal(list_push, A, (logger_category[]) {{ name : "C" }});
  fatal(list_push, A, (logger_category[]) {{ name : "C" }});
  fatal(list_push, A, (logger_category[]) {{ name : "C" }});

  fatal(list_push, B, (logger_category[]) {{ name : "D" }});
  fatal(list_push, B, (logger_category[]) {{ name : "E" }});

  fatal(category_list_merge, A, B, C);

  assert_eq_zu(7, C->l);
  fatal(assert_together, C);
  fatal(assert_before, C, "B", "C");
  fatal(assert_before, C, "D", "E");

finally:
  fatal(list_xfree, A);
  fatal(list_xfree, B);
  fatal(list_xfree, C);
coda;
}

int main()
{
  enter;

  xapi R = 0;
  int x = 0;

  struct {
    xapi (*entry)();
    xapi expected;
  } tests[] = {
      { entry : test_category_list_merge_success }
    , { entry : test_category_list_merge_success_two }
    , { entry : test_category_list_merge_success_two_activate }
    , { entry : test_category_list_merge_success_nonunique }
    , { entry : test_category_list_merge_success_max }

    , { entry : test_category_list_merge_failure_order, expected : LOGGER_ILLORDER }
    , { entry : test_category_list_merge_failure_order_single, expected : LOGGER_ILLREPEAT }
    , { entry : test_category_list_merge_failure_toomany, expected : LOGGER_TOOMANY }

    , { entry : test_category_list_merge_attr_rank }
    , { entry : test_category_list_merge_common_A }
    , { entry : test_category_list_merge_common_B }
    , { entry : test_category_list_merge_nocommon }
  };

  for(x = 0; x < sizeof(tests) / sizeof(tests[0]); x++)
  {
    fatal(category_setup);

    xapi exit;
    if((exit = invoke(tests[x].entry)))
    {
      // propagate unexpected errors
      if(xapi_exit_errtab(exit) != perrtab_LOGGER)
        fail(0);

      if(exit != tests[x].expected)
        xapi_backtrace_to(1);
      xapi_calltree_unwind();
    }

    assert_eq_exit(tests[x].expected, exit);
    fatal(category_cleanup);
  }

finally:
  summarize;
  fatal(category_cleanup);

  if(XAPI_UNWINDING)
    xapi_backtrace();

conclude(&R);
  xapi_teardown();

  return !!R;
}
