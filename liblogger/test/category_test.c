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
        assertf(logs[1]->id == logs[0]->id
          , "id %"PRIu64
          , "id %"PRIu64
          , logs[0]->id
          , logs[1]->id
        );
      }
      else
      {
        assertf(logs[1]->id > logs[0]->id
          , "id %"PRIu64
          , "id %"PRIu64
          , logs[0]->id
          , logs[1]->id
        );
      }
    }

    logs[0] = logs[1];
    logs[1]++;
  } while(logs[1]->name);

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

  fatal(assert_ascending, logs_a);
  assertf((logs_a[0].attr & COLOR_OPT) == L_BLUE, "expected : %s, actual : %s", COLOR_VALUE(L_BLUE), COLOR_VALUE(logs_a[0].attr));
  assertf((logs_a[1].attr & COLOR_OPT) == L_BLUE, "expected : %s, actual : %s", COLOR_VALUE(L_BLUE), COLOR_VALUE(logs_a[1].attr));
  assertf((logs_a[2].attr & COLOR_OPT) == L_RED, "expected : %s, actual : %s", COLOR_VALUE(L_RED), COLOR_VALUE(logs_a[2].attr));
  assertf((logs_a[2].attr & TRACE_OPT) == L_TRACE, "expected : %s, actual : %s", TRACE_VALUE(L_TRACE), TRACE_VALUE(logs_a[2].attr));
  assertf((logs_a[3].attr & COLOR_OPT) == L_RED, "expected : %s, actual : %s", COLOR_VALUE(L_RED), COLOR_VALUE(logs_a[3].attr));
  assertf((logs_a[3].attr & TRACE_OPT) == L_TRACE, "expected : %s, actual : %s", TRACE_VALUE(L_TRACE), TRACE_VALUE(logs_a[3].attr));

  finally : coda;
}

int main()
{
  enter;

  xapi R = 0;
  int x = 0;
  fatal(xapi_errtab_register, perrtab_LOGGER);
  fatal(xapi_errtab_register, perrtab_TEST);

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

      // print the stacktrace to stdout
      xapi_backtrace_to(1);
      xapi_calltree_unwind();
    }

    assert_exit(tests[x].expected, exit);
    success;

    fatal(category_cleanup);
  }

finally:
  fatal(category_cleanup);

  if(XAPI_UNWINDING)
  {
    xapi_infof("test", "%d", x);
    xapi_backtrace();
  }
conclude(&R);
  xapi_teardown();

  return !!R;
}
