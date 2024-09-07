/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "lorien/load.h"
#include "bacon/load.h"


#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/growing.h"

#include "configset_parser/glue.h"
#include "configset_parser.h"
#include "configset.h"
#include "config.h"
#include "request.h"
#include "fsent.h"

typedef struct {
  XUNITTEST;

  char ** texts;
  char *expected;
  config * cfg;
} configset_parser_test;

static void configset_parser_test_unit_setup(xunit_unit * unit)
{
  bacon_load();
}

static void configset_parser_test_unit_cleanup(xunit_unit * unit)
{
  bacon_unload();
}

static void configset_parser_test_entry(xunit_test * _test)
{
  configset_parser_test * test = containerof(_test, configset_parser_test, xu);

  configset_parser * parser = 0;
  config *cfg = 0;
  config *B = 0;
  narrator_growing * N1 = 0;
  size_t len;
  char **text;
  const char *names[] = { "(first)", "(second)", "(third)" };
  const char **name;

  // arrange
  configset_parser_create(&parser);
  narrator_growing_create(&N1);

  /* parse all the texts */
  config_alloc(&cfg);
  name = names;
  for(text = test->texts; *text; text++, name++) {
    if(configset_parser_parse(parser, *text, strlen(*text) + 2, *name, 0, cfg) != 0) {
      tracef("%.*s", (int)parser->yyu.error_len, parser->yyu.error_str);
      RTA;
    }

    // round-trip parse-ability at each step
    narrator_reset(&N1->base);
    config_say(cfg, &N1->base);

    if((len = N1->l)) {
      config_free(B, parser);
      config_alloc(&B);
      N1->s[N1->l + 0] = 0;
      N1->s[N1->l + 1] = 0;
      if(configset_parser_parse(parser, N1->s, N1->l + 2, *name, 0, B) != 0) {
        tracef("%.*s", (int)parser->yyu.error_len, parser->yyu.error_str);
        RTA;
      }
    }
  }

  narrator_reset(&N1->base);

  config_say(cfg, &N1->base);
  assert_eq_w(test->expected, strlen(test->expected), N1->s, N1->l);

  config_free(cfg, parser);
  config_free(B, parser);
  configset_parser_free(parser);
  narrator_growing_free(N1);
}

//
// manifest
//

static xunit_unit xunit = {
    xu_setup : configset_parser_test_unit_setup
  , xu_cleanup : configset_parser_test_unit_cleanup
  , xu_entry : configset_parser_test_entry
  , xu_tests : (configset_parser_test*[]) {
      (configset_parser_test[]) {{
          texts : (char*[]) {
            (char[]) {
              "\0\0"
            }
          , 0
        }
        , expected : ""
      }}
    , (configset_parser_test[]) {{
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
    , (configset_parser_test[]) {{
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
    , (configset_parser_test[]) {{
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
    , (configset_parser_test[]) {{
          texts : (char*[]) {
            (char[]) {
              "filesystem : {"
              "  fstrees : {"
              "    /usr/lib/fab/builtin"
              "    /home/todd/my"
              " }"
              "}\0\0"
            }
          , 0
        }
        , expected : ""
              "filesystem : {\n"
              "  fstrees : {\n"
              "    /home/todd/my                                            # @ (first) [1,56 1,69]\n"
              "    /usr/lib/fab/builtin                                     # @ (first) [1,32 1,52]\n"
              "  }\n"
              "}"
          , 0
      }}
    , (configset_parser_test[]) {{
          texts : (char*[]) {
            (char[]) {
              "filesystem : {"
              "  fstrees : {"
              "    /usr/lib/fab/builtin"
              "    /home/todd/my"
              " }"
              "}\0\0"
            }
          , (char[]) {
              "filesystem : {"
              "  exclude : {"
              "    build"
              " }"
              "}\0\0"
            }
          , 0
        }
        , expected : ""
            "filesystem : {\n"
            "  fstrees : {\n"
            "    /home/todd/my                                            # @ (first) [1,56 1,69]\n"
            "    /usr/lib/fab/builtin                                     # @ (first) [1,32 1,52]\n"
            "  }\n"
            "  exclude : {\n"
            "    build                                                    # @ (second) [1,32 1,37]\n"
            "  }\n"
            "}"
          , 0
      }}
    , (configset_parser_test[]) {{
          texts : (char*[]) {
            (char[]) {
              "filesystem : {"
              "  exclude : {"
              "    .git"
              " }"
              "}\0\0"
            }
          , 0
        }
        , expected : ""
              "filesystem : {\n"
              "  exclude : {\n"
              "    .git                                                     # @ (first) [1,32 1,36]\n"
              "  }\n"
              "}"
      }}
    , (configset_parser_test[]) {{
          texts : (char*[]) {
            (char[]) {
              "filesystem : {"
              "  exclude : {"
              "    .git"
              " }"
              "}\0\0"
            }
          , (char[]) {
              "filesystem : {"
              "  exclude : {"
              "    .svn"
              " }"
              "}\0\0"
            }
          , (char[]) {
              "filesystem : {"
              "  exclude : {"
              "    .hg"
              " }"
              "}\0\0"
            }
          , 0
        }
        , expected : ""
              "filesystem : {\n"
              "  exclude : {\n"
              "    .git                                                     # @ (first) [1,32 1,36]\n"
              "    .svn                                                     # @ (second) [1,32 1,36]\n"
              "    .hg                                                      # @ (third) [1,32 1,35]\n"
              "  }\n"
              "}"
      }}
    , (configset_parser_test[]) {{
          texts : (char*[]) {
            (char[]) {
              "filesystem : {"
              "  exclude : {"
              "    .git"
              " }"
              "}\0\0"
            }
          , (char[]) {
              "filesystem : {"
              "  exclude : {"
              "    .svn"
              " }"
              "}\0\0"
            }
          , (char[]) {
              "filesystem : {"
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
              "filesystem : {\n"
              "  exclude : {\n"
              "    .git                                                     # @ (third) [1,32 1,36]\n"
              "    .svn                                                     # @ (third) [1,40 1,44]\n"
              "    .hg                                                      # @ (third) [1,48 1,51]\n"
              "  }\n"
              "}"
      }}
    , (configset_parser_test[]) {{
          texts : (char*[]) {
            (char[]) {
              "filesystem : {"
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
              "filesystem : {\n"
              "  special : {\n"
              "    var.bam.debug                                            # @ (first) [1,32 1,45]\n"
              "    var.bam.trace                                            # @ (first) [1,83 1,96]\n"
              "    var.bam.final                                            # @ (first) [1,66 1,79]\n"
              "    var.bam.devel                                            # @ (first) [1,49 1,62]\n"
              "  }\n"
              "}"
      }}
   , (configset_parser_test[]) {{
          texts : (char*[]) {
            (char[]) {
              "filesystem : {"
              "  fstrees : {"
              "    / : {"
              "      invalidation : notify"
              "    }"
              "    /home : {"
              "      invalidation : stat"
              "    }"
              "  }"
              "}\0\0"
            }
          , 0
        }
      , expected : ""
              "filesystem : {\n"
              "  fstrees : {\n"
              "    /home : {\n"
              "      invalidation : stat                                    # @ (first) [1,103 1,107]\n"
              "    }\n"
              "    / : {\n"
              "      invalidation : notify                                  # @ (first) [1,58 1,64]\n"
              "    }\n"
              "  }\n"
              "}"
      }}
    , (configset_parser_test[]) {{
          texts : (char*[]) {
            (char[]) {
              "filesystem : {"
              "  fstrees : {"
              "    / : {"
              "      invalidation : notify"
              "      readlink : true"
              "    }"
              "    /home : {"
              "      invalidation : stat"
              "    }"
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
            "  concurrency : 0                                            # @ (first) [1,162 1,163]\n"
            "}\n"
            "filesystem : {\n"
            "  fstrees : {\n"
            "    /home : {\n"
            "      invalidation : stat                                    # @ (first) [1,124 1,128]\n"
            "    }\n"
            "    / : {\n"
            "      invalidation : notify                                  # @ (first) [1,58 1,64]\n"
            "      readlink : true                                        # @ (first) [1,81 1,85]\n"
            "    }\n"
            "  }\n"
            "}"
      }}
    , (configset_parser_test[]) {{
          texts : (char*[]) {
            (char[]) {
              "filesystem : {"
              "  fstrees : {"
              "    / : {"
              "      invalidation : notify"
              "    }"
              "  }"
              "}"
              "\0\0"
            }
          , (char[]) {
              "filesystem : {"
              "  fstrees : {"
              "    / : {"
              "      invalidation : notify"
              "    }"
              "  }"
              "}"
              "\0\0"
            }
          , 0
        }
        , expected : ""
            "filesystem : {\n"
            "  fstrees : {\n"
            "    / : {\n"
            "      invalidation : notify                                  # @ (second) [1,58 1,64]\n"
            "    }\n"
            "  }\n"
            "}"
      }}
    , (configset_parser_test[]) {{
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
    , (configset_parser_test[]) {{
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
    , (configset_parser_test[]) {{
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
    , (configset_parser_test[]) {{
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
    , (configset_parser_test[]) {{
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
    , (configset_parser_test[]) {{
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
