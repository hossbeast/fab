/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "lorien/load.h"
#include "bacon/load.h"
#include "xlinux/xstdlib.h"

#include "valyria/list.h"
#include "valyria/hashtable.h"

#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/growing.h"

#include "formula_parser.internal.h"
#include "formula.h"
#include "formula_value.internal.h"
#include "selector.internal.h"

typedef struct {
  XUNITTEST;

  /* input */
  char *text;
  int expected;

  /* output */
  char *path;
  formula_value ** args;
  formula_value ** envs;
} formula_parser_test;

static void formula_parser_test_unit_setup(xunit_unit * unit)
{
  bacon_load();
}

static void formula_parser_test_unit_cleanup(xunit_unit * unit)
{
  bacon_unload();
}

static void fml_say(narrator * restrict N, formula_value * restrict file, formula_value * restrict args, formula_value * restrict envs)
{
  if(file)
  {
    xsays("file : ");
    formula_value_say(file, N);
    xsays("\n");
  }
  if(envs)
  {
    xsays("envs : ");
    formula_value_say(envs, N);
    xsays("\n");
  }
  if(args)
  {
    xsays("args : ");
    formula_value_say(args, N);
    xsays("\n");
  }
}

static void formula_parser_test_entry(xunit_test * _test)
{
  formula_parser_test * test = containerof(_test, formula_parser_test, xu);

  formula_parser * parser = 0;
  formula_value *args = 0;
  formula_value *envs = 0;
  formula_value *path = 0;
  narrator_growing * N1 = 0;
  narrator_growing * N2 = 0;
  char buf[1024];
  size_t len;
  int r;

  // arrange
  formula_parser_create(&parser);

  // act - initial parse
  if((r = formula_parser_bacon_parse(parser, test->text, strlen(test->text) + 2, 0, &path, &args, &envs)) != 0) {
    tracef("%.*s", (int)parser->yyu.error_len, parser->yyu.error_str);
  }
  assert_eq_d(test->expected, r);
  if(r) {
    goto end;
  }

  // round-trip
  narrator_growing_create(&N1);
  fml_say(&N1->base, path, args, envs);
  formula_value_free(path); path = 0;
  formula_value_free(args); args = 0;
  formula_value_free(envs); envs = 0;

  len = N1->l;
  narrator_seek(&N1->base, 0, NARRATOR_SEEK_SET, 0);
  narrator_read(&N1->base, buf, len, 0);
  buf[len] = buf[len + 1] = 0;
  if(formula_parser_bacon_parse(parser, buf, len + 2, 0, &path, &args, &envs) != 0) {
    tracef("%.*s", (int)parser->yyu.error_len, parser->yyu.error_str);
    RTA;
  }

  // round-trip
  narrator_growing_create(&N2);
  fml_say(&N2->base, path, args, envs);

end:
  narrator_growing_free(N1);
  narrator_growing_free(N2);
  formula_value_free(path);
  formula_value_free(args);
  formula_value_free(envs);
  formula_parser_free(parser);
}

//
// manifest
//

static xunit_unit xunit = {
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
                    " graph : depends"
                  " }"
                " ]"
              " ]"
              " localdeps : ["
                " select : ["
                  " traverse : {"
                    " direction : down"
                    " graph : fs"
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
            " file : $cc"
            " args : ["
              " $cflags"
              " sequence : ["
                " select : ["
                  " module"
                  " union : ["
                    " traverse : {"
                      " direction : down"
                      " min-distance : 0"
                      " graph : dirtree"
                    " }"
                    " traverse : {"
                      " direction : down"
                      " graph : requires"
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

    /* on parse failure, there should be no leaks */
    , (formula_parser_test[]) {{
          text : (char[]) {
            " args : ["
              " sequence : ["
                " foo"
                " property : relpath"
                " prepend : !!"
              " ]"
            " ]"
            "\0\0"
          }
        , expected : 2
      }}
    , 0
  }
};
XUNIT_UNIT(xunit);
