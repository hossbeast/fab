/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "lorien/load.h"
#include "bacon/load.h"
#include "bacon/value.h"

#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/growing.h"

#include "mak_parser/glue.h"
#include "mak_parser.h"

typedef struct {
  XUNITTEST;

  char ** texts;
  char *expected;
} mak_parser_test;

static void mak_parser_test_unit_setup(xunit_unit * unit)
{
  bacon_load();
}

static void mak_parser_test_unit_cleanup(xunit_unit * unit)
{
  bacon_unload();
}

static void mak_parser_test_entry(xunit_test * _test)
{
  mak_parser_test *test = containerof(_test, mak_parser_test, xu);

  mak_parser *parser = 0;
  narrator_growing * N1 = 0;
  char **text;
  const char *names[] = { "-first-", "-second-", "-third-" };
  const char **name;
  bacon_value *root;

  // arrange
  parser = mak_parser_create();
  narrator_growing_create(&N1);

  /* parse all the texts */
  name = names;
  for(text = test->texts; *text; text++, name++) {
    if(mak_parser_parse(parser, *text, strlen(*text), *name, &root) != 0) {
      tracef("%.*s", (int)parser->yyu.error_len, parser->yyu.error_str);
      RTA;
    }
  }

  bacon_value_say(root, &N1->base);
  assert_eq_w(test->expected, strlen(test->expected), N1->s, N1->l);

  bacon_value_free(parser->bacon_parser, root);
  mak_parser_free(parser);
  narrator_growing_free(N1);
}

//
// manifest
//

static xunit_unit xunit = {
    xu_setup : mak_parser_test_unit_setup
  , xu_cleanup : mak_parser_test_unit_cleanup
  , xu_entry : mak_parser_test_entry
  , xu_tests : (mak_parser_test*[]) {
      (mak_parser_test[]) {{
          texts : (char*[]) {
            (char[]) { }
          , 0
        }
        , expected : "-none-"
      }}
    , (mak_parser_test[]) {{
          texts : (char*[]) {
            (char[]) {
              "BINDIR=/usr/bin\n"
              "RUNDIR=/var/run\n"
              "SHAREDIR=/usr/share\n"
              "ETCDIR=/etc\n"
            }
          , 0
        }
        , expected : ""
            "{\n"
            "  ETCDIR : /etc\n"
            "  BINDIR : /usr/bin\n"
            "  SHAREDIR : /usr/share\n"
            "  RUNDIR : /var/run\n"
            "}"
      }}
      /* value with spaces */
    , (mak_parser_test[]) {{
          texts : (char*[]) {
            (char[]) {
              "BINDIR=/usr/bin /etc/bam\n"
            }
          , 0
        }
        , expected : ""
            "{\n"
            "  BINDIR : \"/usr/bin /etc/bam\"\n"
            "}"
      }}

      /* value with equals */
    , (mak_parser_test[]) {{
          texts : (char*[]) {
            (char[]) {
              "BINDIR=/usr/bin=/etc/bam\n"
            }
          , 0
        }
        , expected : ""
            "{\n"
            "  BINDIR : /usr/bin=/etc/bam\n"
            "}"
      }}

      /* value with comma */
    , (mak_parser_test[]) {{
          texts : (char*[]) {
            (char[]) {
              "BINDIR=/usr/bin,/etc/bam\n"
            }
          , 0
        }
        , expected : ""
            "{\n"
            "  BINDIR : \"/usr/bin,/etc/bam\"\n"
            "}"
      }}

      /* empty value, single letter name */
    , (mak_parser_test[]) {{
          texts : (char*[]) {
            (char[]) {
              "A=1\n"
              "C=\n"
            }
          , 0
        }
        , expected : ""
            "{\n"
            "  A : \"1\"\n"
            "}"
      }}
    , (mak_parser_test[]) {{
          texts : (char*[]) {
            (char[]) {
              "C=\n"
            }
          , 0
        }
        , expected : "-none-"
      }}

      /* comments */
    , (mak_parser_test[]) {{
          texts : (char*[]) {
            (char[]) {
              "BINDIR=/usr/bin,/etc/bam\n"
              "# BIN=/bin\n"
            }
          , 0
        }
        , expected : ""
            "{\n"
            "  BINDIR : \"/usr/bin,/etc/bam\"\n"
            "}"
      }}
    , 0
  }
};
XUNIT_UNIT(xunit);
