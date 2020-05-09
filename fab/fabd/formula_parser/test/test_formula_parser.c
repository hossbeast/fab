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
#include "xlinux/xstdlib.h"

#include "valyria/list.h"
#include "valyria/map.h"
#include "valyria/set.h"

#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/growing.h"

#include "formula_parser.internal.h"
#include "formula.internal.h"
#include "formula_value.internal.h"
#include "selector.internal.h"
#include "logging.h"

typedef struct {
  XUNITTEST;

  /* input */
  char * text;

  /* output */
  char * path;
  formula_value ** args;
  formula_value ** envs;
} formula_parser_test;

static xapi formula_parser_test_unit_setup(xunit_unit * unit)
{
  enter;

  fatal(yyutil_load);
  fatal(value_load);
  fatal(logging_finalize);

  finally : coda;
}

static xapi formula_parser_test_unit_cleanup(xunit_unit * unit)
{
  enter;

  fatal(yyutil_unload);
  fatal(value_unload);

  finally : coda;
}

static xapi fml_say(narrator * restrict N, formula_value * restrict path, formula_value * restrict args, formula_value * restrict envs)
{
  enter;

  if(path)
  {
    xsays("path : ");
    fatal(formula_value_say, path, N);
    xsays("\n");
  }
  if(envs)
  {
    xsays("envs : ");
    fatal(formula_value_say, envs, N);
    xsays("\n");
  }
  if(args)
  {
    xsays("args : ");
    fatal(formula_value_say, args, N);
    xsays("\n");
  }

  finally : coda;
}

static xapi formula_parser_test_entry(xunit_test * _test)
{
  enter;

  formula_parser_test * test = containerof(_test, formula_parser_test, xu);

  formula_parser * parser = 0;
  formula_value *args = 0;
  formula_value *envs = 0;
  formula_value *path = 0;
  narrator * N1 = 0;
  narrator * N2 = 0;
  char buf[512];

  // arrange
  fatal(formula_parser_create, &parser);

  // act - initial parse
  fatal(formula_parser_bacon_parse, parser, test->text, strlen(test->text) + 2, 0, &path, &args, &envs);

  // round-trip
  fatal(narrator_growing_create, &N1);
  fatal(fml_say, N1, path, args, envs);
  formula_value_free(path); path = 0;
  formula_value_free(args); args = 0;
  formula_value_free(envs); envs = 0;

  size_t len = narrator_growing_size(N1);
  fatal(narrator_xseek, N1, 0, NARRATOR_SEEK_SET, 0);
  fatal(narrator_xread, N1, buf, len, 0);
  buf[len] = buf[len + 1] = 0;
  fatal(formula_parser_bacon_parse, parser, buf, len + 2, 0, &path, &args, &envs);

  // round-trip
  fatal(narrator_growing_create, &N2);
  fatal(fml_say, N2, path, args, envs);

finally:
  fatal(narrator_xfree, N1);
  fatal(narrator_xfree, N2);
  formula_value_free(path);
  formula_value_free(args);
  formula_value_free(envs);
  fatal(formula_parser_xfree, parser);
coda;
}

//
// manifest
//

xunit_unit xunit = {
    xu_setup : formula_parser_test_unit_setup
  , xu_cleanup : formula_parser_test_unit_cleanup
  , xu_entry : formula_parser_test_entry
  , xu_tests : (formula_parser_test*[]) {
      (formula_parser_test[]) {{
          text : (char[]) {
            " envs : {"
              " libdeps : ["
                " true"
              " ]"
            " }\0\0"
          }
      }}
    , (formula_parser_test[]) {{
          text : (char[]) {
            " envs : {"
              " libdeps : ["
                " select : ["
                  " module"
                " ]"
              " ]"
            " }\0\0"
          }
      }}
    , (formula_parser_test[]) {{
          text : (char[]) {
            " envs : {"
              " libdeps : ["
                " select : ["
                  " traverse : {"
                    " direction : down"
                    " relation : strong"
                  " }"
                " ]"
              " ]"
              " localdeps : ["
                " select : ["
                  " traverse : {"
                    " direction : down"
                    " relation : fs"
                  " }"
                " ]"
              " ]"
            " }\0\0"
          }
      }}
    , (formula_parser_test[]) {{
          text : (char[]) {
            " envs : {"
              " deps : ["
                " select : ["
                  " traverse : {"
                    " direction : down"
                  " }"
                " ]"
              " ]"
            " }\0\0"
          }
      }}
    , (formula_parser_test[]) {{
          text : (char[]) {
            " path : $cc"
            " args : ["
              " $cflags"
              " sequence : ["
                " select : ["
                  " module"
                  " union : ["
                    " traverse : {"
                      " direction : down"
                      " min-distance : 0"
                      " relation : fs"
                      " file-type : dir"
                    " }"

                    " traverse : {"
                      " direction : down"
                      " relation : requires"
                    " }"
                  " ]"
                " ]"
                " property : relpath"
                " prepend : \"-I\""
              " ]"
              " $bm_source"
              " \"-c\""
              " \"-o\" $bm_target"
            " ]"
            "\0\0"
          }
      }}
    , 0
  }
};
