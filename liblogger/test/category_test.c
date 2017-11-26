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
#include "xlinux/xstdlib.h"
#include "valyria/strutil.h"

#include "logger.h"
#include "internal.h"
#include "category.internal.h"
#include "attr.internal.h"
#include "LOGGER.errtab.h"

#include "test_util.h"
#include "macros.h"

struct test {
  xapi (*entry)(struct test *);
  xapi exit;
  char ** lists;
};

/// assert_relation
//
// SUMMARY
//  assert that categories with name A appear before categories with name B
//
// PARAMETERS
//  category - category which is before
//  other    - category which is after
//
static xapi assert_relation(list * restrict C, const char * category, size_t categoryl, const char * other, size_t otherl)
{
  enter;

  logger_category * cat;

  int loc = 0;
  int othermin = 0xFFFF;

  int x;
  for(x = 0; x < C->l; x++)
  {
    cat = list_get(C, x);
    size_t namel = strlen(cat->name);
    if(memncmp(cat->name, namel, category, categoryl) == 0)
    {
      if(x > loc)
        loc = x;
    }
    else if(memncmp(cat->name, namel, other, otherl) == 0)
    {
      if(x < othermin)
        othermin = x;
    }
  }

  assert_infow("category", category, categoryl);
  assert_infow("other", other, otherl);
  assert_lt_d(othermin, loc);
  assert_info_unstage();

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

static xapi test_category_list_merge_failure_toomany(struct test * test)
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
    , { }
  };

  fatal(logger_category_register, logs_a);
  fatal(categories_activate);

  finally : coda;
}

static xapi test_category_list_merge_attr_rank(struct test * test)
{
  enter;

  logger_category * logs_a = (logger_category[]) {
      { name : "A" , attr : L_RED }
    , { name : "B" , attr : L_RED | L_TRACE, rank : 10 }
    , { }
  };

  logger_category * logs_b = (logger_category[]) {
      { name : "A" , attr : L_BLUE , rank : 10 }
    , { name : "B" , attr : L_BLUE }
    , { }
  };

  fatal(logger_category_register, logs_a);
  fatal(logger_category_register, logs_b);
  fatal(categories_activate);

#define assert_eq_color(exp, act) _asserts((act & COLOR_OPT) == exp, QUOTE(act), color_option_name(act), color_option_name(exp))
#define assert_eq_trace(exp, act) _asserts((act & TRACE_OPT) == exp, QUOTE(act), trace_option_name(act), trace_option_name(exp))

  assert_eq_color(L_BLUE, logs_a[0].attr);
  assert_eq_color(L_BLUE, logs_b[0].attr);
  assert_eq_color(L_RED, logs_a[1].attr);
  assert_eq_trace(L_TRACE, logs_a[1].attr);
  assert_eq_color(L_RED, logs_b[1].attr);
  assert_eq_trace(L_TRACE, logs_b[1].attr);

  finally : coda;
}

static void free_cat(logger_category * cat)
{
  wfree(cat->name);
  wfree(cat);
}

static xapi test_category_lists_merge(struct test * test)
{
  enter;

  list * A = 0;
  list * lists = 0;
  list * C = 0;
  logger_category * cat = 0;

  fatal(list_createx, &lists, 0, list_xfree, 0);
  fatal(list_create, &C);

  // arrange
  char ** input = test->lists;
  while(*input)
  {
    fatal(list_createx, &A, free_cat, 0, 0);

    char * s = *input;
    while(*s)
    {
      char * e = s;
      while(*e >= 'A' && *e <= 'Z')
        e++;

      if(e - s)
      {
        fatal(xmalloc, &cat, sizeof(*cat));
        fatal(strloadw, &cat->name, s, e - s);
        fatal(list_push, A, cat);
        cat = 0;
      }

      s = e;
      while(*s == ' ')
        s++;
    }

    fatal(list_push, lists, A);
    A = 0;
    input++;
  }

  // act
  fatal(category_lists_merge, lists, C);

  // assert
  input = test->lists;
  while(*input)
  {
    char * cats[2] = {};
    size_t lens[2] = {};

    char * s = *input;
    while(*s)
    {
      char * e = s;
      while(*e >= 'A' && *e <= 'Z')
        e++;

      cats[0] = cats[1];
      lens[0] = lens[1];

      cats[1] = s;
      lens[1] = e - s;

      if(lens[0])
        fatal(assert_relation, C, cats[0], lens[0], cats[1], lens[1]);

      s = e;
      while(*s == ' ')
        s++;
    }

    input++;
  }

finally:
  if(cat)
    wfree(cat->name);
  wfree(cat);
  fatal(list_xfree, A);
  fatal(list_xfree, lists);
  fatal(list_xfree, C);
coda;
}

static xapi run_tests()
{
  enter;

  struct test tests[] = {
      { entry : test_category_list_merge_success_max }
    , { entry : test_category_list_merge_failure_toomany, exit : LOGGER_TOOMANY }
    , { entry : test_category_list_merge_attr_rank }
    , {
        lists : (char*[]) {
              "A"
            , 0
          }
      }
    , {
          lists : (char*[]) {
              "A"
            , "A"
            , 0
          }
      }
    , {
          lists : (char*[]) {
              "A"
            , "A"
            , "A"
            , 0
          }
      }
    , {
          lists : (char*[]) {
              "B C D"
            , "  C D"
            , 0
          }
      }
    , {
          lists : (char*[]) {
              "C D"
            , "C D E"
            , 0
          }
      }
    , {
          lists : (char*[]) {
              "B C D"
            , "  C D"
            , "B C D"
            , 0
          }
      }
    , {
          lists : (char*[]) {
              "B C D"
            , "B C"
            , "B C D"
            , 0
          }
      }
    , {
          lists : (char*[]) {
              "B C D"
            , "B C D"
            , "B C D"
            , 0
          }
      }
    , {
          lists : (char*[]) {
               "A C E"
             , "B E G"
             , "A G Z"
             , 0
           }
      }
    , {
          lists : (char*[]) {
               "B"
             , "A B"
             , "A B"
             , 0
           }
      }
/*
 * failure cases
 */
    , {
          lists : (char*[]) { "A B A", 0 }
        , exit : LOGGER_ILLREPEAT
      }
    , {
          lists : (char*[]) {
               "B A"
             , "A B C"
             , 0
           }
        , exit : LOGGER_ILLORDER
      }
    , {
          lists : (char*[]) {
               "A B C"
             , "  B C D"
             , "    C D X Y"
             , "  B     X   Z  A"
             , 0
           }
        , exit : LOGGER_ILLORDER
      }
    , {
          lists : (char*[]) {
               "  A B C"
             , "      C D W X Y"
             , "X   B   D W"
             , 0
           }
        , exit : LOGGER_ILLORDER
      }
  };

  int x = 0;
  for(x = 0; x < sizeof(tests) / sizeof(tests[0]); x++)
  {
    fatal(category_cleanup);
    fatal(category_setup);

    xapi exit;
    if(tests[x].entry)
      exit = invoke(tests[x].entry, &tests[x]);
    else
      exit = invoke(test_category_lists_merge, &tests[x]);

    if(exit)
    {
      // propagate unexpected errors
      if(xapi_exit_errtab(exit) != perrtab_LOGGER)
        fail(0);

      if(exit != tests[x].exit)
        xapi_backtrace_to(1);
      xapi_calltree_unwind();
    }

    assert_infof("test", "%d in [%d,%zu]", x, 0, sizeof(tests) / sizeof(tests[0]));
    assert_eq_exit(tests[x].exit, exit);
    assert_info_unstage();
  }

finally:
  fatal(category_cleanup);
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
