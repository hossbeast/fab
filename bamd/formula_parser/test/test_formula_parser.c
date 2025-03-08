/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "lorien/load.h"
#include "bacon/load.h"
#include "bacon/writer.h"
#include "xlinux/xstdlib.h"

#include "valyria/list.h"
#include "valyria/hashtable.h"

#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/growing.h"

#include "formula_parser.internal.h"
#include "formula.h"
#include "exec_value.h"
#include "selector.internal.h"

typedef struct {
  XUNITTEST;

  /* input */
  char *text;
  char *expected;

  int expected_result;

  /* output */
  bacon_value *path;
  bacon_value *args;
  bacon_value *envs;
} formula_parser_test;

static void formula_parser_test_unit_setup(xunit_unit * unit)
{
  backtracef_setup();

  bacon_load();
  exec_value_setup();
}

static void formula_parser_test_unit_cleanup(xunit_unit * unit)
{
  bacon_unload();
}

static void formula_parser_test_entry(xunit_test * _test)
{
  formula_parser_test * test = containerof(_test, formula_parser_test, xu);

  formula fml;
  formula_parser * parser = 0;
  narrator_growing * N1 = 0;
  narrator_growing * N2 = 0;
  char buf[1024];
  size_t len;
  int r;
  bacon_writer writer;
  const char *expected;

  // arrange
  formula_parser_create(&parser);

  // act - initial parse
  if((r = formula_parser_bacon_parse(parser, test->text, strlen(test->text) + 2, 0, 0, &fml.file, &fml.args, &fml.envs)) != 0) {
    if(!test->expected_result) {
      tracef("%.*s", (int)parser->error_len, parser->error);
    }
  }
  assert_eq_d(test->expected_result, r);
  if(r) {
    goto end;
  }

  // round-trip
  narrator_growing_create(&N1);
  bacon_writer_init(&writer);
  bacon_writer_open(&writer, &N1->base);
  formula_bacon_describe(&fml, &writer);
  bacon_value_ifree(parser->bacon_parser, &fml.file);
  bacon_value_ifree(parser->bacon_parser, &fml.args);
  bacon_value_ifree(parser->bacon_parser, &fml.envs);

  len = N1->l;
  narrator_seek(&N1->base, 0, NARRATOR_SEEK_SET, 0);
  narrator_read(&N1->base, buf, len, 0);
  buf[len] = buf[len + 1] = 0;

  if(formula_parser_bacon_parse(parser, buf, len + 2, 0, 0, &fml.file, &fml.args, &fml.envs) != 0) {
    tracef("%.*s", (int)parser->error_len, parser->error);
    RTA;
  }

  // round-trip
  narrator_growing_create(&N2);
  bacon_writer_init(&writer);
  bacon_writer_open(&writer, &N2->base);
  formula_bacon_describe(&fml, &writer);

  expected = test->expected ?: test->text;
  assert_eq_w(expected, strlen(expected), N1->s, N1->l);

end:
  narrator_growing_free(N1);
  narrator_growing_free(N2);
  bacon_value_free(parser->bacon_parser, fml.file);
  bacon_value_free(parser->bacon_parser, fml.args);
  bacon_value_free(parser->bacon_parser, fml.envs);
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
            "envs : {\n"
            "  cc : {\n"
            "    lookup : {\n"
            "      pattern : a/b/c\n"
            "      namespace : formula\n"
            "    }\n"
            "  }\n"
            "}\0\0"
          }
      }}
    , (formula_parser_test[]) {{
          text : (char[]) {
            "args : [\n"
            "  lookup : {\n"
            "    pattern : d/e/f\n"
            "    namespace : formula\n"
            "  }\n"
            "]\0\0"
          }
      }}
    , (formula_parser_test[]) {{
          text : (char[]) {
            "envs : {\n"
            "  libdeps : {\n"
            "    sequence : [\n"
            "      true\n"
            "    ]\n"
            "  }\n"
            "}\0\0"
          }
      }}
    , (formula_parser_test[]) {{
          text : (char[]) {
            "file : {\n"
            "  lookup : {\n"
            "    pattern : a/b/c\n"
            "    namespace : module\n"
            "  }\n"
            "}\0\0"
          }
      }}
    , (formula_parser_test[]) {{
          text : (char[]) {
            "args : [\n"
            "  lookup : {\n"
            "    pattern : a/b/c\n"
            "    namespace : formula\n"
            "  }\n"
            "  lookup : {\n"
            "    pattern : d/e/f\n"
            "    namespace : formula\n"
            "  }\n"
            "]\0\0"
          }
      }}
    , (formula_parser_test[]) {{
          text : (char[]) {
            "envs : {\n"
            "  ar : {\n"
            "    lookup : {\n"
            "      pattern : ar\n"
            "      namespace : binutils\n"
            "    }\n"
            "  }\n"
            "  cc : {\n"
            "    lookup : {\n"
            "      pattern : cc\n"
            "      namespace : binutils\n"
            "    }\n"
            "  }\n"
            "}\0\0"
          }
        , expected : (char[]) {
            "envs : {\n"
            "  cc : {\n"
            "    lookup : {\n"
            "      pattern : cc\n"
            "      namespace : binutils\n"
            "    }\n"
            "  }\n"
            "  ar : {\n"
            "    lookup : {\n"
            "      pattern : ar\n"
            "      namespace : binutils\n"
            "    }\n"
            "  }\n"
            "}\0\0"
          }
      }}
    , (formula_parser_test[]) {{
          text : (char[]) {
            "envs : {\n"
            "  libdeps : {\n"
            "    sequence : [\n"
            "      select : [\n"
            "        module\n"
            "      ]\n"
            "    ]\n"
            "  }\n"
            "}\0\0"
          }
      }}
    , (formula_parser_test[]) {{
          text : (char[]) {
            "envs : {\n"
            "  libdeps : {\n"
            "    sequence : [\n"
            "      select : [\n"
            "        traverse : {\n"
            "          direction : down\n"
            "          graph : depends\n"
            "        }\n"
            "      ]\n"
            "    ]\n"
            "  }\n"
            "  localdeps : {\n"
            "    sequence : [\n"
            "      select : [\n"
            "        traverse : {\n"
            "          direction : up\n"
            "          graph : fs\n"
            "        }\n"
            "      ]\n"
            "    ]\n"
            "  }\n"
            "}\0\0"
          }
        , expected : (char[]) {
            "envs : {\n"
            "  localdeps : {\n"
            "    sequence : [\n"
            "      select : [\n"
            "        traverse : {\n"
            "          direction : up\n"
            "          order : preorder\n"
            "          graph : fs\n"
            "          exhaustive : false\n"
            "          cross-module : true\n"
            "          cross-fstree : true\n"
            "          hidden : false\n"
            "          nofollow : false\n"
            "        }\n"
            "      ]\n"
            "    ]\n"
            "  }\n"
            "  libdeps : {\n"
            "    sequence : [\n"
            "      select : [\n"
            "        traverse : {\n"
            "          direction : down\n"
            "          order : preorder\n"
            "          graph : depends\n"
            "          exhaustive : false\n"
            "          cross-module : true\n"
            "          cross-fstree : true\n"
            "          hidden : false\n"
            "          nofollow : false\n"
            "        }\n"
            "      ]\n"
            "    ]\n"
            "  }\n"
            "}\0\0"
         }
      }}
    , (formula_parser_test[]) {{
          text : (char[]) {
            "envs : {\n"
            "  deps : {\n"
            "    sequence : [\n"
            "      select : [\n"
            "        traverse : {\n"
            "          direction : down\n"
            "        }\n"
            "      ]\n"
            "    ]\n"
            "  }\n"
            "}\0\0"
          }
        , expected : (char[]) {
            "envs : {\n"
            "  deps : {\n"
            "    sequence : [\n"
            "      select : [\n"
            "        traverse : {\n"
            "          direction : down\n"
            "          order : preorder\n"
            "          graph : fs\n"
            "          exhaustive : false\n"
            "          cross-module : true\n"
            "          cross-fstree : true\n"
            "          hidden : false\n"
            "          nofollow : false\n"
            "        }\n"
            "      ]\n"
            "    ]\n"
            "  }\n"
            "}\0\0"
          }
      }}
    , (formula_parser_test[]) {{
          text : (char[]) {
            " args : ["
              " $cflags"
              " sequence : ["
                " select : ["
                  " module"
                  " union : ["
                    " traverse : {"
                      " direction : down"
                      " min-distance : 0"
                      " graph : fs"
                      " visit : directory"
                    " }"
                    " traverse : {"
                      " direction : down"
                      " graph : records"
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
        , expected : (char[]) {
            "args : [\n"
            "  $cflags\n"
            "  sequence : [\n"
            "    select : [\n"
            "      sequence : [\n"
            "        module\n"
            "        union : [\n"
            "          traverse : {\n"
            "            min-distance : 0\n"
            "            direction : down\n"
            "            order : preorder\n"
            "            graph : fs\n"
            "            visit : directory\n"
            "            exhaustive : false\n"
            "            cross-module : true\n"
            "            cross-fstree : true\n"
            "            hidden : false\n"
            "            nofollow : false\n"
            "          }\n"
            "          traverse : {\n"
            "            direction : down\n"
            "            order : preorder\n"
            "            graph : records\n"
            "            exhaustive : false\n"
            "            cross-module : true\n"
            "            cross-fstree : true\n"
            "            hidden : false\n"
            "            nofollow : false\n"
            "          }\n"
            "        ]\n"
            "      ]\n"
            "    ]\n"
            "    property : relpath\n"
            "    prepend : -I\n"
            "  ]\n"
            "  $bm_source\n"
            "  -c\n"
            "  -o\n"
            "  $bm_target\n"
            "]"
            "\0\0"
          }
      }}

    /* duplicates dont leak */
    , (formula_parser_test[]) {{
          text : (char[]) {
            "envs : {\n"
            "  libdeps : {\n"
            "    sequence : [\n"
            "      select : [\n"
            "        module\n"
            "      ]\n"
            "    ]\n"
            "  }\n"
            "}\n"
            "envs : {\n"
            "  libdeps : {\n"
            "    sequence : [\n"
            "      select : [\n"
            "        module\n"
            "      ]\n"
            "    ]\n"
            "  }\n"
            "}\0\0"
          }
        , expected : (char[]) {
            "envs : {\n"
            "  libdeps : {\n"
            "    sequence : [\n"
            "      select : [\n"
            "        module\n"
            "      ]\n"
            "    ]\n"
            "  }\n"
            "}"
          }
      }}
    , (formula_parser_test[]) {{
          text : (char[]) {
            "file : {\n"
            "  lookup : {\n"
            "    pattern : a/b/c\n"
            "    namespace : module\n"
            "  }\n"
            "}\n"
            "file : {\n"
            "  lookup : {\n"
            "    pattern : a/b/c\n"
            "    namespace : module\n"
            "  }\n"
            "}\0\0"
          }
        , expected : (char[]) {
            "file : {\n"
            "  lookup : {\n"
            "    pattern : a/b/c\n"
            "    namespace : module\n"
            "  }\n"
            "}"
          }
      }}
    , (formula_parser_test[]) {{
          text : (char[]) {
            "args : [\n"
            "  lookup : {\n"
            "    pattern : d/e/f\n"
            "    namespace : formula\n"
            "  }\n"
            "]\n"
            "args : [\n"
            "  lookup : {\n"
            "    pattern : d/e/f\n"
            "    namespace : formula\n"
            "  }\n"
            "]\0\0"
          }
        , expected : (char[]) {
            "args : [\n"
            "  lookup : {\n"
            "    pattern : d/e/f\n"
            "    namespace : formula\n"
            "  }\n"
            "]"
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
        , expected_result : 2
      }}
    , 0
  }
};
XUNIT_UNIT(xunit);
