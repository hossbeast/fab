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
#include "valyria/load.h"
#include "moria/load.h"
#include "value/load.h"

#include "valyria/list.h"
#include "valyria/map.h"
#include "moria/graph.h"
#include "moria/operations.h"
#include "moria/parser.h"
#include "yyutil/parser.h"
#include "yyutil/load.h"

#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/growing.h"
#include "logging.h"
#include "rule.h"
#include "fsent.h"
#include "pattern_parser.h"
#include "pattern.h"
#include "shadow.h"
#include "node_operations_test.h"
#include "filesystem.internal.h"
#include "lookup.h"
#include "module.internal.h"
#include "selection.h"

typedef struct node_lookup_test
{
  XUNITTEST;

  char * operations;   // create the starting graph
  bool shadow;

  // context
  char * pattern;      // lookup pattern
  char * module;       // name for module node
  char * module_shadow; // module shadow id

  // expected
  char * abspath;
} node_lookup_test;

static xapi node_lookup_test_unit_setup(xunit_unit * unit)
{
  enter;

  fatal(valyria_load);
  fatal(moria_load);
  fatal(logging_finalize);

  fatal(filesystem_setup);
  fatal(graph_setup);
  fatal(fsent_setup);
  fatal(shadow_setup);

  finally : coda;
}

static xapi node_lookup_test_unit_cleanup(xunit_unit * unit)
{
  enter;

  fatal(yyutil_unload);
  fatal(value_unload);

  fatal(filesystem_cleanup);
  fatal(fsent_cleanup);
  fatal(graph_cleanup);

  finally : coda;
}

//
// tests
//

static xapi node_lookup_test_entry(xunit_test * _test)
{
  enter;

  node_lookup_test * test = containerof(_test, node_lookup_test, xu);
  char abspath[128];
  pattern_parser * parser = 0;
  yyu_location loc;
  pattern * pat = 0;
  graph_parser * op_parser = 0;
  selection sel;
  selected *result;
  module mod;
  char err[256];
  uint16_t errlen;

  fatal(selection_xinit, &sel);
  fatal(pattern_parser_create, &parser);

  // setup initial graph
  fatal(graph_parser_create, &op_parser, &g_graph, &fsent_list, node_operations_test_dispatch, graph_vertex_attrs, graph_edge_attrs);
  fatal(graph_parser_operations_parse, op_parser, MMS(test->operations));

  memset(&mod, 0, sizeof(mod));
  if(test->module)
  {
    fatal(resolve_fragment, MMS(test->module), &mod.dir_node);
  }
  if(test->module_shadow)
  {
    fatal(resolve_fragment, MMS(test->module_shadow), &mod.shadow);
    g_project_shadow = mod.shadow;
  }

  fatal(lookup_pattern_parse_partial, parser, test->pattern, strlen(test->pattern) + 2, "-test-", 0, &loc, &pat);
  assert_eq_u32(strlen(test->pattern), loc.l);

  // act
  fatal(pattern_lookup, pat, 0, &sel, err, sizeof(err), &errlen);
  assert_eq_w(0, 0, err, errlen);

  // assert
  fatal(selection_finalize, &sel);
  assert_eq_zu(1, llist_count(&sel.list));

  result = llist_first(&sel.list, selected, lln);

  fsent_absolute_path_znload(abspath, sizeof(abspath), containerof(result->v, fsent, vertex));
  assert_eq_s(test->abspath, abspath);

finally:
  fatal(pattern_parser_xfree, parser);
  pattern_free(pat);
  fatal(graph_parser_xfree, op_parser);
  fatal(selection_xdestroy, &sel);
coda;
}

//
// manifest
//

xunit_unit xunit = {
    xu_setup : node_lookup_test_unit_setup
  , xu_cleanup : node_lookup_test_unit_cleanup
  , xu_entry : node_lookup_test_entry
  , xu_tests : (node_lookup_test*[]) {
      (node_lookup_test[]) {{
          operations : "a/b/c/d"
        , pattern : (char[]) { "c\0" }
        , abspath : "/a/b/c"
      }}
    , (node_lookup_test[]) {{
          operations : "a/b/c/d"
        , pattern : (char[]) { "b/c\0" }
        , abspath : "/a/b/c"
      }}
    , (node_lookup_test[]) {{
          operations : "a/b/c/d"
        , pattern : (char[]) { "\"\\x63\"\0" }
        , abspath : "/a/b/c"
      }}
    , (node_lookup_test[]) {{
          operations : "a/bob/c/d"
        , pattern : (char[]) { "b\"o\"b/c\0" }
        , abspath : "/a/bob/c"
      }}
    , (node_lookup_test[]) {{
          operations : "a/.D/c"
        , pattern : (char[]) { "a/.D\0" }
        , abspath : "/a/.D"
      }}
    , (node_lookup_test[]) {{
          operations : "a/b.D/c/d"
        , pattern : (char[]) { "b.D/c\0" }
        , abspath : "/a/b.D/c"
      }}

    /* relative, down */
    , (node_lookup_test[]) {{
          operations : "a/bob/c/d/e"
        , pattern : (char[]) { "./c/d\0" }
        , abspath : "/a/bob/c/d"
      }}
    , (node_lookup_test[]) {{
          operations : "a/bob/c/d"
        , pattern : (char[]) { "bob/c\0" }
        , abspath : "/a/bob/c"
      }}

    /* relative, up */
    , (node_lookup_test[]) {{
          operations : "a/bob/c/d"
        , pattern : (char[]) { "../c\0" }
        , abspath : "/a/bob/c"
      }}
    , 0
  }
};
