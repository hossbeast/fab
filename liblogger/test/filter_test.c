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

#include <stdlib.h>

#include "xapi.h"
#include "xapi/trace.h"
#include "xapi/calltree.h"
#include "valyria/list.h"

#include "internal.h"
#include "LOGGER.errtab.h"
#include "filter.internal.h"
#include "category.internal.h"
#include "expr.internal.h"

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

static void ids_say(uint64_t ids, char * restrict dst, size_t sz)
{
  size_t z = 0;

  dst[0] = 0;

  logger_category * t = logs_test;
  while(t->id)
  {
    if(ids & t->id)
    {
      if(z)
        z += znloads(dst + z, sz - z, ",");
      z += znloads(dst + z, sz - z, t->name);
    }
    t++;
  }
}

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

static xapi test_filters_would()
{
  enter;

  list * filters = 0;
  uint32_t attrs;
  char space[64];
  int x;

  struct {
    char * expr;
    uint64_t ids;
    int would;
  } tests[] = {
      { expr : "+A +BAR"         , ids : L_A            , would : 1 }
    , { expr : "+BAR"            , ids : L_BAR          , would : 1 }
    , { expr : "+A,BAR"          , ids : L_BAR | L_A    , would : 1 }
    , { expr : "+A,BAR -BAR"     , ids : L_A | L_BAR    , would : 1 }
    , { expr : "+A"              , ids : L_A            , would : 1 }
    , { expr : "+A,BAR"          , ids : L_A | L_BAR    , would : 1 }
    , { expr : "+LABEL"          , ids : L_LABEL        , would : 1 }
    , { expr : "+A"              , ids : L_A            , would : 1 }
    , { expr : "+A"              , ids : L_A | L_LABEL  , would : 1 }
    , { expr : "+A,LABEL"        , ids : L_A | L_LABEL  , would : 1 }

    , { expr : "+A +BAR"         , ids : L_A | L_BAR    , would : 0 }
    , { expr : "+A,BAR"          , ids : L_BAR          , would : 0 }
    , { expr : "+A,BAR -BAR"     , ids : L_A            , would : 0 }
    , { expr : "+A,BAR -BAR"     , ids : L_BAR          , would : 0 }
    , { expr : "+BAR"            , ids : L_DELTA        , would : 0 }
    , { expr : "+A"              , ids : L_BAR          , would : 0 }
    , { expr : "+A"              , ids : L_A | L_BAR    , would : 0 }
    , { expr : "+A,BAR"          , ids : L_A            , would : 0 }
    , { expr : "+A,BAR"          , ids : L_BAR          , would : 0 }
    , { expr : "+A"              , ids : L_LABEL        , would : 0 }
    , { expr : "+A,LABEL"        , ids : L_LABEL        , would : 0 }
    , { expr : "+LABEL"          , ids : L_A            , would : 0 }
    , { expr : "+A,LABEL"        , ids : L_A            , would : 0 }
    , { expr : "+LABEL"          , ids : L_A | L_LABEL  , would : 0 }

    // unrecognized categories in the filter
    , { expr : "+A,BAR,BAZ"      , ids : L_A | L_BAR    , would : 0 }
    , { expr : "+BAZ"            , ids : L_A            , would : 0 }
  };

  fatal(list_createx, &filters, filter_free, 0, 0);

  for(x = 0; x < ARRAY_LEN(tests); x++)
  {
    // arrange
    fatal(list_recycle, filters);
    fatal(expr_parse, tests[x].expr, filters, &attrs);

    // act
    int would = filters_would(filters, tests[x].ids);

    // assert
    assert_infos("expr", tests[x].expr);
    ids_say(tests[x].ids, space, sizeof(space));
    assert_infos("ids", space);
    assert_eq_b(tests[x].would, would);
    assert_info_unstage();
  }

finally:
  fatal(list_xfree, filters);
coda;
}

static xapi run_tests()
{
  enter;

  fatal(suite_setup);

  struct {
    xapi (*entry)();
    int expected;
  } tests[] = {
      { entry : test_filters_would }
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
