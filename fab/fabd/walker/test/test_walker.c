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
#include "types.h"

#include "lorien/load.h"
#include "lorien/nftwat.h"
#include "moria/edge.h"
#include "moria/graph.h"
#include "moria/load.h"
#include "moria/vertex.h"
#include "moria/operations.h"
#include "moria/parser.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "narrator/growing.h"
#include "valyria/array.h"
#include "valyria/dictionary.h"
#include "valyria/load.h"
#include "valyria/map.h"
#include "valyria/list.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xstring.h"
#include "xunit.h"
#include "xunit/assert.h"

#include "walker.internal.h"
#include "filesystem.h"
#include "logging.h"
#include "lookup.h"
#include "module.h"
#include "fsent.h"
#include "fsedge.h"
#include "node_operations_test.h"

typedef struct {
  XUNITTEST;

  char * operations;  // to create the starting graph
  char * base;        // starting node, if any

  char * sequence;    // operations to perform

  char * results;     // expected results
  char * graph;       // expected result graph
} walker_test;

typedef struct
{
  int op;
  union {
    fsent * n;
    struct {          // connect
      fsent * parent;
      fsent * child;
    };
    fsedge * e;
  };
} walker_test_operation;

typedef struct {
  walker_context base;
  array * operations;
} walker_test_context;

//
// tests
//

static void info_destroy(void * _info)
{
  ftwinfo * info = _info;
  wfree((void*)info->path);
}

static xapi walker_test_unit_setup(xunit_unit * unit)
{
  enter;

  fatal(valyria_load);
  fatal(lorien_load);
  fatal(moria_load);
  fatal(logging_finalize);

  fatal(filesystem_setup);
  fatal(graph_setup);
  fatal(fsent_setup);

/* suppress rcu-registration checks */
extern __thread bool rcu_is_registered ; rcu_is_registered = true;

  finally : coda;
}

static xapi walker_test_unit_cleanup(xunit_unit * unit)
{
  enter;

  fatal(valyria_unload);
  fatal(lorien_unload);
  fatal(moria_unload);
  fatal(graph_cleanup);
  fatal(filesystem_cleanup);

  finally : coda;
}

static xapi walker_test_entry(xunit_test * _test)
{
  enter;

  walker_test * test = (walker_test *)_test;

  walker_test_context ctx = {};
  dictionary * infos = 0;
  map * nodes = 0;
  graph_invalidation_context invalidation = { 0 };
  graph_parser * op_parser = 0;
  narrator_growing * N = 0;
  int method;
  int stop;
  char * seq;
  const char * graph;

  fatal(narrator_growing_create, &N);

  // setup initial graph
  fatal(graph_parser_create, &op_parser, &g_graph, &fsent_list, node_operations_test_dispatch, graph_vertex_attrs, graph_edge_attrs);
  fatal(graph_parser_operations_parse, op_parser, MMS(test->operations));

  fatal(map_create, &nodes);
  fatal(dictionary_createx, &infos, sizeof(ftwinfo), 0, info_destroy, 0);
  fatal(array_create, &ctx.operations, sizeof(walker_test_operation));
  fatal(graph_invalidation_begin, &invalidation);

  static int walk_ids;
  ctx.base.walk_id = ++walk_ids;
  if(test->base)
    fatal(resolve_fragment, MMS(test->base), &ctx.base.base);

  ctx.base.invalidation = &invalidation;

  // carry out the test operation sequence
  seq = test->sequence;
  while(*seq)
  {
    method = 0;
    if(*seq == '*')
    {
      method = FTWAT_PRE;
      seq++;
    }

    ftwinfo * A = dictionary_get(infos, MM(*seq));
    seq++;

    ftwinfo * B = 0;
    if((B = dictionary_get(infos, MM(*seq))) == 0)
    {
      fatal(dictionary_store, infos, MM(*seq), &B);
      B->pathl = 1;
      fatal(ixsprintf, (void*)&B->path, "%c", *seq);
    }
    seq++;

    if(*seq == '*')
    {
      method = FTWAT_POST;
      seq++;
    }

    B->parent = A;
    B->type = method ? FTWAT_D : FTWAT_F;

    fatal(walker_visit, method, B, &ctx, &stop);

    while(*seq == ' ') {
      seq++;
    }
  }

  // ordered list of edges
  fatal(graph_say, &N->base);
  graph = N->s;
  assert_eq_s(test->graph, graph);

finally:
  fatal(fsent_cleanup);
  fatal(map_xfree, nodes);
  fatal(dictionary_xfree, infos);
  fatal(array_xfree, ctx.operations);
  graph_invalidation_end(&invalidation);
  fatal(graph_parser_xfree, op_parser);
  fatal(narrator_growing_free, N);
coda;
}

//
// manifest
//

xunit_unit xunit = {
    xu_setup : walker_test_unit_setup
  , xu_cleanup : walker_test_unit_cleanup
  , xu_entry : walker_test_entry
  , xu_tests : (walker_test*[]) {
    /* INVALIDATE_STAT */
      (walker_test[]) {{        // no changes
          operations : "A/B"
        , base : "A"
        , sequence : "*0A AB 0A*"
        , graph :  "1-(root)!dir 2-A!dir"
                  " 3-B!file"
                  " 1:fs:2 2:fs:3"
      }}
    , (walker_test[]) {{
          operations : "A/B A/C"
        , base : "A"
        , sequence : "*0A AB AC 0A*"
        , graph :  "1-(root)!dir 2-A!dir"
                  " 3-B!file 4-C!file"
                  " 1:fs:2 2:fs:3 2:fs:4"
      }}
    , (walker_test[]) {{
          operations : "A/B A/C B/D B/E C/F C/G"
        , base : "A"
        , sequence : "*0A *AB BD BE AB* *AC CF CG AC* 0A*"
        , graph :  "1-(root)!dir 2-A!dir 3-B!dir 4-C!dir"
                  " 5-D!file 6-E!file 7-F!file 8-G!file"
                  " 1:fs:2 2:fs:3 2:fs:4 3:fs:5 3:fs:6 4:fs:7 4:fs:8"
      }}
    , (walker_test[]) {{        // new leaves
          operations : "A/B"
        , base : "A"
        , sequence : "*0A *AB BD BE AB* *AC CF CG AC* 0A*"
        , graph :  "1-(root)!dir 2-A!dir 3-B!dir 4-C!dir"
                  " 5-D!file 6-E!file 7-F!file 8-G!file"
                  " 1:fs:2 2:fs:3 2:fs:4 3:fs:5 3:fs:6 4:fs:7 4:fs:8"
, xu_weight : 1
      }}
    , (walker_test[]) {{
          operations : "A/B B/D B/E"
        , base : "A"
        , sequence : "*0A *AB BD BE AB* *AC CF CG AC* 0A*"
        , graph :  "1-(root)!dir 2-A!dir 3-B!dir 4-C!dir"
                  " 5-D!file 6-E!file 7-F!file 8-G!file"
                  " 1:fs:2 2:fs:3 2:fs:4 3:fs:5 3:fs:6 4:fs:7 4:fs:8"
      }}
    , (walker_test[]) {{        // deleted leaves - A/C
          operations : "A/B A/C"
        , base : "A"
        , sequence : "*0A AB 0A*"
        , graph :  "1-(root)!dir 2-A!dir"
                  " 3-B!file"
                  " 1:fs:2 2:fs:3"
      }}
    , (walker_test[]) {{        // deleted leaves
          operations : "A/B A/C B/D B/E C/F C/G"
        , base : "A"
        , sequence : "*0A *AB AB* *AC AC* 0A*"
        , graph :  "1-(root)!dir 2-A!dir 3-B!dir 4-C!dir"
                  " 1:fs:2 2:fs:3 2:fs:4"
      }}
    , (walker_test[]) {{        // deleted directory
          operations : "A/B A/C B/D B/E C/F C/G"
        , base : "A"
        , sequence : "*0A *AB BD BE AB* 0A*"
        , graph :  "1-(root)!dir 2-A!dir 3-B!dir"
                  " 4-D!file 5-E!file"
                  " 1:fs:2 2:fs:3 3:fs:4 3:fs:5"
      }}
    , 0
  }
};
