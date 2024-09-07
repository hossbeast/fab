/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "types.h"

#include "lorien/load.h"
#include "lorien/nftwat.h"
#include "bacon/load.h"
#include "moria/edge.h"
#include "moria/graph.h"
#include "moria/load.h"
#include "moria/vertex.h"
#include "moria/parser.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "narrator/growing.h"
#include "valyria/map.h"
#include "valyria/list.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xstring.h"
#include "xunit.h"
#include "xunit/assert.h"
#include "common/hash.h"

#include "walker.internal.h"
#include "fstree.h"
#include "lookup.h"
#include "module.h"
#include "fsent.h"
#include "fsedge.h"
#include "node_operations_test.h"
#include "entlist.h"
#include "args.h"
#include "shadow.h"

static rbtree exclude_list;

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
} walker_test_context;

//
// tests
//

static uint32_t info_hash(uint32_t h, const void * _ent)
{
  const ftwinfo *ent = _ent;
  return hash32(h, &ent->path[0], sizeof(ent->path[0]));
}

static int info_cmp(const void *_A, const void *_B)
{
  const ftwinfo *A, *B;
  A = _A;
  B = _B;
  return INTCMP(A->path[0], B->path[0]);
}

static void info_destroy(void * _info)
{
  ftwinfo * info = _info;

  free(info->path);
  free(info);
}

static void walker_test_unit_setup(xunit_unit * unit)
{
  lorien_load();
  moria_load();
  bacon_load();

  args_init();
  fsent_root_setup();
  shadow_setup();

  graph_setup();
  entlist_setup();

  /* suppress rcu-registration checks performed under devel build for events fired from the walker */
  extern __thread bool rcu_is_registered ; rcu_is_registered = true;

  walker_exclude_list = &exclude_list;
  rbtree_init(walker_exclude_list);
}

static void walker_test_unit_cleanup(xunit_unit * unit)
{
  lorien_unload();
  moria_unload();
  bacon_unload();
  graph_cleanup();
  fstree_cleanup();
  entlist_cleanup();
}

static void walker_test_entry(xunit_test * _test)
{
  walker_test * test = (walker_test *)_test;

  walker_test_context ctx = { };
  hashtable infos;
  map * nodes = 0;
  graph_invalidation_context invalidation = { };
  moria_parser * op_parser = 0;
  narrator_growing * N = 0;
  int method;
  int stop;
  char * seq;
  const char * graph;
  ftwinfo *A, *B, key;
  hashtable_search_context sctx;

  narrator_growing_create(&N);

  // setup initial graph
  moria_parser_create(&op_parser, &g_graph, (llist *[]) { &fsent_list }, 1, node_operations_test_dispatch, graph_vertex_attrs, graph_edge_attrs);
  moria_parser_operations_parse(op_parser, MMS(test->operations));

  map_create(&nodes);
  hashtable_initx(&infos, sizeof(ftwinfo), info_hash, info_cmp, info_destroy);
  graph_invalidation_begin(&invalidation);

  static int walk_ids;
  ctx.base.walk_id = ++walk_ids;
  if(test->base)
    resolve_fragment(MMS(test->base), &ctx.base.base);

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

    key.path = seq;
    A = hashtable_get(&infos, &key);
    seq++;

    B = 0;
    key.path = seq;
    sctx = (typeof(sctx)) {
      entry : &key
    };
    if((hashtable_search(&infos, &sctx))) {
      B = hashtable_fetch(&infos, &sctx);
    } else {
      xmalloc(&B, sizeof(*B));
      sctx.entry = B;
      ixsprintf((void*)&B->path, "%c", *seq);
      B->pathl = 1;
      hashtable_set(&infos, &sctx);
    }
    seq++;

    if(*seq == '*')
    {
      method = FTWAT_POST;
      seq++;
    }

    B->parent = A;
    B->type = method ? FTWAT_D : FTWAT_F;

    walker_visit(method, B, &ctx, &stop);

    while(*seq == ' ') {
      seq++;
    }
  }

  // ordered list of edges
  graph_say(&N->base);
  graph = N->s;
  assert_eq_s(test->graph, graph);

  fsent_cleanup();
  map_free(nodes);
  hashtable_destroy(&infos);
  graph_invalidation_end(&invalidation);
  moria_parser_free(op_parser);
  narrator_growing_free(N);
}

//
// manifest
//

static xunit_unit xunit = {
    xu_setup : walker_test_unit_setup
  , xu_cleanup : walker_test_unit_cleanup
  , xu_entry : walker_test_entry
  , xu_tests : (walker_test*[]) {
    /* INVALIDATE_STAT */
      (walker_test[]) {{        // no changes
          operations : "A/B"
        , base : "A"
        , sequence : "*0A AB 0A*"
        , graph :  "1-(root)!dir|protect 2-A!dir"
                  " 3-B!file"
                  " 2:fs:1 3:fs:2"
      }}
    , (walker_test[]) {{
          operations : "A/B A/C"
        , base : "A"
        , sequence : "*0A AB AC 0A*"
        , graph :  "1-(root)!dir|protect 2-A!dir"
                  " 3-B!file 4-C!file"
                  " 2:fs:1 3:fs:2 4:fs:2"
      }}
    , (walker_test[]) {{
          operations : "A/B A/C B/D B/E C/F C/G"
        , base : "A"
        , sequence : "*0A *AB BD BE AB* *AC CF CG AC* 0A*"
        , graph :  "1-(root)!dir|protect 2-A!dir 3-B!dir 4-C!dir"
                  " 5-D!file 6-E!file 7-F!file 8-G!file"
                  " 2:fs:1 3:fs:2 4:fs:2 5:fs:3 6:fs:3 7:fs:4 8:fs:4"
      }}
    , (walker_test[]) {{        // new leaves
          operations : "A/B"
        , base : "A"
        , sequence : "*0A *AB BD BE AB* *AC CF CG AC* 0A*"
        , graph :  "1-(root)!dir|protect 2-A!dir 3-B!dir 4-C!dir"
                  " 5-D!file 6-E!file 7-F!file 8-G!file"
                  " 2:fs:1 3:fs:2 4:fs:2 5:fs:3 6:fs:3 7:fs:4 8:fs:4"
      }}
    , (walker_test[]) {{
          operations : "A/B B/D B/E"
        , base : "A"
        , sequence : "*0A *AB BD BE AB* *AC CF CG AC* 0A*"
        , graph :  "1-(root)!dir|protect 2-A!dir 3-B!dir 4-C!dir"
                  " 5-D!file 6-E!file 7-F!file 8-G!file"
                  " 2:fs:1 3:fs:2 4:fs:2 5:fs:3 6:fs:3 7:fs:4 8:fs:4"
      }}
    , (walker_test[]) {{        // deleted leaves - A/C
          operations : "A/B A/C"
        , base : "A"
        , sequence : "*0A AB 0A*"
        , graph :  "1-(root)!dir|protect 2-A!dir"
                  " 3-B!file"
                  " 2:fs:1 3:fs:2"
      }}
    , (walker_test[]) {{        // deleted leaves
          operations : "A/B A/C B/D B/E C/F C/G"
        , base : "A"
        , sequence : "*0A *AB AB* *AC AC* 0A*"
        , graph :  "1-(root)!dir|protect 2-A!dir 3-B!dir 4-C!dir"
                  " 2:fs:1 3:fs:2 4:fs:2"
      }}
    , (walker_test[]) {{        // deleted directory
          operations : "A/B A/C B/D B/E C/F C/G"
        , base : "A"
        , sequence : "*0A *AB BD BE AB* 0A*"
        , graph :  "1-(root)!dir|protect 2-A!dir 3-B!dir"
                  " 4-D!file 5-E!file"
                  " 2:fs:1 3:fs:2 4:fs:3 5:fs:3"
      }}
    , 0
  }
};
XUNIT_UNIT(xunit);
