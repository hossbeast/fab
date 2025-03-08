/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "moria/load.h"
#include "xlinux/xstdlib.h"
#include "bacon/load.h"

#include "valyria/list.h"
#include "valyria/llist.h"
#include "valyria/hashtable.h"
#include "bacon/value.h"
#include "bacon/parser.h"
#include "bacon/writer.h"
#include "moria/graph.h"
#include "moria/parser.h"
#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "rule.h"
#include "fsent.h"
#include "exec.h"
#include "pattern.h"
#include "pattern_parser.h"
#include "module.h"
#include "exec_builder.h"
#include "exec_render.h"
#include "formula_parser.internal.h"
#include "exec_value.h"
#include "varset.h"
#include "build_slot.h"
#include "dependency.h"
#include "channel.h"
#include "bpe.h"
#include "formula.h"
#include "shadow.h"
#include "fstree.h"
#include "node_operations_test.h"
#include "notify.h"
#include "config_system.h"
#include "events/events.h"
#include "configset_parser.h"
#include "config.h"
#include "args.h"
#include "fsedge.h"
#include "ref_edge.h"

typedef struct exec_builder_test {
  XUNITTEST;

  // inputs
  char *config;        // filesystems config
  char *operations;    // build the node graph
  char *vars;          // bacon text
  char *fml;           // formula bacon text
  char *lookup_cwd;    // cwd to use for lookups

  struct env_addf_args {
    const char * name_fmt;
    const char * val_fmt;
    void ** args;
  } **env_addf_args;

  struct exec_builder_args** builder_args;

  // expected outputs
  char *cwd;
  char *filepath;
  char **args;
  char **envs;
} exec_builder_test;

static void exec_builder_test_unit_setup(xunit_unit * unit)
{
  bacon_load();
  args_init();

  fsent_root_setup();
  shadow_root_setup();
  fstree_root_setup();
  shadow_setup();
  fstree_setup();
  fsent_base_setup();

  module_setup();
  graph_setup();
  namespace_setup();
  notify_setup();
  config_system_setup();
  events_setup();
  exec_value_setup();

  extern __thread bool rcu_is_reader ; rcu_is_reader = true;
}

static void exec_builder_test_unit_cleanup(xunit_unit * unit)
{
  bacon_unload();

  fsent_root_cleanup();
  module_cleanup();
  shadow_cleanup();
  fstree_cleanup();
  notify_cleanup();
  fsent_cleanup();
  graph_cleanup();
  fsedge_cleanup();
  ref_edge_cleanup();
}

//
// tests
//

static void resolve_lookup(lookup_store * restrict lookups, const fsent *base, bacon_value * restrict vars, bacon_value * restrict val)
{
  lookup_context ctx;
  exec_value *ev;

  if((int)val->type != EXEC_VALUE_LOOKUP) {
    return;
  }

  ev = containerof(val, typeof(*ev), bacon);
  memset(&ctx, 0, sizeof(ctx));
  ctx.rctx.base = base;
  ctx.rctx.vars = vars;
  ctx.ns_name = ev->lookup.namespace;
  ctx.ns_name_len = ev->lookup.namespace_len;
  if(lookup_resolve(&ctx, ev->lookup.pattern) != 1) {
    RTA;
  }

  lookup_store_put(lookups, ev, ctx.rctx.vertices[0]);
}

static void exec_builder_test_entry(xunit_test * _test)
{
  exec_builder_test * test = containerof(_test, exec_builder_test, xu);
  exec *e;
  bacon_parser * parser = 0;
  formula_parser * fml_parser = 0;
  int x;
  lookup_store lookups;
  exec_builder builder;
  bacon_value *bv;
  exec_render_context renderer;
  struct env_addf_args **argsp, *args;
  exec_builder_args **builder_argsp, *builder_args;
  size_t nargs;
  bacon_value *vars = 0;
  module mod = { 0 };
  buildplan_entity bpe = { 0 };
  build_slot bs = { .bpe = &bpe };
  channel *chan;
  int r;
  fsent *cwd;
  formula fml;
  graph_invalidation_context invalidation = { };
  configset_parser *cfg_parser = 0;
  config *cfg;
  moria_parser *op_parser = 0;

  bacon_parser_alloc(&parser);
  formula_parser_create(&fml_parser);
  exec_builder_init(&builder);
  exec_render_context_init(&renderer);
  renderer.lookups = &lookups;
  xmalloc(&chan, sizeof(*chan));
  graph_invalidation_begin(&invalidation);
  (void)namespace_get(MMS("module"), &invalidation);
  graph_invalidation_end(&invalidation);
  lookup_store_init(&lookups);

  // setup filesystems
  configset_parser_create(&cfg_parser);
  config_alloc(&cfg);
  if(test->config) {
    if(configset_parser_parse(cfg_parser, test->config, strlen(test->config) + 2, 0, 0, cfg) != 0) {
      tracef("%.*s", cfg_parser->yyu.error_len, cfg_parser->yyu.error_str);
      RTA;
    }
  }

  graph_invalidation_begin(&invalidation);
  namespace_reconfigure(cfg, false, &invalidation);
  fstree_reconfigure(cfg, false);
  namespace_reconfigure(cfg, true, &invalidation);
  fstree_reconfigure(cfg, true);
  graph_invalidation_end(&invalidation);

  // setup initial graph
  moria_parser_create(&op_parser, &g_graph, (llist *[]) { &fsent_list }, 1, node_operations_test_dispatch, graph_vertex_attrs, graph_edge_attrs);
  if(test->operations) {
    if(moria_parser_operations_parse(op_parser, MMS(test->operations)) != 0) {
      RTA;
    }
  }

  /* index fsents */
  graph_invalidation_begin(&invalidation);
  fsent_fstree_refresh(&invalidation);
  namespace_reconcile(&invalidation);
  graph_invalidation_end(&invalidation);

  if(test->lookup_cwd) {
    cwd = resolve_fragment(MMS(test->lookup_cwd));
  }

  // act
  if(test->vars)
  {
    if((r = bacon_parser_parse(parser, test->vars, strlen(test->vars), "-test-", BACON_TYPE_SET, &vars)) != 0) {
      tracef("%.*s", (int)parser->error_len, parser->error);
    }
    assert_eq_d(0, r);
  }
  exec_render_context_configure(&renderer, &builder, cwd, &mod, 0, vars, &bs);

  if(test->vars) {
    exec_render_env_vars(&renderer);
  }

  argsp = test->env_addf_args;
  if(argsp)
  {
    while(*argsp)
    {
      args = *argsp;
      nargs = sentinel(args->args);
      if(nargs == 0) {
        exec_builder_env_addf(&builder, args->name_fmt, args->val_fmt);
      } else if(nargs == 1) {
        exec_builder_env_addf(&builder, args->name_fmt, args->val_fmt, args->args[0]);
      } else if(nargs == 2) {
        exec_builder_env_addf(&builder, args->name_fmt, args->val_fmt, args->args[0], args->args[1]);
      }

      argsp++;
    }
  }
  if((builder_argsp = test->builder_args))
  {
    while(*builder_argsp)
    {
      builder_args = *builder_argsp;
      exec_builder_add(&builder, builder_args);
      builder_argsp++;
    }
  }

  if(test->fml)
  {
    if((r = formula_parser_bacon_parse(fml_parser, test->fml, strlen(test->fml) + 2, "-formula-", 0, &fml.file, &fml.args, &fml.envs) != 0)) {
      tracef("%.*s", (int)fml_parser->error_len, fml_parser->error);
    }
    assert_eq_d(0, r);
  }

  /* resolve any lookups */
  if(test->fml)
  {
    if(fml.file) {
      resolve_lookup(&lookups, cwd, vars, fml.file);
    }
    if(fml.args) {
      llist_foreach(&fml.args->list, bv, lln) {
        resolve_lookup(&lookups, cwd, vars, bv);
      }
    }
    if(fml.envs) {
      rbtree_foreach(&fml.envs->set, bv, rbn) {
        resolve_lookup(&lookups, cwd, vars, bv);
      }
    }
  }

  if(test->fml)
  {
    if(fml.file) {
      exec_render_file(&renderer, fml.file);
    }
    if(fml.args) {
      exec_render_args(&renderer, fml.args);
    }
    if(fml.envs) {
      exec_render_envs(&renderer, fml.envs);
    }
  }

  exec_builder_build(&builder, &e);

  if(test->cwd) {
    assert_eq_s(test->cwd, e->cwd);
  }

  if(test->filepath) {
    assert_eq_s(test->filepath, e->file_path);
  }

  if(test->args)
  {
    assert_eq_d(sentinel(test->args), sentinel(e->args));
    for(x = 0; x < sentinel(test->args); x++) {
      assert_eq_s(test->args[x], e->args[x]);
    }
  }

  if(test->envs)
  {
    assert_eq_d(sentinel(test->envs), sentinel(e->envs));
    for(x = 0; x < sentinel(test->envs); x++) {
      assert_eq_s(test->envs[x], e->envs[x]);
    }
  }

  formula_parser_free(fml_parser);
  exec_builder_destroy(&builder);
  exec_render_context_destroy(&renderer);
  moria_parser_free(op_parser);
  bacon_value_free(parser, vars);
  bacon_value_free(parser, fml.file);
  bacon_value_free(parser, fml.args);
  bacon_value_free(parser, fml.envs);
  bacon_parser_free(parser);
  config_free(cfg, cfg_parser);
  configset_parser_free(cfg_parser);
  free(chan);
  lookup_store_destroy(&lookups);
}

//
// manifest
//

static xunit_unit xunit = {
    xu_setup : exec_builder_test_unit_setup
  , xu_cleanup : exec_builder_test_unit_cleanup
  , xu_entry : exec_builder_test_entry
  , xu_tests : (exec_builder_test*[]) {
     /* vars */
      (exec_builder_test[]) {{
          vars : "cc : gcc"
        , envs : (char*[]) {
              "cc=gcc"
            , NULL
          }
      }}
    , (exec_builder_test[]) {{
          vars : " cflags : 42.4"
        , envs : (char*[]) {
              "cflags=42.400000"
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
          vars : " boats : { foo bar foo }"
        , envs : (char*[]) {
              "boats=bar foo"
            , NULL
          }
      }}
    , (exec_builder_test[]) {{
          vars : " cc : [ goat boat ]"
                 " boats : { foo bar foo }"
        , envs : (char*[]) {
              "boats=bar foo"
            , "cc=goat boat"
            , NULL
          }
      }}

    /* empty-list */
    , (exec_builder_test[]) {{
          vars : " A : [ ]"
        , envs : (char*[]) {
              "A="
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
              "boats=bar foo"
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
    , (exec_builder_test[]) {{
          vars : "cflags : {"
                 "\"-DFABIPCDIR=/var/run/fab\""
                 "}"
        , fml : (char[]) { "args : [ $cflags ]\0\0" }
        , args : (char*[]) {
              "-DFABIPCDIR=/var/run/fab"
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

    /* cwd */
    , (exec_builder_test[]) {{
          cwd : "/usr/bin"
        , builder_args : (exec_builder_args*[]) {
              (exec_builder_args[]) {{
                  .item = BUILDER_CWD
                , .mode = BUILDER_REPLACE
                , .render_val = BUILDER_STRING
                , .val.s = "/usr/bin"
              }}
            , 0
          }
      }}

    /* file */
    /* root-relative */
    , (exec_builder_test[]) {{
          operations : "foo"
        , fml : (char[]) {
            "file : {"
            "  lookup : {"
            "    pattern : /foo"
            "    namespace : module"
            "  }"
            "}"
            "\0"
          }
        , filepath : "/foo"
        , args : (char*[]) {
            "foo" , NULL
          }
      }}
    , (exec_builder_test[]) {{
          operations : "foo/bar"
        , vars : "baz : bar"
        , fml : (char[]) {
            "file : {"
            "  lookup : {"
            "    pattern : /foo/$baz"
            "  }"
            "}"
            "\0"
          }
        , filepath : "/foo/bar"
        , args : (char*[]) {
            "bar" , NULL
          }
      }}

    /* shadow-relative */
    , (exec_builder_test[]) {{
          operations : "foo"
        , fml : (char[]) {
            "file : {"
            "  lookup : {"
            "    pattern : //bm/modules"
            "    namespace : module"
            "  }"
            "}"
            "\0"
          }
        , filepath : "//bm/modules"
      }}
    , (exec_builder_test[]) {{
          operations : "foo"
        , fml : (char[]) {
            "file : {"
            "  lookup : {"
            "    pattern : //bm/modules"
            "  }"
            "}"
            "\0"
          }
        , filepath : "//bm/modules"
      }}
    /* dot-relative */
    , (exec_builder_test[]) {{
          operations : "foo"
        , lookup_cwd : "foo"
        , fml : (char[]) {
            "file : {"
            "  lookup : {"
            "    pattern : ."
            "  }"
            "}"
            "\0"
          }
        , filepath : "/foo"
      }}
    , (exec_builder_test[]) {{
          operations : "foo/bar"
        , lookup_cwd : "foo"
        , fml : (char[]) {
            "file : {"
            "  lookup : {"
            "    pattern : ./bar"
            "  }"
            "}"
            "\0"
          }
        , filepath : "/foo/bar"
      }}
    /* dot-dot-relative */
    , (exec_builder_test[]) {{
          operations : "foo"
        , lookup_cwd : "foo"
        , fml : (char[]) {
            "file : {"
            "  lookup : {"
            "    pattern : .."
            "  }"
            "}"
            "\0"
          }
        , filepath : "/"
      }}
    , (exec_builder_test[]) {{
          operations : "foo bar"
        , lookup_cwd : "foo"
        , fml : (char[]) {
            "file : {"
            "  lookup : {"
            "    pattern : ../bar"
            "  }"
            "}"
            "\0"
          }
        , filepath : "/bar"
      }}
    , (exec_builder_test[]) {{
          operations : "foo"
        , fml : (char[]) {
            "file : {"
            "  lookup : {"
            "    pattern : //bm/modules"
            "    namespace : module"
            "  }"
            "}"
            "\0"
          }
        , filepath : "//bm/modules"
      }}
    /* unbound */
    , (exec_builder_test[]) {{
          config : (char[]) {
            "filesystem : {\n"
            "  fstrees : {\n"
            "    / : {\n"
            "      namespaces : {\n"
            "        module\n"
            "      }\n"
            "    }\n"
            "  }\n"
            "}\n"
            "\0\0"
          }
        , operations : "foo/bar"
        , fml : (char[]) {
            "file : {"
            "  lookup : {"
            "    pattern : foo"
            "    namespace : module"
            "  }"
            "}"
            "\0"
          }
        , filepath : "/foo"
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
            "    true"
            "  ]"
            "}"
            "\0"
          }
        , envs : (char*[]) {
              "primary=foo true"
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
XUNIT_UNIT(xunit);
