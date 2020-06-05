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
#include <inttypes.h>
#include <stdlib.h>

#include "xapi.h"
#include "xapi/trace.h"
#include "xapi/calltree.h"
#include "valyria/list.h"

#include "logger.h"
#include "expr.internal.h"
#include "LOGGER.errtab.h"
#include "filter.internal.h"
#include "category.internal.h"

#include "test_util.h"
#include "macros.h"
#include "zbuffer.h"

static logger_category * logs_test = (logger_category[]) {
    { name : "A" }
  , { name : "BAR" }
  , { name : "DELTA" }
  , { name : "LABEL", optional : 1 }
  , {}
};

#define L_A     logs_test[0].id
#define L_BAR   logs_test[1].id
#define L_DELTA logs_test[2].id
#define L_LABEL logs_test[3].id

static xapi suite_setup()
{
  enter;

  fatal(category_setup);
  fatal(logger_category_register, logs_test);
  fatal(categories_activate);

  finally : coda;
}

static xapi suite_cleanup()
{
  enter;

  fatal(category_cleanup);

  finally : coda;
}

static xapi test_expr_lex()
{
  enter;

  struct {
    char * expr;  // expression

    int r;        // expected return value
    uint64_t v;   // expected filter ids
    char o;       // expected filter operation
    char u;       // expected filter unknown
    uint32_t a;   // expected attrs
  } tests[] = {
      { expr : "+A"             , r : 1 , o : '+', v : L_A         }
    , { expr : "+A,BAR"         , r : 1 , o : '+', v : L_A | L_BAR }
    , { expr : "-BAR"           , r : 1 , o : '-', v : L_BAR       }
    , { expr : "+BAR,,BAR"      , r : 1 , o : '+', v : L_BAR       }
    , { expr : "%""CATEGORY"    , r : 1 , a : L_CATEGORY           }
    , { expr : "%NOFILTER,RED"  , r : 1 , a : L_NOFILTER | L_RED   }
    , { expr : " +A" }
    , { expr : "--foo" }
    , { expr : "-bar" }
    , { expr : "+bar" }
    , { expr : "bar" }
    , { expr : "BAR" }
    , { expr : "--foo" }
    , { expr : "-bar" }
    , { expr : "+bar" }
  };

  int x;
  for(x = 0; x < sizeof(tests) / sizeof(tests[0]); x++)
  {
    filter filter;
    uint32_t attrs;
    int parsed = expr_lex(MMS(tests[x].expr), &filter, &attrs);

    assert_infos("expr", tests[x].expr);
    assert_infof("case", "%d", x);
    assert_eq_d(tests[x].r, parsed);
    if(parsed)
    {
      assert_eq_u64(tests[x].v, filter.v);
      assert_eq_c(tests[x].o, filter.o);
      assert_eq_c(tests[x].u, filter.u);
      assert_eq_u32(tests[x].a, attrs);
    }
    assert_info_unstage();
  }

  finally : coda;
}

static xapi run_tests()
{
  enter;

  fatal(suite_setup);

  struct {
    xapi (*entry)();
    int expected;
  } tests[] = {
      { entry : test_expr_lex }
  };

  int x = 0;
  for(x = 0; x < sizeof(tests) / sizeof(tests[0]); x++)
  {
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

    assert_eq_exit(tests[x].expected, exit);
  }

finally:
  fatal(suite_cleanup);
  summarize;
coda;
}

int main()
{
  enter;

  xapi R = 0;
  fatal(run_tests);

finally:
  if(XAPI_UNWINDING)
    xapi_backtrace();
conclude(&R);
  xapi_teardown();

  return !!R;
}
