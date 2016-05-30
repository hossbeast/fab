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

#define LIST_ELEMENT_TYPE filter
#include "valyria/list.h"

#include "test_util.h"

static logger_category * logs_test = (logger_category[]) {
    { name : "A" }
  , { name : "BAR" }
  , { name : "DELTA" }
  , {}
};

#define L_A     logs_test[0].id
#define L_BAR   logs_test[1].id
#define L_DELTA logs_test[2].id

xapi suite_setup()
{
  enter;

  fatal(category_setup);
  fatal(logger_category_register, logs_test);
  fatal(logger_category_activate);

  finally : coda;
}

xapi test_filter_parse()
{
  enter;

  filter * filterp = 0;

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
    , { expr : "+DELTA,A,BAR^"  , v : L_A | L_BAR | L_DELTA , m : '^' , o : '+' , off : 13 }
    , { expr : "-BAR  "         , v : L_BAR                 , m : ' ' , o : '-' , off : 4 }
    , { expr : "  -BAR"         , v : L_BAR                 , m : ' ' , o : '-' , off : 6 }
    , { expr : "+BAR,,BAR"      , v : L_BAR                 , m : ' ' , o : '+' , off : 9 }
    , { expr : "+BAR||BAR"      , v : L_BAR                 , m : ' ' , o : '+' , off : 9 }
    , { expr : "+|BAR||BAR|"    , v : L_BAR                 , m : ' ' , o : '+' , off : 11 }
    , { expr : "+DELTA%"        , v : L_DELTA               , m : '%' , o : '+' , off : 7 }
    , { expr : "+DELTA|%"       , v : L_DELTA               , m : '%' , o : '+' , off : 8 }
    , { expr : "+A +BAR"        , v : L_A                   , m : ' ' , o : '+' , off : 2 }
  };

  int x;
  for(x = 0; x < sizeof(tests) / sizeof(tests[0]); x++)
  {
    size_t off = 0;
    fatal(filter_parses, tests[x].expr, &filterp, &off);

    if(tests[x].v)
    {
      assertf(filterp, "parsed", "not parsed");
      assertf(filterp->v == tests[x].v, "ids 0x%"PRIx64, "ids 0x%"PRIx64, tests[x].v, filterp->v);
      assertf(filterp->m == tests[x].m, "mode %c", "mode %c", tests[x].m, filterp->m);
      assertf(filterp->o == tests[x].o, "operation %c", "operation %c", tests[x].o, filterp->o);
      assertf(off == tests[x].off, "offset %zu", "offset %zu", tests[x].off, off);
    }
    else
    {
      assertf(filterp == 0, "%p", "%p", 0, filterp);
    }

    filter_ifree(&filterp);
  }

finally:
  if(XAPI_UNWINDING)
  {
    xapi_infos("test", "test_filter_parse");
    xapi_infos("input", tests[x].expr);
    xapi_infof("case", "%d", x);
  }

  filter_free(filterp);
coda;
}

xapi test_filters_would()
{
  enter;

  list * filters = 0;
  filter * filterp = 0;

  struct {
    char ** expr;

    uint64_t ids;
    int would;
  } tests[] = {
      { expr : (char*[]) { "+A", "+BAR", 0 }    , ids : L_A , would : 1 }
    , { expr : (char*[]) { "+A", "+BAR", 0 }    , ids : L_A | L_BAR , would : 1 }
    , { expr : (char*[]) { "+A,BAR", 0 }        , ids : L_BAR , would : 1 }
    , { expr : (char*[]) { "+A,BAR", 0 }        , ids : L_BAR | L_A, would : 1 }
    , { expr : (char*[]) { "+A,BAR", "-BAR", 0 }, ids : L_BAR | L_A, would : 0 }
    , { expr : (char*[]) { "+A,BAR", "-BAR", 0 }, ids : L_A, would : 1 }
    , { expr : (char*[]) { "+BAR", 0 }          , ids : L_DELTA, would : 0 }
  };

  fatal(list_create, &filters, filter_free);

  int x;
  for(x = 0; x < sizeof(tests) / sizeof(tests[0]); x++)
  {
    list_clear(filters);

    char ** expr;
    for(expr = tests[x].expr; *expr; expr++)
    {
      fatal(filter_parses, *expr, &filterp, 0);
      fatal(list_push, filters, filterp);
      filterp = 0;
    }

    int would = filters_would(filters, tests[x].ids);

    assertf(would == tests[x].would, "%s", "%s", tests[x].would ? "true" : "false", would ? "true" : "false");
  }

finally:
  if(XAPI_UNWINDING)
  {
    xapi_infos("test", __FUNCTION__);
    xapi_infof("case", "%d", x);
  }

  filter_free(filterp);
  list_free(filters);
coda;
}

int main()
{
  enter;

  int x = 0;
  fatal(xapi_errtab_register, perrtab_LOGGER);
  fatal(xapi_errtab_register, perrtab_TEST);
  fatal(suite_setup);

  struct {
    xapi (*entry)();
    int expected;
  } tests[] = {
      { entry : test_filter_parse }
    , { entry : test_filters_would }
  };

  for(x = 0; x < sizeof(tests) / sizeof(tests[0]); x++)
  {
    xapi exit;
    if((exit = invoke(tests[x].entry)))
    {
      // propagate unexpected errors
      if(xapi_exit_errtab(exit) != perrtab_LOGGER)
        tfail(0, 0);

      // print the stacktrace to stdout
      xapi_backtrace_to(1);
      xapi_calltree_unwind();
    }

    assert_exit(exit, perrtab_LOGGER, tests[x].expected);
    success;
  }

finally:
  if(XAPI_UNWINDING)
  {
    xapi_backtrace();
  }
coda;
}
