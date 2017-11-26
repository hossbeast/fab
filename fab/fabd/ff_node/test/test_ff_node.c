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

#include "xapi.h"
#include "types.h"

#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/growing.h"

#include "ff_node.internal.h"
#include "ff_node_rule.internal.h"
#include "ff_node_pattern.internal.h"
#include "ff_node_patterns.internal.h"
#include "ff_parser.h"
#include "logging.h"

typedef struct ff_node_test
{
  xunit_test;

  char * string;
  char * sequence;
} ff_node_test;

static xapi ff_node_unit_setup(xunit_unit * unit)
{
  enter;

  fatal(logging_finalize);

  finally : coda;
}

//
// tests
//

static xapi ff_node_rtl_test_entry(xunit_test * _test)
{
  enter;

  ff_node_test * test = (typeof(test))_test;
  ff_node_rule * ffn = 0;
  narrator * N = 0;

  fatal(ff_parser_parse, 0, MMS(test->string), 0, (ff_node**)&ffn);
  fatal(narrator_growing_create, &N);

  ffn_bydir_context walk;
  ffn_pattern_bydir_rtl_init(ffn->consequent_list->chain->chain, &walk);

  char * seq = test->sequence;
  while(*seq || walk.first)
  {
    assert_ne_c(0, *seq);
    assert_notnull(walk.first);

    while(*seq && *seq == ' ')
      seq++;

    char * expected = seq;
    while(*seq && *seq != ' ')
      seq++;

    fatal(narrator_xreset, N);
    fatal(ffn_segment_say_normal, walk.first, walk.stop, N);
    assert_eq_w(expected, seq - expected, narrator_growing_buffer(N), narrator_growing_size(N));

    ffn_pattern_bydir_rtl(&walk);
  }

finally:
  ffn_free(ffn);
  fatal(narrator_xfree, N);
coda;
}

static xapi ff_node_ltr_test_entry(xunit_test * _test)
{
  enter;

  ff_node_test * test = (typeof(test))_test;
  ff_node_rule * ffn = 0;
  narrator * N = 0;

  fatal(ff_parser_parse, 0, MMS(test->string), 0, (ff_node**)&ffn);
  fatal(narrator_growing_create, &N);

  ffn_bydir_context walk;
  ffn_pattern_bydir_ltr_init(ffn->consequent_list->chain->chain, &walk);

  char * seq = test->sequence;
  while(*seq || walk.first)
  {
    assert_ne_c(0, *seq);
    assert_notnull(walk.first);

    while(*seq && *seq == ' ')
      seq++;

    char * expected = seq;
    while(*seq && *seq != ' ')
      seq++;

    fatal(narrator_xreset, N);
    fatal(ffn_segment_say_normal, walk.first, walk.stop, N);
    assert_eq_w(expected, seq - expected, narrator_growing_buffer(N), narrator_growing_size(N));

    ffn_pattern_bydir_ltr(&walk);
  }

finally:
  ffn_free(ffn);
  fatal(narrator_xfree, N);
coda;
}

//
// manifest
//

xunit_unit xunit = {
    xu_setup : ff_node_unit_setup
  , xu_tests : (xunit_test*[]) {
    /* ffn_bydir_rtl tests */
      (ff_node_test[]) {{
          xu_entry : ff_node_rtl_test_entry
        , string : "rule A : foo"
        , sequence : "A"
      }}
    , (ff_node_test[]) {{
          xu_entry : ff_node_rtl_test_entry
        , string : "rule A.B : foo"
        , sequence : "A.B"
      }}
    , (ff_node_test[]) {{
          xu_entry : ff_node_rtl_test_entry
        , string : "rule A/B : foo"
        , sequence : "B A"
      }}
    , (ff_node_test[]) {{
          xu_entry : ff_node_rtl_test_entry
        , string : "rule A/B/C : foo"
        , sequence : "C B A"
      }}
    , (ff_node_test[]) {{
          xu_entry : ff_node_rtl_test_entry
        , string : "rule A/zZz.B/C : foo"
        , sequence : "C zZz.B A"
      }}
    , (ff_node_test[]) {{
          xu_entry : ff_node_rtl_test_entry
        , string : "rule $foo/A/B : foo"
        , sequence : "B A $foo"
      }}

    /* ffn_bydir_ltr tests */
    , (ff_node_test[]) {{
          xu_entry : ff_node_ltr_test_entry
        , string : "rule A : foo"
        , sequence : "A"
      }}
    , (ff_node_test[]) {{
          xu_entry : ff_node_ltr_test_entry
        , string : "rule A.B : foo"
        , sequence : "A.B"
      }}
    , (ff_node_test[]) {{
          xu_entry : ff_node_ltr_test_entry
        , string : "rule A/B : foo"
        , sequence : "A B"
      }}
    , (ff_node_test[]) {{
          xu_entry : ff_node_ltr_test_entry
        , string : "rule A/B/C : foo"
        , sequence : "A B C"
      }}
    , (ff_node_test[]) {{
          xu_entry : ff_node_ltr_test_entry
        , string : "rule A/zZz.B/C : foo"
        , sequence : "A zZz.B C"
      }}
    , (ff_node_test[]) {{
          xu_entry : ff_node_ltr_test_entry
        , string : "rule $foo/A/B : foo"
        , sequence : "$foo A B"
      }}
    , 0
  }
};
