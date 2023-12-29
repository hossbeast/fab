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
#include "filesystem.internal.h"
#include "module.h"
#include "node_operations_test.h"
#include "config_parser.h"
#include "config.h"
#include "configuration.h"
#include "lookup.h"

typedef struct node_filesystem_test {
  XUNITTEST;

  /* setup */
  char * config;        // filesystems config
  char * operations;    // create the graph

  /* assert */
  struct node_check {
    char * name;          // node to check
    bool mkdirs;          // expected
    bool readlinks;
    bool indexed;
    invalidate_type invalidate;
  } **nodes;
} node_filesystem_test;

static void node_filesystem_test_unit_setup(xunit_unit * unit)
{
  moria_load();
  bacon_load();

  filesystem_setup();
  module_setup();
  graph_setup();
  fsent_setup();
}

static void node_filesystem_test_unit_cleanup(xunit_unit * unit)
{
  moria_unload();
  bacon_unload();
  filesystem_cleanup();
  module_cleanup();
  fsent_cleanup();
  graph_cleanup();
}

//
// tests
//

static void node_filesystem_test_entry(xunit_test * _test)
{
  node_filesystem_test * test = (node_filesystem_test *)_test;
  moria_parser * ops_parser = 0;
  fsent * n;
  const filesystem *fs;
  config_parser *parser = 0;
  configuration *cfg;
  graph_invalidation_context invalidation = {};

  // setup filesystems
  config_parser_create(&parser);
  configuration_alloc(&cfg);
  config_parser_parse(parser, test->config, strlen(test->config) + 2, 0, 0, cfg);

  // act
  filesystem_reconfigure(cfg, true);
  filesystem_reconfigure(cfg, false);

  // setup initial graph
  moria_parser_create(&ops_parser, &g_graph, (llist *[]) { &fsent_list }, 1, node_operations_test_dispatch, graph_vertex_attrs, graph_edge_attrs);
  moria_parser_operations_parse(ops_parser, MMS(test->operations));

  graph_invalidation_begin(&invalidation);
  fsent_fstree_refresh(&invalidation);
  graph_invalidation_end(&invalidation);

  typeof(*test->nodes) *nodes = test->nodes;
  while(*nodes)
  {
    resolve_fragment(MMS((*nodes)->name), &n);
    assert_notnull(n);

    // act
    fs = fsent_filesystem_get(n);

    // assert
    assert_infos("node", (*nodes)->name);
    assert_eq_u32((*nodes)->invalidate, fs->invalidate);
    assert_info_unstage();
    nodes++;
  }

  moria_parser_free(ops_parser);
  configuration_free(cfg, parser);
  config_parser_free(parser);
}

//
// manifest
//

static xunit_unit xunit = {
    xu_setup : node_filesystem_test_unit_setup
  , xu_cleanup : node_filesystem_test_unit_cleanup
  , xu_entry : node_filesystem_test_entry
  , xu_tests : (node_filesystem_test*[]) {
     /* directory fsent */
      (node_filesystem_test[]) {{
          config : (char[]) {
            "filesystems : {"
              "\"/A\"      : { invalidate : content } "
              "\"/A/BB/C\" : { invalidate : notify } "
              "\"/A/B\"    : { invalidate : stat } "
            "}\0\0"
        , }
        , operations : "A/BB/C/D"
        , nodes : (struct node_check*[]) {
              (struct node_check[]) {{
                  name : "A"
                , invalidate : INVALIDATE_CONTENT
              }}
            , (struct node_check[]) {{
                  name : "BB"
                , invalidate : INVALIDATE_CONTENT
              }}
            , (struct node_check[]) {{
                  name : "C"
                , invalidate : INVALIDATE_NOTIFY
              }}
            , (struct node_check[]) {{
                  name : "D"
                , invalidate : INVALIDATE_NOTIFY
              }}
            , 0
          }
      }}
    , 0
  }
};
XUNIT_UNIT(xunit);
