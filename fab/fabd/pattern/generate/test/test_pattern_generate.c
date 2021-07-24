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

#include "xunit.h"
#include "xunit/assert.h"

#include "xlinux/xstdlib.h"
#include "moria/graph.h"
#include "moria/vertex.h"
#include "moria/operations.h"
#include "moria/parser.h"
#include "valyria/list.h"
#include "valyria/set.h"
#include "valyria/map.h"
#include "narrator.h"
#include "narrator/growing.h"
#include "yyutil/load.h"

#include "generate.internal.h"
#include "search.internal.h"
#include "lookup.h"
#include "pattern_parser.h"
#include "pattern.h"
#include "fsent.h"
#include "shadow.h"
#include "variant.h"
#include "module.internal.h"
#include "node_operations_test.h"
#include "filesystem.h"

typedef struct pattern_generate_test {
  XUNITTEST;

  // setup
  char * operations;
  bool root;                // whether to setup the root node
  char ** modules_shadow;   // module node -> id list
  char ** modules_requires; // module A : requires : module B list

  // context
  char * module;
  char * match;
  char * match_variant;
  char ** groups;
  char * pattern;
  char * variant;
  char ** variants;

  // results
  char * graph;
  char * nodes;
} pattern_generate_test;

static void pattern_test_unit_setup(xunit_unit * unit)
{
  yyutil_load();
  logging_finalize();

  filesystem_setup();
  graph_setup();

logs(L_INFO, "GOATS");
}

static void pattern_test_unit_cleanup(xunit_unit * unit)
{
  graph_cleanup();
  filesystem_cleanup();
}

static int node_compare(const void * _A, const void * _B)
{
  const fsent * A = *(const fsent **)_A;
  const fsent * B = *(const fsent **)_B;

//  return strcmp(A->name.name, B->name.name);
  return memncmp(A->name.name, A->name.namel, B->name.name, B->name.namel);
}

//
// tests
//

static void pattern_generate_test_entry(xunit_test * _test)
{
  pattern_generate_test * test = (pattern_generate_test *)_test;

  narrator_growing * N = 0;
  pattern_parser * parser = 0;
  pattern * pattern = 0;
  set * nodes_list = 0;
  fsent ** node_list = 0;
  size_t * node_list_lens = 0;
  size_t node_list_len = 0;
  module mods[4] = { 0 };
  module *mod;
  yyu_location loc;
  graph_parser * op_parser = 0;
  set * variants = 0;
  variant * var = 0;
  pattern_search_node match = { 0 };
  int x;
  graph_invalidation_context invalidation = { 0 };
  fsent *na, *nb;
  const char *graph;
  size_t graph_len;
  const char *nodes;
  size_t nodes_len;

  variant_setup();

  /* whether to include the root fs */
  if(test->root) {
    fsent_setup();
  }

  // build out the initial graph
  graph_parser_create(&op_parser, &g_graph, &fsent_list, node_operations_test_dispatch, graph_vertex_attrs, graph_edge_attrs);
  graph_parser_operations_parse(op_parser, MMS(test->operations));

  // setup the shadow fs
  if(test->modules_shadow)
  {
    shadow_setup();
    graph_invalidation_begin(&invalidation);

    for(x = 0; x < sentinel(test->modules_shadow); x += 2)
    {
      resolve_fragment(MMS(test->modules_shadow[x]), &mods[x / 2].dir_node);
      moria_vertex_init(&mods[x/2].vertex, &g_graph, VERTEX_MODULE);
      snprintf(mods[x / 2].id, sizeof(mods[x / 2].id), "%s", test->modules_shadow[x + 1]);
      shadow_module_init(&mods[x / 2], &invalidation);
      mods[x/2].vertex.label = test->modules_shadow[x];
      mods[x/2].vertex.label_len = strlen(test->modules_shadow[x]);
      mods[x / 2].dir_node->mod = &mods[x / 2];
    }

    if(test->modules_requires)
    {
      for(x = 0; x < sentinel(test->modules_requires); x += 2)
      {
        resolve_fragment(MMS(test->modules_requires[x + 0]), &na);
        resolve_fragment(MMS(test->modules_requires[x + 1]), &nb);

        module_resolve_require(na->self_mod, nb->self_mod, &invalidation);
      }
    }

    graph_invalidation_end(&invalidation);

    for(x = 0; x < sizeof(mods) / sizeof(*mods); x++)
    {
      if(strcmp(mods[x].dir_node->name.name, test->module) == 0)
      {
        mod = &mods[x];
        break;
      }
    }
  }

  if(!test->modules_shadow)
  {
    resolve_fragment(MMS(test->module), &mods[0].dir_node);
    mod = &mods[0];
    mod->shadow_uses = mod->dir_node;
  }

  narrator_growing_create(&N);

  set_create(&variants);
  if(test->variants)
  {
    for(x = 0; x < sentinel(test->variants); x++)
    {
      variant_get(test->variants[x], strlen(test->variants[x]), &var);
      set_put(variants, var, 0);
    }
  }

  if(test->match)
    resolve_fragment(MMS(test->match), &match.node);

  if(test->groups)
  {
    match.group_max = sentinel(test->groups) - 1;
    for(x = 0; x <= match.group_max; x++)
    {
      match.groups[x].start = test->groups[x];
      match.groups[x].len = strlen(test->groups[x]);
    }
  }

  if(test->match_variant)
  {
    variant_get(MMS(test->match_variant), (variant **)&match.variant);
  }

  // parse the pattern
  pattern_parser_create(&parser);
  generate_pattern_parse_partial(parser, test->pattern, strlen(test->pattern) + 2, "-test-", 0, &loc, &pattern);
  assert_eq_u32(strlen(test->pattern), loc.l);

  // act
  set_create(&nodes_list);
  fatal(pattern_generate
    , pattern
    , mod
    , mod->dir_node
    , mod->shadow_uses
    , variants
    , &invalidation
    , &match
    , nodes_list
  );

  // ordered list of vertices and edges
  narrator_xreset(&N->base);
  graph_say(&N->base);
  graph = N->s;
  graph_len = N->l;
  assert_eq_s(test->graph, graph);
  assert_eq_w(test->graph, strlen(test->graph), graph, graph_len);

  // ordered list of nodes
  narrator_xreset(&N->base);

  set_elements(nodes_list, &node_list, &node_list_lens, &node_list_len);
  qsort(node_list, node_list_len, sizeof(*node_list), (void*)node_compare);

  for(x = 0; x < node_list_len; x++)
  {
    fsent * n = node_list[x];

    if(x)
      narrator_xsays(&N->base, " ");
    narrator_xsays(&N->base, n->name.name);
  }
  nodes = N->s;
  nodes_len = N->l;
  assert_eq_w(test->nodes, strlen(test->nodes), nodes, nodes_len);

finally:
  void_infos("test->pattern", test->pattern);

  wfree(node_list);
  wfree(node_list_lens);
  set_xfree(variants);
  pattern_free(pattern);
  fsent_cleanup();
  variant_cleanup();
  narrator_growing_free(N);
  pattern_parser_xfree(parser);
  set_xfree(nodes_list);
  graph_parser_xfree(op_parser);
coda;
}

//
// manifest
//

xunit_unit xunit = {
    xu_setup : pattern_test_unit_setup
  , xu_cleanup : pattern_test_unit_cleanup
  , xu_entry : pattern_generate_test_entry
  , xu_tests : (pattern_generate_test*[]) {
    /* single fsent */
      (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "b\0" }
        , graph : ""
                  "1-MOD!dir 2-b!U|file"
                  " 1:fs:2"
        , nodes : "b"
      }}
    , (pattern_generate_test[]) {{
          operations : "amod"
        , module : "amod"
        , pattern : (char[]) { "b/c\0" }
        , graph :  ""
                  "1-amod!dir 2-b!U|dir 3-c!U|file"
                  " 1:fs:2 2:fs:3"
        , nodes : "c"
      }}

    /* absolute and relative paths */
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "./b\0" }
        , graph :  ""
                  "1-MOD!dir 2-b!U|file"
                  " 1:fs:2"
        , nodes : "b"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "././b\0" }
        , graph :  ""
                  "1-MOD!dir 2-b!U|file"
                  " 1:fs:2"
        , nodes : "b"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "a/./b\0" }
        , graph :  ""
                  "1-MOD!dir 2-a!U|dir 3-b!U|file"
                  " 1:fs:2 2:fs:3"
        , nodes : "b"
      }}

    /* alternations */
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "{b}\0" }
        , graph :  ""
                  "1-MOD!dir 2-b!U|file"
                  " 1:fs:2"
        , nodes : "b"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "{{b}}\0" }
        , graph :  ""
                  "1-MOD!dir 2-b!U|file"
                  " 1:fs:2"
        , nodes : "b"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "{{{b}}}\0" }
        , graph :  ""
                  "1-MOD!dir 2-b!U|file"
                  " 1:fs:2"
        , nodes : "b"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "{b,}\0" }
        , graph :  ""
                  "1-MOD!dir 2-b!U|file"
                  " 1:fs:2"
        , nodes : "b"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "{b,c}\0" }
        , graph :  ""
                  "1-MOD!dir 2-b!U|file 3-c!U|file"
                  " 1:fs:2 1:fs:3"
        , nodes : "b c"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "{{b,c}}\0" }
        , graph :  ""
                  "1-MOD!dir 2-b!U|file 3-c!U|file"
                  " 1:fs:2 1:fs:3"
        , nodes : "b c"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "{b,c,d}\0" }
        , graph :  ""
                  "1-MOD!dir 2-b!U|file 3-c!U|file 4-d!U|file"
                  " 1:fs:2 1:fs:3 1:fs:4"
        , nodes : "b c d"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "b/{c,d}\0" }
        , graph :  ""
                  "1-MOD!dir 2-b!U|dir 3-c!U|file 4-d!U|file"
                  " 1:fs:2 2:fs:3 2:fs:4"
        , nodes : "c d"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "b/{c,}\0" }
        , graph :  ""
                  "1-MOD!dir 2-b!U|dir 3-c!U|file"
                  " 1:fs:2 2:fs:3"
        , nodes : "c"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "{c,d}/e\0" }
        , graph :  ""
                  "1-MOD!dir 2-c!U|dir 3-d!U|dir 4-e!U|file 5-e!U|file"
                  " 1:fs:2 1:fs:3 2:fs:4 3:fs:5"
        , nodes : "e e"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "a{c,d}/xyz\0" }
        , graph :  ""
                  "1-MOD!dir 2-ac!U|dir 3-ad!U|dir 4-xyz!U|file 5-xyz!U|file"
                  " 1:fs:2 1:fs:3 2:fs:4 3:fs:5"
        , nodes : "xyz xyz"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "b/{c,d}/e\0" }
        , graph :  ""
                  "1-MOD!dir 2-b!U|dir 3-c!U|dir 4-d!U|dir 5-e!U|file 6-e!U|file"
                  " 1:fs:2 2:fs:3 2:fs:4 3:fs:5 4:fs:6"
        , nodes : "e e"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "{b,c}/{d,e}\0" }
        , graph :  ""
                  "1-MOD!dir 2-b!U|dir 3-c!U|dir 4-d!U|file 5-d!U|file 6-e!U|file 7-e!U|file"
                  " 1:fs:2 1:fs:3 2:fs:4 2:fs:6 3:fs:5 3:fs:7"
        , nodes : "d d e e"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "b{c,d}\0" }
        , graph :  ""
                  "1-MOD!dir 2-bc!U|file 3-bd!U|file"
                  " 1:fs:2 1:fs:3"
        , nodes : "bc bd"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "b{{c,d}}\0" }
        , graph :  ""
                  "1-MOD!dir 2-bc!U|file 3-bd!U|file"
                  " 1:fs:2 1:fs:3"
        , nodes : "bc bd"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "b{{{c,d}}}\0" }
        , graph :  ""
                  "1-MOD!dir 2-bc!U|file 3-bd!U|file"
                  " 1:fs:2 1:fs:3"
        , nodes : "bc bd"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "{c,d}b\0" }
        , graph :  ""
                  "1-MOD!dir 2-cb!U|file 3-db!U|file"
                  " 1:fs:2 1:fs:3"
        , nodes : "cb db"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "{{c,d}}b\0" }
        , graph :  ""
                  "1-MOD!dir 2-cb!U|file 3-db!U|file"
                  " 1:fs:2 1:fs:3"
        , nodes : "cb db"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "{{{c,d}}}b\0" }
        , graph :  ""
                  "1-MOD!dir 2-cb!U|file 3-db!U|file"
                  " 1:fs:2 1:fs:3"
        , nodes : "cb db"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "b{c,d,}\0" }
        , graph :  ""
                  "1-MOD!dir 2-b!U|file 3-bc!U|file 4-bd!U|file"
                  " 1:fs:2 1:fs:3 1:fs:4"
        , nodes : "b bc bd"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "{b}{c}\0" }
        , graph :  ""
                  "1-MOD!dir 2-bc!U|file"
                  " 1:fs:2"
        , nodes : "bc"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "{b,c}{d}\0" }
        , graph :  ""
                  "1-MOD!dir 2-bd!U|file 3-cd!U|file"
                  " 1:fs:2 1:fs:3"
        , nodes : "bd cd"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "{b}{c,d}\0" }
        , graph :  ""
                  "1-MOD!dir 2-bc!U|file 3-bd!U|file"
                  " 1:fs:2 1:fs:3"
        , nodes : "bc bd"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "{b,c}{d,e}\0" }
        , graph :  ""
                  "1-MOD!dir 2-bd!U|file 3-be!U|file 4-cd!U|file 5-ce!U|file"
                  " 1:fs:2 1:fs:3 1:fs:4 1:fs:5"
        , nodes : "bd be cd ce"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "{b,}{d,}\0" }
        , graph :  ""
                  "1-MOD!dir 2-b!U|file 3-bd!U|file 4-d!U|file"
                  " 1:fs:2 1:fs:3 1:fs:4"
        , nodes : "b bd d"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "{x{b,c}}\0" }
        , graph :  ""
                  "1-MOD!dir 2-xb!U|file 3-xc!U|file"
                  " 1:fs:2 1:fs:3"
        , nodes : "xb xc"
      }}

    /* classes */
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "[bcd]\0" }
        , graph :  ""
                  "1-MOD!dir 2-b!U|file 3-c!U|file 4-d!U|file"
                  " 1:fs:2 1:fs:3 1:fs:4"
        , nodes : "b c d"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "x[bc]\0" }
        , graph :  ""
                  "1-MOD!dir 2-xb!U|file 3-xc!U|file"
                  " 1:fs:2 1:fs:3"
        , nodes : "xb xc"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "[bc]x\0" }
        , graph :  ""
                  "1-MOD!dir 2-bx!U|file 3-cx!U|file"
                  " 1:fs:2 1:fs:3"
        , nodes : "bx cx"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "x[bc]y\0" }
        , graph :  ""
                  "1-MOD!dir 2-xby!U|file 3-xcy!U|file"
                  " 1:fs:2 1:fs:3"
        , nodes : "xby xcy"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "a[bc]/xyz\0" }
        , graph :  ""
                  "1-MOD!dir 2-ab!U|dir 3-ac!U|dir 4-xyz!U|file 5-xyz!U|file"
                  " 1:fs:2 1:fs:3 2:fs:4 3:fs:5"
        , nodes : "xyz xyz"
      }}

    /* ranges */
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "[b-e]\0" }
        , graph :  ""
                  "1-MOD!dir 2-b!U|file 3-c!U|file 4-d!U|file 5-e!U|file"
                  " 1:fs:2 1:fs:3 1:fs:4 1:fs:5"
        , nodes : "b c d e"
      }}

    /* nested alternations and classes */
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "{[b-e]}\0" }
        , graph :  ""
                  "1-MOD!dir 2-b!U|file 3-c!U|file 4-d!U|file 5-e!U|file"
                  " 1:fs:2 1:fs:3 1:fs:4 1:fs:5"
        , nodes : "b c d e"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "{x[b-d]}\0" }
        , graph :  ""
                  "1-MOD!dir 2-xb!U|file 3-xc!U|file 4-xd!U|file"
                  " 1:fs:2 1:fs:3 1:fs:4"
        , nodes : "xb xc xd"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "{[b-d]y}\0" }
        , graph :  ""
                  "1-MOD!dir 2-by!U|file 3-cy!U|file 4-dy!U|file"
                  " 1:fs:2 1:fs:3 1:fs:4"
        , nodes : "by cy dy"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "x{[bc]}\0" }
        , graph :  ""
                  "1-MOD!dir 2-xb!U|file 3-xc!U|file"
                  " 1:fs:2 1:fs:3"
        , nodes : "xb xc"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "{[bc]}y\0" }
        , graph :  ""
                  "1-MOD!dir 2-by!U|file 3-cy!U|file"
                  " 1:fs:2 1:fs:3"
        , nodes : "by cy"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "x{[bc]}y\0" }
        , graph :  ""
                  "1-MOD!dir 2-xby!U|file 3-xcy!U|file"
                  " 1:fs:2 1:fs:3"
        , nodes : "xby xcy"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "x{{,[bc]}}y\0" }
        , graph :  ""
                  "1-MOD!dir 2-xby!U|file 3-xcy!U|file 4-xy!U|file"
                  " 1:fs:2 1:fs:3 1:fs:4"
        , nodes : "xby xcy xy"
      }}

    /* axes */

    /* variants */
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , variants : (char*[]) { "void", 0 }
        , pattern : (char[]) { "b.?\0" }
        , graph :  ""
                  "1-MOD!dir 2-b.void!U|file"
                  " 1:fs:2"
        , nodes : "b.void"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , variants : (char*[]) { "void", "devel", 0 }
        , pattern : (char[]) { "b.?\0" }
        , graph :  ""
                  "1-MOD!dir 2-b.devel!U|file 3-b.void!U|file"
                  " 1:fs:2 1:fs:3"
        , nodes : "b.devel b.void"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "b.?\0" }
        , graph :  ""
                  "1-MOD!dir 2-b.!U|file"
                  " 1:fs:2"
        , nodes : "b."
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , variants : (char*[]) { "x", "y", 0 }
        , pattern : (char[]) { "b?c\0" }
        , graph :  ""
                  "1-MOD!dir 2-bxc!U|file 3-byc!U|file"
                  " 1:fs:2 1:fs:3"
        , nodes : "bxc byc"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , variants : (char*[]) { "x", "y", 0 }
        , pattern : (char[]) { "c?\0" }
        , graph :  ""
                  "1-MOD!dir 2-cx!U|file 3-cy!U|file"
                  " 1:fs:2 1:fs:3"
        , nodes : "cx cy"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , variants : (char*[]) { "x", "y", 0 }
        , pattern : (char[]) { "?c\0" }
        , graph :  ""
                  "1-MOD!dir 2-xc!U|file 3-yc!U|file"
                  " 1:fs:2 1:fs:3"
        , nodes : "xc yc"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , variants : (char*[]) { "x", "y", 0 }
        , pattern : (char[]) { "b??c\0" }
        , graph :  ""
                  "1-MOD!dir 2-bxxc!U|file 3-byyc!U|file"
                  " 1:fs:2 1:fs:3"
        , nodes : "bxxc byyc"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , variants : (char*[]) { "x", "y", 0 }
        , pattern : (char[]) { "?/?\0" }
        , graph :  ""
                  "1-MOD!dir 2-x!U|dir 3-y!U|dir"
                  " 4-x!U|file 5-y!U|file"
                  " 1:fs:2 1:fs:3 2:fs:4 3:fs:5"
        , nodes : "x y"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , variants : (char*[]) { "x", "y", 0 }
        , pattern : (char[]) { "??/??\0" }
        , graph :  ""
                  "1-MOD!dir 2-xx!U|dir 3-yy!U|dir"
                  " 4-xx!U|file 5-yy!U|file"
                  " 1:fs:2 1:fs:3 2:fs:4 3:fs:5"
        , nodes : "xx yy"
      }}

    /* replacements */
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , groups : (char*[]) { "", "foo", 0 }
        , pattern : (char[]) { "$1\0" }
        , graph :  ""
                  "1-MOD!dir 2-foo!U|file"
                  " 1:fs:2"
        , nodes : "foo"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , groups : (char*[]) { "", "foo", 0 }
        , pattern : (char[]) { "bar/$1\0" }
        , graph :  ""
                  "1-MOD!dir 2-bar!U|dir 3-foo!U|file"
                  " 1:fs:2 2:fs:3"
        , nodes : "foo"
      }}
    , (pattern_generate_test[]) {{
          operations : ""
          " MOD/bar/baz"
        , module : "MOD"
        , match : "baz"
        , pattern : (char[]) { "$^D/foo\0" }
        , graph :  ""
                  "1-MOD!dir 2-bar!dir 3-baz!file 4-foo!U|file"
                  " 1:fs:2 2:fs:3 2:fs:4"
        , nodes : "foo"
      }}
    , (pattern_generate_test[]) {{
          operations : ""
          " MOD/bar/baz"
        , module : "MOD"
        , groups : (char*[]) { "", "foo", 0 }
        , variants : (char*[]) { "x", "y", 0 }
        , match : "baz"
        , pattern : (char[]) { "$^D/$1.?.o\0" }
        , graph :  ""
                  "1-MOD!dir 2-bar!dir 3-baz!file 4-foo.x.o!U|file 5-foo.y.o!U|file"
                  " 1:fs:2 2:fs:3 2:fs:4 2:fs:5"
        , nodes : "foo.x.o foo.y.o"
      }}

    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , match_variant : "foo"
        , pattern : (char[]) { "$?\0" }
        , graph :  ""
                  "1-MOD!dir 2-foo!U|file"
                  " 1:fs:2"
        , nodes : "foo"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , match_variant : "foo"
        , groups : (char*[]) { "", "bar", 0 }
        , pattern : (char[]) { "$?A$1\0" }
        , graph :  ""
                  "1-MOD!dir 2-fooAbar!U|file"
                  " 1:fs:2"
        , nodes : "fooAbar"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , match_variant : "foo"
        , groups : (char*[]) { "", "bar", 0 }
        , pattern : (char[]) { "$1A$?\0" }
        , graph :  ""
                  "1-MOD!dir 2-barAfoo!U|file"
                  " 1:fs:2"
        , nodes : "barAfoo"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , match_variant : "foo"
        , variants : (char*[]) { "x", "y", 0 }
        , pattern : (char[]) { "bar/$?A?\0" }
        , graph :  ""
                  "1-MOD!dir 2-bar!U|dir 3-fooAx!U|file 4-fooAy!U|file"
                  " 1:fs:2 2:fs:3 2:fs:4"
        , nodes : "fooAx fooAy"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , match_variant : "foo"
        , pattern : (char[]) { "${?}\0" }
        , graph :  ""
                  "1-MOD!dir 2-foo!U|file"
                  " 1:fs:2"
        , nodes : "foo"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , match_variant : "foo"
        , pattern : (char[]) { "${?+bar}\0" }
        , graph :  ""
                  "1-MOD!dir 2-bar.foo!U|file"
                  " 1:fs:2"
        , nodes : "bar.foo"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , match_variant : "foo"
        , pattern : (char[]) { "${?+bar+baz}\0" }
        , graph :  ""
                  "1-MOD!dir 2-bar.baz.foo!U|file"
                  " 1:fs:2"
        , nodes : "bar.baz.foo"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , match_variant : "foo"
        , pattern : (char[]) { "${?+bar-foo}\0" }
        , graph :  ""
                  "1-MOD!dir 2-bar!U|file"
                  " 1:fs:2"
        , nodes : "bar"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , match_variant : "void.xunit.final"
        , pattern : (char[]) { "foo.${?+pic}\0" }
        , graph :  ""
                  "1-MOD!dir 2-foo.final.pic.void.xunit!U|file"
                  " 1:fs:2"
        , nodes : "foo.final.pic.void.xunit"
      }}

    /* under the shadow fs */
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , modules_shadow : (char*[]) { "MOD", "beef", 0 }
        , root : true
        , module : "MOD"
        , pattern : (char[]) { "//tests\0" }
        , graph :  "1-fs!shadow-link 2-modules!shadow-modules 3-module!shadow-module"
                  " 4-(shadow)!shadow-dir 5-beef!shadow-dir"
                  " 6-imports!shadow-dir 7-requires!shadow-dir 8-uses!shadow-dir"
                  " 9-targets!shadow-file 10-tests!shadow-file"
                  " 11-(root)!dir"
                  " 12-MOD!file"
                  " 2:fs:5"
                  " 4:fs:2 4:fs:3 4:fs:10"
                  " 5:fs:1 5:fs:6 5:fs:7 5:fs:8 5:fs:9"
                  " 11:fs:12"
        , nodes : "tests"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , modules_shadow : (char*[]) { "MOD", "beef", 0 }
        , root : true
        , module : "MOD"
        , pattern : (char[]) { "//tests/fast\0" }
        , graph :  "1-fs!shadow-link 2-modules!shadow-modules 3-module!shadow-module"
                  " 4-(shadow)!shadow-dir 5-beef!shadow-dir"
                  " 6-imports!shadow-dir 7-requires!shadow-dir"
                  " 8-tests!shadow-dir"
                  " 9-uses!shadow-dir"
                  " 10-fast!shadow-file"
                  " 11-targets!shadow-file"
                  " 12-(root)!dir 13-MOD!file"
                  " 2:fs:5"
                  " 4:fs:2 4:fs:3 4:fs:8"
                  " 5:fs:1 5:fs:6 5:fs:7 5:fs:9 5:fs:11"
                  " 8:fs:10"
                  " 12:fs:13"
        , nodes : "fast"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , modules_shadow : (char*[]) { "MOD", "beef", 0 }
        , root : true
        , module : "MOD"
        /* generated under the module directory node because of //module/fs */
        , pattern : (char[]) { "//module/fs/tests\0" }
        , graph :  "1-fs!shadow-link 2-modules!shadow-modules 3-module!shadow-module"
                  " 4-(shadow)!shadow-dir 5-beef!shadow-dir"
                  " 6-imports!shadow-dir 7-requires!shadow-dir 8-uses!shadow-dir"
                  " 9-targets!shadow-file"
                  " 10-(root)!dir 11-MOD!dir 12-tests!U|file"
                  " 2:fs:5"
                  " 4:fs:2 4:fs:3"
                  " 5:fs:1 5:fs:6 5:fs:7 5:fs:8 5:fs:9"
                  " 10:fs:11 11:fs:12"
        , nodes : "tests"
      }}

    /* shadow-requires */
    , (pattern_generate_test[]) {{
          operations :
            " mods/A"
            " mods/B"
        , modules_shadow : (char*[]) { "A", "00abcd", "B", "00xyzw", 0 }
        , modules_requires : (char*[]) { "A", "B", 0 }
        , root : true
        , module : "A"
        , pattern : (char[]) { "//module/imports/B/goats\0" }
        , graph :  "1-B!shadow-link 2-fs!shadow-link 3-fs!shadow-link 4-modules!shadow-modules 5-module!shadow-module"
                  " 6-(shadow)!shadow-dir 7-00abcd!shadow-dir 8-00xyzw!shadow-dir"
                  " 9-B!shadow-dir"
                  " 10-imports!shadow-dir 11-imports!shadow-dir 12-requires!shadow-dir 13-requires!shadow-dir 14-uses!shadow-dir 15-uses!shadow-dir"
                  " 16-goats!shadow-file"
                  " 17-targets!shadow-file 18-targets!shadow-file"
                  " 19-(root)!dir 20-mods!dir 21-A!file 22-B!file"
                  " 4:fs:7 4:fs:8"
                  " 6:fs:4 6:fs:5"
                  " 7:fs:2 7:fs:10 7:fs:12 7:fs:14 7:fs:17"
                  " 8:fs:3 8:fs:11 8:fs:13 8:fs:15 8:fs:18"
                  " 9:fs:16"
                  " 10:fs:9 12:fs:1 19:fs:20 20:fs:21 20:fs:22"
        , nodes : "goats"
      }}
    , 0
  }
};
