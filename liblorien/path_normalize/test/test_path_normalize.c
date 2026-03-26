/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdlib.h>


#include "xunit.h"
#include "xunit/assert.h"

#include "path_normalize.h"
#include "macros.h"

typedef struct path_test {
  XUNITTEST;

  char *input;
  uint16_t input_len;
  char *output;
  char *base;
} path_test;

static void path_test_entry(xunit_test * _test)
{
  char space[PATH_MAX];
  path_test *test;
  size_t len;
  uint16_t input_len;
  path_normalize_context ctx;

  test = containerof(_test, path_test, xu);
  if((input_len = test->input_len) == 0) {
    input_len = strlen(test->input);
  }
  memset(&ctx, 0, sizeof(ctx));
  if((ctx.base = test->base)) {
    ctx.base_len = strlen(ctx.base);
  }
  len = path_normalize(&ctx, space, sizeof(space), test->input, input_len);

  assert_eq_w(test->output, strlen(test->output), space, len);
}

//
// public
//

static xunit_unit xunit = {
    xu_entry : path_test_entry
  , xu_tests : (void*)(path_test*[]) {
      /* redundant slashes */
        (path_test[]){{
            input : "//foo"
          , output : "/foo"
        }}
      , (path_test[]){{
            input : "//foo//bar"
          , output : "/foo/bar"
        }}
      , (path_test[]){{
            input : "foo//bar"
          , output : "./foo/bar"
        }}

      /* leading dot-dot */
      , (path_test[]){{
            base : "/a"
          , input : "../b"
          , output : "/b"
        }}
      , (path_test[]){{
            input : "../b"
          , output : "../b"
        }}

      /* internal dot-dot - with base */
      , (path_test[]){{
            base : "/a"
          , input : "a/b/c/../b"
          , output : "/a/a/b/b"
        }}
      , (path_test[]){{
            base : "/a"
          , input : "a0/b1/c2/../b3"
          , output : "/a/a0/b1/b3"
        }}
      , (path_test[]){{
            base : "/a"
          , input : "a/b/c/../../b"
          , output : "/a/a/b"
        }}
      , (path_test[]){{
            base : "/a"
          , input : "a/b/c/../../../b"
          , output : "/a/b"
        }}

      /* internal dot-dot */
      , (path_test[]){{
            input : "a/b/c/../b"
          , output : "./a/b/b"
        }}
      , (path_test[]){{
            input : "a0/b1/c2/../b3"
          , output : "./a0/b1/b3"
        }}
      , (path_test[]){{
            input : "a/b/c/../../b"
          , output : "./a/b"
        }}
      , (path_test[]){{
            input : "a/b/c/../../../b"
          , output : "./b"
        }}

      /* internal dot-dot */
      , (path_test[]){{
            input : "foo/../bar.qux"
          , output : "./bar.qux"
        }}
      , (path_test[]){{
            input : "alpha/omega/../../bar.baz.qux"
          , output : "./bar.baz.qux"
        }}
      , (path_test[]){{
            base : "/CWD"
          , input : "alpha/omega/../../../bar"
          , output : "/bar"
        }}
      , (path_test[]){{
            base : "/CWD"
          , input : "/alpha/omega/../../../bar"
          , output : "/bar"
        }}
      , (path_test[]){{
            input : "/alpha/omega/../../../bar"
          , output : "/bar"
        }}
      , (path_test[]){{
            input : "alpha/omega/../../../bar"
          , output : "../bar"
        }}

      /* leading single-dot */
      , (path_test[]){{
            input : "./bar.baz.qux"
          , output : "./bar.baz.qux"
        }}
      , (path_test[]){{
            base : "/base"
          , input : "./bar.baz.qux"
          , output : "/base/bar.baz.qux"
        }}
      , (path_test[]){{
            base : "/root/base"
          , input : "./../bar.baz.qux"
          , output : "/root/bar.baz.qux"
        }}
      , (path_test[]){{
            base : "/base"
          , input : "./../bar.baz.qux"
          , output : "/base/bar.baz.qux"
        }}
      , (path_test[]){{
            input : "./../bar.baz.qux"
          , output : "../bar.baz.qux"
        }}

      /* internal dot */
      , (path_test[]){{
            base : "/base"
          , input : "bar/baz/./qux"
          , output : "/base/bar/baz/qux"
        }}
      , (path_test[]){{
            input : "bar/baz/./qux"
          , output : "./bar/baz/qux"
        }}

      /* relative path */
      , (path_test[]){{
            input : "foo/bar.baz.qux"
          , output : "./foo/bar.baz.qux"
        }}
      , (path_test[]){{
            base : "/CWD"
          , input : "foo/bar.baz.qux"
          , output : "/CWD/foo/bar.baz.qux"
        }}

      /* absolute paths */
      , (path_test[]){{
            input : "/foo/bar.baz"
          , output : "/foo/bar.baz"
        }}
      , (path_test[]){{
            input : "/bar/../baz.c"
          , output : "/baz.c"
        }}
      , (path_test[]){{
            input : "/bar/baz/../tez"
          , output : "/bar/tez"
        }}
      , (path_test[]){{
            input : "/bar/baz/../../tez"
          , output : "/tez"
        }}
      , (path_test[]){{
            input : "/bar/baz/../../../tez"
          , output : "/tez"
        }}

      /* edge cases */
      , (path_test[]){{
            input : "/../bar.baz.qux"
          , output : "/bar.baz.qux"
        }}
      , (path_test[]){{
            input : "/../../bar"
          , output : "/bar"
        }}

      /* the root */
      , (path_test[]){{
            input : "/"
          , output : "/"
        }}
      , (path_test[]){{
            input : "//"
          , output : "/"
        }}

      /* input not null-terminated */
      , (path_test[]){{
            input : "abcd"
          , input_len : 3
          , output : "./abc"
        }}
      , 0
    }
};
XUNIT_UNIT(xunit);
