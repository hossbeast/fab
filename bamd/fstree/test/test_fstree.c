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
#include "shadow.h"
#include "config_system.h"
#include "zcat.h"
#include "namespace.h"
#include "notify.h"
#include "edges.h"
#include "vertices.h"
#include "edge.h"
#include "ns_edge.h"
#include "ref_edge.h"
#include "fsedge.h"

typedef struct node_filesystem_test {
  XUNITTEST;

  /* setup */
  char *config;        // filesystems config
  char *operations;    // create the graph

  /* assert */
  struct node_check {
    char *name;           // node to check

    /* expected values */
    bool mkdir;
    bool readlink;
    invalidation_type invalidation;
    char *namespaces;
  } **nodes;
} node_filesystem_test;

static void node_filesystem_test_unit_setup(xunit_unit * unit)
{
  moria_load();
  bacon_load();
  args_init();

  fsent_root_setup();
  shadow_root_setup();
  fstree_root_setup();
  shadow_setup();
  fstree_setup();
  graph_setup();
  fsent_base_setup();

  notify_setup();
  config_system_setup();

  /* suppress rcu-registration checks */
  extern __thread bool rcu_is_reader ; rcu_is_reader = true;
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
  ref_edge_cleanup();
  fsedge_cleanup();
}

//
// tests
//

static void node_filesystem_test_entry(xunit_test * _test)
{
  node_filesystem_test * test = (node_filesystem_test *)_test;
  moria_parser * ops_parser = 0;
  fsent *n;
  const fstree *fst;
  configset_parser *parser = 0;
  config *cfg;
  graph_invalidation_context invalidation = {};
  struct node_check **nodes;
  struct node_check *node;
  edges list, *eln;
  ns_edge *nse;
  namespace *ns;
  char ens[512];
  uint16_t ensl;
  char ans[512];
  uint16_t ansl;

  // setup filesystems
  configset_parser_create(&parser);
  config_alloc(&cfg);
  if(configset_parser_parse(parser, test->config, strlen(test->config) + 2, 0, 0, cfg) != 0) {
    tracef("%.*s", parser->yyu.error_len, parser->yyu.error_str);
    RTA;
  }

  // act
  graph_invalidation_begin(&invalidation);
  namespace_reconfigure(cfg, false, &invalidation);
  fstree_reconfigure(cfg, false);
  namespace_reconfigure(cfg, true, &invalidation);
  fstree_reconfigure(cfg, true);
  graph_invalidation_end(&invalidation);

  // setup graph nodes
  moria_parser_create(&ops_parser, &g_graph, (llist *[]) { &fsent_list }, 1, node_operations_test_dispatch, graph_vertex_attrs, graph_edge_attrs);
  moria_parser_operations_parse(ops_parser, MMS(test->operations));

  graph_invalidation_begin(&invalidation);
  fsent_fstree_refresh(&invalidation);
  namespace_reconcile(&invalidation);
  graph_invalidation_end(&invalidation);

  for(nodes = test->nodes; *nodes; nodes++)
  {
    node = *nodes;
    n = resolve_fragment(MMS(node->name));
    assert_notnull(n);

    // act
    fst = vertex_fstree_of(&n->vertex);

    ensl = 0;
    if(node->namespaces) {
      ensl += zcats(ens, sizeof(ens), node->namespaces);
    }

    ansl = 0;
    if(edges_gather_up(&fst->vertex, EDGE_FSTREE_NS, &list)) {
      llist_foreach(&list.lln_sorted, eln, lln_sorted) {
        nse = containerof(eln->p, typeof(*nse), edge);
        ns = containerof(nse->edge.A, typeof(*ns), vertex);
        if(ansl) {
          ansl += zcats(ans + ansl, sizeof(ans) - ansl, " ");
        }
        ansl += zcatw(ans + ansl, sizeof(ans) - ansl, ns->name, ns->name_len);
      }

      edges_destroy(&list);
    }

    // assert
    assert_infos("base", node->name);
    assert_infos("id", fst->id);
    assert_eq_b(node->mkdir, fst->mkdir);
    assert_eq_b(node->readlink, fst->readlink);
    assert_eq_u32(node->invalidation, fst->invalidation);
    assert_eq_w(ens, ensl, ans, ansl);
    assert_info_unstage();
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
                , readlink : READLINK_YES
              }}
            , (struct node_check[]) {{
                  name : "BB"
                , invalidation : INVALIDATION_STAT
                , readlink : READLINK_YES
              }}
            , (struct node_check[]) {{
                  name : "C"
                , invalidation : INVALIDATION_STAT
                , readlink : READLINK_YES
              }}
            , (struct node_check[]) {{
                  name : "D"
                , invalidation : INVALIDATION_STAT
                , readlink : READLINK_YES
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
                , readlink : READLINK_YES
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
            "      readlink : yes\n"
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
                , readlink : READLINK_YES
              }}
            , (struct node_check[]) {{
                  name : "A"
                , invalidation : INVALIDATION_NOTIFY
                , readlink : READLINK_YES
              }}
            , 0
          }
      }}
      , (node_filesystem_test[]) {{
          config : (char[]) {
            "filesystem : {\n"
            "  fstrees : {\n"
            "    / : {\n"
            "      mkdir : true\n"
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
                , readlink : READLINK_YES
                , mkdir : true
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
                , readlink : READLINK_YES
              }}
            , (struct node_check[]) {{
                  name : "BB"
                , invalidation : INVALIDATION_CONTENT
                , readlink : READLINK_YES
              }}
            , (struct node_check[]) {{
                  name : "C"
                , invalidation : INVALIDATION_NOTIFY
                , readlink : READLINK_YES
              }}
            , (struct node_check[]) {{
                  name : "D"
                , invalidation : INVALIDATION_NOTIFY
                , readlink : READLINK_YES
              }}
            , 0
          }
      }}
      , (node_filesystem_test[]) {{
          config : (char[]) {
            "namespaces : {\n"
            " foo\n"
            " bar\n"
            "}\n"
            "filesystem : {\n"
            "  fstrees : {\n"
            "    / : {\n"
            "      namespaces : {\n"
            "        foo\n"
            "        bar\n"
            "      }\n"
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
                , namespaces : "foo bar"
                , readlink : READLINK_YES
              }}
            , 0
          }
      }}
      , (node_filesystem_test[]) {{
          config : (char[]) {
            "namespaces : {\n"
            " foo\n"
            " bar\n"
            " baz\n"
            "}\n"
            "filesystem : {\n"
            "  fstrees : {\n"
            "    /a : {\n"
            "      namespaces : {\n"
            "        foo\n"
            "        bar\n"
            "      }\n"
            "    }\n"
            "    /c : {\n"
            "      namespaces : {\n"
            "        baz\n"
            "      }\n"
            "    }\n"
            "  }\n"
            "}\n"
            "\0\0"
          }
        , operations : "a/b c/d"
        , nodes : (struct node_check*[]) {
              (struct node_check[]) {{
                  name : "b"
                , invalidation : INVALIDATION_STAT
                , namespaces : "foo bar"
                , readlink : READLINK_YES
              }}
            , (struct node_check[]) {{
                  name : "d"
                , invalidation : INVALIDATION_STAT
                , namespaces : "baz"
                , readlink : READLINK_YES
              }}
            , 0
          }
      }}
    , 0
  }
};
XUNIT_UNIT(xunit);
