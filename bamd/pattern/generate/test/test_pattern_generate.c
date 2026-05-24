/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "xunit.h"
#include "xunit/assert.h"

#include "xlinux/xstdlib.h"
#include "moria/parser.h"
#include "narrator/growing.h"
#include "bacon/parser.h"
#include "bacon/load.h"

#include "generate.internal.h"
#include "search.internal.h"
#include "pattern_parser.internal.h"
#include "pattern.h"
#include "fsent.h"
#include "shadow.h"
#include "module.h"
#include "node_operations_test.h"
#include "fstree.h"
#include "args.h"
#include "vertices.h"

typedef struct pattern_generate_test {
  XUNITTEST;

  // setup
  char *operations;
  char **modules_shadow;   // module node -> id list

  // context
  char *module;
  char *match;
  char **groups;
  char *pattern;
  char *mod_vars;
  char *vars;

  // results
  char *graph;
  char *nodes;
} pattern_generate_test;

static void pattern_test_unit_setup(xunit_unit * unit)
{
  bacon_load();

  args_init();
  fsent_root_setup();
  shadow_root_setup();
  fstree_root_setup();
  shadow_setup();
  fstree_setup();
  graph_setup();

  extern __thread bool rcu_is_reader ; rcu_is_reader = true;
}

static void pattern_test_unit_cleanup(xunit_unit * unit)
{
  bacon_unload();

  shadow_cleanup();
  fstree_cleanup();
  module_cleanup();
  fsent_cleanup();
  graph_cleanup();
}

static int node_compare(const void * _A, const void * _B)
{
  const fsent * A = *(const fsent **)_A;
  const fsent * B = *(const fsent **)_B;

  return memncmp(A->name.name, A->name.namel, B->name.name, B->name.namel);
}

static void generate_hit(void *ctxp, const struct moria_vertex *hit)
{
  vertices *hits = ctxp;
  vertices_push(hits, hit);
}

//
// tests
//

static void pattern_generate_test_entry(xunit_test * _test)
{
  pattern_generate_test * test = (pattern_generate_test *)_test;

  narrator_growing *N = 0;
  pattern_parser *parser = 0;
  generate_pattern *pattern = 0;
  vertices hits, *eln;
  fsent **node_list = 0;
  size_t node_list_len = 0;
  module mods[4] = { 0 };
  module *mod;
  yyu_location loc;
  moria_parser *op_parser = 0;
  pattern_search_hit match = { 0 };
  int x;
  moria_vertex *v;
  graph_invalidation_context invalidation = { 0 };
  const char *graph;
  size_t graph_len;
  const char *nodes;
  size_t nodes_len;
  fsent *n;
  bacon_value *mod_vars, *vars;
  bacon_parser *val_parser;

  // build out the initial graph
  moria_parser_create(&op_parser, &g_graph, (llist *[]) { &fsent_list }, 1, node_operations_test_dispatch, graph_vertex_attrs, graph_edge_attrs);
  moria_parser_operations_parse(op_parser, MMS(test->operations));

  // setup the shadow fs
  if(test->modules_shadow)
  {
    shadow_setup();

    for(x = 0; x < sentinel(test->modules_shadow); x += 2)
    {
      mods[x / 2].build_dir = resolve_fragment(MMS(test->modules_shadow[x]));
      mods[x/2].anchor_dir = mods[x / 2].build_dir;
      moria_vertex_init(&mods[x/2].vertex, &g_graph, VERTEX_MODULE | VERTEX_OK);
      snprintf(mods[x / 2].id, sizeof(mods[x / 2].id), "%s", test->modules_shadow[x + 1]);
      mods[x/2].vertex.label = test->modules_shadow[x];
      mods[x/2].vertex.label_len = strlen(test->modules_shadow[x]);
    }

    for(x = 0; x < sizeof(mods) / sizeof(*mods); x++)
    {
      if(strcmp(mods[x].build_dir->name.name, test->module) == 0)
      {
        mod = &mods[x];
        break;
      }
    }
  }

  /* setup the module */
  if(!test->modules_shadow)
  {
    mods[0].build_dir = resolve_fragment(MMS(test->module));
    mods[0].anchor_dir = mods[0].build_dir;
    mod = &mods[0];
  }

  bacon_parser_alloc(&val_parser);
  mod_vars = 0;
  if(test->mod_vars) {
    bacon_parser_parse(val_parser, MMS(test->mod_vars), "-mod_vars-", BACON_TYPE_SET, &mod_vars);
  }
  mod->vars = mod_vars;

  vars = 0;
  if(test->vars) {
    bacon_parser_parse(val_parser, MMS(test->vars), "-vars-", BACON_TYPE_SET, &vars);
  }

  narrator_growing_create(&N);

  if(test->match) {
    match.node = resolve_fragment(MMS(test->match));
  }

  if(test->groups)
  {
    match.group_max = sentinel(test->groups) - 1;
    for(x = 0; x <= match.group_max; x++)
    {
      match.groups[x].start = test->groups[x];
      match.groups[x].len = strlen(test->groups[x]);
    }
  }

  // parse the pattern
  pattern_parser_create(&parser);
  if(generate_pattern_parse_partial(parser, test->pattern, strlen(test->pattern) + 2, "-test-", 0, &loc, &pattern) != 0) {
    tracef("%.*s", (int)parser->yyu->error_len, parser->yyu->error_str);
  }
  assert_eq_u32(strlen(test->pattern), loc.l);

  // act
  vertices_init(&hits);
  graph_invalidation_begin(&invalidation);
  pattern_generate(
      pattern
    , mod
    , vars
    , val_parser
    , mod->build_dir
    , &invalidation
    , &match
    , 0
    , generate_hit
    , &hits
  );
  graph_invalidation_end(&invalidation);

  // ordered list of vertices and edges
  narrator_reset(&N->base);
  graph_say(&N->base);
  graph = N->s;
  graph_len = N->l;
  assert_eq_s(test->graph, graph);
  assert_eq_w(test->graph, strlen(test->graph), graph, graph_len);

  // ordered list of nodes
  narrator_reset(&N->base);

  xmalloc(&node_list, sizeof(*node_list) * llist_count(&hits.lln));
  vertices_foreach(&hits, v, eln) {
    n = containerof(v, typeof(*n), vertex);
    node_list[node_list_len++] = n;
  }
  qsort(node_list, node_list_len, sizeof(*node_list), (void*)node_compare);

  for(x = 0; x < node_list_len; x++)
  {
    n = node_list[x];

    if(x)
      narrator_says(&N->base, " ");
    narrator_says(&N->base, n->name.name);
  }
  nodes = N->s;
  nodes_len = N->l;
  assert_eq_w(test->nodes, strlen(test->nodes), nodes, nodes_len);

  free(node_list);
  pattern_free(&pattern->pattern);
  fsent_cleanup();
  narrator_growing_free(N);
  pattern_parser_free(parser);
  vertices_destroy(&hits);
  moria_parser_free(op_parser);
  bacon_value_free(val_parser, mod_vars);
  bacon_value_free(val_parser, vars);
  bacon_parser_free(val_parser);
}

//
// manifest
//

static xunit_unit xunit = {
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
                  "1-(root)!dir 2-MOD!dir 3-b!U|file"
                  " 2:fs:1 3:fs:2"
        , nodes : "b"
      }}
    , (pattern_generate_test[]) {{
          operations : "amod"
        , module : "amod"
        , pattern : (char[]) { "b/c\0" }
        , graph :
                  "1-(root)!dir 2-amod!dir 3-b!U|dir 4-c!U|file"
                  " 2:fs:1 3:fs:2 4:fs:3"
        , nodes : "c"
      }}

    /* absolute and relative paths */
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "./b\0" }
        , graph :
                  "1-(root)!dir 2-MOD!dir 3-b!U|file"
                  " 2:fs:1 3:fs:2"
        , nodes : "b"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "b.{c,h}\0" }
        , graph :
                   "1-(root)!dir 2-MOD!dir 3-b.c!U|file 4-b.h!U|file"
                  " 2:fs:1 3:fs:2 4:fs:2"
        , nodes : "b.c b.h"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "././b\0" }
        , graph :
                  "1-(root)!dir 2-MOD!dir 3-b!U|file"
                  " 2:fs:1 3:fs:2"
        , nodes : "b"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "a/./b\0" }
        , graph :
                  "1-(root)!dir 2-MOD!dir 3-a!U|dir 4-b!U|file"
                  " 2:fs:1 3:fs:2 4:fs:3"
        , nodes : "b"
      }}

    /* alternations */
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "{b}\0" }
        , graph :
                  "1-(root)!dir 2-MOD!dir 3-b!U|file"
                  " 2:fs:1 3:fs:2"
        , nodes : "b"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "{{b}}\0" }
        , graph :
                  "1-(root)!dir 2-MOD!dir 3-b!U|file"
                  " 2:fs:1 3:fs:2"
        , nodes : "b"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "{{{b}}}\0" }
        , graph :
                  "1-(root)!dir 2-MOD!dir 3-b!U|file"
                  " 2:fs:1 3:fs:2"
        , nodes : "b"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "{b,}\0" }
        , graph :
                  "1-(root)!dir 2-MOD!dir 3-b!U|file"
                  " 2:fs:1 3:fs:2"
        , nodes : "b"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "{b,c}\0" }
        , graph :
                  "1-(root)!dir 2-MOD!dir 3-b!U|file 4-c!U|file"
                  " 2:fs:1 3:fs:2 4:fs:2"
        , nodes : "b c"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "{{b,c}}\0" }
        , graph :
                  "1-(root)!dir 2-MOD!dir 3-b!U|file 4-c!U|file"
                  " 2:fs:1 3:fs:2 4:fs:2"
        , nodes : "b c"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "{b,c,d}\0" }
        , graph :
                  "1-(root)!dir 2-MOD!dir 3-b!U|file 4-c!U|file 5-d!U|file"
                  " 2:fs:1 3:fs:2 4:fs:2 5:fs:2"
        , nodes : "b c d"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "b/{c,d}\0" }
        , graph :
                  "1-(root)!dir 2-MOD!dir 3-b!U|dir 4-c!U|file 5-d!U|file"
                  " 2:fs:1 3:fs:2 4:fs:3 5:fs:3"
        , nodes : "c d"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "b/{c,}\0" }
        , graph :
                  "1-(root)!dir 2-MOD!dir 3-b!U|dir 4-c!U|file"
                  " 2:fs:1 3:fs:2 4:fs:3"
        , nodes : "c"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "{c,d}/e\0" }
        , graph :
                  "1-(root)!dir 2-MOD!dir 3-c!U|dir 4-d!U|dir 5-e!U|file 6-e!U|file"
                  " 2:fs:1 3:fs:2 4:fs:2 5:fs:3 6:fs:4"
        , nodes : "e e"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "a{c,d}/xyz\0" }
        , graph :
                  "1-(root)!dir 2-MOD!dir 3-ac!U|dir 4-ad!U|dir 5-xyz!U|file 6-xyz!U|file"
                  " 2:fs:1 3:fs:2 4:fs:2 5:fs:3 6:fs:4"
        , nodes : "xyz xyz"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "b/{c,d}/e\0" }
        , graph :
                  "1-(root)!dir 2-MOD!dir 3-b!U|dir 4-c!U|dir 5-d!U|dir 6-e!U|file 7-e!U|file"
                  " 2:fs:1 3:fs:2 4:fs:3 5:fs:3 6:fs:4 7:fs:5"
        , nodes : "e e"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "{b,c}/{d,e}\0" }
        , graph :
                  "1-(root)!dir 2-MOD!dir 3-b!U|dir 4-c!U|dir 5-d!U|file 6-d!U|file 7-e!U|file 8-e!U|file"
                  " 2:fs:1 3:fs:2 4:fs:2 5:fs:3 7:fs:3 6:fs:4 8:fs:4"
        , nodes : "d d e e"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "b{c,d}\0" }
        , graph :
                  "1-(root)!dir 2-MOD!dir 3-bc!U|file 4-bd!U|file"
                  " 2:fs:1 3:fs:2 4:fs:2"
        , nodes : "bc bd"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "b{{c,d}}\0" }
        , graph :
                  "1-(root)!dir 2-MOD!dir 3-bc!U|file 4-bd!U|file"
                  " 2:fs:1 3:fs:2 4:fs:2"
        , nodes : "bc bd"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "b{{{c,d}}}\0" }
        , graph :
                  "1-(root)!dir 2-MOD!dir 3-bc!U|file 4-bd!U|file"
                  " 2:fs:1 3:fs:2 4:fs:2"
        , nodes : "bc bd"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "{c,d}b\0" }
        , graph :
                  "1-(root)!dir 2-MOD!dir 3-cb!U|file 4-db!U|file"
                  " 2:fs:1 3:fs:2 4:fs:2"
        , nodes : "cb db"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "{{c,d}}b\0" }
        , graph :
                  "1-(root)!dir 2-MOD!dir 3-cb!U|file 4-db!U|file"
                  " 2:fs:1 3:fs:2 4:fs:2"
        , nodes : "cb db"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "{{{c,d}}}b\0" }
        , graph :
                  "1-(root)!dir 2-MOD!dir 3-cb!U|file 4-db!U|file"
                  " 2:fs:1 3:fs:2 4:fs:2"
        , nodes : "cb db"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "b{c,d,}\0" }
        , graph :
                  "1-(root)!dir 2-MOD!dir 3-b!U|file 4-bc!U|file 5-bd!U|file"
                  " 2:fs:1 3:fs:2 4:fs:2 5:fs:2"
        , nodes : "b bc bd"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "{b}{c}\0" }
        , graph :
                  "1-(root)!dir 2-MOD!dir 3-bc!U|file"
                  " 2:fs:1 3:fs:2"
        , nodes : "bc"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "{b,c}{d}\0" }
        , graph :
                  "1-(root)!dir 2-MOD!dir 3-bd!U|file 4-cd!U|file"
                  " 2:fs:1 3:fs:2 4:fs:2"
        , nodes : "bd cd"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "{b}{c,d}\0" }
        , graph :
                  "1-(root)!dir 2-MOD!dir 3-bc!U|file 4-bd!U|file"
                  " 2:fs:1 3:fs:2 4:fs:2"
        , nodes : "bc bd"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "{b,c}{d,e}\0" }
        , graph :
                  "1-(root)!dir 2-MOD!dir 3-bd!U|file 4-be!U|file 5-cd!U|file 6-ce!U|file"
                  " 2:fs:1 3:fs:2 4:fs:2 5:fs:2 6:fs:2"
        , nodes : "bd be cd ce"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "{b,}{d,}\0" }
        , graph :
                  "1-(root)!dir 2-MOD!dir 3-b!U|file 4-bd!U|file 5-d!U|file"
                  " 2:fs:1 3:fs:2 4:fs:2 5:fs:2"
        , nodes : "b bd d"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "{x{b,c}}\0" }
        , graph :
                  "1-(root)!dir 2-MOD!dir 3-xb!U|file 4-xc!U|file"
                  " 2:fs:1 3:fs:2 4:fs:2"
        , nodes : "xb xc"
      }}

    /* classes */
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "x[bc]\0" }
        , graph :
                  "1-(root)!dir 2-MOD!dir 3-xb!U|file 4-xc!U|file"
                  " 2:fs:1 3:fs:2 4:fs:2"
        , nodes : "xb xc"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "[bc]x\0" }
        , graph :
                  "1-(root)!dir 2-MOD!dir 3-bx!U|file 4-cx!U|file"
                  " 2:fs:1 3:fs:2 4:fs:2"
        , nodes : "bx cx"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "x[bc]y\0" }
        , graph :
                  "1-(root)!dir 2-MOD!dir 3-xby!U|file 4-xcy!U|file"
                  " 2:fs:1 3:fs:2 4:fs:2"
        , nodes : "xby xcy"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "a[bc]/xyz\0" }
        , graph :
                  "1-(root)!dir 2-MOD!dir 3-ab!U|dir 4-ac!U|dir 5-xyz!U|file 6-xyz!U|file"
                  " 2:fs:1 3:fs:2 4:fs:2 5:fs:3 6:fs:4"
        , nodes : "xyz xyz"
      }}

    /* ranges */
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "[b-e]\0" }
        , graph :
                  "1-(root)!dir 2-MOD!dir 3-b!U|file 4-c!U|file 5-d!U|file 6-e!U|file"
                  " 2:fs:1 3:fs:2 4:fs:2 5:fs:2 6:fs:2"
        , nodes : "b c d e"
      }}

    /* nested alternations and classes */
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "{[b-e]}\0" }
        , graph :
                  "1-(root)!dir 2-MOD!dir 3-b!U|file 4-c!U|file 5-d!U|file 6-e!U|file"
                  " 2:fs:1 3:fs:2 4:fs:2 5:fs:2 6:fs:2"
        , nodes : "b c d e"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "{x[b-d]}\0" }
        , graph :
                  "1-(root)!dir 2-MOD!dir 3-xb!U|file 4-xc!U|file 5-xd!U|file"
                  " 2:fs:1 3:fs:2 4:fs:2 5:fs:2"
        , nodes : "xb xc xd"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "{[b-d]y}\0" }
        , graph :
                  "1-(root)!dir 2-MOD!dir 3-by!U|file 4-cy!U|file 5-dy!U|file"
                  " 2:fs:1 3:fs:2 4:fs:2 5:fs:2"
        , nodes : "by cy dy"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "x{[bc]}\0" }
        , graph :
                  "1-(root)!dir 2-MOD!dir 3-xb!U|file 4-xc!U|file"
                  " 2:fs:1 3:fs:2 4:fs:2"
        , nodes : "xb xc"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "{[bc]}y\0" }
        , graph :
                  "1-(root)!dir 2-MOD!dir 3-by!U|file 4-cy!U|file"
                  " 2:fs:1 3:fs:2 4:fs:2"
        , nodes : "by cy"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "x{[bc]}y\0" }
        , graph :
                  "1-(root)!dir 2-MOD!dir 3-xby!U|file 4-xcy!U|file"
                  " 2:fs:1 3:fs:2 4:fs:2"
        , nodes : "xby xcy"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , pattern : (char[]) { "x{{,[bc]}}y\0" }
        , graph :
                  "1-(root)!dir 2-MOD!dir 3-xby!U|file 4-xcy!U|file 5-xy!U|file"
                  " 2:fs:1 3:fs:2 4:fs:2 5:fs:2"
        , nodes : "xby xcy xy"
      }}

    /* axes */

    /* replacements - matching group references */
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , groups : (char*[]) { "", "foo", 0 }
        , pattern : (char[]) { "$1\0" }
        , graph :
                  "1-(root)!dir 2-MOD!dir 3-foo!U|file"
                  " 2:fs:1 3:fs:2"
        , nodes : "foo"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , module : "MOD"
        , groups : (char*[]) { "", "foo", 0 }
        , pattern : (char[]) { "bar/$1\0" }
        , graph :
                  "1-(root)!dir 2-MOD!dir 3-bar!U|dir 4-foo!U|file"
                  " 2:fs:1 3:fs:2 4:fs:3"
        , nodes : "foo"
      }}
    , (pattern_generate_test[]) {{
          operations : ""
          " MOD/bar/baz"
        , module : "MOD"
        , groups : (char*[]) { "", "foo", 0 }
        , match : "baz"
        , pattern : (char[]) { "$^D/$1.o\0" }
        , graph :
                  "1-(root)!dir 2-MOD!dir 3-bar!dir 4-baz!file 5-foo.o!U|file"
                  " 2:fs:1 3:fs:2 4:fs:3 5:fs:3"
        , nodes : "foo.o"
      }}
    /* replacements - builtin variables */
    , (pattern_generate_test[]) {{
          operations : ""
          " MOD/bar/baz"
        , module : "MOD"
        , match : "baz"
        , pattern : (char[]) { "$^D/foo\0" }
        , graph :
                  "1-(root)!dir 2-MOD!dir 3-bar!dir 4-baz!file 5-foo!U|file"
                  " 2:fs:1 3:fs:2 4:fs:3 5:fs:3"
        , nodes : "foo"
      }}
    , (pattern_generate_test[]) {{
          operations : ""
          " MOD/bar/baz"
        , module : "MOD"
        , match : "baz"
        , pattern : (char[]) { "$bm_build_rel/foo\0" }
        , graph :
                  "1-(root)!dir 2-MOD!dir 3-bar!dir 4-baz!file 5-foo!U|file"
                  " 2:fs:1 3:fs:2 4:fs:3 5:fs:3"
        , nodes : "foo"
      }}
    , (pattern_generate_test[]) {{
          operations : ""
          " MOD/bar/baz"
        , module : "MOD"
        , match : "baz"
        , pattern : (char[]) { "$bm_build/foo\0" }
        , graph :
                  "1-(root)!dir 2-MOD!dir 3-bar!dir 4-baz!file 5-foo!U|file"
                  " 2:fs:1 3:fs:2 5:fs:2 4:fs:3"
        , nodes : "foo"
      }}
    /* replacements - variables */
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , mod_vars : "a : b"
        , module : "MOD"
        , groups : (char*[]) { "", "foo", 0 }
        , pattern : (char[]) { "$a\0" }
        , graph :
                  "1-(root)!dir 2-MOD!dir 3-b!U|file"
                  " 2:fs:1 3:fs:2"
        , nodes : "b"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , mod_vars : "a : b"
        , module : "MOD"
        , groups : (char*[]) { "", "foo", 0 }
        , pattern : (char[]) { "a$a\0" }
        , graph :
                  "1-(root)!dir 2-MOD!dir 3-ab!U|file"
                  " 2:fs:1 3:fs:2"
        , nodes : "ab"
      }}
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , mod_vars : "a : b"
        , module : "MOD"
        , groups : (char*[]) { "", "foo", 0 }
        , pattern : (char[]) { "${a}a\0" }
        , graph :
                  "1-(root)!dir 2-MOD!dir 3-ba!U|file"
                  " 2:fs:1 3:fs:2"
        , nodes : "ba"
      }}
    /* replacements - segment sysvars */
    , (pattern_generate_test[]) {{
          operations : "MOD"
        , vars : "bm_bamd_pid : 1234"
        , module : "MOD"
        , pattern : (char[]) { "${bm_bamd_pid}a\0" }
        , graph :
                  "1-(root)!dir 2-MOD!dir 3-1234a!U|file"
                  " 2:fs:1 3:fs:2"
        , nodes : "1234a"
      }}
    , 0
  }
};
XUNIT_UNIT(xunit);
