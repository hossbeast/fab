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

#include "valyria/list.h"
#include "valyria/map.h"
#include "moria/graph.h"
#include "moria/operations.h"
#include "yyutil/parser.h"

#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/growing.h"
#include "logging.h"
#include "rule.internal.h"
#include "node.internal.h"
#include "node_operations.h"
#include "path.h"
#include "filesystem.internal.h"
#include "module.h"
#include "node_operations_test.h"
#include "lookup.internal.h"
#include "graph.h"
#include "shadow.h"

typedef struct node_path_test {
  XUNITTEST;

  char * operations;    // create the graph
  bool shadow;
  char * project;       // project node
  char * module;        // module node
  char * node;          // node
  char * base;          // base

  char * project_relative_path; // expected project-relative path
  char * module_relative_path;  // expected module-relative path
  char * absolute_path;         // expected absolute path
  char * relative_path;         // expected path relative to base
} node_path_test;

static xapi node_path_test_unit_setup(xunit_unit * unit)
{
  enter;

  fatal(valyria_load);
  fatal(moria_load);
  fatal(logging_finalize);

  fatal(filesystem_setup);
  fatal(module_setup);
  fatal(graph_setup);
  fatal(node_setup);

  finally : coda;
}

static xapi node_path_test_unit_cleanup(xunit_unit * unit)
{
  enter;

  fatal(filesystem_cleanup);
  fatal(module_cleanup);
  fatal(node_cleanup);
  fatal(graph_cleanup);

  finally : coda;
}

//
// tests
//

static xapi node_path_test_entry(xunit_test * _test)
{
  enter;

  node_path_test * test = (node_path_test *)_test;
  narrator_growing * N = 0;
  list * operations = 0;
  operations_parser * parser = 0;
  node * subject;
  node * project_node = 0;
  node * module_node = 0;
  node * base_node = 0;
  char actual[512];
  module mod;
  size_t pathl;

  if(test->shadow)
  {
    fatal(shadow_setup);
  }

  fatal(narrator_growing_create, &N);

  // setup initial graph
  fatal(operations_parser_operations_create, &operations);
  fatal(operations_parser_create, &parser);
  fatal(operations_parser_parse, parser, g_graph, MMS(test->operations), operations);
  fatal(operations_perform, g_graph, node_operations_test_dispatch, operations);

  fatal(pattern_lookup_fragment, MMS(test->node), 0, 0, 0, 0, 0, 0, &subject);
  assert_notnull(subject);
  if(test->project)
  {
    fatal(pattern_lookup_fragment, MMS(test->project), 0, 0, 0, 0, 0, 0, &project_node);
    assert_notnull(project_node);
  }
  if(test->module)
  {
    fatal(pattern_lookup_fragment, MMS(test->module), 0, 0, 0, 0, 0, 0, &module_node);
    assert_notnull(module_node);

    subject->mod = &mod;
    node_fsparent(subject)->mod = &mod;
  }
  if(test->base)
  {
    fatal(pattern_lookup_fragment, MMS(test->base), 0, 0, 0, 0, 0, 0, &base_node);
    assert_notnull(base_node);
  }

  g_project_root = project_node;
  mod.dir_node = module_node;

  // act
  if(test->project_relative_path)
  {
    pathl = node_project_relative_path_znload(actual, sizeof(actual), subject);
    const char *project_relative = actual;
    assert_eq_w(test->project_relative_path, strlen(test->project_relative_path), project_relative, pathl);
  }

  if(test->module_relative_path)
  {
    pathl = node_module_relative_path_znload(actual, sizeof(actual), subject);
    const char *module_relative = actual;
    assert_eq_w(test->module_relative_path, strlen(test->module_relative_path), module_relative, pathl);
  }

  if(test->absolute_path)
  {
    pathl = node_absolute_path_znload(actual, sizeof(actual), subject);
    const char *absolute = actual;
    assert_eq_w(test->absolute_path, strlen(test->absolute_path), absolute, pathl);
  }

  if(test->relative_path)
  {
    pathl = node_relative_path_znload(actual, sizeof(actual), subject, base_node);
    const char *relative = actual;
    assert_eq_w(test->relative_path, strlen(test->relative_path), relative, pathl);
  }

finally:
  fatal(narrator_growing_free, N);
  fatal(list_xfree, operations);
  fatal(operations_parser_xfree, parser);
coda;
}

//
// manifest
//

xunit_unit xunit = {
    xu_setup : node_path_test_unit_setup
  , xu_cleanup : node_path_test_unit_cleanup
  , xu_entry : node_path_test_entry
  , xu_tests : (node_path_test*[]) {
     /* directory node */
      (node_path_test[]) {{
          operations : "B/C/D/E"
        , project : "B"
        , module : "C"
        , node : "D"

        , project_relative_path : "C/D"
        , module_relative_path : "D"
        , absolute_path : "/B/C/D"
      }}
    /* file node */
    , (node_path_test[]) {{
          operations : "B/C/D/E"
        , project : "B"
        , module : "C"
        , node : "E"

        , project_relative_path : "C/D/E"
        , module_relative_path : "D/E"
        , absolute_path : "/B/C/D/E"
      }}
    /* node which does not belong to a module */
    , (node_path_test[]) {{
          operations :
            " B/C/D/E"
            " b/c/d/e"
        , project : "B"
        , node : "e"

        , project_relative_path : "../b/c/d/e"
        , module_relative_path : "/b/c/d/e"
        , absolute_path : "/b/c/d/e"
      }}
    , (node_path_test[]) {{
          operations :
            " B/C/D/E"
        , node : "D"
        , base : "C"
        , relative_path : "D"
      }}
    , (node_path_test[]) {{
          operations :
            " B/C/D/E"
        , node : "C"
        , base : "E"
        , relative_path : "../.."
      }}
    , (node_path_test[]) {{
          operations :
            " B/C/D/E"
        , node : "D"
        , base : "E"
        , relative_path : ".."
      }}
    , (node_path_test[]) {{
          operations :
            " B/C/D/E"
        , node : "E"
        , base : "D"
        , relative_path : "E"
      }}
    , (node_path_test[]) {{
          operations :
            " B/C/D/E"
        , node : "E"
        , base : "C"
        , relative_path : "D/E"
      }}
    , (node_path_test[]) {{
          operations :
            " B/C/D/E"
        , node : "B"
        , base : "B"
        , relative_path : "."
      }}
    , (node_path_test[]) {{
          operations :
            " B/C/D/E"
            " b/c/d/e"
        , node : "C"
        , base : "c"
        , relative_path : "../../B/C"
      }}
    , (node_path_test[]) {{
          operations :
            " B/C/D/E"
            " B/x/y/z"
        , base : "x"
        , node : "C"
        , relative_path : "../C"
      }}
    , (node_path_test[]) {{
          operations :
            " B/b/C/D/E"
            " B/b/x/y/z"
        , base : "x"
        , node : "C"
        , relative_path : "../C"
      }}
    , (node_path_test[]) {{
          operations :
            " B/C/D/E"
            " B/b/x/y/z"
        , base : "x"
        , node : "C"
        , relative_path : "../../C"
      }}
    , (node_path_test[]) {{
          operations :
            " B/b/C/D/E"
            " B/x/y/z"
        , base : "x"
        , node : "C"
        , relative_path : "../b/C"
      }}

    /* under the shadow fs */
    , (node_path_test[]) {{
          operations :
            " //modules/foo"
        , shadow : true
        , node : "foo"
        , absolute_path : "//modules/foo"
      }}
    , 0
  }
};
