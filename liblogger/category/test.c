#include <stdio.h>

#include "xapi.h"
#include "xapi/calltree.h"

#include "test.h"
#include "test_util.h"

#include "logger.h"
#include "category.internal.h"
#include "errtab/LOGGER.errtab.h"
#include "errtab/TEST.errtab.h"

#define QUOTE(x) #x
#define XQUOTE(x) QUOTE(x)

/// test_setup
//
// SUMMARY
//  initialize the category module
//
xapi test_setup()
{
  enter;

  category_teardown();
  fatal(category_setup);

  finally : coda;
}

/// assert_ascending
//
// SUMMARY
//  verify that a sequence of category definitions are sorted by id in
//  ascending order
//
xapi assert_ascending(logger_category * logsp)
{
  enter;

  logger_category * logs[2] = { [1] = logsp };
  do
  {
    if(logs[0])
    {
      if(strcmp(logs[0]->name, logs[1]->name) == 0)
      {
        assert(logs[1]->id == logs[0]->id
          , perrtab_TEST, TEST_FAIL
          , "expected a, actual b"
        );
      }
      else
      {
        assert(logs[1]->id > logs[0]->id
          , perrtab_TEST, TEST_FAIL
          , "expected a, actual b"
        );
      }
    }

    logs[0] = logs[1];
    logs[1]++;
  } while(logs[1]->name);

  finally : coda;
}

xapi test_category_list_merge_success()
{
  enter;

  logger_category * logs_a = (logger_category[]) {
      { name : "A" }
    , { name : "B" }
    , { name : "C" }
    , { name : "D" }
    , {}
  };

  fatal(logger_category_register, logs_a, __FILE__ " : " XQUOTE(__LINE__));
  fatal(logger_category_activate);

  fatal(assert_ascending, logs_a);

  finally : coda;
}

xapi test_category_list_merge_success_two()
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

  fatal(logger_category_register, logs_a, __FILE__ " : " XQUOTE(__LINE__));
  fatal(logger_category_register, logs_b, __FILE__ " : " XQUOTE(__LINE__));
  fatal(logger_category_activate);

  fatal(assert_ascending, logs_a);
  fatal(assert_ascending, logs_b);

  finally : coda;
}

xapi test_category_list_merge_success_two_activate()
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

  fatal(logger_category_register, logs_a, __FILE__ " : " XQUOTE(__LINE__));
  fatal(logger_category_activate);
  fatal(logger_category_register, logs_b, __FILE__ " : " XQUOTE(__LINE__));
  fatal(logger_category_activate);

  fatal(assert_ascending, logs_a);
  fatal(assert_ascending, logs_b);

  finally : coda;
}

xapi test_category_list_merge_success_nonunique()
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

  fatal(logger_category_register, logs_a, __FILE__ " : " XQUOTE(__LINE__));
  fatal(logger_category_activate);

  fatal(assert_ascending, logs_a);

  finally : coda;
}

xapi test_category_list_merge_success_max()
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

  fatal(logger_category_register, logs_a, __FILE__ " : " XQUOTE(__LINE__));
  fatal(logger_category_activate);

  finally : coda;
}

xapi test_category_list_merge_failure_order()
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

  fatal(logger_category_register, logs_a, __FILE__ " : " XQUOTE(__LINE__));
  fatal(logger_category_register, logs_b, __FILE__ " : " XQUOTE(__LINE__));
  fatal(logger_category_activate);

  finally : coda;
}

xapi test_category_list_merge_failure_order_single()
{
  enter;

  logger_category * logs_a = (logger_category[]) {
      { name : "A" }
    , { name : "B" }
    , { name : "A" }
    , {}
  };

  fatal(logger_category_register, logs_a, __FILE__ " : " XQUOTE(__LINE__));
  fatal(logger_category_activate);

  finally : coda;
}

xapi test_category_list_merge_failure_toomany()
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

  fatal(logger_category_register, logs_a, __FILE__ " : " XQUOTE(__LINE__));
  fatal(logger_category_activate);

  finally : coda;
}

int main()
{
  enter;

  int x = 0;
  fatal(logger_setup, 0, 0, 0);
  fatal(xapi_errtab_register, perrtab_TEST);

  struct {
    xapi (*entry)();
    int expected;
  } tests[] = {
      { entry : test_category_list_merge_success }
    , { entry : test_category_list_merge_success_two }
    , { entry : test_category_list_merge_success_two_activate }
    , { entry : test_category_list_merge_success_nonunique }
    , { entry : test_category_list_merge_success_max }

    , { entry : test_category_list_merge_failure_order, expected : LOGGER_ILLORDER }
    , { entry : test_category_list_merge_failure_order_single, expected : LOGGER_ILLORDER }
    , { entry : test_category_list_merge_failure_toomany, expected : LOGGER_TOOMANY }
  };

  for(x = 0; x < sizeof(tests) / sizeof(tests[0]); x++)
  {
    fatal(test_setup);

    int exit;
    if((exit = invoke(tests[x].entry)))
    {
      // some unexpected error
      if(xapi_exit_errtab(exit) != perrtab_LOGGER)
        tfail(0, 0);

      // print the stacktrace to stdout
      char space[512];
      xapi_trace_full(space, sizeof(space));
      printf("%s\n", space);

      xapi_calltree_unwind();
    }

    assert_exit(exit, perrtab_LOGGER, tests[x].expected);
    succeed;
  }

finally:
  if(XAPI_UNWINDING)
  {
    XAPI_INFOF("test", "%d", x);
    xapi_backtrace();
  }

  logger_teardown();
coda;
}
