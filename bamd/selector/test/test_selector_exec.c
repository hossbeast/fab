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
#include "lookup.h"
#include "selection.h"
#include "dependency.h"
#include "module.h"
#include "channel.h"
#include "buildplan_entity.h"
#include "args.h"
#include "fsindex.h"
#include "fsname.h"
#include "shadow.h"
#include "notify.h"
#include "events.h"

#include "zcat.h"

typedef struct {
  XUNITTEST;

  char * operations;  // build the node graph
  char * selector;    // selector
  char * module;

  // outputs
  char * result;      // expected result set
} selector_exec_test;

static void selector_exec_test_unit_setup(xunit_unit * unit)
{
  fsent_name_module = "module.bam";

  bacon_load();

  args_init();
  fsent_root_setup();
  shadow_setup();
  fstree_setup();
  notify_setup();
  events_setup();

  graph_setup();
  fsindex_setup();

  extern __thread bool rcu_is_reader ; rcu_is_reader = true;
}

static void selector_exec_test_unit_cleanup(xunit_unit * unit)
{
  bacon_unload();
  graph_cleanup();
  fstree_cleanup();
  fsname_cleanup();
  fsindex_cleanup();
  shadow_cleanup();
  events_cleanup();
}

static int cmp(const void *_A, const void *_B, void *_ctx)
{
  const char ** A = (void*)_A;
  const char ** B = (void*)_B;

  return **A - **B;
};

static void selector_exec_test_entry(xunit_test * _test)
{
  selector_exec_test * test = (typeof(test))_test;

  selector_parser * parser = 0;
  selector * sel = 0;
  moria_parser * op_parser = 0;
  list * actual_list = 0;
  char actual[PATH_MAX];
  selector_context ctx = { 0 };
  int x;
  selected *sn;
  yyu_location loc = { 0 };
  buildplan_entity *bpe = 0;
  module mod = { };
  fsent *mod_dir_n;
  graph_invalidation_context invalidation = { };
  channel *chan;
  size_t z;
  char * c;

  // arrange
  fsent_root_setup();
  list_create(&actual_list);
  graph_invalidation_begin(&invalidation);
  xmalloc(&chan, sizeof(*chan));
  selector_context_init(&ctx);

  // setup initial graph
  moria_parser_create(&op_parser, &g_graph,
      (llist *[]) { &fsent_list, &bpe_list }, 2
    , node_operations_test_dispatch
    , graph_vertex_attrs
    , graph_edge_attrs
  );
  moria_parser_operations_parse(op_parser, MMS(test->operations));
  fsent_fstree_refresh(&invalidation);

  resolve_fragment(MMS(test->module), &mod_dir_n);
  module_initialize(mod_dir_n, mod_dir_n, containerof(moria_vertex_downs(&mod_dir_n->vertex, "module.bam"), fsent, vertex));

  // setup the module
  resolve_fragment(MMS(test->module), &mod.dir_node);
  mod.dir_node->mod = &mod;
  snprintf(mod.id, sizeof(mod.id), "%.*s", 15, mod.dir_node->name.name);
  fsent_module_epoch++;

  selector_parser_create(&parser);
  if(selector_parser_parse_partial(parser, test->selector, strlen(test->selector) + 2, 0, 0, &loc, &sel) != 0) {
    tracef("%.*s", (int)parser->yyu.error_len, parser->yyu.error_str);
  }
  assert_eq_u32(strlen(test->selector), loc.l);

  // act
  ctx.mod = &mod;
  ctx.bpe = bpe;
  ctx.chan = chan;
  ctx.cwd = mod.dir_node;
  ctx.iteration_type = SELECTION_ITERATION_TYPE_ORDER;
  selector_exec(sel, &ctx);
  assert_eq_b(false, chan->error);

  llist_foreach(&ctx.selection->list, sn, lln) {
    list_push(actual_list, (void*)sn->v->label);
  }

  list_sort(actual_list, cmp);

  z = 0;
  for(x = 0; x < actual_list->size; x++)
  {
    c = list_get(actual_list, x);
    if(x) {
      z += zcatc(actual + z, sizeof(actual) - z, ' ');
    }
    z += zcatc(actual + z, sizeof(actual) - z, *c);
  }
  actual[z] = 0;

  assert_eq_s(test->result, actual);

  selector_parser_free(parser);
  selector_free(sel);
  selector_context_destroy(&ctx);
  list_free(actual_list);
  fsent_cleanup();
  moria_parser_free(op_parser);
  graph_invalidation_end(&invalidation);
  free(chan);
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
            " lookup : {"
            "   path : /M/A"
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
             " lookup : { path : /M/A/B/C }"
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
             " lookup : { path : /M/A/B }"
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
             " lookup : { path : /M/A/B }"
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
             " lookup : { path : /M/A/B }"
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
            " lookup : { path : /E/F/G }"
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
            " lookup : { path : /M/A/B/C }"
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
            " lookup : { path : /M/A/B/C }"
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
                " lookup : { path : /M/A/B/C }"
                " traverse : {"
                  " direction : down"
                  " graph : depends"
                " }"
              " ]"
              " sequence : ["
                " lookup : { path : /M/A/B/C }"
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
                " lookup : { path : /M/A/B/C }"
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
            " lookup : { path : /M/A/B }"
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
    , (selector_exec_test[]) {{
          operations :
           " foo!bpe"
           " +E/F/G:src-bpe:foo"
           " +foo:bpe-tgt:M/A/B/C/D"
        , module : "M"
        , selector : (char[]) {
            "["
            " pattern : B"
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
