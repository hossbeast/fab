/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "lorien/load.h"
#include "bacon/load.h"


#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/growing.h"

#include "config_parser/glue.h"
#include "config_parser.h"
#include "config.h"
#include "configuration.h"
#include "request.h"
#include "fsent.h"

typedef struct {
  XUNITTEST;

  char ** texts;
  char *expected;
  config * cfg;
} config_parser_test;

static void config_parser_test_unit_setup(xunit_unit * unit)
{
  bacon_load();
}

static void config_parser_test_unit_cleanup(xunit_unit * unit)
{
  bacon_unload();
}

static void config_parser_test_entry(xunit_test * _test)
{
  config_parser_test * test = containerof(_test, config_parser_test, xu);

  config_parser * parser = 0;
  configuration *cfg = 0;
  configuration *B = 0;
  narrator_growing * N1 = 0;
  size_t len;
  char **text;
  const char *names[] = { "(first)", "(second)", "(third)" };
  const char **name;

  // arrange
  config_parser_create(&parser);
  narrator_growing_create(&N1);

  /* parse all the texts */
  configuration_alloc(&cfg);
  name = names;
  for(text = test->texts; *text; text++, name++) {
    if(config_parser_parse(parser, *text, strlen(*text) + 2, *name, 0, cfg) != 0) {
      tracef("%.*s", (int)parser->yyu.error_len, parser->yyu.error_str);
      RTA;
    }

    // round-trip parse-ability at each step
    narrator_reset(&N1->base);
    configuration_say(cfg, &N1->base);

    if((len = N1->l)) {
      configuration_free(B, parser);
      configuration_alloc(&B);
      N1->s[N1->l + 0] = 0;
      N1->s[N1->l + 1] = 0;
      if(config_parser_parse(parser, N1->s, N1->l + 2, *name, 0, B) != 0) {
        tracef("%.*s", (int)parser->yyu.error_len, parser->yyu.error_str);
        RTA;
      }
    }
  }

  narrator_reset(&N1->base);

  configuration_say(cfg, &N1->base);
  assert_eq_w(test->expected, strlen(test->expected), N1->s, N1->l);

  configuration_free(cfg, parser);
  configuration_free(B, parser);
  config_parser_free(parser);
  narrator_growing_free(N1);
}

//
// manifest
//

static xunit_unit xunit = {
    xu_setup : config_parser_test_unit_setup
  , xu_cleanup : config_parser_test_unit_cleanup
  , xu_entry : config_parser_test_entry
  , xu_tests : (config_parser_test*[]) {
      (config_parser_test[]) {{
          texts : (char*[]) {
            (char[]) {
              "\0\0"
            }
          , 0
        }
        , expected : ""
      }}
    , (config_parser_test[]) {{
          texts : (char*[]) {
            (char[]) {
              "build : {"
              " concurrency : 0"
              "}"
              "\0\0"
            }
          , 0
        }
        , expected : ""
              "build : {\n"
              "  concurrency : 0                                            # @ (first) [1,25 1,26]\n"
              "}"
      }}
    , (config_parser_test[]) {{
          texts : (char*[]) {
            (char[]) {
              "build : {"
              " concurrency : 42"
              "}"
              "\0\0"
            }
          , (char[]) {
              "build : {"
              " concurrency : 0"
              "}"
              "\0\0"
            }
          , 0
        }
        , expected : ""
              "build : {\n"
              "  concurrency : 0                                            # @ (second) [1,25 1,26]\n"
              "}"
      }}
    , (config_parser_test[]) {{
          texts : (char*[]) {
            (char[]) {
              "build : {"
              " concurrency : 42"
              "}"
              "\0\0"
            }
          , 0
         }
        , expected : ""
              "build : {\n"
              "  concurrency : 42                                           # @ (first) [1,25 1,27]\n"
              "}"
      }}
    , (config_parser_test[]) {{
          texts : (char*[]) {
            (char[]) {
              "walker : {"
              "  include : {"
              "    /usr/lib/fab/builtin"
              "    /home/todd/my"
              " }"
              "}\0\0"
            }
          , 0
        }
        , expected : ""
              "walker : {\n"
              "  include : {\n"
              "    /home/todd/my                                            # @ (first) [1,52 1,65]\n"
              "    /usr/lib/fab/builtin                                     # @ (first) [1,28 1,48]\n"
              "  }\n"
              "}"
          , 0
      }}
    , (config_parser_test[]) {{
          texts : (char*[]) {
            (char[]) {
              "walker : {"
              "  include : {"
              "    /usr/lib/fab/builtin"
              "    /home/todd/my"
              " }"
              "}\0\0"
            }
          , (char[]) {
              "walker : {"
              "  exclude : {"
              "    build"
              " }"
              "}\0\0"
            }
          , 0
        }
        , expected : ""
            "walker : {\n"
            "  include : {\n"
            "    /home/todd/my                                            # @ (first) [1,52 1,65]\n"
            "    /usr/lib/fab/builtin                                     # @ (first) [1,28 1,48]\n"
            "  }\n"
            "  exclude : {\n"
            "    build                                                    # @ (second) [1,28 1,33]\n"
            "  }\n"
            "}"
          , 0
      }}
    , (config_parser_test[]) {{
          texts : (char*[]) {
            (char[]) {
              "walker : {"
              "  exclude : {"
              "    .git"
              " }"
              "}\0\0"
            }
          , 0
        }
        , expected : ""
              "walker : {\n"
              "  exclude : {\n"
              "    .git                                                     # @ (first) [1,28 1,32]\n"
              "  }\n"
              "}"
      }}
    , (config_parser_test[]) {{
          texts : (char*[]) {
            (char[]) {
              "walker : {"
              "  exclude : {"
              "    .git"
              " }"
              "}\0\0"
            }
          , (char[]) {
              "walker : {"
              "  exclude : {"
              "    .svn"
              " }"
              "}\0\0"
            }
          , (char[]) {
              "walker : {"
              "  exclude : {"
              "    .hg"
              " }"
              "}\0\0"
            }
          , 0
        }
        , expected : ""
              "walker : {\n"
              "  exclude : {\n"
              "    .git                                                     # @ (first) [1,28 1,32]\n"
              "    .svn                                                     # @ (second) [1,28 1,32]\n"
              "    .hg                                                      # @ (third) [1,28 1,31]\n"
              "  }\n"
              "}"
      }}
    , (config_parser_test[]) {{
          texts : (char*[]) {
            (char[]) {
              "walker : {"
              "  exclude : {"
              "    .git"
              " }"
              "}\0\0"
            }
          , (char[]) {
              "walker : {"
              "  exclude : {"
              "    .svn"
              " }"
              "}\0\0"
            }
          , (char[]) {
              "walker : {"
              "  exclude : {"
              "    .git"
              "    .svn"
              "    .hg"
              " }"
              "}\0\0"
            }
          , 0
        }
        , expected : ""
              "walker : {\n"
              "  exclude : {\n"
              "    .git                                                     # @ (third) [1,28 1,32]\n"
              "    .svn                                                     # @ (third) [1,36 1,40]\n"
              "    .hg                                                      # @ (third) [1,44 1,47]\n"
              "  }\n"
              "}"
      }}
    , (config_parser_test[]) {{
          texts : (char*[]) {
            (char[]) {
              "walker : {"
              "  special : {"
              "    var.bam.debug"
              "    var.bam.devel"
              "    var.bam.final"
              "    var.bam.trace"
              " }"
              "}\0\0"
            }
          , 0
        }
        , expected : ""
              "walker : {\n"
              "  special : {\n"
              "    var.bam.debug                                            # @ (first) [1,28 1,41]\n"
              "    var.bam.trace                                            # @ (first) [1,79 1,92]\n"
              "    var.bam.final                                            # @ (first) [1,62 1,75]\n"
              "    var.bam.devel                                            # @ (first) [1,45 1,58]\n"
              "  }\n"
              "}"
      }}
   , (config_parser_test[]) {{
          texts : (char*[]) {
            (char[]) {
              "filesystems : {"
              "  \"/\" : {"
              "    invalidate : notify"
              "  }"
              "  /home : {"
              "    invalidate : stat"
              "  }"
              "}\0\0"
            }
          , 0
        }
      , expected : ""
              "filesystems : {\n"
              "  /home : {\n"
              "    invalidate : stat                                        # @ (first) [1,79 1,83]\n"
              "  }\n"
              "  / : {\n"
              "    invalidate : notify                                      # @ (first) [1,42 1,48]\n"
              "  }\n"
              "}"
      }}
    , (config_parser_test[]) {{
          texts : (char*[]) {
            (char[]) {
              "filesystems : {"
              "  \"/\" : {"
              "    invalidate : notify"
              "    readlinks : true"
              "  }"
              "  /home : {"
              "    invalidate : stat"
              "  }"
              "}"
              "build : {"
              "  concurrency : 0"
              "}"
              "\0\0"
            }
          , 0
        }
        , expected : ""
            "build : {\n"
            "  concurrency : 0                                            # @ (first) [1,132 1,133]\n"
            "}\n"
            "filesystems : {\n"
            "  /home : {\n"
            "    invalidate : stat                                        # @ (first) [1,99 1,103]\n"
            "  }\n"
            "  / : {\n"
            "    invalidate : notify                                      # @ (first) [1,42 1,48]\n"
            "    readlinks : true                                         # @ (first) [1,64 1,68]\n"
            "  }\n"
            "}"
      }}
    , (config_parser_test[]) {{
          texts : (char*[]) {
            (char[]) {
              "filesystems : {"
              "  / : {"
              "    invalidate : notify"
              "  }"
              "}"
              "\0\0"
            }
          , (char[]) {
              "filesystems : {"
              "  / : {"
              "    invalidate : notify"
              "  }"
              "}"
              "\0\0"
            }
          , 0
        }
        , expected : ""
              "filesystems : {\n"
              "  / : {\n"
              "    invalidate : notify                                      # @ (second) [1,40 1,46]\n"
              "  }\n"
              "}"
      }}
    , (config_parser_test[]) {{
          texts : (char*[]) {
            (char[]) {
              "formula : {"
              "  path : {"
              "    dirs : {"
              "      /usr/bin"
              "    }"
              "  }"
              "}"
              "\0\0"
            }
          , 0
        }
        , expected : ""
              "formula : {\n"
              "  path : {\n"
              "    dirs : {\n"
              "      /usr/bin                                               # @ (first) [1,40 1,48]\n"
              "    }\n"
              "  }\n"
              "}"
      }}
    , (config_parser_test[]) {{
          texts : (char*[]) {
            (char[]) {
              "formula : {"
              "  path : {"
              "    dirs : {"
              "    }"
              "  }"
              "}"
              "\0\0"
            }
          , 0
        }
        , expected : ""
      }}
    , (config_parser_test[]) {{
          texts : (char*[]) {
            (char[]) {
              "formula : {"
              "  path : {"
              "  }"
              "}"
              "\0\0"
            }
          , 0
        }
        , expected : ""
      }}
    , (config_parser_test[]) {{
          texts : (char*[]) {
            (char[]) {
              "formula : {"
              "}"
              "\0\0"
            }
          , 0
        }
        , expected : ""
      }}
    , (config_parser_test[]) {{
          texts : (char*[]) {
            (char[]) {
              "formula : {"
              "  path : {"
              "    copy-from-env : true"
              "  }"
              "}"
              "\0\0"
            }
          , 0
        }
        , expected : ""
              "formula : {\n"
              "  path : {\n"
              "    copy-from-env : true                                     # @ (first) [1,42 1,46]\n"
              "  }\n"
              "}"
      }}
    , (config_parser_test[]) {{
          texts : (char*[]) {
            (char[]) {
              "build : {"
              " concurrency : 42"
              "}"
              "\0\0"
            }
          , (char[]) {
              "build : {"
              "}"
              "formula : {"
              "  path : {"
              "    copy-from-env : true"
              "  }"
              "}"
              "\0\0"
            }
          , 0
        }
        , expected : ""
            "build : {\n"
            "  concurrency : 42                                           # @ (first) [1,25 1,27]\n"
            "}\n"
            "formula : {\n"
            "  path : {\n"
            "    copy-from-env : true                                     # @ (second) [1,52 1,56]\n"
            "  }\n"
            "}"
      }}
    , 0
  }
};
XUNIT_UNIT(xunit);
