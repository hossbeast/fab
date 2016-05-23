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

#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "xapi.h"
#include "xapi/trace.h"
#include "xapi/exit.h"
#include "xapi/calltree.h"
#include "xapi/errtab.h"

struct graph_test;
#define TEST_TYPE struct graph_test
#include "xunit.h"
#include "xunit/logging.h"
#include "xunit/XUNIT.errtab.h"

struct item;
#define DICTIONARY_VALUE_TYPE struct item
#include "valyria/dictionary.h"

#include "narrator.h"
#include "narrator/fixed.h"
#include "logger.h"

#define VERTEX_VALUE_TYPE struct item
#include "internal.h"
#include "graph.internal.h"
#include "vertex.internal.h"
#include "errtab/LORIEN.errtab.h"

#include "macros.h"

#define ATTR_TABLE(x)                  \
  ATTR_DEF(WEAK , 0x01  , x)

enum {
#define ATTR_DEF(a, b, x) ATTR_ ## a = UINT32_C(b),
ATTR_TABLE(0)
#undef ATTR_DEF
};

#define ATTR_DEF(a, b, x) (x) == b ? #a :
#define ATTR_STRING(x) ATTR_TABLE(x) "UNKNOWN"

xapi graph_test_entry(const struct graph_test*)
  __attribute__((nonnull));

typedef struct graph_test
{
  xunit_test;

  struct edges {
    char *    edges;
    uint32_t  attrs;
  } ** edges;

  uint32_t  skip;
  uint32_t  finish;
  uint32_t  stop;
  uint32_t  attrs;
  char *    from;
  char *    expected;   // expected path
  char *    cycle;      // expected cycle
} graph_test;

typedef struct item
{
  char label;
  vertex * v;
} item;

xapi vertex_say(vertex * const restrict v, narrator * const restrict N)
{
  enter;

//printf("%c", vertex_value(v)->label);
  sayc(vertex_value(v)->label);

  finally : coda;
}

xapi graph_test_entry(const graph_test * test)
{
  enter;

  char space[512];
  graph * g = 0;
  dictionary * items = 0;
  narrator * N = 0;

  fatal(xapi_errtab_register, perrtab_LORIEN);  

  fatal(graph_create, &g, vertex_say);
  fatal(dictionary_create, &items, sizeof(item));
  fatal(narrator_fixed_create, &N, 4096);

  // setup the test graph

  int x;
  for(x = 0; x < sentinel(test->edges); x++)
  {
    char * edges = test->edges[x]->edges;
    uint32_t attrs = test->edges[x]->attrs;
    while(*edges)
    {
      item * A;
      if((A = dictionary_get(items, MM(edges[0]))) == 0)
      {
        fatal(dictionary_set, items, MM(edges[0]), &A);
        fatal(graph_vertex_create, g, &A->v);
        A->label = edges[0];
        vertex_value_set(A->v, A);
      }

      item * B;
      if((B = dictionary_get(items, MM(edges[1]))) == 0)
      {
        fatal(dictionary_set, items, MM(edges[1]), &B);
        fatal(graph_vertex_create, g, &B->v);
        B->label = edges[1];
        vertex_value_set(B->v, B);
      }

printf("%c -> %c", A->label, B->label);
if(attrs)
  printf(" %s", ATTR_STRING(attrs));
printf("\n");

      fatal(graph_relate, g, A->v, B->v, attrs);

      edges += 2;
      while(*edges && *edges == ' ')
        edges++;
    }
  }

  // perform the traversal
  item * root = dictionary_get(items, MMS(test->from));
  xapi exit;
  if((exit = invoke(graph_traverse, g, root->v, (void*)vertex_say, test->skip, test->finish, test->stop, test->attrs, N)))
  {
    if(xapi_exit_errtab(exit) != perrtab_LORIEN || xapi_exit_errcode(exit) != LORIEN_CYCLE)
    {
      fail(0);  // propagate unexpected errors
    }

    // save the trace
    xapi_trace_full(space, sizeof(space));
    xapi_calltree_unwind();
  }

  logf(L_OUTSET, "path %s", narrator_fixed_buffer(N));

  if(test->expected)
  {
    if(strcmp(narrator_fixed_buffer(N), test->expected))
    {
      xapi_fail_intent();
      xapi_info_adds("expected", test->expected);
      xapi_info_adds("actual", narrator_fixed_buffer(N));
      tfail(perrtab_XUNIT, XUNIT_FAIL);
    }
  }
  else if(test->cycle)
  {
    if(xapi_exit_errtab(exit) != perrtab_LORIEN || xapi_exit_errcode(exit) != LORIEN_CYCLE)
    {
      xapi_fail_intent();
      xapi_info_adds("expected cycle", test->cycle);
      xapi_info_adds("actual", "no cycle detected");
      tfail(perrtab_XUNIT, XUNIT_FAIL);
    }
    else
    {
      char * marker = "in graph_traverse(path=";
      char * actual = strstr(space, marker);
      char * end = 0;
      if(actual)
      {
        actual += strlen(marker);
        end = strstr(actual, ")");
      }
      if(actual == 0 || end == 0 || end - actual != strlen(test->cycle))
      {
        xapi_fail_intent();
        xapi_info_adds("expected cycle", test->cycle);
        xapi_info_adds("actual", "cycle path not reported");
        tfail(perrtab_XUNIT, XUNIT_FAIL);
      }
      else if(memcmp(actual, test->cycle, end - actual))
      {
        xapi_fail_intent();
        xapi_info_adds("expected cycle", test->cycle);
        xapi_info_addf("actual cycle", "%.*s", (int)(end - actual), actual);
        tfail(perrtab_XUNIT, XUNIT_FAIL);
      }
    }
  }

finally:
  dictionary_free(items);
coda;
}

xunit_unit xunit = {
    .tests = (xunit_test*[]) {
        // downward, breadth
        (graph_test[]){{ .entry = graph_test_entry
          , edges : (struct edges*[]) {
                (struct edges[]) {{ edges : "AB BC AC CD DE" }}
              , 0
            }
          , attrs     : GRAPH_TRAVERSE_DOWN | GRAPH_TRAVERSE_BREADTH
          , from      : "A"
          , expected  : "ABCDE"
        }}
        // downward, depth
      , (graph_test[]){{ .entry = graph_test_entry
          , edges : (struct edges*[]) {
                (struct edges[]) {{ edges : "AB BC AC CD DE" }}
              , 0
            }
          , attrs     : GRAPH_TRAVERSE_DOWN | GRAPH_TRAVERSE_DEPTH
          , from      : "A"
          , expected  : "EDCBA"
        }}
        // downward, depth
      , (graph_test[]){{ .entry = graph_test_entry
          , edges : (struct edges*[]) {
                (struct edges[]) {{ edges : "AB BC AC CD DE" }}
              , 0
            }
          , attrs     : GRAPH_TRAVERSE_DOWN | GRAPH_TRAVERSE_DEPTH
          , from      : "C"
          , expected  : "EDC"
        }}
        // upward, breadth, skip weak
      , (graph_test[]){{ .entry = graph_test_entry
          , edges : (struct edges*[]) {
                (struct edges[]) {{ edges : "AB BC AD DE" }}
              , (struct edges[]) {{ edges : "BD", attrs : ATTR_WEAK }}
              , 0
            }
          , attrs     : GRAPH_TRAVERSE_UP | GRAPH_TRAVERSE_BREADTH
          , from      : "C"
          , skip      : ATTR_WEAK
          , expected  : "CBA"
        }}
        // downward, breadth, treat weak normally
      , (graph_test[]){{ .entry = graph_test_entry
          , edges : (struct edges*[]) {
                (struct edges[]) {{ edges : "AB BC DE" }}
              , (struct edges[]) {{ edges : "BD", attrs : ATTR_WEAK }}
              , 0
            }
          , attrs     : GRAPH_TRAVERSE_DOWN | GRAPH_TRAVERSE_BREADTH
          , from      : "A"
          , expected  : "ABCDE"
        }}
        // downward, breadth, skip weak
      , (graph_test[]){{ .entry = graph_test_entry
          , edges : (struct edges*[]) {
                (struct edges[]) {{ edges : "AB BC DE" }}
              , (struct edges[]) {{ edges : "BD", attrs : ATTR_WEAK }}
              , 0
            }
          , attrs     : GRAPH_TRAVERSE_DOWN | GRAPH_TRAVERSE_BREADTH
          , from      : "A"
          , skip      : ATTR_WEAK
          , expected  : "ABCE"
        }}
        // downward, breadth, finish
      , (graph_test[]){{ .entry = graph_test_entry
          , edges : (struct edges*[]) {
                (struct edges[]) {{ edges : "AB BC DE" }}
              , (struct edges[]) {{ edges : "BD", attrs : ATTR_WEAK }}
              , 0
            }
          , attrs     : GRAPH_TRAVERSE_DOWN | GRAPH_TRAVERSE_BREADTH
          , from      : "A"
          , finish    : ATTR_WEAK
          , expected  : "ABCD"
        }}
        // downward, breadth, stop on weak
      , (graph_test[]){{ .entry = graph_test_entry
          , edges : (struct edges*[]) {
                (struct edges[]) {{ edges : "AB BC DE" }}
              , (struct edges[]) {{ edges : "BD", attrs : ATTR_WEAK }}
              , 0
            }
          , attrs     : GRAPH_TRAVERSE_DOWN | GRAPH_TRAVERSE_BREADTH
          , from      : "A"
          , stop      : ATTR_WEAK
          , expected  : "ABC"
        }}
        // cycle detection
      , (graph_test[]){{ .entry = graph_test_entry
          , edges : (struct edges*[]) {
                (struct edges[]) {{ edges : "AB BC CD DE EF FG GA" }}
              , 0
            }
          , attrs     : GRAPH_TRAVERSE_DOWN | GRAPH_TRAVERSE_BREADTH
          , from      : "A"
          , cycle     : "A -> B -> C -> D -> E -> F -> G -> A"
        }}

        // duplicate labels
        // cycle that exceeds the stack size
        // cycle message format

      , 0
  }
};
