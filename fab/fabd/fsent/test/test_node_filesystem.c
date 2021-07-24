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

#include "valyria/load.h"
#include "moria/load.h"

#include "valyria/list.h"
#include "valyria/map.h"
#include "moria/graph.h"
#include "moria/operations.h"
#include "moria/parser.h"
#include "yyutil/parser.h"

#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/growing.h"
#include "rule.h"
#include "fsent.h"
#include "filesystem.internal.h"
#include "module.h"
#include "node_operations_test.h"
#include "config_parser.h"
#include "config.internal.h"
#include "lookup.h"

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

static void node_path_test_unit_setup(xunit_unit * unit)
{
  valyria_load();
  moria_load();
  logging_finalize();

  filesystem_setup();
  module_setup();
  graph_setup();
  fsent_setup();
}

static void node_path_test_unit_cleanup(xunit_unit * unit)
{
  filesystem_cleanup();
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
  graph_parser * ops_parser = 0;
  fsent * n;
  const filesystem *fs;
  config_parser * parser = 0;
  configblob * cfg = 0;
  moria_vertex *v;

  // setup filesystems
  config_parser_create(&parser);
  config_parser_parse(parser, test->config, strlen(test->config) + 2, 0, 0, &cfg);

  // act
  cfg->filesystems.changed = true;
  filesystem_reconfigure(cfg, true);
  filesystem_reconfigure(cfg, false);

  // setup initial graph
  graph_parser_create(&ops_parser, &g_graph, &fsent_list, node_operations_test_dispatch, graph_vertex_attrs, graph_edge_attrs);
  node_operations_test_dispatch->create_vertex(ops_parser, &v, VERTEX_DIR, 0, MMS(test->base));
  node_operations_test_dispatch->connect(ops_parser, &g_root->vertex, v, EDGE_TYPE_FSTREE, 0);
  graph_parser_operations_parse(ops_parser, MMS(test->operations));

  typeof(*test->nodes) *nodes = test->nodes;
  while(*nodes)
  {
    resolve_fragment(MMS((*nodes)->name), &n);
    assert_notnull(n);

    // act
    fs = fsent_filesystem_get(n);

    // assert
    assert_infos("node", (*nodes)->name);
    assert_eq_u32((*nodes)->attrs, fs->attrs);
    assert_info_unstage();
    nodes++;
  }

finally:
  graph_parser_xfree(ops_parser);
  config_parser_xfree(parser);
  config_xfree(cfg);
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
     /* directory fsent */
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
