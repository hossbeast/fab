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
#include "lorien/load.h"
#include "yyutil/load.h"
#include "logger/load.h"
#include "value/load.h"

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
#include "node.h"
#include "logging.h"
#include "variant.h"
#include "pattern.h"

#include "box.h"

typedef struct {
  XUNITTEST;

  char * text;
  module * cfg;
} module_parser_test;

static xapi module_parser_test_unit_setup(xunit_unit * unit)
{
  enter;

  fatal(yyutil_load);
  fatal(value_load);
  fatal(logging_finalize);
  fatal(variant_setup);
  fatal(graph_setup);

  finally : coda;
}

static xapi module_parser_test_unit_cleanup(xunit_unit * unit)
{
  enter;

  fatal(yyutil_unload);
  fatal(value_unload);
  fatal(variant_cleanup);
  fatal(graph_cleanup);

  finally : coda;
}

static xapi load_submodule(module_parser * restrict parser, pattern * c, bool scoped, char * restrict name, uint16_t namel)
{
  enter;

  pattern_free(c);

  finally : coda;
}

static xapi load_formula(module_parser * restrict parser, pattern * c, node ** restrict target)
{
  enter;

  pattern_free(c);
  *target = 0;

  finally : coda;
}

static xapi module_parser_test_entry(xunit_test * _test)
{
  enter;

  module_parser_test * test = containerof(_test, module_parser_test, xu);

  module_parser * parser = 0;
  module * mod = 0;
  graph_invalidation_context invalidation = { 0 };

  // arrange
  fatal(module_parser_create, &parser);
  parser->import_resolve = load_submodule;
  parser->formula_resolve = load_formula;
  fatal(graph_invalidation_begin, &invalidation);

  // parse
  fatal(module_create, &mod);
  fatal(module_parser_parse, parser, mod, &invalidation, test->text, strlen(test->text) + 2, "-fname-");

finally:
  fatal(module_parser_xfree, parser);
  fatal(module_xfree, mod);
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
            "variant {foo,bar}.xapi.{debug,devel,final}\0"
          }
      }}

      /* ltr one-to-one */
    , (module_parser_test[]) {{
          text : (char[]) {
            "rule *~integ/**/*.?.o -> libxapi.$?.so\0"
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
            "rule -- libxapi.?.so : ./bam-cc\0"
          }
      }}
      /* zero-to-many generate */
    , (module_parser_test[]) {{
          text : (char[]) {
            "rule -- [*] libxapi.?.so : ./bam-cc\0"
          }
      }}

      /* ltr one-to-many */
    , (module_parser_test[]) {{
          text : (char[]) {
            "rule *~integ/**/*.?.o -> [*] libxapi.$?.so : ./bam-cc\0"
          }
      }}
    , 0
  }
};
