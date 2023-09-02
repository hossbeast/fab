/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "lorien/load.h"
#include "bacon/value.h"
#include "bacon/parser.h"
#include "bacon/load.h"
#include "xlinux/xstdlib.h"

#include "valyria/hashtable.h"

#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/fd.h"
#include "narrator/load.h"
#include "narrator/growing.h"

#include "rules_parser.h"
#include "module.h"
#include "rule.h"
#include "request.h"
#include "fsent.h"
#include "pattern.h"
#include "system_state.h"


typedef struct {
  XUNITTEST;

  /* inputs */
  char *text;
  char *vars;

  /* expected */
  int expected_rules;
} rules_parser_test;

static void rules_parser_test_unit_setup(xunit_unit * unit)
{
  bacon_load();
  graph_setup();
  narrator_load();
  module_setup();
}

static void rules_parser_test_unit_cleanup(xunit_unit * unit)
{
  module_cleanup();
  bacon_unload();
  graph_cleanup();
  narrator_unload();
}

static void rules_parser_test_entry(xunit_test * _test)
{
  rules_parser_test * test = containerof(_test, rules_parser_test, xu);
  bacon_value *vars;
  rules_parser * parser = 0;
  module * mod = 0;
  graph_invalidation_context invalidation = { };
  bacon_parser *val_parser = 0;

  // arrange
  bacon_parser_alloc(&val_parser);
  rules_parser_alloc(&parser);
  graph_invalidation_begin(&invalidation);

  if(test->vars) {
    bacon_parser_parse(val_parser, MMS(test->vars), "-vars-", BACON_TYPE_SET, &vars);
  }

  // parse
  module_alloc(&mod);
printf("%s\n\n", test->text);
  if(rules_parser_parse(parser, test->text, strlen(test->text) + 2, MMS("-fname-")) != 0) {
    tracef("%.*s", parser->yyu.error_len, parser->yyu.error_str);
    RTA;
  }

  module_dispose(mod);
  rules_parser_free(parser);
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
  , xu_tests : (rules_parser_test*[]) {
      /* ltr one-to-one */
      (rules_parser_test[]) {{
          text : (char[]) {
            "rule *~integ/**/*.o -> libvoid.so\0"
          }
        , expected_rules : 1
      }}

      /* zero-to-one match */
    , (rules_parser_test[]) {{
          text : (char[]) {
            "rule *~integ/**/*.o -- : ./bam-cc\0"
          }
        , expected_rules : 1
      }}
      /* zero-to-many search */
    , (rules_parser_test[]) {{
          text : (char[]) {
            "rule *~integ/**/*.o [*] -- : ./bam-cc\0"
          }
        , expected_rules : 1
      }}
    , (rules_parser_test[]) {{
          text : (char[]) {
            "rule { search : module } *~integ/**/*.o [*] -- : ./bam-cc\0"
          }
        , expected_rules : 1
      }}

      /* zero-to-one generate */
    , (rules_parser_test[]) {{
          text : (char[]) {
            "rule -- libvoid.so : ./bam-cc\0"
          }
        , expected_rules : 1
      }}

      /* zero-to-many generate */
    , (rules_parser_test[]) {{
          text : (char[]) {
            "rule -- [*] libvoid.so : ./bam-cc\0"
          }
        , expected_rules : 1
      }}

      /* ltr one-to-many */
    , (rules_parser_test[]) {{
          text : (char[]) {
            "rule *~integ/**/*.o -> [*] libvoid.so : ./bam-cc\0"
          }
        , expected_rules : 1
      }}

      /* bind rules */
    , (rules_parser_test[]) {{
          text : (char[]) {
            "bind fab/libxlinux => //module/bindings\0"
          }
        , expected_rules : 1
      }}
    , (rules_parser_test[]) {{
          text : (char[]) {
            "bind fab-build/libxlinux/$CONFIG_VARIANT/libxlinux.a => //module/static-libs\0"
          }
        , expected_rules : 1
      }}

    /* search/search */
    , (rules_parser_test[]) {{
          text : (char[]) {
            "bind fab-build/libxlinux/$CONFIG_VARIANT/libxlinux.a => //module/static-libs\0"
          }
        , expected_rules : 1
      }}
    , 0
  }
};
XUNIT_UNIT(xunit);
