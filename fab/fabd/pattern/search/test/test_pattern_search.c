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

#include "xlinux/xstdlib.h"
#include "yyutil/load.h"
#include "moria/graph.h"
#include "moria/vertex.h"
#include "moria/operations.h"
#include "moria/parser.h"
#include "valyria/list.h"
#include "valyria/map.h"
#include "valyria/set.h"
#include "valyria/assert.h"
#include "narrator.h"
#include "narrator/growing.h"

#include "search.internal.h"
#include "pattern_parser.h"
#include "lookup.h"
#include "pattern.h"
#include "logging.h"
#include "fsent.h"
#include "shadow.h"
#include "module.internal.h"
#include "variant.h"
#include "filesystem.h"
#include "node_operations_test.h"

#include "common/hash.h"
#include "zbuffer.h"

typedef struct pattern_search_test {
  XUNITTEST;

  // setup
  char * operations;
  char ** modules_shadow;   // module node -> id list
  char ** modules_imports;  // module A : requires : module B list
  char * module_shadow;     // module shadow id

  // context
  char * pattern;
  char * module;        // name for module node
  char ** variants;

  // results
  char ** matches;  // space separated, 1, 2, etc
} pattern_search_test;

static xapi pattern_search_test_unit_setup(xunit_unit * unit)
{
  enter;

  fatal(yyutil_load);
  fatal(logging_finalize);
  fatal(filesystem_setup);

  // arrange
  fatal(graph_setup);
  fatal(fsent_setup);
  fatal(shadow_setup);
  fatal(variant_setup);

  finally : coda;
}

static xapi pattern_search_test_unit_cleanup(xunit_unit * unit)
{
  enter;

  fatal(filesystem_cleanup);

  fatal(fsent_cleanup);
  fatal(variant_cleanup);

  finally : coda;
}

//
// tests
//

static xapi pattern_search_test_entry(xunit_test * _test)
{
  enter;

  pattern_search_test * test = (pattern_search_test *)_test;

  map * node_map = 0;
  pattern_parser * parser = 0;
  pattern * pattern = 0;
  list * nodes_list = 0;
  graph_parser * op_parser = 0;
  set * matches = 0;
  module mods[4] = { 0 };
  module *mod;
  set * expected = 0;
  set * actual = 0;
  int x;
  int y;
  char path[512];
  char * actual_match = 0;
  yyu_location loc;
  set * variants = 0;
  llist modules;
  variant *v = 0;
  fsent *na, *nb;
  graph_invalidation_context invalidation = { 0 };

  // setup initial graph
  fatal(graph_parser_create, &op_parser, &g_graph, &fsent_list, node_operations_test_dispatch, graph_vertex_attrs, graph_edge_attrs);
  fatal(graph_parser_operations_parse, op_parser, MMS(test->operations));

//fatal(graph_say, g_narrator_stdout);
//printf("\n");

  // setup the shadow fs
  if(test->modules_shadow)
  {
    fatal(shadow_setup);
    fatal(graph_invalidation_begin, &invalidation);

    for(x = 0; x < sentinel(test->modules_shadow); x += 2)
    {
      fatal(resolve_fragment, MMS(test->modules_shadow[x]), &mods[x / 2].dir_node);
      moria_vertex_init(&mods[x/2].vertex, &g_graph, VERTEX_MODULE);
      snprintf(mods[x / 2].id, sizeof(mods[x / 2].id), "%s", test->modules_shadow[x + 1]);
      fatal(shadow_module_init, &mods[x / 2], &invalidation);
      mods[x/2].vertex.label = test->modules_shadow[x];
      mods[x/2].vertex.label_len = strlen(test->modules_shadow[x]);
      mods[x / 2].dir_node->mod = &mods[x / 2];
    }

    if(test->modules_imports)
    {
      for(x = 0; x < sentinel(test->modules_imports); x += 2)
      {
        fatal(resolve_fragment, MMS(test->modules_imports[x + 0]), &na);
        fatal(resolve_fragment, MMS(test->modules_imports[x + 1]), &nb);
//        fatal(shadow_graft_imports, na->mod, nb, nb->name.name, nb->name.namel, 0, &invalidation);
        fatal(module_resolve_import, na->mod, nb, nb->name.name, nb->name.namel, 0, &invalidation);
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
    fatal(resolve_fragment, MMS(test->module), &mods[0].dir_node);
    mod = &mods[0];
  }

  fatal(map_create, &node_map);
  fatal(list_create, &nodes_list);

  fatal(set_create, &variants);
  if(test->variants)
  {
    for(x = 0; x < sentinel(test->variants); x++)
    {
      fatal(variant_get, test->variants[x], strlen(test->variants[x]), &v);
      fatal(set_put, variants, v, 0);
    }
  }

  llist_init_node(&modules);

  // parse the pattern
  fatal(pattern_parser_create, &parser);
  fatal(search_pattern_parse_partial, parser, test->pattern, strlen(test->pattern) + 2, "-test-", 0, &loc, &pattern);
  assert_eq_u32(strlen(test->pattern), loc.l);

  // act
  fatal(pattern_search_matches_create, &matches);
  fatal(pattern_search, pattern, mod, &modules, variants, matches, 0, 0);

  // assert
  fatal(set_createx, &expected, 0, hash32, memncmp, 0, 0);
  for(x = 0; x < sentinel(test->matches); x++)
    fatal(set_put, expected, MMS(test->matches[x]));

  fatal(set_createx, &actual, 0, hash32, memncmp, wfree, 0);
  for(x = 0; x < matches->table_size; x++)
  {
    pattern_search_node * m;
    if(!(m = set_table_get(matches, x)))
      continue;

    size_t pz = fsent_absolute_path_znload(path, sizeof(path), m->node);
    size_t sz = pz;

    for(y = 0; y <= m->group_max; y++)
    {
      if(!m->groups[y].start)
        continue;

      if(y <= 9) { sz += 3; }
      else { sz += 4; }

      sz += m->groups[y].len;
    }

    if(m->variant)
      sz += m->variant->norm_len + 3;

    sz += 1;
    fatal(xmalloc, &actual_match, sz);

    size_t z = 0;
    z += znloadw(actual_match + z, sz - z, path, pz);

    for(y = 1; y <= m->group_max; y++)
    {
      if(!m->groups[y].start)
        continue;

      z += znloadf(actual_match + z, sz - z, " %d ", y);
      z += znloadw(actual_match + z, sz - z, m->groups[y].start, m->groups[y].len);
    }

    if(m->variant)
      z += znloadf(actual_match + z, sz - z, " ? %.*s", m->variant->norm_len, m->variant->norm);

    fatal(set_put, actual, actual_match, z);
    actual_match = 0;
  }

  assert_eq_set(expected, actual);

finally:
  fatal(set_xfree, variants);
  fatal(set_xfree, matches);
  fatal(map_xfree, node_map);
  fatal(pattern_parser_xfree, parser);
  pattern_free(pattern);
  fatal(list_xfree, nodes_list);
  fatal(graph_parser_xfree, op_parser);
  fatal(set_xfree, expected);
  fatal(set_xfree, actual);
  wfree(actual_match);
  graph_invalidation_end(&invalidation);
coda;
}

//
// manifest
//

xunit_unit xunit = {
    xu_setup : pattern_search_test_unit_setup
  , xu_cleanup : pattern_search_test_unit_cleanup
  , xu_entry : pattern_search_test_entry
  , xu_tests : (pattern_search_test*[]) {
    /* single word */
      (pattern_search_test[]) {{
          operations : "dir/bar"
        , module : "dir"
        , pattern : (char[]) { "bar\0" }
        , matches : (char*[]) {
              "/dir/bar"
            , 0
          }
      }}

    /* words, characters */
    , (pattern_search_test[]) {{
          operations : ""
            " mod/foo"
        , module : "mod"
        , pattern : (char[]) { "foo.a\0" }
        , matches : (char*[]) {
              0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/foo.a"
        , module : "mod"
        , pattern : (char[]) { "foo\0" }
        , matches : (char*[]) {
              0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/foo.a"
            " mod/foo.b"
        , module : "mod"
        , pattern : (char[]) { "foo.b\0" }
        , matches : (char*[]) {
              "/mod/foo.b"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/foo.b"
            " mod/foo.b.bar"
        , module : "mod"
        , pattern : (char[]) { "foo.b.bar\0" }
        , matches : (char*[]) {
              "/mod/foo.b.bar"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : "A.B/C/D"
        , module : "A.B"
        , pattern : (char[]) { "C/D\0" }
        , matches : (char*[]) {
              "/A.B/C/D"
            , 0
          }
      }}

    /* multi directory */
    , (pattern_search_test[]) {{
          operations : "dir/foo/bar"
        , module : "dir"
        , pattern : (char[]) { "foo/bar\0" }
        , matches : (char*[]) {
              "/dir/foo/bar"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : "mod/A/B"
        , module : "A"
        , pattern : (char[]) { "main.o\0" }
        , matches : (char*[]) { 0 }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/foo/abcd/bar/a"
            " mod/foo/abcd/bar/gondor/valyria/b"
            " mod/foo/abcd/baz"
            " mod/foo/abcd/oof/a"
            " mod/foo/abcd/oof/b"
            " mod/foo/abcd/qux/a"
            " mod/foo/abcd/qux/b"
        , module : "mod"
        , pattern : (char[]) { "foo/abcd/oof/b\0" }
        , matches : (char*[]) {
              "/mod/foo/abcd/oof/b"
            , 0
          }
      }}

    /* dentry section with consecutive words */
    , (pattern_search_test[]) {{
          operations : "mod/BCD"
        , module : "mod"
        , pattern : (char[]) { "B\"C\"D\0" }
        , matches : (char*[]) {
              "/mod/BCD"
            , 0
          }
      }}

    /* alternations */
    , (pattern_search_test[]) {{
          operations : ""
            " mod/A"
            " mod/B"
        , module : "mod"
        , pattern : (char[]) { "{A,B}\0" }
        , matches : (char*[]) {
              "/mod/A"
            , "/mod/B"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/A"
            " mod/C"
        , module : "mod"
        , pattern : (char[]) { "{A,B}\0" }
        , matches : (char*[]) {
              "/mod/A"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/AC"
        , module : "mod"
        , pattern : (char[]) { "{A,B}{C,D}\0" }
        , matches : (char*[]) {
              "/mod/AC"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/A"
            " mod/B"
            " mod/C"
        , module : "mod"
        , pattern : (char[]) { "{A,{B}}\0" }
        , matches : (char*[]) {
              "/mod/A"
            , "/mod/B"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/A/BCD/E"
        , module : "mod"
        , pattern : (char[]) { "A/B{X,C,Y}D/E\0" }
        , matches : (char*[]) {
              "/mod/A/BCD/E"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/A/BCD/E"
            " mod/A/BXD/E"
            " mod/A/BYD/E"
            " mod/A/B"
            " mod/A/BX"
            " mod/A/BC"
            " mod/BCD/E"
        , module : "mod"
        , pattern : (char[]) { "A/B{X,C,Y}D/E\0" }
        , matches : (char*[]) {
              "/mod/A/BCD/E"
            , "/mod/A/BXD/E"
            , "/mod/A/BYD/E"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/A/BCD/E"
        , module : "mod"
        , pattern : (char[]) { "A/B{{X,C},Y}D/E\0" }
        , matches : (char*[]) {
              "/mod/A/BCD/E"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/A/BCD/E"
            " mod/A/BYD/E"
            " mod/A/BCD/e"
            " mod/A/aBCD/E"
        , module : "mod"
        , pattern : (char[]) { "A/B{{X,C},Y}D/E\0" }
        , matches : (char*[]) {
              "/mod/A/BCD/E"
            , "/mod/A/BYD/E"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/A/BCD/E"
        , module : "mod"
        , pattern : (char[]) { "A/B{X,{C,Y}}D/E\0" }
        , matches : (char*[]) {
              "/mod/A/BCD/E"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/A/E"
            " mod/X/E"
        , module : "mod"
        , pattern : (char[]) { "{A.B,X}/E\0" }
        , matches : (char*[]) {
              "/mod/X/E"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/A/E"
            " mod/X/E"
        , module : "mod"
        , pattern : (char[]) { "{A.B,{X}}/E\0" }
        , matches : (char*[]) {
              "/mod/X/E"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/A/E"
        , module : "mod"
        , pattern : (char[]) { "{A.B,X}/E\0" }
        , matches : (char*[]) {
              0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/A/E"
        , module : "mod"
        , pattern : (char[]) { "{{A.B,X}}/E\0" }
        , matches : (char*[]) {
              0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/AB/E"
        , module : "mod"
        , pattern : (char[]) { "AB{,X}/E\0" }
        , matches : (char*[]) {
              "/mod/AB/E"
            , 0
          }
      }}

    /* alternations + epsilon */

    /* classes */
    , (pattern_search_test[]) {{
          operations : "mod/A"
        , module : "mod"
        , pattern : (char[]) { "[ABC]\0" }
        , matches : (char*[]) { "/mod/A", 0 }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/A"
            " mod/b"
        , module : "mod"
        , pattern : (char[]) { "[A-Za-z]\0" }
        , matches : (char*[]) {
              "/mod/A"
            , "/mod/b"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : "mod/A mod/B"
        , module : "mod"
        , pattern : (char[]) { "[A-Z]\0" }
        , matches : (char*[]) { "/mod/A", "/mod/B", 0 }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/A"
            " mod/Bb"
        , module : "mod"
        , pattern : (char[]) { "[A-Z][a-z]\0" }
        , matches : (char*[]) { "/mod/Bb", 0 }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/A"
            " mod/bB"
        , module : "mod"
        , pattern : (char[]) { "[a-z][A-Z]\0" }
        , matches : (char*[]) {
              "/mod/bB"
            , 0
          }
      }}

    /* star */
    , (pattern_search_test[]) {{
          operations : ""
            " mod/abcd"
        , module : "mod"
        , pattern : (char[]) { "ab*\0" }
        , matches : (char*[]) {
              "/mod/abcd"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/abcd"
        , module : "mod"
        , pattern : (char[]) { "*cd\0" }
        , matches : (char*[]) {
              "/mod/abcd"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/abcd"
        , module : "mod"
        , pattern : (char[]) { "a*d\0" }
        , matches : (char*[]) {
              "/mod/abcd"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/abcd"
        , module : "mod"
        , pattern : (char[]) { "ab*cd\0" }
        , matches : (char*[]) {
              "/mod/abcd"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/abcd"
        , module : "mod"
        , pattern : (char[]) { "a*c*\0" }
        , matches : (char*[]) {
              "/mod/abcd"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/abcd"
        , module : "mod"
        , pattern : (char[]) { "*c*\0" }
        , matches : (char*[]) {
              "/mod/abcd"
            , 0
          }
      }}

    /* star + alternation */
    , (pattern_search_test[]) {{
          operations : ""
            " mod/bcd/e"
            " mod/bd/e"
        , module : "mod"
        , pattern : (char[]) { "*{d,cd}/e\0" }
        , matches : (char*[]) {
              "/mod/bcd/e"
            , "/mod/bd/e"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/abcd"
        , module : "mod"
        , pattern : (char[]) { "*{lm,ab}*\0" }
        , matches : (char*[]) {
              "/mod/abcd"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/abcd/efgh"
//            " mod/jklm/xyzw"
        , module : "mod"
        , pattern : (char[]) { "*{lm,ab}*/[a-z][a-z][a-z][a-z]\0" }
        , matches : (char*[]) {
              "/mod/abcd/efgh"
//            , "/mod/jklm/xyzw"
            , 0
          }
      }}

    /* star / multi directory */
    , (pattern_search_test[]) {{
          operations : ""
            " mod/abcd/x/abcd"
            " mod/xyzw/x/abcd"
        , module : "mod"
        , pattern : (char[]) { "*/*/abcd\0" }
        , matches : (char*[]) {
              "/mod/abcd/x/abcd"
            , "/mod/xyzw/x/abcd"
            , 0
          }
      }}

    /* variants */
    , (pattern_search_test[]) {{
          operations : ""
            " mod/abcd"
            " mod/xyzw"
        , module : "mod"
        , pattern : (char[]) { "?\0" }
        , variants : (char*[]) { "abcd", "xyzw", 0 }
        , matches : (char*[]) {
              "/mod/abcd ? abcd"
            , "/mod/xyzw ? xyzw"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/foo.abcd"
            " mod/foo.xyzw"
        , module : "mod"
        , pattern : (char[]) { "*.?\0" }
        , variants : (char*[]) { "abcd", "xyzw", 0 }
        , matches : (char*[]) {
              "/mod/foo.abcd ? abcd"
            , "/mod/foo.xyzw ? xyzw"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/abcdfoo"
            " mod/xyzwfoo"
        , module : "mod"
        , pattern : (char[]) { "?*\0" }
        , variants : (char*[]) { "abcd", "xyzw", 0 }
        , matches : (char*[]) {
              "/mod/abcdfoo ? abcd"
            , "/mod/xyzwfoo ? xyzw"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/fooabcdbar"
            " mod/fooxyzwbar"
        , module : "mod"
        , pattern : (char[]) { "*?*\0" }
        , variants : (char*[]) { "abcd", "xyzw", 0 }
        , matches : (char*[]) {
              "/mod/fooabcdbar ? abcd"
            , "/mod/fooxyzwbar ? xyzw"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/xax"
            " mod/xay"
            " mod/yax"
            " mod/yay"
        , module : "mod"
        , pattern : (char[]) { "?a?\0" }
        , variants : (char*[]) { "x", "y", 0 }
        , matches : (char*[]) {
              "/mod/xax ? x"
            , "/mod/yay ? y"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/x/x"
            " mod/x/y"
            " mod/y/x"
            " mod/y/y"
        , module : "mod"
        , pattern : (char[]) { "?/?\0" }
        , variants : (char*[]) { "x", "y", 0 }
        , matches : (char*[]) {
              "/mod/x/x ? x"
            , "/mod/y/y ? y"
            , 0
          }
      }}

    /* starstar */
    , (pattern_search_test[]) {{
          operations : ""
            " mod/foo/qux"
            " mod/foo/bar/qux"
            " mod/foo/bar/baz/qux"
        , module : "mod"
        , pattern : (char[]) { "**\0" } // only matches directories
        , matches : (char*[]) {
              0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/foo/qux"
            " mod/foo/bar/qux"
            " mod/foo/bar/baz/qux"
        , module : "mod"
        , pattern : (char[]) { "**/*\0" }
        , matches : (char*[]) {
              "/mod/foo/qux"
            , "/mod/foo/bar/qux"
            , "/mod/foo/bar/baz/qux"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/foo/qux"
            " mod/foo/bar/qux"
            " mod/foo/bar/baz/qux"
        , module : "mod"
        , pattern : (char[]) { "**/**\0" }  // only matches directories
        , matches : (char*[]) {
              0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/foo/qux"
            " mod/foo/bar/qux"
            " mod/foo/bar/baz/qux"
        , module : "mod"
        , pattern : (char[]) { "**/**/*\0" }
        , matches : (char*[]) {
              "/mod/foo/qux"
            , "/mod/foo/bar/qux"
            , "/mod/foo/bar/baz/qux"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/foo/qux"
            " mod/foo/bar/qux"
            " mod/foo/bar/baz/qux"
        , module : "mod"
        , pattern : (char[]) { "**/qux\0" }
        , matches : (char*[]) {
              "/mod/foo/qux"
            , "/mod/foo/bar/qux"
            , "/mod/foo/bar/baz/qux"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/A"
            " mod/foo/B"
            " mod/foo/bar/C"
            " mod/foo/bar/baz/D"
        , module : "mod"
        , pattern : (char[]) { "**/*\0" }
        , matches : (char*[]) {
              "/mod/A"
            , "/mod/foo/B"
            , "/mod/foo/bar/C"
            , "/mod/foo/bar/baz/D"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/A"
            " mod/foo/B"
            " mod/foo/bar/C"
            " mod/foo/bar/baz/D"
        , module : "mod"
        , pattern : (char[]) { "**/**/*\0" }
        , matches : (char*[]) {
              "/mod/A"
            , "/mod/foo/B"
            , "/mod/foo/bar/C"
            , "/mod/foo/bar/baz/D"
            , 0
          }
      }}

    /* starstar+conjunctions */
    , (pattern_search_test[]) {{
          operations : ""
            " mod/foo/qux"
            " mod/foo/bar/qux"
            " mod/foo/bar/baz/qux"
        , module : "mod"
        , pattern : (char[]) { "**~baz/*\0" }
        , matches : (char*[]) {
              "/mod/foo/qux"
            , "/mod/foo/bar/qux"
            , 0
          }
      }}

    /* star+conjunctions */
    , (pattern_search_test[]) {{
          operations : ""
            " mod/bar"
            " mod/baz"
        , module : "mod"
        , pattern : (char[]) { "*~baz\0" }
        , matches : (char*[]) {
              "/mod/bar"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/bar"
            " mod/baz"
        , module : "mod"
        , pattern : (char[]) { "*+baz\0" }
        , matches : (char*[]) {
              "/mod/baz"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/foo"
            " mod/bar"
            " mod/baz"
        , module : "mod"
        , pattern : (char[]) { "*~bar~baz\0" }
        , matches : (char*[]) {
              "/mod/foo"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/integ/a"
            " mod/integ/a/a"
            " mod/integ/a/a/a"
            " mod/src/a"
            " mod/src/b/a"
            " mod/src/c/d/a"
        , module : "mod"
        , pattern : (char[]) { "*~integ/**/a\0" }
        , matches : (char*[]) {
              "/mod/src/a"
            , "/mod/src/b/a"
            , "/mod/src/c/d/a"
            , 0
          }
      }}

    /* groups */
    , (pattern_search_test[]) {{
          operations : ""
            " mod/bar"
        , module : "mod"
        , pattern : (char[]) { "ba([rz])\0" }
        , matches : (char*[]) {
              "/mod/bar 1 r"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/bar"
            " mod/baz"
        , module : "mod"
        , pattern : (char[]) { "ba({r,z})\0" }
        , matches : (char*[]) {
              "/mod/bar 1 r"
            , "/mod/baz 1 z"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/bar"
            " mod/baz"
        , module : "mod"
        , pattern : (char[]) { "*ba(r)\0" }
        , matches : (char*[]) {
              "/mod/bar 1 r"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mods/mod/foo/bar"
        , module : "mod"
        , pattern : (char[]) { "(f)oo/ba(r)\0" }
        , matches : (char*[]) {
              "/mods/mod/foo/bar 1 f 2 r"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mods/mod/A"
            " mods/mod/foo/B"
            " mods/mod/foo/bar/C"
        , module : "mod"
        , pattern : (char[]) { "**/(*)\0" }
        , matches : (char*[]) {
              "/mods/mod/A 1 A"
            , "/mods/mod/foo/B 1 B"
            , "/mods/mod/foo/bar/C 1 C"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mods/mod/abc"
        , module : "mod"
        , pattern : (char[]) { "(*)bc\0" }
        , matches : (char*[]) {
              "/mods/mod/abc 1 a"
            , 0
          }
      }}

    /* nested groups */
    , (pattern_search_test[]) {{
          operations : ""
            " mod/car"
            " mod/carf"
            " mod/carff"
        , module : "mod"
        , pattern : (char[]) { "c((ar))f\0" }
        , matches : (char*[]) {
              "/mod/carf 1 ar 2 ar"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/c"
            " mod/ca"
            " mod/car"
            " mod/carf"
            " mod/carff"
        , module : "mod"
        , pattern : (char[]) { "c(((ar)))f\0" }
        , matches : (char*[]) {
              "/mod/carf 1 ar 2 ar 3 ar"
            , 0
          }
      }}

    /* conjunctions + groups */
    , (pattern_search_test[]) {{
          operations : ""
            " mod/barf"
            " mod/barb"
        , module : "mod"
        , pattern : (char[]) { "(([bc]ar)+(bar))f\0" }
        , matches : (char*[]) {
              "/mod/barf 1 bar 2 bar 3 bar"
            , 0
          }
      }}
    , (pattern_search_test[]) {{
          operations : ""
            " mod/barf"
            " mod/carb"
            " mod/carf"
        , module : "mod"
        , pattern : (char[]) { "([bc]ar~bar)f\0" }
        , matches : (char*[]) {
              "/mod/carf 1 car"
            , 0
          }
      }}

    /* shadow-requires */
    , (pattern_search_test[]) {{
          operations : ""
            " mods/mod/a"
            " mods/foo/A"
            " mods/bar/B"
        , module : "mod"
        , modules_shadow : (char*[]) { "foo", "00foo", "mod", "00mod", "bar", "00bar", 0 }
        , modules_imports : (char*[]) { "mod", "foo", "mod", "bar", 0 }
        , pattern : (char[]) { "//module/imports/*\0" }
        , matches : (char*[]) {
            /* patterns only ever match nodes, not directories */
            0
          }
      }}
    , 0
  }
};
