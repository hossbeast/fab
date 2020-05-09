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
#include "node.h"
#include "node_operations.h"
#include "path.h"
#include "filesystem.internal.h"
#include "module.h"
#include "node_operations_test.h"
#include "config_parser.h"
#include "config.internal.h"
#include "lookup.internal.h"

typedef struct node_path_test {
  XUNITTEST;

  /* setup */
  char * config;        // filesystems config
  char * operations;    // create the graph
  char * base;          // attached to root

  /* assert */
  struct node_check {
    char * name;          // node to check
    uint32_t attrs;       // expected fs attrs
  } **nodes;
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
  narrator * N = 0;
  list * operations = 0;
  operations_parser * ops_parser = 0;
  node * n;
  const filesystem *fs;
  config_parser * parser = 0;
  config * cfg = 0;
  vertex *v;

  // setup filesystems
  fatal(config_parser_create, &parser);
  fatal(config_parser_parse, parser, test->config, strlen(test->config) + 2, 0, 0, &cfg);

  // act
  cfg->filesystems.changed = true;
  fatal(filesystem_reconfigure, cfg, true);
  fatal(filesystem_reconfigure, cfg, false);

  // setup initial graph
  fatal(node_operations_test_dispatch->create_vertex, &v, g_graph, VERTEX_DIR, 0, MMS(test->base));
  fatal(node_operations_test_dispatch->connect, g_graph, vertex_containerof(g_root), v, EDGE_TYPE_FS, 0, 0);

  fatal(operations_parser_operations_create, &operations);
  fatal(operations_parser_create, &ops_parser);
  fatal(operations_parser_parse, ops_parser, g_graph, MMS(test->operations), operations);
  fatal(operations_perform, g_graph, node_operations_test_dispatch, operations);

  typeof(*test->nodes) *nodes = test->nodes;
  while(*nodes)
  {
    fatal(pattern_lookup_fragment, MMS((*nodes)->name), 0, 0, 0, 0, 0, 0, &n);
    assert_notnull(n);

    // act
    fs = node_filesystem_get(n);

    // assert
    assert_infos("node", (*nodes)->name);
    assert_eq_u32((*nodes)->attrs, fs->attrs);
    assert_info_unstage();
    nodes++;
  }

finally:
  fatal(narrator_xfree, N);
  fatal(list_xfree, operations);
  fatal(operations_parser_xfree, ops_parser);
  fatal(config_parser_xfree, parser);
  fatal(config_xfree, cfg);
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
          config : (char[]) {
            "filesystems : {"
              "\"/A\"      : { invalidate : content } "
              "\"/A/BB/C\" : { invalidate : notify } "
              "\"/A/B\"    : { invalidate : stat } "
            "}\0\0"
        , }
        , operations : "A/BB/C/D"
        , base : "A"
        , nodes : (struct node_check*[]) {
              (struct node_check[]) {{
                  name : "A"
                , attrs : INVALIDATE_CONTENT
              }}
            , (struct node_check[]) {{
                  name : "BB"
                , attrs : INVALIDATE_CONTENT
              }}
            , (struct node_check[]) {{
                  name : "C"
                , attrs : INVALIDATE_NOTIFY
              }}
            , (struct node_check[]) {{
                  name : "D"
                , attrs : INVALIDATE_NOTIFY
              }}
            , 0
          }
      }}
    , 0
  }
};
