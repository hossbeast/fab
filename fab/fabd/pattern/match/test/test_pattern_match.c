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
#include "xunit.h"
#include "xunit/assert.h"

#include "moria/graph.h"
#include "moria/vertex.h"
#include "moria/operations.h"
#include "valyria/list.h"
#include "valyria/map.h"

#include "match.h"
#include "artifact.h"
#include "ff_node.h"
#include "ff_node_pattern.h"
#include "ff_node_patterns.h"
#include "ff_node_rule.h"
#include "ff_parser.h"
#include "logging.h"
#include "narrator.h"
#include "narrator/growing.h"
#include "node.h"
#include "path.h"

typedef struct pattern_match_test
{
  xunit_test;

  char * node;
  char * pattern;
  char * variant;

  bool expected_matched;
  char * expected_stem;
} pattern_match_test;

static xapi pattern_match_test_unit_setup(xunit_unit * unit)
{
  enter;

  fatal(logging_finalize);

  finally : coda;
}

static xapi get_node(map * node_map, char * path, size_t len, node ** n)
{
  enter;

  node * A = 0;
  node * B;

  while(*path)
  {
    char * label = path;
    while(*path && *path != '/')
      path++;
    size_t label_len = path - label;

    if((B = map_get(node_map, label, label_len)) == 0)
    {
      vertex * v;
      fatal(vertex_createw, &v, g_node_graph, 0, label, label_len);
      B = vertex_value(v);
      fatal(path_createw, &B->name, label, label_len);
      fatal(map_set, node_map, label, label_len, B);
    }

    if(*path == '/')
      path++;

    if(A)
    {
      fatal(graph_connect, g_node_graph, vertex_containerof(A), vertex_containerof(B), NODE_RELATION_FS);
    }
    
    A = B;
  }

  *n = A;

  finally : coda;
}

//
// tests
//

static xapi pattern_match_test_entry(xunit_test * _test)
{
  enter;

  pattern_match_test * test = (pattern_match_test *)_test;

  map * node_map = 0;
  narrator * N = 0;
  ff_parser * parser = 0;
  ff_node * ffn = 0;
  list * nodes_list = 0;
  node * A = 0;

  fatal(node_setup);
  fatal(narrator_growing_create, &N);
  fatal(map_create, &node_map);
  fatal(list_create, &nodes_list);

  // context
  fatal(get_node, node_map, MMS(test->node), &A);
  artifact af = { variant : test->variant };
  if(test->variant)
    af.variant_len = strlen(test->variant);

  // parse the pattern
  fatal(ff_parser_parse, &parser, MMS(test->pattern), 0, &ffn);
  ff_node_pattern * pattern = ((ff_node_rule*)ffn)->consequent_list->chain;

//  fatal(ffn_say_tree, pattern, g_narrator_stdout);

  // act
  bool actual_matched = false;
  const char * actual_stem = 0;
  uint16_t actual_stem_len = 0;
  fatal(pattern_match, pattern, A, &af, &actual_matched, &actual_stem, &actual_stem_len);

  // assert
  assert_eq_b(test->expected_matched, actual_matched);
  assert_eq_w(test->expected_stem, test->expected_stem ? strlen(test->expected_stem) : 0, actual_stem, actual_stem_len);

finally:
  fatal(node_cleanup);
  fatal(map_xfree, node_map);
  fatal(narrator_xfree, N);
  ff_parser_free(parser);
  ffn_free(ffn);
  fatal(list_xfree, nodes_list);
coda;
}

//
// manifest
//

xunit_unit xunit = {
    xu_setup : pattern_match_test_unit_setup
  , xu_entry : pattern_match_test_entry
  , xu_tests : (xunit_test*[]) {
    /* single word */
      (pattern_match_test[]) {{
          node : "dir"
        , pattern : "rule main.o : xunit"
        , expected_matched : false
      }}
    , (pattern_match_test[]) {{
          node : "dir"
        , pattern : "rule dir : xunit"
        , expected_matched : true
      }}

    /* single word, multi directory */
    , (pattern_match_test[]) {{
          node : "A/B"
        , pattern : "rule main.o : xunit"
        , expected_matched : false
      }}
    , (pattern_match_test[]) {{
          node : "A/B"
        , pattern : "rule A/B : xunit"
        , expected_matched : true
      }}
    , (pattern_match_test[]) {{
          node : "A/B/C"
        , pattern : "rule main.o : xunit"
        , expected_matched : false
      }}
    , (pattern_match_test[]) {{
          node : "A/B/C"
        , pattern : "rule main.o : xunit"
        , expected_matched : false
      }}
    , (pattern_match_test[]) {{
          node : "A/B/C"
        , pattern : "rule A/B/C : xunit"
        , expected_matched : true 
      }}

    /* words and tokens */
    , (pattern_match_test[]) {{
          node : "foo.bar"
        , pattern : "rule main.o : xunit"
        , expected_matched : false
      }}
    , (pattern_match_test[]) {{
          node : "foo.bar"
        , pattern : "rule foo.bar : xunit"
        , expected_matched : true
      }}

    /* pattern with consecutive words */
    , (pattern_match_test[]) {{
          node : "BCD"
        , pattern : "rule B \"C\" D : xunit"
        , expected_matched : true 
      }}

    /* words, tokens, directories */
    , (pattern_match_test[]) {{
          node : "A.B/C/D"
        , pattern : "rule A.B/C/D : xunit"
        , expected_matched : true 
      }}
    , (pattern_match_test[]) {{
          node : "A/B.C/D"
        , pattern : "rule A/B.C/D : xunit"
        , expected_matched : true 
      }}
    , (pattern_match_test[]) {{
          node : "A/B/C.D"
        , pattern : "rule A/B/C.D : xunit"
        , expected_matched : true 
      }}

    /* alternations */
    , (pattern_match_test[]) {{
          node : "A"
        , pattern : "rule {A,B} : xunit"
        , expected_matched : true
      }}
    , (pattern_match_test[]) {{
          node : "B"
        , pattern : "rule {A,B} : xunit"
        , expected_matched : true 
      }}
    , (pattern_match_test[]) {{
          node : "C"
        , pattern : "rule {A,B} : xunit"
        , expected_matched : false 
      }}
    , (pattern_match_test[]) {{
          node : "A/BCD/E"
        , pattern : "rule A/B{X,C,Y}D/E : xunit"
        , expected_matched : true
      }}
    , (pattern_match_test[]) {{
          node : "X/E"
        , pattern : "rule {A.B,X}/E : xunit"
        , expected_matched : true
      }}

    /* classes */
    , (pattern_match_test[]) {{
          node : "A"
        , pattern : "rule [ABC] : xunit"
        , expected_matched : true
      }}
    , (pattern_match_test[]) {{
          node : "B"
        , pattern : "rule [ABC] : xunit"
        , expected_matched : true
      }}
    , (pattern_match_test[]) {{
          node : "C"
        , pattern : "rule [ABC] : xunit"
        , expected_matched : true
      }}
    , (pattern_match_test[]) {{
          node : "D"
        , pattern : "rule [ABC] : xunit"
        , expected_matched : false
      }}
    , (pattern_match_test[]) {{
          node : "\""
        , pattern : "rule [\"] : xunit"
        , expected_matched : true
      }}

    /* ranges */
    , (pattern_match_test[]) {{
          node : "A"
        , pattern : "rule [A-Z] : xunit"
        , expected_matched : true
      }}
    , (pattern_match_test[]) {{
          node : "A"
        , pattern : "rule [A-Z0-9] : xunit"
        , expected_matched : true
      }}
    , (pattern_match_test[]) {{
          node : "7"
        , pattern : "rule [A-Z0-9] : xunit"
        , expected_matched : true
      }}
    , (pattern_match_test[]) {{
          node : "a"
        , pattern : "rule [A-Z0-9] : xunit"
        , expected_matched : false
      }}
    , (pattern_match_test[]) {{
          node : "42"
        , pattern : "rule [A-Z0-9][0-9] : xunit"
        , expected_matched : true
      }}
    , (pattern_match_test[]) {{
          node : "7A"
        , pattern : "rule [A-Z][A-Z0-9] : xunit"
        , expected_matched : false
      }}
    , (pattern_match_test[]) {{
          node : "7A"
        , pattern : "rule [A-Z0-9][0-9] : xunit"
        , expected_matched : false
      }}

    /* stem */
    , (pattern_match_test[]) {{
          node : "foo.bar"
        , pattern : "rule foo% : xunit"
        , expected_matched : true
        , expected_stem : ".bar"
      }}
    , (pattern_match_test[]) {{
          node : "foo.bar"
        , pattern : "rule %.bar : xunit"
        , expected_matched : true
        , expected_stem : "foo"
      }}
    , (pattern_match_test[]) {{
          node : "foo.bar"
        , pattern : "rule %o.bar : xunit"
        , expected_matched : true
        , expected_stem : "fo"
      }}
    , (pattern_match_test[]) {{
          node : "foo.bar"
        , pattern : "rule %.{foo,bar,baz} : xunit"
        , expected_matched : true
        , expected_stem : "foo"
      }}
    , (pattern_match_test[]) {{
          node : "foo.foo"
        , pattern : "rule %.{foo,bar,baz} : xunit"
        , expected_matched : true
        , expected_stem : "foo"
      }}

    /* variant */
    , (pattern_match_test[]) {{
          node : "foo.foo"
        , variant : "xapi"
        , pattern : "rule %.? : xunit"
        , expected_matched : false
      }}
    , (pattern_match_test[]) {{
          node : "foo.xapi"
        , variant : "xapi"
        , pattern : "rule %.? : xunit"
        , expected_matched : true
        , expected_stem : "foo"
      }}
    , (pattern_match_test[]) {{
          node : "foo.xapi.bar"
        , variant : "xapi"
        , pattern : "rule %.?.bar : xunit"
        , expected_matched : true
        , expected_stem : "foo"
      }}
    , (pattern_match_test[]) {{
          node : "foo.xapi"
        , variant : "foo"
        , pattern : "rule ?.% : xunit"
        , expected_matched : true
        , expected_stem : "xapi"
      }}
    , 0
  }
};
