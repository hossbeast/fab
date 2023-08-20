/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "lorien/load.h"
#include "bacon/load.h"

#include "valyria/dictionary.h"

#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"

#include "filesystem.internal.h"
#include "config_parser.h"
#include "config.internal.h"

typedef struct {
  XUNITTEST;

  char * config;
  char * filesystems;
} filesystem_test;

static void filesystem_test_unit_setup(xunit_unit * unit)
{
  // load libraries
  lorien_load();
  bacon_load();

  filesystem_setup();
}

static void filesystem_test_unit_cleanup(xunit_unit * unit)
{
  lorien_unload();
  bacon_unload();

  filesystem_cleanup();
}

static void filesystem_test_entry(xunit_test * _test)
{
  filesystem_test * test = (filesystem_test *)_test;

  config_parser * parser = 0;
  configblob * cfg = 0;
  char space[1024];

  // arrange
  config_parser_create(&parser);
  config_parser_parse(parser, test->config, strlen(test->config) + 2, 0, 0, &cfg);

  cfg->filesystems.changed = true;

  // act
  filesystem_cleanup();
  filesystem_setup();

  filesystem_reconfigure(cfg, space, sizeof(space));
  filesystem_reconfigure(cfg, 0, 0);

  fstree_znload(space, sizeof(space));
  assert_eq_s(test->filesystems, space);

  config_parser_xfree(parser);
  config_xfree(cfg);
}

//
// manifest
//

static xunit_unit xunit = {
    xu_setup : filesystem_test_unit_setup
  , xu_cleanup : filesystem_test_unit_cleanup
  , xu_entry : filesystem_test_entry
  , xu_tests : (void*)(filesystem_test*[]) {
      (filesystem_test[]) {{
          config : (char[]) { "\0\0" }
        , filesystems :
              "/ : (none)"
            "\n"
      }}
    , (filesystem_test[]) {{
          config : (char[]) {
            "filesystems : {"
              "\"/\" : { invalidate : stat } "
            "}\0\0"
          }
        , filesystems :
              "/ : stat"
            "\n"
      }}
    , (filesystem_test[]) {{
          config : (char[]) {
            "filesystems : {"
              "\"/\" : { invalidate : content } "
              "\"/A\" : { invalidate : notify } "
            "}\0\0"
          }
        , filesystems :
              "/ : content"
            "\n A : notify"
            "\n"
      }}
    , (filesystem_test[]) {{
          config : (char[]) {
            "filesystems : {"
              "\"/\" : { invalidate : content } "
              "\"/A\" : { invalidate : notify } "
              "\"/B\" : { invalidate : stat } "
            "}\0\0"
          }
        , filesystems :
              "/ : content"
            "\n A : notify"
            "\n B : stat"
            "\n"
      }}
    , (filesystem_test[]) {{
          config : (char[]) {
            "filesystems : {"
              "\"/\" : { invalidate : content } "
              "\"/A\" : { invalidate : notify } "
              "\"/A/B\" : { invalidate : stat } "
            "}\0\0"
          }
        , filesystems :
              "/ : content"
            "\n A : notify"
            "\n  B : stat"
            "\n"
      }}
    , (filesystem_test[]) {{
          config : (char[]) {
            "filesystems : {"
              "\"/\" : { invalidate : content } "
              "\"/A\" : { invalidate : notify } "
              "\"/A/B/C\" : { invalidate : stat } "
            "}\0\0"
          }
        , filesystems :
              "/ : content"
            "\n A : notify"
            "\n  B"
            "\n   C : stat"
            "\n"
      }}
    , (filesystem_test[]) {{
          config : (char[]) {
            "filesystems : {"
              "\"/A\" : { invalidate : notify } "
            "}\0\0"
          }
        , filesystems :
              "/ : (none)"
            "\n A : notify"
            "\n"
      }}
    , (filesystem_test[]) {{
          config : (char[]) {
            "filesystems : {"
              "\"/A\" : { invalidate : notify } "
              "\"/B\" : { invalidate : content } "
            "}\0\0"
          }
        , filesystems :
              "/ : (none)"
            "\n A : notify"
            "\n B : content"
            "\n"
      }}
    , (filesystem_test[]) {{
          config : (char[]) {
            "filesystems : {"
              "\"/A\"      : { invalidate : content } "
              "\"/A/BB/C\" : { invalidate : notify } "
              "\"/A/B\"    : { invalidate : stat } "
            "}\0\0"
          }
        , filesystems :
              "/ : (none)"
            "\n A : content"
            "\n  B : stat"
            "\n  BB"
            "\n   C : notify"
            "\n"
      }}
    , 0
  }
};
XUNIT_UNIT(xunit);
