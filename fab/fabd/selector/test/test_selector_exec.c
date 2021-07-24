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

#include "lorien/load.h"
#include "yyutil/load.h"
#include "value/load.h"
#include "xlinux/xstdlib.h"

#include "yyutil/grammar.h"
#include "valyria/map.h"
#include "valyria/set.h"
#include "valyria/list.h"
#include "valyria/llist.h"
#include "moria/operations.h"
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
#include "filesystem.internal.h"
#include "lookup.h"
#include "selection.h"
#include "dependency.h"
#include "module.internal.h"
#include "channel.h"

#include "zbuffer.h"

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
  yyutil_load();
  value_load();
  logging_finalize();
  graph_setup();
  filesystem_setup();

  fsent_name_module = "module.bam";
}

static void selector_exec_test_unit_cleanup(xunit_unit * unit)
{
  yyutil_unload();
  value_unload();
  graph_cleanup();
  filesystem_cleanup();
}

static void selector_exec_test_entry(xunit_test * _test)
{
  selector_exec_test * test = (typeof(test))_test;

  selector_parser * parser = 0;
  selector * sel = 0;
  graph_parser * op_parser = 0;
  list * actual_list = 0;
  char actual[512];
  selector_context ctx = { 0 };
  int x;
  selected *sn;
  yyu_location loc = { 0 };
  dependency *bpe = 0;
  module mod = { };
  fsent *mod_dir_n;
  graph_invalidation_context invalidation = { };
  channel *chan;

  // arrange
  fsent_setup();
  list_create(&actual_list);
  graph_invalidation_begin(&invalidation);
  xmalloc(&chan, sizeof(*chan));

  // setup initial graph
  graph_parser_create(&op_parser, &g_graph, &fsent_list, node_operations_test_dispatch, graph_vertex_attrs, graph_edge_attrs);
  graph_parser_operations_parse(op_parser, MMS(test->operations));

  resolve_fragment(MMS(test->module), &mod_dir_n);
  module_initialize(mod_dir_n, containerof(moria_vertex_downs(&mod_dir_n->vertex, "module.bam"), fsent, vertex), &invalidation);

  // setup the module
  resolve_fragment(MMS(test->module), &mod.dir_node);
  mod.dir_node->mod = &mod;
  snprintf(mod.id, sizeof(mod.id), "%.*s", 15, mod.dir_node->name.name);
  fsent_module_epoch++;

  selector_parser_create(&parser);
  selector_parser_parse_partial(parser, test->selector, strlen(test->selector) + 2, 0, 0, &loc, &sel);
  assert_eq_u32(strlen(test->selector), loc.l);

  // act
  ctx.mod = &mod;
  ctx.bpe = bpe;
  ctx.chan = chan;
  selector_exec(sel, &ctx, SELECTION_ITERATION_TYPE_ORDER);
  assert_eq_b(false, chan->error);

  llist_foreach(&ctx.selection->list, sn, lln) {
    list_push(actual_list, &sn->v->label, 1);
  }

  int cmp(const void *_A, size_t Asz, const void *_B, size_t Bsz)
  {
    const char ** A = (void*)_A;
    const char ** B = (void*)_B;

    return **A - **B;
  };
  list_sort(actual_list, cmp);

  size_t z = 0;
  for(x = 0; x < actual_list->size; x++)
  {
    char ** c = list_get(actual_list, x);
    if(x)
      z += znloadc(actual + z, sizeof(actual) - z, ' ');
    z += znloadc(actual + z, sizeof(actual) - z, **c);
  }
  actual[z] = 0;

  assert_eq_s(test->result, actual);

finally:
  selector_parser_xfree(parser);
  selector_free(sel);
  selector_context_xdestroy(&ctx);
  list_xfree(actual_list);
  fsent_cleanup();
  graph_parser_xfree(op_parser);
  graph_invalidation_end(&invalidation);
  wfree(chan);
coda;
}

//
// manifest
//

xunit_unit xunit = {
    xu_setup : selector_exec_test_unit_setup
  , xu_cleanup : selector_exec_test_unit_cleanup
  , xu_entry : selector_exec_test_entry
  , xu_tests : (selector_exec_test*[]) {
    /* default traversal is fstree, skip self */
      (selector_exec_test[]) {{
          operations :
            " M/module.bam"
            " +M/A/B/C:depends:E/F/G"
        , module : "M"
        , selector : (char[]) {
            "["
            " path : /M/A"
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
            " M/module.bam"
            " +M/A/B/C:depends:E/F/G"
        , module : "M"
        , selector : (char[]) {
            "sequence : ["       // the output of sequence is the output of the last operation
             " path : /M/A/B/C"
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
           " M/module.bam"
           " +M/A/B/C:depends:E/F/G"
        , module : "M"
        , selector : (char[]) {
            "["
             " path : /M/A/B"
             " traverse : {"
              " direction : up"
              " graph : fs"
              " min-distance : 0"
             " }"
            "]"
            "\0\0"
          }
        , result : "A B M"
      }}
    , (selector_exec_test[]) {{
          operations :
           " M/module.bam"
           " +M/A/B/C:depends:E/F/G"
        , module : "M"
        , selector : (char[]) {
            "sequence : ["
             " path : /M/A/B"
             " traverse : {"
              " direction : up"
              " graph : fs"
              " min-distance : 1"
             " }"
            " ]"
            "\0\0"
          }
        , result : "A M"
      }}
    , (selector_exec_test[]) {{
          operations :
           " M/module.bam"
           " +M/A/B/C:depends:E/F/G"
        , module : "M"
        , selector : (char[]) {
            "sequence : ["
             " path : /M/A/B"
             " traverse : {"
              " direction : up"
              " graph : fs"
              " min-distance : 2"
             " }"
            " ]"
            "\0\0"
          }
        , result : "M"
      }}

    /* multiple traverse in sequence */
    , (selector_exec_test[]) {{
          operations :
           " M/module.bam"
           " +M/A/B/C:depends:E/F/G"
        , module : "M"
        , selector : (char[]) {
            "["
            " path : /E/F/G"
            " traverse : {"
             " direction : up"
             " graph : depends"
            " }"
            " traverse : {"
             " direction : up"
             " graph : fs"
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
            " path : /M/A/B/C"
            " union : ["
              " traverse : {"
               " direction : up"
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
           " M/module.bam"
           " +M/A/B/C:depends:E/F/G"
        , module : "M"
        , selector : (char[]) {
            "["
            " path : /M/A/B/C"
            " union : ["
             " traverse : {"
              " direction : up"
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
           " M/module.bam"
           " +M/A/B/C:depends:E/F/G"
        , module : "M"
        , selector : (char[]) {
            "union : ["
              " sequence : ["
                " path : /M/A/B/C"
                " traverse : {"
                  " direction : down"
                  " graph : depends"
                " }"
              " ]"
              " sequence : ["
                " path : /M/A/B/C"
                " traverse : {"
                  " direction : up"
                  " graph : fs"
                " }"
              " ]"
            "]"
            "\0\0"
          }
        , result : "A B G M"
      }}
    , (selector_exec_test[]) {{
          operations :
           " M/module.bam"
           " +M/A/B/C:depends:E/F/G"
        , module : "M"
        , selector : (char[]) {
            "sequence : ["
              " sequence : ["
                " path : /M/A/B/C"
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
            " M/module.bam"
            " +M/A/B/C/D:depends:E/F/G"
        , module : "M"
        , selector : (char[]) {
            "["
            " path : /M/A/B"
            " union : ["
             " traverse : {"
              " direction : up"
             " }"
             " traverse : {"
              " direction : down"
              " graph : dirtree"
             " }"
            " ]"
            "]"
            "\0\0"
          }
        , result : "A C M"
      }}
    , (selector_exec_test[]) {{
          operations :
           " M/module.bam"
           " +M/A/B/C/D:depends:E/F/G"
        , module : "M"
        , selector : (char[]) {
            "["
            " pattern : B"
            " union : ["
             " traverse : {"
              " direction : up"
             " }"
             " traverse : {"
              " direction : down"
              " graph : dirtree"
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
