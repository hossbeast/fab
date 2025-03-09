/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "lorien/load.h"
#include "bacon/value.h"
#include "bacon/parser.h"
#include "bacon/load.h"
#include "xlinux/xstdlib.h"
#include "narrator/fixed.h"

#include "valyria/hashtable.h"

#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/fd.h"
#include "narrator/load.h"
#include "narrator/growing.h"

#include "ruleset_parser.h"
#include "module.h"
#include "rule.h"
#include "request.h"
#include "fsent.h"
#include "pattern.h"
#include "system.h"
#include "shadow.h"
#include "fstree.h"
#include "ruleset.h"

typedef struct {
  XUNITTEST;

  /* inputs */
  char *text;
  char *vars;

  /* expected */
  char *expected;
} ruleset_parser_test;

static void rules_parser_test_unit_setup(xunit_unit * unit)
{
  bacon_load();

  fsent_root_setup();
  shadow_root_setup();
  fstree_root_setup();
  shadow_setup();
  fstree_setup();
  module_setup();
  graph_setup();

  extern __thread bool rcu_is_reader ; rcu_is_reader = true;
}

static void rules_parser_test_unit_cleanup(xunit_unit * unit)
{
  bacon_unload();

  shadow_cleanup();
  fstree_cleanup();
  module_cleanup();
  fsent_cleanup();
  graph_cleanup();
}

static void block_say(struct block * restrict block, narrator * restrict N)
{
  struct block *sb;
  struct rule *rule;

  llist_foreach(&block->blocks, sb, lln) {
    block_say(sb, N);
  }
  llist_foreach(&block->rules, rule, lln) {
    if(N->off) {
      narrator_says(N, "\n");
    }
    rule_say(rule, N);
  }
}

static void rules_parser_test_entry(xunit_test * _test)
{
  ruleset_parser_test * test = containerof(_test, ruleset_parser_test, xu);
  bacon_value *vars;
  ruleset_parser *parser;
  module *mod;
  ruleset *rules;
  graph_invalidation_context invalidation = { };
  bacon_parser *val_parser;
  fsent build_dir;
  char buf[512];
  narrator_fixed fixed;
  narrator *N;
  struct block *block;
  size_t len;

  // arrange
  bacon_parser_alloc(&val_parser);
  ruleset_parser_alloc(&parser);
  graph_invalidation_begin(&invalidation);

  if(test->vars) {
    bacon_parser_parse(val_parser, MMS(test->vars), "-vars-", BACON_TYPE_SET, &vars);
  }

  memset(&build_dir, 0, sizeof(build_dir));

  module_alloc(&mod);
  mod->build_dir = &build_dir;
  ruleset_alloc(&rules);
  parser->container = rules;

  /* parse */
  if(ruleset_parser_parse(parser, test->text, strlen(test->text) + 2, MMS("-fname-")) != 0) {
    tracef("%.*s", parser->error_len, parser->error);
    RTA;
  }

  N = narrator_fixed_init(&fixed, buf, sizeof(buf));
  llist_foreach(&parser->blocks, block, lln) {
    block_say(block, N);
  }

  /* this incredible hack allows the rules in the next parse to have a different id */
  rules->vertex.attrs += 0xff;

  /* round-trip parse */
  len = fixed.l;
  fixed.s[len + 0] = 0;
  fixed.s[len + 1] = 0;
  if(ruleset_parser_parse(parser, buf, len + 2, MMS("-fname-")) != 0) {
    tracef("%.*s", parser->error_len, parser->error);
    RTA;
  }

  N = narrator_fixed_init(&fixed, buf, sizeof(buf));
  llist_foreach(&parser->blocks, block, lln) {
    block_say(block, N);
  }

  if(test->expected) {
    assert_eq_w(test->expected, strlen(test->expected), fixed.s, fixed.l);
  } else {
    assert_eq_w(test->text, strlen(test->text), fixed.s, fixed.l);
  }

  module_release(mod);
  ruleset_parser_free(parser);
  graph_invalidation_end(&invalidation);
  bacon_parser_free(val_parser);
}

//
// manifest
//

static xunit_unit xunit = {
    xu_setup : rules_parser_test_unit_setup
  , xu_cleanup : rules_parser_test_unit_cleanup
  , xu_entry : rules_parser_test_entry
  , xu_tests : (ruleset_parser_test*[]) {
      /* ltr one-to-one */
      (ruleset_parser_test[]) {{
          text : (char[]) {
            "rule *~integ/**/*.o -> libvoid.so\0"
          }
      }}
      /* rtl one-to-one */
    , (ruleset_parser_test[]) {{
          text : (char[]) {
            "rule tests.xunit <- test/blob.a\0"
          }
      }}

      /* zero-to-one match */
    , (ruleset_parser_test[]) {{
          text : (char[]) {
            "rule *~integ/**/*.o -- : ./bam-cc\0"
          }
      }}
      /* zero-to-many search */
    , (ruleset_parser_test[]) {{
          text : (char[]) {
            "rule *~integ/**/*.o [*] -- : ./bam-cc\0"
          }
      }}
    , (ruleset_parser_test[]) {{
          text : (char[]) {
            "rule $^D/*~integ/**/*.o [*] -- : ./bam-cc\0"
          }
      }}

      /* with tag */
    , (ruleset_parser_test[]) {{
          text : (char[]) {
            "rule { name : foobar } *.o [*] -- : ./bam-cc\0"
          }
      }}

      /* zero-to-one generate */
    , (ruleset_parser_test[]) {{
          text : (char[]) {
            "rule -- libvoid.so : ./bam-cc\0"
          }
      }}

      /* zero-to-many generate */
    , (ruleset_parser_test[]) {{
          text : (char[]) {
            "rule -- [*] libvoid.so : ./bam-cc\0"
          }
      }}

      /* ltr one-to-many */
    , (ruleset_parser_test[]) {{
          text : (char[]) {
            "rule *~integ/**/*.o -> [*] libvoid.so : ./bam-cc\0"
          }
      }}
      /* rtl one-to-many */
    , (ruleset_parser_test[]) {{
          text : (char[]) {
            "rule *~integ/**/*.o [*] <- libvoid.so : ./bam-cc\0"
          }
      }}

      /* ltr many-to-many */
    , (ruleset_parser_test[]) {{
          text : (char[]) {
            "rule **/(*).tab.[ch] [*] -> [*] $^D/$1.tokens.[ch] : ./tokens.bam\0"
          }
      }}

      /* bind rules */
    , (ruleset_parser_test[]) {{
          text : (char[]) {
            "bind fab/libxlinux => //module/bindings\0"
          }
      }}
    , (ruleset_parser_test[]) {{
          text : (char[]) {
            "bind fab/libxlinux => [*] //module/bindings\0"
          }
      }}
    , (ruleset_parser_test[]) {{
          text : (char[]) {
            "bind fab-build/libxlinux/$CONFIG_VARIANT/libxlinux.a => //module/static-libs\0"
          }
      }}

    /* search/search -- ltr only atm */
    , (ruleset_parser_test[]) {{
          text : (char[]) {
            "rule search //artifacts/*/$CONFIG_VARIANT/*.{a,o} [*] -> [*] search **/test/test_*.xunit\0"
          }
        , expected :
            "search //artifacts/*/$CONFIG_VARIANT/*.{a,o} [*] -> [*] search **/test/test_*.xunit"
      }}
    , (ruleset_parser_test[]) {{
          text : (char[]) {
            "search //artifacts/*/$CONFIG_VARIANT/*.{a,o} [*] -> [*] search **/test/test_*.xunit\0"
          }
      }}
    , (ruleset_parser_test[]) {{
          text : (char[]) {
            "rule search //artifacts/*/$CONFIG_VARIANT/*.{a,o} [*] -> [*] search tests.xunit\0"
          }
        , expected :
            "search //artifacts/*/$CONFIG_VARIANT/*.{a,o} [*] -> [*] search tests.xunit"
      }}
    , (ruleset_parser_test[]) {{
          text : (char[]) {
            "search //artifacts/*/$CONFIG_VARIANT/*.{a,o} [*] -> [*] search tests.xunit\0"
          }
      }}
    /* search/search with tag requires using the rule keyword */
    , (ruleset_parser_test[]) {{
          text : (char[]) {
            "search //artifacts/*/$CONFIG_VARIANT/*.{a,o} [*] -> [*] search $^D/**/test/test_*.unit\0"
          }
      }}
    , (ruleset_parser_test[]) {{
          text : (char[]) {
            "rule { name : boat } search //artifacts/*/$CONFIG_VARIANT/*.{a,o} [*] -> [*] search **/test/test_*.xunit\0"
          }
        , expected :
            "rule { name : boat } search //artifacts/*/$CONFIG_VARIANT/*.{a,o} [*] -> [*] search **/test/test_*.xunit"
      }}

    /* search/generate -- optional search keyword */
    , (ruleset_parser_test[]) {{
          text : (char[]) {
            "rule search $^D/*~integ/**/*.o -> foo : ./bam-cc\0"
          }
        , expected : "rule $^D/*~integ/**/*.o -> foo : ./bam-cc"
      }}
    , (ruleset_parser_test[]) {{
          text : (char[]) {
            "search $^D/*~integ/**/*.o -> foo : ./bam-cc\0"
          }
        , expected : "rule $^D/*~integ/**/*.o -> foo : ./bam-cc"
      }}

    /* search/generate -- optional generate keyword */
    , (ruleset_parser_test[]) {{
          text : (char[]) {
            "search $^D/*~integ/**/*.o -> foo\0"
          }
        , expected : "rule $^D/*~integ/**/*.o -> foo"
      }}
    , (ruleset_parser_test[]) {{
          text : (char[]) {
            "search $^D/*~integ/**/*.o -> foo : ./bam-cc\0"
          }
        , expected : "rule $^D/*~integ/**/*.o -> foo : ./bam-cc"
      }}
    , (ruleset_parser_test[]) {{
          text : (char[]) {
            "search $^D/*~integ/**/*.o -> generate foo\0"
          }
        , expected : "rule $^D/*~integ/**/*.o -> foo"
      }}
    , (ruleset_parser_test[]) {{
          text : (char[]) {
            "search $^D/*~integ/**/*.o -> generate foo : ./bam-cc\0"
          }
        , expected : "rule $^D/*~integ/**/*.o -> foo : ./bam-cc"
      }}

    /* various formula lookup patterns */
    , (ruleset_parser_test[]) {{
          text : (char[]) {
            "rule -- [*] sigtable/sigtable.{c,h} : ./\"sigtable.bam+devel\"\0"
          }
        , expected : "rule -- [*] sigtable/sigtable.{c,h} : ./\"sigtable.bam+devel\""
      }}
    , (ruleset_parser_test[]) {{
          text : (char[]) {
            "rule a -> b : foo/bar.bam+(.)\0"
          }
        , expected : "rule a -> b : foo/bar.bam+(.)"
      }}
    , (ruleset_parser_test[]) {{
          text : (char[]) {
            "rule a -> b : bar.bam+(.)\0"
          }
        , expected : "rule a -> b : bar.bam+(.)"
      }}
    , (ruleset_parser_test[]) {{
          text : (char[]) {
            "rule a -> b : /bar.bam%devel+(.)\0"
          }
        , expected : "rule a -> b : /bar.bam%devel+(.)"
      }}
    , 0
  }
};
XUNIT_UNIT(xunit);
