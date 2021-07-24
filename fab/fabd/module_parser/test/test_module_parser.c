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

#include "lorien/load.h"
#include "yyutil/load.h"
#include "value/load.h"
#include "xlinux/xstdlib.h"

#include "valyria/map.h"
#include "valyria/set.h"
#include "valyria/array.h"

#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/growing.h"

#include "module_parser.h"
#include "module.internal.h"
#include "rule.h"
#include "request.h"
#include "fsent.h"
#include "variant.h"
#include "pattern.h"
#include "system_state.h"

#include "yyutil/box.h"

typedef struct {
  XUNITTEST;

  char * text;
  module * cfg;
} module_parser_test;

static void module_parser_test_unit_setup(xunit_unit * unit)
{
  yyutil_load();
  value_load();
  logging_finalize();
  variant_setup();
  graph_setup();
}

static void module_parser_test_unit_cleanup(xunit_unit * unit)
{
  yyutil_unload();
  value_unload();
  variant_cleanup();
  graph_cleanup();
}

static void load_submodule(module_parser * restrict parser, pattern * c, bool scoped, char * restrict name, uint16_t namel)
{
  pattern_free(c);
}

static void module_parser_test_entry(xunit_test * _test)
{
  module_parser_test * test = containerof(_test, module_parser_test, xu);

  module_parser * parser = 0;
  module * mod = 0;
  graph_invalidation_context invalidation = { };

  // arrange
  module_parser_create(&parser);
  parser->import_resolve = load_submodule;
  graph_invalidation_begin(&invalidation);

  // parse
  module_alloc(&mod);
  module_parser_parse(parser, mod, &invalidation, test->text, strlen(test->text) + 2, "-fname-");
  assert_eq_b(false, system_error);

finally:
  module_xrelease(mod, parser);
  module_parser_xfree(parser);
  graph_invalidation_end(&invalidation);
coda;
}

//
// manifest
//

xunit_unit xunit = {
    xu_setup : module_parser_test_unit_setup
  , xu_cleanup : module_parser_test_unit_cleanup
  , xu_entry : module_parser_test_entry
  , xu_tests : (module_parser_test*[]) {
      /* variant declarations */
      (module_parser_test[]) {{
          text : (char[]) {
            "variant {foo,bar}.void.{debug,devel,final}\0"
          }
      }}

      /* ltr one-to-one */
    , (module_parser_test[]) {{
          text : (char[]) {
            "rule *~integ/**/*.?.o -> libvoid.$?.so\0"
          }
      }}

      /* zero-to-one match */
    , (module_parser_test[]) {{
          text : (char[]) {
            "rule *~integ/**/*.?.o -- : ./bam-cc\0"
          }
      }}
      /* zero-to-many match */
    , (module_parser_test[]) {{
          text : (char[]) {
            "rule *~integ/**/*.?.o [*] -- : ./bam-cc\0"
          }
      }}

      /* zero-to-one generate */
    , (module_parser_test[]) {{
          text : (char[]) {
            "rule -- libvoid.?.so : ./bam-cc\0"
          }
      }}
      /* zero-to-many generate */
    , (module_parser_test[]) {{
          text : (char[]) {
            "rule -- [*] libvoid.?.so : ./bam-cc\0"
          }
      }}

      /* ltr one-to-many */
    , (module_parser_test[]) {{
          text : (char[]) {
            "rule *~integ/**/*.?.o -> [*] libvoid.$?.so : ./bam-cc\0"
          }
      }}
    , 0
  }
};
