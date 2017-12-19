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

#include "moria/load.h"
#include "valyria/load.h"
#include "xlinux/xstdlib.h"

#include "xunit.h"
#include "xunit/assert.h"
#include "moria/graph.h"
#include "moria/operations.h"
#include "narrator.h"
#include "narrator/growing.h"
#include "valyria/list.h"
#include "valyria/map.h"
#include "valyria/set.h"
#include "valyria/assert.h"

#include "artifact.h"
#include "ff_node.h"
#include "ff_node_pattern.h"
#include "ff_node_patterns.h"
#include "ff_node_require.h"
#include "ff_parser.h"
#include "logging.h"
#include "node.h"
#include "node_operations.h"
#include "path.h"
#include "rule.internal.h"

static map * attrs_definitions;

typedef struct ff_node_pattern_test
{
  xunit_test;

  char * pattern;             // context node for applying rules
  char ** paths;
} ff_node_pattern_test;

static xapi ff_node_pattern_test_unit_setup(xunit_unit * unit)
{
  enter;

  fatal(valyria_load);
  fatal(moria_load);
  fatal(logging_finalize);

  // attrs name map for graph_say
  fatal(map_create, &attrs_definitions);
  fatal(map_set, attrs_definitions, (uint32_t[]) { RELATION_TYPE_FS }, sizeof(uint32_t), "FS");
  fatal(map_set, attrs_definitions, (uint32_t[]) { RELATION_TYPE_STRONG }, sizeof(uint32_t), "ST");
  fatal(map_set, attrs_definitions, (uint32_t[]) { NODE_FSTYPE_FILE }, sizeof(uint32_t), "FILE");
  fatal(map_set, attrs_definitions, (uint32_t[]) { NODE_FSTYPE_DIR}, sizeof(uint32_t), "DIR");

  finally : coda;
}

static xapi ff_node_pattern_test_unit_cleanup(xunit_unit * unit)
{
  enter;

  fatal(map_xfree, attrs_definitions);

  finally : coda;
}

//
// tests
//

static xapi ff_node_pattern_test_entry(xunit_test * _test)
{
  enter;

  ff_node_pattern_test * test = (ff_node_pattern_test *)_test;

  narrator * N = 0;
  ff_parser * parser = 0;
  ff_node * ffn = 0;
  ff_node_patterns * patterns = 0;
  rendered_patterns * block = 0;
  set * expected = 0;
  set * actual = 0;
  uint16_t expected_num;

  fatal(narrator_growing_create, &N);
  fatal(set_create, &expected);
  fatal(set_create, &actual);
  expected_num = sentinel(test->paths);

  // parse the rules
  fatal(ff_parser_parse, &parser, MMS(test->pattern), 0, &ffn);
  patterns = ((ff_node_require*)ffn)->target_list;

  // act
  fatal(ffn_patterns_render, patterns, &block);

  // assert
  assert_notnull(block);

  int x;
  for(x = 0; x < expected_num; x++)
    fatal(set_put, expected, MMS(test->paths[x]));

  typeof(*block->items) *item;

  for(x = 0; x < expected_num; x++)
    fatal(set_put, expected, MMS(test->paths[x]));

  item = block->items;
  for(x = 0; x < block->num; x++)
  {
    fatal(set_put, actual, item->name, item->len);
    item = rendered_item_next(item);
  }

  assert_eq_set(expected, actual);
  assert_eq_d(expected_num, block->num);

finally:
  wfree(block);
  fatal(narrator_xfree, N);
  ff_parser_free(parser);
  fatal(set_xfree, expected);
  fatal(set_xfree, actual);
coda;
}

//
// manifest
//

xunit_unit xunit = {
    xu_setup : ff_node_pattern_test_unit_setup
  , xu_cleanup : ff_node_pattern_test_unit_cleanup
  , xu_entry : ff_node_pattern_test_entry
  , xu_tests : (xunit_test*[]) {
/* alternations */
      (ff_node_pattern_test[]) {{
          pattern : "require a"
        , paths : (char*[]) {
              "a"
            , 0
          }
      }}
    , (ff_node_pattern_test[]) {{
          pattern : "require lib{xapi,xlinux,xunit}, lib{logger,narrator}"
        , paths : (char*[]) {
              "libxapi"
            , "libxlinux"
            , "libxunit"
            , "liblogger"
            , "libnarrator"
            , 0
          }
      }}
    , (ff_node_pattern_test[]) {{
          pattern : "require {a}"
        , paths : (char*[]) {
              "a"
            , 0
          }
      }}
    , (ff_node_pattern_test[]) {{
          pattern : "require {a,}"
        , paths : (char*[]) {
              "a"
            , ""
            , 0
          }
      }}
    , (ff_node_pattern_test[]) {{
          pattern : "require {,a}"
        , paths : (char*[]) {
              "a"
            , ""
            , 0
          }
      }}
    , (ff_node_pattern_test[]) {{
          pattern : "require {{a}}"
        , paths : (char*[]) {
              "a"
            , 0
          }
      }}
    , (ff_node_pattern_test[]) {{
          pattern : "require {{{a}}}"
        , paths : (char*[]) {
              "a"
            , 0
          }
      }}
    , (ff_node_pattern_test[]) {{
          pattern : "require {{{a}}}b"
        , paths : (char*[]) {
              "ab"
            , 0
          }
      }}
    , (ff_node_pattern_test[]) {{
          pattern : "require {{{a}}b}"
        , paths : (char*[]) {
              "ab"
            , 0
          }
      }}
    , (ff_node_pattern_test[]) {{
          pattern : "require {{{a}b}}"
        , paths : (char*[]) {
              "ab"
            , 0
          }
      }}
    , (ff_node_pattern_test[]) {{
          pattern : "require ab{c,d}"
        , paths : (char*[]) {
              "abc"
            , "abd"
            , 0
          }
      }}
    , (ff_node_pattern_test[]) {{
          pattern : "require ab{c,d}e"
        , paths : (char*[]) {
              "abce"
            , "abde"
            , 0
          }
      }}
    , (ff_node_pattern_test[]) {{
          pattern : "require a{b{1,2}}"
        , paths : (char*[]) {
              "ab1"
            , "ab2"
            , 0
          }
      }}
    , (ff_node_pattern_test[]) {{
          pattern : "require a{b{c,d}e}f"
        , paths : (char*[]) {
              "abcef"
            , "abdef"
            , 0
          }
      }}
    , (ff_node_pattern_test[]) {{
          pattern : "require a{b{c,d}}ef"
        , paths : (char*[]) {
              "abcef"
            , "abdef"
            , 0
          }
      }}
    , (ff_node_pattern_test[]) {{
          pattern : "require a{{b,c}d}ef"
        , paths : (char*[]) {
              "abdef"
            , "acdef"
            , 0
          }
      }}
    , (ff_node_pattern_test[]) {{
          pattern : "require {a,b}{c,d}"
        , paths : (char*[]) {
              "ac"
            , "ad"
            , "bc"
            , "bd"
            , 0
          }
      }}
    , (ff_node_pattern_test[]) {{
          pattern : "require {a{b,c}}"
        , paths : (char*[]) {
              "ab"
            , "ac"
            , 0
          }
      }}

/* classes */
    , (ff_node_pattern_test[]) {{
          pattern : "require [cd]"
        , paths : (char*[]) {
              "c"
            , "d"
            , 0
          }
      }}
    , (ff_node_pattern_test[]) {{
          pattern : "require ab[cd]"
        , paths : (char*[]) {
              "abc"
            , "abd"
            , 0
          }
      }}
    , (ff_node_pattern_test[]) {{
          pattern : "require [ab]cd"
        , paths : (char*[]) {
              "acd"
            , "bcd"
            , 0
          }
      }}
    , (ff_node_pattern_test[]) {{
          pattern : "require ab[cd]ef"
        , paths : (char*[]) {
              "abcef"
            , "abdef"
            , 0
          }
      }}
    , (ff_node_pattern_test[]) {{
          pattern : "require ab[c-f]"
        , paths : (char*[]) {
              "abc"
            , "abd"
            , "abe"
            , "abf"
            , 0
          }
      }}
    , (ff_node_pattern_test[]) {{
          pattern : "require {[c-d]}"
        , paths : (char*[]) {
              "c"
            , "d"
            , 0
          }
      }}
    , 0
  }
};
