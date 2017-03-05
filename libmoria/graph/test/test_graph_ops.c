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

#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/growing.h"
#include "valyria/map.h"

#include "internal.h"
#include "graph.internal.h"
#include "vertex.internal.h"

typedef struct graph_test
{
  xunit_test;

  char * constr;
  char * expected;
} graph_test;

static xapi graph_test_entry(xunit_test * _test)
{
  enter;

  graph * g = 0;
  narrator * N = 0;
  map * vertices = 0;

  graph_test * test = (typeof(test)) _test;

  fatal(graph_create, &g);
  fatal(narrator_growing_create, &N);
  fatal(map_create, &vertices);

  char * seq= test->constr;
  while(*seq)
  {
    int oper = *seq++;

    vertex * A;
    if((A = map_get(vertices, MM(*seq))) == 0)
    {
      fatal(graph_vertex_createw, &A, g, 0, MM(*seq));
      fatal(map_set, vertices, MM(*seq), A);
    }
    seq++;

    vertex * B;
    if((B = map_get(vertices, MM(*seq))) == 0)
    {
      fatal(graph_vertex_createw, &B, g, 0, MM(*seq));
      fatal(map_set, vertices, MM(*seq), B);
    }
    seq++;

    if(oper == '+')
      fatal(graph_connect_edge, g, A, B, 0);
    else if(oper == '-')
      fatal(graph_disconnect_edge, g, A, B);

    while(*seq == ' ')
      seq++;
  }

  fatal(graph_say, g, N);
  assert_eq_s(test->expected, narrator_growing_buffer(N));

finally:
  fatal(graph_xfree, g);
  fatal(narrator_xfree, N);
  fatal(map_xfree, vertices);
coda;
}

xunit_unit xunit = {
    .xu_entry = graph_test_entry
  , .xu_tests = (xunit_test*[]) {
        // duplicate labels are ignored
        (graph_test[]){{
            constr    : "+AB +BC +AB +CD +AB +AB"
          , expected  : "A:B B:C C:D"
        }}
        // deleted internal edges
      , (graph_test[]){{
            constr    : "+AB +BD +AC +DE -DE"
          , expected  : "A:B A:C B:D"
        }}
        // deleted leaf edges
      , (graph_test[]){{
            constr    : "+AB +BD +AC +DE -BD"
          , expected  : "A:B A:C D:E"
        }}
    , 0
  }
};
