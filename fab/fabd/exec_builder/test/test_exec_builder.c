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
#include "valyria/load.h"
#include "moria/load.h"
#include "xlinux/xstdlib.h"
#include "value/load.h"

#include "valyria/list.h"
#include "valyria/llist.h"
#include "valyria/map.h"
#include "valyria/set.h"
#include "value.h"
#include "value/parser.h"
#include "moria/graph.h"
#include "moria/operations.h"

#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/growing.h"
#include "logging.h"
#include "rule.h"
#include "fsent.h"
#include "exec.h"
#include "pattern.h"
#include "pattern_parser.h"
#include "module.internal.h"
#include "exec_builder.h"
#include "exec_render.h"
#include "variant.h"
#include "formula_parser.internal.h"
#include "formula_value.h"
#include "var.h"
#include "build_slot.h"
#include "dependency.h"
#include "channel.h"

typedef struct exec_builder_test {
  XUNITTEST;

  // inputs
  char * vars;          // value text
  char * fml;           // formula bacon text
  char * variant;

  struct env_addf_args {
    const char * name_fmt;
    const char * val_fmt;
    void ** args;
  } **env_addf_args;

  // outputs
  char * file;
  char ** args;
  char ** envs;
} exec_builder_test;

static xapi exec_builder_test_unit_setup(xunit_unit * unit)
{
  enter;

  fatal(value_load);
  fatal(logging_finalize);

  finally : coda;
}

static xapi exec_builder_test_unit_cleanup(xunit_unit * unit)
{
  enter;

  fatal(value_unload);

  finally : coda;
}

//
// tests
//

static xapi exec_builder_test_entry(xunit_test * _test)
{
  enter;

  exec_builder_test * test = containerof(_test, exec_builder_test, xu);
  exec *e;
  value * val;
  value_parser * parser = 0;
  formula_parser * fml_parser = 0;
  int x;
  exec_builder builder;
  exec_render_context renderer;
  struct env_addf_args ** argsp;
  struct env_addf_args * args;
  size_t nargs;
  formula_value * fml_file = 0;
  formula_value * fml_args = 0;
  formula_value * fml_envs = 0;
  variant * var = 0;
  value * vars = 0;
  module mod = { 0 };
  dependency bpe = { 0 };
  build_slot bs = { .bpe = &bpe };
  channel *chan;

  fatal(value_parser_create, &parser);
  fatal(formula_parser_create, &fml_parser);
  fatal(exec_builder_xinit, &builder);
  fatal(exec_render_context_xinit, &renderer);
  fatal(xmalloc, &chan, sizeof(*chan));

  // arrange
  if(test->variant)
    fatal(variant_get, test->variant, strlen(test->variant), &var);

  // act
  if(test->vars)
  {
    fatal(value_parser_parse, parser, test->vars, strlen(test->vars), "-test-", VALUE_TYPE_SET, &val);
    fatal(var_denormalize, parser, var, val, &vars);

    exec_render_context_configure(&renderer, &builder, 0, vars, 0, chan);;
    fatal(exec_render_env_vars, &renderer);
  }

  argsp = test->env_addf_args;
  if(argsp)
  {
    while(*argsp)
    {
      args = *argsp;
      nargs = sentinel(args->args);
      if(nargs == 0)
        fatal(exec_builder_env_addf, &builder, args->name_fmt, args->val_fmt);
      if(nargs == 1)
        fatal(exec_builder_env_addf, &builder, args->name_fmt, args->val_fmt, args->args[0]);
      if(nargs == 2)
        fatal(exec_builder_env_addf, &builder, args->name_fmt, args->val_fmt, args->args[0], args->args[1]);

      argsp++;
    }
  }

  if(test->fml)
  {
    fatal(formula_parser_bacon_parse, fml_parser, test->fml, strlen(test->fml) + 2, "-formula-", &fml_file, &fml_args, &fml_envs);

    exec_render_context_configure(&renderer, &builder, &mod, vars, &bs, chan);

    if(fml_file)
    {
      fatal(exec_render_file, &renderer, fml_file);
    }

    if(fml_args)
    {
      fatal(exec_render_args, &renderer, fml_args);
    }

    if(fml_envs)
    {
      fatal(exec_render_envs, &renderer, fml_envs);
    }
  }

  fatal(exec_builder_build, &builder, &e);

  assert_eq_s(test->file, e->path);

  if(test->args)
  {
    assert_eq_d(sentinel(test->args), sentinel(e->args));
    for(x = 0; x < sentinel(test->args); x++)
    {
      assert_eq_s(test->args[x], e->args[x]);
    }
  }

  if(test->envs)
  {
    assert_eq_d(sentinel(test->envs), sentinel(e->envs));
    for(x = 0; x < sentinel(test->envs); x++)
    {
      assert_eq_s(test->envs[x], e->envs[x]);
    }
  }

finally:
  fatal(value_parser_xfree, parser);
  fatal(formula_parser_xfree, fml_parser);
  fatal(exec_builder_xdestroy, &builder);
  fatal(exec_render_context_xdestroy, &renderer);
  formula_value_free(fml_file);
  formula_value_free(fml_args);
  formula_value_free(fml_envs);
  wfree(chan);
coda;
}

//
// manifest
//

xunit_unit xunit = {
    xu_setup : exec_builder_test_unit_setup
  , xu_cleanup : exec_builder_test_unit_cleanup
  , xu_entry : exec_builder_test_entry
  , xu_tests : (exec_builder_test*[]) {
     /* vars */
      (exec_builder_test[]) {{
          vars : " cc : gcc"
        , envs : (char*[]) {
              "cc=gcc"
            , NULL
          }
      }}
    , (exec_builder_test[]) {{
          vars : " cflags : 42.4"
        , envs : (char*[]) {
              "cflags=42.40"
            , NULL
          }
      }}
    , (exec_builder_test[]) {{
          vars : " cc : a"
                 " lflags : c"
                 " cflags : b"
                 " bflags : asasdf"
                 " aa : z"
        , envs : (char*[]) {
              "aa=z"
            , "bflags=asasdf"
            , "cc=a"
            , "cflags=b"
            , "lflags=c"
            , NULL
          }
      }}

    /* aggregates */
    , (exec_builder_test[]) {{
          vars : " cc : [ goat boat ]"
                 " boats : { foo bar foo }"
        , envs : (char*[]) {
              "boats=foo bar"
            , "cc=goat boat"
            , NULL
          }
      }}

    /* builder_env_addf */
    , (exec_builder_test[]) {{
          env_addf_args : (struct env_addf_args*[]) {
              (struct env_addf_args[]) {{
                  name_fmt : "foo%d"
                , val_fmt : "bar%d"
                , args : (void *[]) {
                      (void*)1
                    , (void*)2
                    , 0
                  }
              }}
            , 0
          }
        , envs : (char*[]) {
              "foo1=bar2"
            , NULL
          }
      }}
    , (exec_builder_test[]) {{
          env_addf_args : (struct env_addf_args*[]) {
              (struct env_addf_args[]) {{
                  name_fmt : "foo"
                , val_fmt : "%dbar%d"
                , args : (void *[]) {
                      (void*)1
                    , (void*)2
                    , 0
                  }
              }}
            , 0
          }
        , envs : (char*[]) {
              "foo=1bar2"
            , NULL
          }
      }}

    /* additive addf calls */
    , (exec_builder_test[]) {{
          env_addf_args : (struct env_addf_args*[]) {
              (struct env_addf_args[]) {{
                  name_fmt : "foo"
                , val_fmt : "%dbar%d"
                , args : (void *[]) {
                      (void*)1
                    , (void*)2
                    , 0
                  }
              }}
            , (struct env_addf_args[]) {{
                  name_fmt : "f%c%c"
                , val_fmt : "bar"
                , args : (void *[]) {
                      (void*)'o'
                    , (void*)'o'
                    , 0
                  }
              }}
            , 0
          }
        , envs : (char*[]) {
              "foo=1bar2 bar"
            , NULL
          }
      }}

    /* many addf calls */
    , (exec_builder_test[]) {{
          env_addf_args : (struct env_addf_args*[]) {
              (struct env_addf_args[]) {{
                  name_fmt : "A"
                , val_fmt : "1"
                , args : (void *[]) { 0 }
              }}
            , (struct env_addf_args[]) {{
                  name_fmt : "B"
                , val_fmt : "2"
                , args : (void *[]) { 0 }
              }}
            , (struct env_addf_args[]) {{
                  name_fmt : "C"
                , val_fmt : "3"
                , args : (void *[]) { 0 }
              }}
            , (struct env_addf_args[]) {{
                  name_fmt : "C1"
                , val_fmt : "31"
                , args : (void *[]) { 0 }
              }}
            , (struct env_addf_args[]) {{
                  name_fmt : "C11"
                , val_fmt : "311"
                , args : (void *[]) { 0 }
              }}
            , (struct env_addf_args[]) {{
                  name_fmt : "C111"
                , val_fmt : "3111"
                , args : (void *[]) { 0 }
              }}
            , (struct env_addf_args[]) {{
                  name_fmt : "C1111"
                , val_fmt : "31111"
                , args : (void *[]) { 0 }
              }}
            , (struct env_addf_args[]) {{
                  name_fmt : "C11111"
                , val_fmt : "311111"
                , args : (void *[]) { 0 }
              }}
            , (struct env_addf_args[]) {{
                  name_fmt : "C111111"
                , val_fmt : "3111111"
                , args : (void *[]) { 0 }
              }}
            , 0
          }
        , envs : (char*[]) {
              "A=1"
            , "B=2"
            , "C=3"
            , "C1=31"
            , "C11=311"
            , "C111=3111"
            , "C1111=31111"
            , "C11111=311111"
            , "C111111=3111111"
            , NULL
          }
      }}

    /* combined var vars + addf calls */
    , (exec_builder_test[]) {{
          vars : " cc : [ goat boat ]"
                 " boats : { foo bar foo }"
        , env_addf_args : (struct env_addf_args*[]) {
              (struct env_addf_args[]) {{
                  name_fmt : "foo"
                , val_fmt : "%dbar%d"
                , args : (void *[]) {
                      (void*)1
                    , (void*)2
                    , 0
                  }
              }}
            , (struct env_addf_args[]) {{
                  name_fmt : "f%c%c"
                , val_fmt : "bar"
                , args : (void *[]) {
                      (void*)'o'
                    , (void*)'o'
                    , 0
                  }
              }}
            , 0
          }
        , envs : (char*[]) {
              "boats=foo bar"
            , "cc=goat boat"
            , "foo=1bar2 bar"
            , NULL
          }
      }}

    /* crash repro */
    , (exec_builder_test[]) {{
          env_addf_args : (struct env_addf_args*[]) {
              (struct env_addf_args[]) {{
                  name_fmt : "errtab"
                , val_fmt : ""
                , args : (void *[]) { 0 }
              }}
            , (struct env_addf_args[]) {{
                  name_fmt : "bm_variant"
                , val_fmt : "-no variant !-"
                , args : (void *[]) { 0 }
              }}
            , (struct env_addf_args[]) {{
                  name_fmt : "bm_targets_len"
                , val_fmt : "2"
                , args : (void *[]) { 0 }
              }}
            , (struct env_addf_args[]) {{
                  name_fmt : "bm_target0"
                , val_fmt : "2"
                , args : (void *[]) { 0 }
              }}
            , 0
          }
        , envs : (char*[]) {
              "bm_target0=2"
            , "bm_targets_len=2"
            , "bm_variant=-no variant !-"
            , "errtab="
            , NULL
          }
      }}

    /* args */
    , (exec_builder_test[]) {{
          vars : " cflags : 42"
        , fml : (char[]) { "args : [ $cflags ]\0\0" }
        , args : (char*[]) {
              "42"
            , NULL
          }
      }}
    , (exec_builder_test[]) {{
          fml : (char[]) { "args : [ a b c ]\0\0" }
        , args : (char*[]) {
            "a", "b", "c"
            , NULL
          }
      }}
    , (exec_builder_test[]) {{
          fml : (char[]) { "args : [ a $cflags c ]\0\0" }
        , args : (char*[]) {
            "a", "c"
            , NULL
          }
      }}
    , (exec_builder_test[]) {{
          vars : "cflags : [ 42 ]"
        , fml : (char[]) { "args : [ a $cflags c ]\0\0" }
        , args : (char*[]) {
              "a", "42", "c"
            , NULL
          }
      }}
    , (exec_builder_test[]) {{
          vars : "cflags : [ 42 43 ]"
        , fml : (char[]) { "args : [ a $cflags c ]\0\0" }
        , args : (char*[]) {
              "a", "42", "43", "c"
            , NULL
          }
      }}
    , (exec_builder_test[]) {{
          vars : "cflags : { 42 43 }"
        , fml : (char[]) { "args : [ a $cflags c ]\0\0" }
        , args : (char*[]) {
              "a", "43", "42", "c"
            , NULL
          }
      }}
    , (exec_builder_test[]) {{
          vars : "cflags : { 42 43 }"
        , fml : (char[]) { "args : [ a $cflags c 42 ]\0\0" }
        , args : (char*[]) {
              "a", "43", "42", "c", "42"
            , NULL
          }
      }}

    /* envs */
    , (exec_builder_test[]) {{
          vars : " cflags : 42"
        , fml : (char[]) { "envs : { foo : $cflags }\0\0" }
        , envs : (char*[]) {
              "cflags=42"
            , "foo=42"
            , NULL
          }
      }}
    , (exec_builder_test[]) {{
          fml : (char[]) { "envs : { foo : a }\0\0" }
        , envs : (char*[]) {
              "foo=a"
            , NULL
          }
      }}
    , (exec_builder_test[]) {{
          vars : " cflags : 42"
        , fml : (char[]) { "envs : { foo : a bar : $cflags }\0\0" }
        , envs : (char*[]) {
              "bar=42"
            , "cflags=42"
            , "foo=a"
            , NULL
          }
      }}
    , (exec_builder_test[]) {{
          vars : "cflags : [ 42 ]"
        , fml : (char[]) { "envs : { foo : $cflags }\0\0" }
        , envs : (char*[]) {
              "cflags=42"
            , "foo=42"
            , NULL
          }
      }}
    , (exec_builder_test[]) {{
          vars : "cflags : [ 42 43 ]"
        , fml : (char[]) { "envs : { foo : $cflags }\0\0" }
        , envs : (char*[]) {
              "cflags=42 43"
            , "foo=42 43"
            , NULL
          }
      }}

    /* file */
    , (exec_builder_test[]) {{
          fml : (char[]) { "file : foo-bar\0\0" }
        , file : "foo-bar"
      }}
    , (exec_builder_test[]) {{
          fml : (char[]) { "file : 14.92\0\0" }
        , file : "14.92"
      }}
    , (exec_builder_test[]) {{
          vars : "cc : /usr/bin/gcc"
        , fml : (char[]) { "file : $cc\0\0" }
        , file : "/usr/bin/gcc"
        , envs : (char*[]) {
              "cc=/usr/bin/gcc"
            , NULL
          }
      }}

    /* sequence/args */
    , (exec_builder_test[]) {{
          fml : (char[]) {
            "args : ["
            "  sequence : ["
            "    foo"
            "    bar"
            "  ]"
            "]"
            "\0"
          }
        , args : (char*[]) {
              "foo"
            , "bar"
            , NULL
          }
      }}
    , (exec_builder_test[]) {{
          fml : (char[]) {
            "args : ["
            "  sequence : ["
            "    foo"
            "    bar"
            "    prepend : abcd"
            "  ]"
            "]"
            "\0"
          }
        , args : (char*[]) {
              "abcdfoo"
            , "abcdbar"
            , NULL
          }
      }}
    , (exec_builder_test[]) {{
          fml : (char[]) {
            "args : ["
            "  sequence : ["
            "    foo"
            "    bar"
            "    prepend : abcd"
            "  ]"
            "]"
            "\0"
          }
        , args : (char*[]) {
              "abcdfoo"
            , "abcdbar"
            , NULL
          }
      }}
    , (exec_builder_test[]) {{
          fml : (char[]) {
            "args : ["
            "  sequence : ["
            "    foo"
            "    bar"
            "    prepend : abcd"
            "    qux"
            "  ]"
            "]"
            "\0"
          }
        , args : (char*[]) {
              "abcdfoo"
            , "abcdbar"
            , "qux"
            , NULL
          }
      }}

    /* sequence/envs */
    , (exec_builder_test[]) {{
          fml : (char[]) {
            "envs : {"
            "  primary : ["
            "    foo"
            "  ]"
            "}"
            "\0"
          }
        , envs : (char*[]) {
              "primary=foo"
            , NULL
          }
      }}
    , (exec_builder_test[]) {{
          fml : (char[]) {
            "envs : {"
            "  primary : ["
            "    foo"
            "    bar"
            "  ]"
            "}"
            "\0"
          }
        , envs : (char*[]) {
              "primary=foo bar"
            , NULL
          }
      }}
    , (exec_builder_test[]) {{
          fml : (char[]) {
            "envs : {"
            "  primary : ["
            "    foo"
            "    bar"
            "    prepend : abcd"
            "  ]"
            "}"
            "\0"
          }
        , envs : (char*[]) {
              "primary=abcdfoo abcdbar"
            , NULL
          }
      }}
    , (exec_builder_test[]) {{
          fml : (char[]) {
            "args : ["
            "  sequence : ["
            "    foo"
            "    bar"
            "    prepend : abcd"
            "  ]"
            "]"
            "\0"
          }
        , args : (char*[]) {
              "abcdfoo"
            , "abcdbar"
            , NULL
          }
      }}
    , (exec_builder_test[]) {{
          fml : (char[]) {
            "args : ["
            "  sequence : ["
            "    foo"
            "    bar"
            "    prepend : abcd"
            "    qux"
            "  ]"
            "]"
            "\0"
          }
        , args : (char*[]) {
              "abcdfoo"
            , "abcdbar"
            , "qux"
            , NULL
          }
      }}
    , (exec_builder_test[]) {{
          fml : (char[]) {
            "args : ["
            "  $cflags"
            "  sequence : ["
            "    foo"
            "    bar"
            "    prepend : abcd"
            "    qux"
            "  ]"
            "]"
            "\0"
          }
        , vars : "cflags : [ 42 43 ]"
        , args : (char*[]) {
              "42"
            , "43"
            , "abcdfoo"
            , "abcdbar"
            , "qux"
            , NULL
          }
      }}
    , 0
  }
};
