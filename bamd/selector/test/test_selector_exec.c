/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "lorien/load.h"
#include "bacon/load.h"
#include "xlinux/xstdlib.h"

#include "yyutil/grammar.h"
#include "valyria/hashtable.h"
#include "valyria/list.h"
#include "valyria/llist.h"
#include "moria/graph.h"
#include "moria/parser.h"

#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/growing.h"

#include "selector_parser.h"
#include "selector.internal.h"
#include "request.h"
#include "fsent.h"
#include "node_operations_test.h"
#include "fstree.h"
#include "selection.h"
#include "dependency.h"
#include "module.h"
#include "channel.h"
#include "bpe.h"
#include "args.h"
#include "namespace.h"
#include "fsname.h"
#include "shadow.h"
#include "notify.h"
#include "events/events.h"
#include "configset_parser.h"
#include "configset.h"
#include "config.h"
#include "config_system.h"
#include "system.h"
#include "ref_edge.h"
#include "fsedge.h"
#include "ns_edge.h"

#include "zcat.h"

typedef struct {
  XUNITTEST;

  char *config;       // filesystems config
  char *operations;   // build the node graph
  char *selector;     // selector
  char *module;

  // outputs
  char * result;      // expected result set
} selector_exec_test;

static void selector_exec_test_unit_setup(xunit_unit * unit)
{
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

  extern __thread bool rcu_is_reader ; rcu_is_reader = true;
}

static void selector_exec_test_unit_cleanup(xunit_unit * unit)
{
  bacon_unload();

  fstree_cleanup();
  fsname_cleanup();
  namespace_cleanup();
  shadow_cleanup();
  events_cleanup();
  fsent_cleanup();
  graph_cleanup();
  ref_edge_cleanup();
  fsedge_cleanup();
  ns_edge_cleanup();
}

static int cmp(const void *_A, const void *_B, void *_ctx)
{
  const char **A = (void*)_A;
  const char **B = (void*)_B;

  return **A - **B;
};

static void selector_exec_test_entry(xunit_test *_test)
{
  selector_exec_test *test = (typeof(test))_test;

  selector_parser *parser = 0;
  selector *sel = 0;
  moria_parser *op_parser = 0;
  configset_parser *cfg_parser = 0;
  config *cfg;
  list *actual_list = 0;
  char selection[PATH_MAX];
  selector_context ctx;
  int x;
  selected *sn;
  yyu_location loc = { 0 };
  buildplan_entity *bpe = 0;
  module mod = { };
  fsent *mod_dir_n;
  graph_invalidation_context invalidation = { };
  size_t z;
  char *c;

  // arrange
  fsent_root_setup();
  list_create(&actual_list);
  selector_context_init(&ctx);

  // setup filesystems
  configset_parser_create(&cfg_parser);
  config_alloc(&cfg);
  if(test->config) {
    if(configset_parser_parse(cfg_parser, test->config, strlen(test->config) + 2, 0, 0, cfg) != 0) {
      tracef("%.*s", cfg_parser->yyu.error_len, cfg_parser->yyu.error_str);
      RTA;
    }
  }

  /* setup fstrees and namespaces */
  graph_invalidation_begin(&invalidation);
  namespace_reconfigure(cfg, false, &invalidation);
  fstree_reconfigure(cfg, false);
  namespace_reconfigure(cfg, true, &invalidation);
  fstree_reconfigure(cfg, true);
  graph_invalidation_end(&invalidation);

  // setup initial graph
  moria_parser_create(&op_parser, &g_graph,
      (llist *[]) { &fsent_list, &bpe_list }, 2
    , node_operations_test_dispatch
    , graph_vertex_attrs
    , graph_edge_attrs
  );
  moria_parser_operations_parse(op_parser, MMS(test->operations));

  /* index fstrees */
  graph_invalidation_begin(&invalidation);
  fsent_fstree_refresh(&invalidation);
  namespace_reconcile(&invalidation);
  graph_invalidation_end(&invalidation);

  mod_dir_n = resolve_fragment(MMS(test->module));
  module_initialize(&mod, mod_dir_n);;

  // setup the module
  mod.build_dir = resolve_fragment(MMS(test->module));
  snprintf(mod.id, sizeof(mod.id), "%.*s", 15, mod.build_dir->name.name);

  selector_parser_create(&parser);
  if(selector_parser_parse_partial(parser, test->selector, strlen(test->selector) + 2, 0, 0, &loc, &sel) != 0) {
    tracef("%.*s", (int)parser->yyu.error_len, parser->yyu.error_str);
  }
  assert_eq_u32(strlen(test->selector), loc.l);

  // act
  ctx.mod = &mod;
  ctx.bpe = bpe;
  ctx.cwd = mod.build_dir;
  ctx.iteration_type = SELECTION_ITERATION_TYPE_ORDER;
  selector_exec(sel, &ctx);
  assert_eq_b(false, system_has_error);

  llist_foreach(&ctx.selection->list, sn, lln) {
    list_push(actual_list, (void*)sn->v->label);
  }

  list_sort(actual_list, cmp);

  z = 0;
  for(x = 0; x < actual_list->size; x++)
  {
    c = list_get(actual_list, x);
    if(x) {
      z += zcatc(selection + z, sizeof(selection) - z, ' ');
    }
    z += zcatc(selection + z, sizeof(selection) - z, *c);
  }
  selection[z] = 0;

  assert_eq_s(test->result, selection);

  selector_parser_free(parser);
  selector_free(sel);
  selector_context_destroy(&ctx);
  list_free(actual_list);
  fsent_cleanup();
  moria_parser_free(op_parser);
  graph_invalidation_end(&invalidation);
  config_free(cfg, cfg_parser);
  configset_parser_free(cfg_parser);
}

//
// manifest
//

static xunit_unit xunit = {
    xu_setup : selector_exec_test_unit_setup
  , xu_cleanup : selector_exec_test_unit_cleanup
  , xu_entry : selector_exec_test_entry
  , xu_tests : (selector_exec_test*[]) {
    /* default traversal is fstree, skip self */
      (selector_exec_test[]) {{
          operations :
            /* E/F/G : depends : M/A/B/C */
            " foo!bpe"
            " +E/F/G:src-bpe:foo"
            " +foo:bpe-tgt:M/A/B/C"
        , module : "M"
        , selector : (char[]) {
            "["
            " path : {"
            "   pattern : /M/A"
            " }"
            " traverse : {"
             " direction : down"
            " }"
            "]"
            "\0\0"
          }
        , result : "B C"
      }}
    , (selector_exec_test[]) {{
          operations :
            " foo!bpe"
            " +E/F/G:src-bpe:foo"
            " +foo:bpe-tgt:M/A/B/C"
        , module : "M"
        , selector : (char[]) {
            "sequence : ["       // the output of sequence is the output of the last operation
             " path : { pattern : /M/A/B/C }"
             " traverse : {"
              " min-distance : 0"
              " direction : down"
              " graph : fs"
             " }"
            " ]"
            "\0\0"
          }
        , result : "C"
      }}
    , (selector_exec_test[]) {{
          operations :
           " foo!bpe"
           " +E/F/G:src-bpe:foo"
           " +foo:bpe-tgt:M/A/B/C"
        , module : "M"
        , selector : (char[]) {
            "["
             " path : { pattern : /M/A/B }"
             " traverse : {"
              " direction : up"
              " graph : fs"
              " min-distance : 0"
              " max-distance : 2"
             " }"
            "]"
            "\0\0"
          }
        , result : "A B M"
      }}
    , (selector_exec_test[]) {{
          operations :
           " foo!bpe"
           " +E/F/G:src-bpe:foo"
           " +foo:bpe-tgt:M/A/B/C"
        , module : "M"
        , selector : (char[]) {
            "sequence : ["
             " path : { pattern : /M/A/B }"
             " traverse : {"
              " direction : up"
              " graph : fs"
              " min-distance : 1"
              " max-distance : 2"
             " }"
            " ]"
            "\0\0"
          }
        , result : "A M"
      }}
    , (selector_exec_test[]) {{
          operations :
           " foo!bpe"
           " +E/F/G:src-bpe:foo"
           " +foo:bpe-tgt:M/A/B/C"
        , module : "M"
        , selector : (char[]) {
            "sequence : ["
             " path : { pattern : /M/A/B }"
             " traverse : {"
              " direction : up"
              " graph : fs"
              " min-distance : 2"
              " max-distance : 2"
             " }"
            " ]"
            "\0\0"
          }
        , result : "M"
      }}

    /* multiple traverse in sequence */
    , (selector_exec_test[]) {{
          operations :
           " foo!bpe"
           " +E/F/G:src-bpe:foo"
           " +foo:bpe-tgt:M/A/B/C"
        , module : "M"
        , selector : (char[]) {
            "["
            " path : { pattern : /E/F/G }"
            " traverse : {"
             " direction : up"
             " graph : depends"
            " }"
            " traverse : {"
             " direction : up"
             " graph : fs"
             " max-distance : 3"
            " }"
            "]"
            "\0\0"
          }
        , result : "A B M"
      }}

    /* union selector */
    , (selector_exec_test[]) {{
          operations :
           " M/module.bam"
           " M/A/B/C/D/E"
        , module : "M"
        , selector : (char[]) {
            "["
            " path : { pattern : /M/A/B/C }"
            " union : ["
              " traverse : {"
               " direction : up"
               " max-distance : 3"
              " }"
              " traverse : {"
               " direction : down"
              " }"
            " ]"
            "]"
            "\0\0"
          }
        , result : "A B D E M"
      }}
    , (selector_exec_test[]) {{
          operations :
           " foo!bpe"
           " +E/F/G:src-bpe:foo"
           " +foo:bpe-tgt:M/A/B/C"
        , module : "M"
        , selector : (char[]) {
            "["
            " path : { pattern : /M/A/B/C }"
            " union : ["
             " traverse : {"
              " direction : up"
              " max-distance : 3"
             " }"
             " traverse : {"
              " direction : down"
              " graph : depends"
             " }"
            " ]"
            "]"
            "\0\0"
          }
        , result : "A B G M"
      }}
    , (selector_exec_test[]) {{
          operations :
           " foo!bpe"
           " +E/F/G:src-bpe:foo"
           " +foo:bpe-tgt:M/A/B/C"
        , module : "M"
        , selector : (char[]) {
            "union : ["
              " sequence : ["
                " path : { pattern : /M/A/B/C }"
                " traverse : {"
                  " direction : down"
                  " graph : depends"
                " }"
              " ]"
              " sequence : ["
                " path : { pattern : /M/A/B/C }"
                " traverse : {"
                  " direction : up"
                  " graph : fs"
                  " max-distance : 3"
                " }"
              " ]"
            "]"
            "\0\0"
          }
        , result : "A B G M"
      }}
    , (selector_exec_test[]) {{
          operations :
           " foo!bpe"
           " +E/F/G:src-bpe:foo"
           " +foo:bpe-tgt:M/A/B/C"
        , module : "M"
        , selector : (char[]) {
            "sequence : ["
              " sequence : ["
                " path : { pattern : /M/A/B/C }"
                " traverse : {"
                  " direction : down"
                  " graph : depends"
                " }"
              " ]"
            "]"
            "\0\0"
          }
        , result : "G"
      }}
    , (selector_exec_test[]) {{
          operations :
            " foo!bpe"
            " +E/F/G:src-bpe:foo"
            " +foo:bpe-tgt:M/A/B/C/D"
        , module : "M"
        , selector : (char[]) {
            "["
            " path : { pattern : /M/A/B }"
            " union : ["
             " traverse : {"
              " direction : up"
              " max-distance : 2"
             " }"
             " traverse : {"
              " direction : down"
              " graph : fs"
              " visit : directory"
             " }"
            " ]"
            "]"
            "\0\0"
          }
        , result : "A C M"
      }}

    /* lookup selelectors */
    , (selector_exec_test[]) {{
          config : (char[]) {
            "namespaces : {\n"
            " module\n"
            "}\n"
            "filesystem : {\n"
            "  fstrees : {\n"
            "    / : {\n"
            "      namespaces : {\n"
            "        module\n"
            "      }\n"
            "    }\n"
            "  }\n"
            "}\n"
            "\0\0"
          }
        , operations :
           " foo!bpe"
           " +E/F/G:src-bpe:foo"
           " +foo:bpe-tgt:M/A/B/C/D"
        , module : "M"
        , selector : (char[]) {
            "["
            " lookup : { pattern : B namespace : module }"
            " union : ["
             " traverse : {"
              " direction : up"
              " max-distance : 2"
             " }"
             " traverse : {"
              " direction : down"
              " graph : fs"
              " visit : directory"
             " }"
            " ]"
            "]"
            "\0\0"
          }
        , result : "A C M"
      }}
    , 0
  }
};
XUNIT_UNIT(xunit);
