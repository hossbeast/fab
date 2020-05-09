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
#include "lorien/load.h"
#include "yyutil/load.h"
#include "logger/load.h"
#include "value/load.h"

#include "yyutil/grammar.h"
#include "valyria/map.h"
#include "valyria/set.h"
#include "valyria/list.h"
#include "valyria/llist.h"
#include "moria/operations.h"
#include "moria/graph.h"

#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/growing.h"

#include "selector_parser.h"
#include "selector.internal.h"
#include "request.h"
#include "node.h"
#include "node_operations.h"
#include "path.h"
#include "logging.h"
#include "node_operations_test.h"
#include "filesystem.internal.h"
#include "lookup.internal.h"
#include "selection.h"

#include "zbuffer.h"

typedef struct {
  XUNITTEST;

  char * operations;  // build the node graph
  char * selector;    // selector
  char * target;
  char * base;

  // outputs
  char * result;      // expected result set
} selector_exec_test;

static xapi selector_exec_test_unit_setup(xunit_unit * unit)
{
  enter;

  fatal(yyutil_load);
  fatal(value_load);
  fatal(logging_finalize);
  fatal(graph_setup);
  fatal(filesystem_setup);

  finally : coda;
}

static xapi selector_exec_test_unit_cleanup(xunit_unit * unit)
{
  enter;

  fatal(yyutil_unload);
  fatal(value_unload);
  fatal(graph_cleanup);
  fatal(filesystem_cleanup);

  finally : coda;
}

static xapi selector_exec_test_entry(xunit_test * _test)
{
  enter;

  selector_exec_test * test = (typeof(test))_test;

  selector_parser * parser = 0;
  selector * sel = 0;
  list * operations = 0;
  operations_parser * op_parser = 0;
  list * actual_list = 0;
  node *target = 0;
  node *base = 0;
  char actual[512];
  selector_context ctx = { 0 };
  int x;
  selected_node *sn;
  yyu_location loc = { 0 };
  buildplan_entity *bpe;

  // arrange
  fatal(node_setup);
  fatal(list_create, &actual_list);

  // setup initial graph
  fatal(operations_parser_operations_create, &operations);
  fatal(operations_parser_create, &op_parser);
  fatal(operations_parser_parse, op_parser, g_graph, MMS(test->operations), operations);
  fatal(operations_perform, g_graph, node_operations_test_dispatch, operations);

  fatal(selector_parser_create, &parser);
  fatal(selector_parser_parse_partial, parser, test->selector, strlen(test->selector) + 2, 0, 0, &loc, &sel);
  assert_eq_u32(strlen(test->selector), loc.l);

  if(test->target)
  {
    fatal(pattern_lookup_fragment, MMS(test->target), 0, 0, 0, 0, 0, 0, &target);
    bpe = &target->self_bpe;
  }

  if(test->base)
  {
    fatal(pattern_lookup_fragment, MMS(test->base), 0, 0, 0, 0, 0, 0, &base);
  }

  // act
  ctx.bpe = bpe;
  ctx.mod = 0;
  ctx.base = base;
  fatal(selector_exec, sel, &ctx);

  llist_foreach(&ctx.selection->list, sn, lln) {
    fatal(list_push, actual_list, &sn->n->name->name, 1);
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
  fatal(selector_parser_xfree, parser);
  selector_free(sel);
  fatal(selector_context_xdestroy, &ctx);
  fatal(list_xfree, actual_list);
  fatal(node_cleanup);
  fatal(list_xfree, operations);
  fatal(operations_parser_xfree, op_parser);
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
      (selector_exec_test[]) {{
          operations :
            " +A/B/C:strong:E/F/G"
        , target : "A"
        , selector : (char[]) {
            "["
            " target"
            " traverse : {"
             " direction : down"
            " }"
            "]"
            "\0\0"
          }
        , result : "B C G"
      }}
    , (selector_exec_test[]) {{
          operations :
            " +A/B/C:strong:E/F/G"
        , target : "C"
        , selector : (char[]) {
            "sequence : ["       // the output of sequence is the output of the last operation
             " target"
             " traverse : {"
              " min-distance : 0"
              " direction : down"
              " relation : fs"
             " }"
            " ]"
            "\0\0"
          }
        , result : "C"
      }}
    , (selector_exec_test[]) {{
          operations :
           " +A/B/C:strong:E/F/G"
        , target: "B"
        , selector : (char[]) {
            "["
             " target"
             " traverse : {"
              " direction : up"
              " relation : fs"
              " min-distance : 0"
             " }"
            "]"
            "\0\0"
          }
        , result : "A B"
      }}
    , (selector_exec_test[]) {{
          operations :
           " +A/B/C:strong:E/F/G"
        , target : "B"
        , selector : (char[]) {
            "sequence : ["
             " target"
             " traverse : {"
              " direction : up"
              " relation : fs"
              " min-distance : 1"
             " }"
            " ]"
            "\0\0"
          }
        , result : "A"
      }}
    , (selector_exec_test[]) {{
          operations :
            " +A/B/C:strong:E/F/G"
        , target : "B"
        , selector : (char[]) {
            "sequence : ["
             " target"
             " traverse : {"
              " direction : up"
              " relation : fs"
              " min-distance : 2"
             " }"
            " ]"
            "\0\0"
          }
        , result : ""
      }}

    /* multiple traverse in sequence */
    , (selector_exec_test[]) {{
          operations :
           " +A/B/C:strong:E/F/G"
        , target : "G"
        , selector : (char[]) {
            "["
            " target"
            " traverse : {"
             " direction : up"
             " relation : strong"
            " }"
            " traverse : {"
             " direction : up"
             " relation : fs"
            " }"
            "]"
            "\0\0"
          }
        , result : "A B"
      }}

    /* union selector */
    , (selector_exec_test[]) {{
          operations :
           " A/B/C/D/E"
        , target : "C"
        , selector : (char[]) {
            "["
            " target "
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
        , result : "A B D E"
      }}
    , (selector_exec_test[]) {{
          operations :
            " +A/B/C:strong:E/F/G"
        , target : "C"
        , selector : (char[]) {
            "["
            " target"
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
        , result : "A B G"
      }}
    , (selector_exec_test[]) {{
          operations :
            " +A/B/C:strong:E/F/G"
        , target : "C"
        , selector : (char[]) {
            "union : ["
              " sequence : ["
                " target"
                " traverse : {"
                  " direction : down"
                  " relation : strong"
                " }"
              " ]"
              " sequence : ["
                " target"
                " traverse : {"
                  " direction : up"
                  " relation : fs"
                " }"
              " ]"
            "]"
            "\0\0"
          }
        , result : "A B G"
      }}
    , (selector_exec_test[]) {{
          operations :
            " +A/B/C:strong:E/F/G"
        , target : "C"
        , selector : (char[]) {
            "sequence : ["
              " sequence : ["
                " target"
                " traverse : {"
                  " direction : down"
                  " relation : strong"
                " }"
              " ]"
            "]"
            "\0\0"
          }
        , result : "G"
      }}
    , (selector_exec_test[]) {{
          operations :
            " +A/B/C/D:strong:E/F/G"
        , target : "B"
        , selector : (char[]) {
            "["
            " target"
            " union : ["
             " traverse : {"
              " direction : up"
             " }"
             " traverse : {"
              " direction : down"
              " file-type : dir"
             " }"
            " ]"
            "]"
            "\0\0"
          }
        , result : "A C"
      }}
    , (selector_exec_test[]) {{
          operations :
            " +A/B/C/D:strong:E/F/G"
        , base : "A"
        , selector : (char[]) {
            "["
            " pattern : B"
            " union : ["
             " traverse : {"
              " direction : up"
             " }"
             " traverse : {"
              " direction : down"
              " file-type : dir"
             " }"
            " ]"
            "]"
            "\0\0"
          }
        , result : "A C"
      }}
    , 0
  }
};
