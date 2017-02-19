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

#include "internal.h"
#include "LOGGER.errtab.h"
#include "filter.internal.h"
#include "category.internal.h"
#include "valyria/list.h"

#include "test_util.h"
#include "macros.h"

static logger_category * logs_test = (logger_category[]) {
    { name : "A" }
  , { name : "BAR" }
  , { name : "DELTA" }
  , {}
};

#define L_A     logs_test[0].id
#define L_BAR   logs_test[1].id
#define L_DELTA logs_test[2].id

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

static xapi test_filter_parse()
{
  enter;

  struct {
    char * expr;  // expression

    uint64_t v;   // expected ids
    char m;       // expected mode
    char o;       // expected operation
    size_t off;   // expected offset
  } tests[] = {
      { expr : "+A"             , v : L_A                   , m : ' ' , o : '+' , off : 2 }
    , { expr : "+A,BAR"         , v : L_A | L_BAR           , m : ' ' , o : '+' , off : 6 }
    , { expr : "-BAR"           , v : L_BAR                 , m : ' ' , o : '-' , off : 4 }
    , { expr : "-BAR%"          , v : L_BAR                 , m : '%' , o : '-' , off : 5 }
    , { expr : "+DELTA,A,BAR%"  , v : L_A | L_BAR | L_DELTA , m : '%' , o : '+' , off : 13 }
    , { expr : "+BAR,,BAR"      , v : L_BAR                 , m : ' ' , o : '+' , off : 9 }
    , { expr : "+BAR||BAR"      , v : L_BAR                 , m : ' ' , o : '+' , off : 9 }
    , { expr : "+|BAR||BAR|"    , v : L_BAR                 , m : ' ' , o : '+' , off : 11 }
    , { expr : "+DELTA%"        , v : L_DELTA               , m : '%' , o : '+' , off : 7 }
    , { expr : "+DELTA|%"       , v : L_DELTA               , m : '%' , o : '+' , off : 8 }
    , { expr : "--foo" }
    , { expr : "-bar" }
    , { expr : "+bar" }
    , { expr : "bar" }
    , { expr : "BAR" }
  };

  int x;
  for(x = 0; x < sizeof(tests) / sizeof(tests[0]); x++)
  {
    filter f;
    int r = filter_parses(tests[x].expr, &f);

    assertf(!!r == !!tests[x].v, !!tests[x].v ? "parsed" : "not parsed", !!r ? "parsed" : "not parsed");
    if(tests[x].v)
    {
      assertf(f.v == tests[x].v, "ids 0x%"PRIx64, "ids 0x%"PRIx64, tests[x].v, f.v);
      assertf(f.m == tests[x].m, "mode %c", "mode %c", tests[x].m, f.m);
      assertf(f.o == tests[x].o, "operation %c", "operation %c", tests[x].o, f.o);
    }
  }

finally:
  xapi_infos("input", tests[x].expr);
  xapi_infof("case", "%d", x);
coda;
}

static xapi test_filter_expr_parse()
{
  enter;

  struct {
    char * expr;  // expression
    char n;       // expected number of filters
  } tests[] = {
      { expr : "+A"                     , n : 1 }
    , { expr : " +A"                    , n : 1 }
    , { expr : "+A "                    , n : 1 }
    , { expr : "+A,BAR -DELTA"          , n : 2 }
    , { expr : " +A,BAR -DELTA"         , n : 2 }
    , { expr : "+A,BAR        +BAR%"    , n : 2 }
    , { expr : "+A,BAR -DELTA "         , n : 2 }
    , { expr : "+A,BAR -DELTA +BAR%"    , n : 3 }
    , { expr : "+A,BAR -DELTA +BAR%		" , n : 3 }
    , { expr : "--foo" }
    , { expr : "-bar" }
    , { expr : "+bar" }
  };

  int x;
  for(x = 0; x < sizeof(tests) / sizeof(tests[0]); x++)
  {
    filter filters[4];
    int r = filter_expr_parse(MMS(tests[x].expr), filters, sizeof(filters) / sizeof(*filters));

    if(tests[x].n)
    {
      assertf(r >= 0, "parsed", "not parsed");
      assertf(r == tests[x].n, "%d filters", "%d filters", tests[x].n, r);
    }
    else
    {
      assertf(r < 0, "not parsed", "parsed");
    }
  }

finally:
  xapi_infos("input", tests[x].expr);
  xapi_infof("case", "%d", x);
coda;
}

static xapi test_filters_would()
{
  enter;

  list * filters = 0;
  int x = -1;

  struct {
    char ** expr;
    uint64_t ids;
    int would;
  } tests[] = {
      { expr : (char*[]) { "+A", "+BAR", 0 }    , ids : L_A , would : 1 }
    , { expr : (char*[]) { "+A", "+BAR", 0 }    , ids : L_A | L_BAR , would : 1 }
    , { expr : (char*[]) { "+A,BAR", 0 }        , ids : L_BAR , would : 0 }
    , { expr : (char*[]) { "+BAR", 0 }        , ids : L_BAR , would : 1 }
    , { expr : (char*[]) { "+A,BAR", 0 }        , ids : L_BAR | L_A, would : 1 }
    , { expr : (char*[]) { "+A,BAR", "-BAR", 0 }, ids : L_A | L_BAR, would : 0 }
    , { expr : (char*[]) { "+A,BAR", "-BAR", 0 }, ids : L_A, would : 0 }
    , { expr : (char*[]) { "+A,BAR", "-BAR", 0 }, ids : L_BAR, would : 0 }
    , { expr : (char*[]) { "+BAR", 0 }          , ids : L_DELTA, would : 0 }

    , { expr : (char*[]) { "+A", 0 }            , ids : L_A, would : 1 }
    , { expr : (char*[]) { "+A", 0 }            , ids : L_BAR, would : 0 }
    , { expr : (char*[]) { "+A", 0 }            , ids : L_A | L_BAR, would : 1 }
    , { expr : (char*[]) { "+A,BAR", 0 }        , ids : L_A, would : 0 }
    , { expr : (char*[]) { "+A,BAR", 0 }        , ids : L_BAR, would : 0 }
    , { expr : (char*[]) { "+A,BAR", 0 }        , ids : L_A | L_BAR, would : 1 }

    , { expr : (char*[]) { "+A%", 0 }            , ids : L_A, would : 1 }
    , { expr : (char*[]) { "+A%", 0 }            , ids : L_BAR, would : 0 }
    , { expr : (char*[]) { "+A%", 0 }            , ids : L_A | L_BAR, would : 0 }
    , { expr : (char*[]) { "+A,BAR%", 0 }        , ids : L_A, would : 1 }
    , { expr : (char*[]) { "+A,BAR%", 0 }        , ids : L_BAR, would : 1 }
    , { expr : (char*[]) { "+A,BAR%", 0 }        , ids : L_A | L_BAR, would : 1 }

    // unrecognized categories in the filter
    , { expr : (char*[]) { "+A,BAR,BAZ", 0 }     , ids : L_A | L_BAR, would : 0 }
    , { expr : (char*[]) { "+A,BAR,BAZ%", 0 }    , ids : L_A | L_BAR, would : 1 }
    , { expr : (char*[]) { "+BAZ", 0 }           , ids : L_A, would : 0 }
    , { expr : (char*[]) { "+BAZ%", 0 }          , ids : L_A, would : 0 }
  };

  fatal(list_createx, &filters, filter_free, 0, 0);

  for(x = 0; x < sizeof(tests) / sizeof(tests[0]); x++)
  {
    fatal(list_recycle, filters);

    char ** expr;
    for(expr = tests[x].expr; *expr; expr++)
      fatal(filter_expr_process, *expr, strlen(*expr), filters, list_push);

    int actual = filters_would(filters, tests[x].ids);
    assert_eq_b(tests[x].would, actual);
  }

finally:
  xapi_infof("case", "%d", x);
  fatal(list_xfree, filters);
coda;
}

int main()
{
  enter;

  xapi R = 0;
  int x = 0;
  fatal(suite_setup);

  struct {
    xapi (*entry)();
    int expected;
  } tests[] = {
      { entry : test_filter_parse }
    , { entry : test_filter_expr_parse }
    , { entry : test_filters_would }
  };

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

    assert_exit(tests[x].expected, exit);
    success;
  }

finally:
  fatal(suite_cleanup);

  if(XAPI_UNWINDING)
  {
    xapi_infof("test", "%d", x);
    xapi_backtrace();
  }
conclude(&R);
  xapi_teardown();

  return !!R;
}
