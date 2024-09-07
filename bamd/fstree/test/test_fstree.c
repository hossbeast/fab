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

#include "args.h"
#include "rule.h"
#include "fsent.h"
#include "fstree.h"
#include "module.h"
#include "node_operations_test.h"
#include "configset_parser.h"
#include "configset.h"
#include "config.h"
#include "lookup.h"
#include "shadow.h"
#include "config_system.h"
#include "zcat.h"
#include "fsindex.h"
#include "notify.h"

typedef struct node_filesystem_test {
  XUNITTEST;

  /* setup */
  char *config;        // filesystems config
  char *operations;    // create the graph

  /* assert */
  struct node_check {
    char *name;           // node to check
    bool mkdirs;          // expected
    bool readlinks;
    bool indexed;
    invalidation_type invalidation;
  } **nodes;
} node_filesystem_test;

static void node_filesystem_test_unit_setup(xunit_unit * unit)
{
  moria_load();
  bacon_load();

  args_init();
  fsent_root_setup();
  fsent_base_setup();
  shadow_setup();
  fstree_setup();
  module_setup();
  graph_setup();
  fsindex_setup();
  notify_setup();
  config_system_setup();
}

static void node_filesystem_test_unit_cleanup(xunit_unit * unit)
{
  moria_unload();
  bacon_unload();

  fstree_cleanup();
  module_cleanup();
  fsent_cleanup();
  graph_cleanup();
  shadow_cleanup();
}

//
// tests
//

static void node_filesystem_test_entry(xunit_test * _test)
{
  node_filesystem_test * test = (node_filesystem_test *)_test;
  moria_parser * ops_parser = 0;
  fsent * n;
  const fstree *fst;
  configset_parser *parser = 0;
  config *cfg;
  graph_invalidation_context invalidation = {};

  // setup filesystems
  configset_parser_create(&parser);
  config_alloc(&cfg);
  if(configset_parser_parse(parser, test->config, strlen(test->config) + 2, 0, 0, cfg) != 0) {
    tracef("%.*s", parser->yyu.error_len, parser->yyu.error_str);
    RTA;
  }

  // act
  fstree_reconfigure(cfg, true);
  fstree_reconfigure(cfg, false);

  // setup graph nodes
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
    fst = vertex_fstree_get(&n->vertex);

    // assert
    assert_infos("node", (*nodes)->name);
    assert_eq_u32((*nodes)->invalidation, fst->invalidation);
    assert_info_unstage();
    nodes++;
  }

  moria_parser_free(ops_parser);
  config_free(cfg, parser);
  configset_parser_free(parser);
}

//
// manifest
//

static xunit_unit xunit = {
    xu_setup : node_filesystem_test_unit_setup
  , xu_cleanup : node_filesystem_test_unit_cleanup
  , xu_entry : node_filesystem_test_entry
  , xu_tests : (node_filesystem_test*[]) {
      (node_filesystem_test[]) {{
          config : (char[]) {
            ""
          }
        , operations : "A/BB/C/D"
        , nodes : (struct node_check*[]) {
              (struct node_check[]) {{
                  name : "A"
                , invalidation : INVALIDATION_STAT
              }}
            , (struct node_check[]) {{
                  name : "BB"
                , invalidation : INVALIDATION_STAT
              }}
            , (struct node_check[]) {{
                  name : "C"
                , invalidation : INVALIDATION_STAT
              }}
            , (struct node_check[]) {{
                  name : "D"
                , invalidation : INVALIDATION_STAT
              }}
            , 0
          }
      }}
      , (node_filesystem_test[]) {{
          config : (char[]) {
            "filesystem : {\n"
            "  fstrees : {\n"
            "    / : {\n"
            "      invalidation : stat\n"
            "    }\n"
            "  }\n"
            "}\n"
            "\0\0"
          }
        , operations : "A"
        , nodes : (struct node_check*[]) {
              (struct node_check[]) {{
                  name : "A"
                , invalidation : INVALIDATION_STAT
              }}
            , 0
          }
      }}
      , (node_filesystem_test[]) {{
          config : (char[]) {
            "filesystem : {\n"
            "  fstrees : {\n"
            "    / : {\n"
            "      invalidation : content\n"
            "    }\n"
            "    /A : {\n"
            "      invalidation : notify\n"
            "    }\n"
            "  }\n"
            "}\0\0"
          }
        , operations : "A/B C/D"
        , nodes : (struct node_check*[]) {
              (struct node_check[]) {{
                  name : "C"
                , invalidation : INVALIDATION_CONTENT
              }}
            , (struct node_check[]) {{
                  name : "A"
                , invalidation : INVALIDATION_NOTIFY
              }}
            , 0
          }
      }}
      , (node_filesystem_test[]) {{
          config : (char[]) {
            "filesystem : {\n"
            "  fstrees : {\n"
            "    / : {\n"
            "      invalidation : stat\n"
            "    }\n"
            "  }\n"
            "}\n"
            "\0\0"
          }
        , operations : "A"
        , nodes : (struct node_check*[]) {
              (struct node_check[]) {{
                  name : "A"
                , invalidation : INVALIDATION_STAT
              }}
            , 0
          }
      }}

      , (node_filesystem_test[]) {{
          config : (char[]) {
            "filesystem : {\n"
            "  fstrees : {\n"
            "    /A : {\n"
            "      invalidation : content\n"
            "    }\n"
            "    /A/BB/C : {\n"
            "      invalidation : notify\n"
            "    }\n"
            "    /A/B : {\n"
            "      invalidation : stat\n"
            "    }\n"
            "  }\n"
            "}\0\0"
          }
        , operations : "A/BB/C/D"
        , nodes : (struct node_check*[]) {
              (struct node_check[]) {{
                  name : "A"
                , invalidation : INVALIDATION_CONTENT
              }}
            , (struct node_check[]) {{
                  name : "BB"
                , invalidation : INVALIDATION_CONTENT
              }}
            , (struct node_check[]) {{
                  name : "C"
                , invalidation : INVALIDATION_NOTIFY
              }}
            , (struct node_check[]) {{
                  name : "D"
                , invalidation : INVALIDATION_NOTIFY
              }}
            , 0
          }
      }}
    , 0
  }
};
XUNIT_UNIT(xunit);
