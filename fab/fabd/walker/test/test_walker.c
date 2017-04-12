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
#include "valyria/load.h"
#include "lorien/load.h"
#include "moria/load.h"

#include "valyria/dictionary.h"
#include "valyria/map.h"
#include "valyria/array.h"
#include "lorien/nftwat.h"
#include "moria/graph.h"
#include "moria/vertex.h"
#include "moria/edge.h"
#include "xlinux/xstring.h"
#include "xlinux/xstdlib.h"

#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "path.h"

#include "walker.internal.h"
#include "node.h"
#include "node_operations.h"
#include "filesystem.h"

typedef struct
{
  xunit_test;

  char * edges;
  char * root;
  char * sequence;
  char * operations;
} walker_test;

#define OP_WATCH          1
#define OP_REFRESH        2
#define OP_CONNECT        3
#define OP_DISINTEGRATE   4

typedef struct
{
  int op;
  union {
    node * n;
    struct {
      node * parent;
      node * child;
    };
    edge * e;
  };
} walker_test_operation;

typedef struct
{
  walker_context;
  array * operations;
} walker_test_context;

static xapi walker_test_unit_setup(xunit_unit * unit)
{
  enter;

  fatal(valyria_load);
  fatal(lorien_load);
  fatal(moria_load);

  finally : coda;
}

static xapi walker_test_unit_cleanup(xunit_unit * unit)
{
  enter;

  fatal(valyria_unload);
  fatal(lorien_unload);
  fatal(moria_unload);

  finally : coda;
}

static xapi walker_test_operation_say(walker_test_operation * op, narrator * N)
{
  enter;

  vertex * v = vertex_containerof(op->n);

  if(op->op == OP_WATCH)
    sayf("@%.*s", (int)v->label_len, v->label);
  else if(op->op == OP_REFRESH)
    sayf("!%.*s", (int)v->label_len, v->label);
  else if(op->op == OP_CONNECT)
  {
    vertex * parent = vertex_containerof(op->parent);
    vertex * child = vertex_containerof(op->child);
    sayf("%.*s:%.*s", (int)parent->label_len, parent->label, (int)child->label_len, child->label);
  }
  else if(op->op == OP_DISINTEGRATE)
  {
    sayf("%.*s~%.*s", (int)op->e->A->label_len, op->e->A->label, (int)op->e->B->label_len, op->e->B->label);
  }
  else
    says("(unknown)");

  finally : coda;
}

//
// tests
//

static void info_free(void * _info)
{
  ftwinfo * info = _info;
  wfree((void*)info->path);
}

static xapi create(walker_context * ctx, node ** restrict n, uint8_t fstype, const struct filesystem * restrict fs, const char * restrict name)
{
  enter;

  vertex * v;
  fatal(graph_vertex_create, &v, g_node_graph, 0);
  *n = vertex_value(v);
  v->label = name;
  v->label_len = 1;

  (*n)->fstype = fstype;
  (*n)->fs = fs;

  if((*n)->fstype == NODE_FS_TYPE_DIR)
    (*n)->wd = -1;

  finally : coda;
}

static const struct filesystem * fslookup(walker_test_context * ctx, const char * const restrict path)
{
  return ctx->udata;
}

static xapi watch(walker_test_context * ctx, node * n)
{
  enter;

  walker_test_operation * operation;
  fatal(array_push, ctx->operations, &operation);
  operation->op = OP_WATCH;
  operation->n = n;

  finally : coda;
}

static xapi refresh(walker_test_context * ctx, node * n)
{
  enter;

  walker_test_operation * operation;
  fatal(array_push, ctx->operations, &operation);
  operation->op = OP_REFRESH;
  operation->n = n;

  finally : coda;
}

static xapi connect(walker_test_context * restrict ctx, node * restrict parent, node * restrict n)
{
  enter;

  walker_test_operation * operation;
  fatal(array_push, ctx->operations, &operation);
  operation->op = OP_CONNECT;
  operation->parent = parent;
  operation->child = n;

  finally : coda;
}

static xapi disintegrate(walker_test_context * restrict ctx, edge * restrict e)
{
  enter;

  walker_test_operation * operation;
  fatal(array_push, ctx->operations, &operation);
  operation->op = OP_DISINTEGRATE;
  operation->e = e;

  finally : coda;
}

static xapi walker_test_entry(xunit_test * _test)
{
  enter;

  walker_test * test = (walker_test *)_test;

  walker_test_context ctx = {};
  dictionary * infos = 0;
  map * nodes = 0;
  filesystem * fs = 0;
  int x;
  char act[32] = { };
  narrator_fixed_storage fixed;
  fixed.s = act;
  fixed.l = 0;
  fixed.a = sizeof(act);

  fatal(node_setup);

  fatal(map_create, &nodes);
  fatal(dictionary_createx, &infos, sizeof(ftwinfo), info_free, 0, 0);
  fatal(array_create, &ctx.operations, sizeof(walker_test_operation));

  // mocked out dependencies
  fatal(xmalloc, &fs, sizeof(*fs));
  fs->attrs = FILESYSTEM_INVALIDATE_STAT;
  fs->leaf = 0;

  ctx.create = (void*)create;
  ctx.fslookup = (void*)fslookup;
  ctx.refresh = (void*)refresh;
  ctx.watch = (void*)watch;
  ctx.connect = (void*)connect;
  ctx.disintegrate = (void*)disintegrate;
  ctx.udata = fs;

  // setup the initial graph
  char * edges = test->edges;
  while(edges && *edges)
  {
    node * A;
    if((A = map_get(nodes, MM(edges[0]))) == 0)
    {
      fatal(create, 0, &A, 0, fs, &edges[0]);
      fatal(map_set, nodes, MM(edges[0]), A);
    }
    A->fstype = NODE_FS_TYPE_DIR;

    node * B;
    if((B = map_get(nodes, MM(edges[1]))) == 0)
    {
      fatal(create, 0, &B, NODE_FS_TYPE_FILE, fs, &edges[1]);
      fatal(map_set, nodes, MM(edges[1]), B);
    }

    fatal(node_connect, A, B);

    edges += 2;
    while(*edges == ' ')
      edges++;
  }

  static int walk_ids;
  ctx.walk_id = ++walk_ids;
  if(test->root)
    ctx.root = map_get(nodes, MMS(test->root)); // pre-existing root, if any

  // carry out the test sequence
  char * seq = test->sequence;
  while(*seq)
  {
    int method = 0;
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
      fatal(dictionary_set, infos, MM(*seq), &B);
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

    int stop;
    fatal(walker_visit, method, B, &ctx, &stop);

    while(*seq == ' ')
      seq++;
  }

  narrator * N = narrator_fixed_init(&fixed);

  // assert operations performed
  char * ops = test->operations;
  for(x = 0; x < ctx.operations->l || *ops; x++)
  {
    char * exp = ops;
    while(*ops && *ops != ' ')
      ops++;

    // expected
    if(x >= ctx.operations->l)
    {
      xapi_info_pushs("expected operations", ops);
      fail(XUNIT_FAIL);
    }

    // actual
    else
    {
      fatal(narrator_reset, N);
      act[0] = 0;
      walker_test_operation * act_op = array_get(ctx.operations, x);
      fatal(walker_test_operation_say, act_op, N);

      if(!*exp)
      {
        xapi_info_pushs("unexpected operation", act);
        fail(XUNIT_FAIL);
      }
      else
      {
        assert_eq_w(exp, ops - exp, act, strlen(act));
      }
    }

    while(*ops == ' ')
      ops++;
  }

finally:
  fatal(node_cleanup);

  fatal(map_xfree, nodes);
  fatal(dictionary_xfree, infos);
  wfree(fs);
coda;
}

//
// manifest
//

xunit_unit xunit = {
    xu_setup : walker_test_unit_setup
  , xu_cleanup : walker_test_unit_cleanup
  , xu_entry : walker_test_entry
  , xu_tests : (xunit_test*[]) {
      (walker_test[]) {{        // something from nothing
          sequence : "*0A AB 0A*"
        , operations : "A:B !B"
      }}
    , (walker_test[]) {{
          sequence : "*0A AB AC 0A*"
        , operations : "A:B !B A:C !C"
      }}
    , (walker_test[]) {{        // no changes
          edges : "AB"
        , root : "A"
        , sequence : "*0A AB 0A*"
        , operations : "!B"
      }}
    , (walker_test[]) {{
          edges : "AB AC"
        , root : "A"
        , sequence : "*0A AB AC 0A*"
        , operations : "!B !C"
, xu_only : 1
      }}
    , (walker_test[]) {{
          edges : "AB AC BD BE CF CG"
        , root : "A"
        , sequence : "*0A *AB BD BE AB* *AC CF CG AC* 0A*"
        , operations : "!D !E !F !G"
      }}
    , (walker_test[]) {{        // new leaves
          edges : "AB"
        , root : "A"
        , sequence : "*0A *AB BD BE AB* *AC CF CG AC* 0A*"
        , operations : "B:D !D B:E !E A:C C:F !F C:G !G"
      }}
    , (walker_test[]) {{
          edges : "AB BD BE"
        , root : "A"
        , sequence : "*0A *AB BD BE AB* *AC CF CG AC* 0A*"
        , operations : "!D !E A:C C:F !F C:G !G"
      }}
    , (walker_test[]) {{        // deleted leaves
          edges : "AB AC"
        , root : "A"
        , sequence : "*0A AB 0A*"
        , operations : "!B A~C"
      }}
    , (walker_test[]) {{        // deleted leaves
          edges : "AB AC BD BE CF CG"
        , root : "A"
        , sequence : "*0A *AB AB* *AC AC* 0A*"
        , operations : "B~E B~D C~G C~F"
      }}
    , (walker_test[]) {{        // deleted directory
          edges : "AB AC BD BE CF CG"
        , root : "A"
        , sequence : "*0A *AB BD BE AB* 0A*"
        , operations : "!D !E A~C"
      }}
    , 0
  }
};
