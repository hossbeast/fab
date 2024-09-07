/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "moria/load.h"

#include "valyria/list.h"
#include "moria/graph.h"
#include "moria/parser.h"
#include "yyutil/parser.h"
#include "bacon/load.h"

#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/growing.h"
#include "rule.h"
#include "fsent.h"
#include "fstree.h"
#include "module.h"
#include "node_operations_test.h"
#include "lookup.h"
#include "graph.h"
#include "shadow.h"

typedef struct node_path_test {
  XUNITTEST;

  char * operations;    // create the graph
  char * module;        // module node
  char * node;          // node
  char * base;          // base

  char * absolute_path;         // expected absolute path
  char * relative_path;         // expected path relative to base
} node_path_test;

static void node_path_test_unit_setup(xunit_unit * unit)
{
  moria_load();
  bacon_load();

  fsent_root_setup();
  shadow_setup();
  fstree_setup();
  module_setup();
  graph_setup();
  fsent_root_setup();
}

static void node_path_test_unit_cleanup(xunit_unit * unit)
{
  bacon_unload();
  moria_unload();

  fstree_cleanup();
  module_cleanup();
  fsent_cleanup();
  graph_cleanup();
}

//
// tests
//

static void node_path_test_entry(xunit_test * _test)
{
  node_path_test * test = (node_path_test *)_test;
  narrator_growing * N = 0;
  moria_parser * parser = 0;
  fsent *subject;
  fsent *module_node = 0;
  fsent *base_node = 0;
  char actual[PATH_MAX];
  module mod;
  size_t pathl;
  const char *absolute;
  const char *relative;

  narrator_growing_create(&N);

  // setup initial graph
  moria_parser_create(&parser, &g_graph, (llist *[]) { &fsent_list }, 1, node_operations_test_dispatch, graph_vertex_attrs, graph_edge_attrs);
  moria_parser_operations_parse(parser, MMS(test->operations));

  resolve_fragment(MMS(test->node), &subject);
  if(test->module)
  {
    resolve_fragment(MMS(test->module), &module_node);
    assert_notnull(module_node);

    subject->mod = &mod;
    fsent_parent(subject)->mod = &mod;
  }
  if(test->base)
  {
    resolve_fragment(MMS(test->base), &base_node);
    assert_notnull(base_node);
  }

  g_base = base_node;
  mod.dir_node = module_node;

  // act
  if(test->absolute_path)
  {
    pathl = fsent_absolute_path_cat(actual, sizeof(actual), subject);
    absolute = actual;
    assert_eq_w(test->absolute_path, strlen(test->absolute_path), absolute, pathl);
  }

  if(test->relative_path)
  {
    pathl = fsent_relative_path_cat(actual, sizeof(actual), subject, base_node);
    relative = actual;
    assert_eq_w(test->relative_path, strlen(test->relative_path), relative, pathl);
  }

  narrator_growing_free(N);
  moria_parser_free(parser);
}

//
// manifest
//

static xunit_unit xunit = {
    xu_setup : node_path_test_unit_setup
  , xu_cleanup : node_path_test_unit_cleanup
  , xu_entry : node_path_test_entry
  , xu_tests : (node_path_test*[]) {
     /* directory fsent */
      (node_path_test[]) {{
          operations : "B/C/D/E"
        , base : "B"
        , module : "C"
        , node : "D"

        , relative_path : "C/D"
        , absolute_path : "/B/C/D"
      }}
    /* file fsent */
    , (node_path_test[]) {{
          operations : "B/C/D/E"
        , base : "B"
        , module : "C"
        , node : "E"

        , relative_path : "C/D/E"
        , absolute_path : "/B/C/D/E"
      }}
    /* node which does not belong to a module */
    , (node_path_test[]) {{
          operations :
            " B/C/D/E"
            " b/c/d/e"
        , base : "B"
        , node : "e"

        , relative_path : "../b/c/d/e"
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
    , 0
  }
};
XUNIT_UNIT(xunit);
