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

xapi test_category_list_merge_1()
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

xapi test_category_list_merge_2()
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

xapi test_category_list_merge_3()
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

xapi test_category_list_merge_4()
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

xapi test_category_list_merge_10()
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
      { entry : test_category_list_merge_1 }
    , { entry : test_category_list_merge_2 }
    , { entry : test_category_list_merge_3 }
    , { entry : test_category_list_merge_4 }

    , { entry : test_category_list_merge_10, expected : LOGGER_ILLORDER }
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
