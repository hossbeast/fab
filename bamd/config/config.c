/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

/*

config keys that should *only* be legal in the global/user config files
 filesystems
 special

*/

#include <unistd.h>
#include <errno.h>

#include "moria/vertex.h"
#include "bam/ipc.h"
#include "xlinux/xstdlib.h"
#include "narrator.h"
#include "narrator/growing.h"
#include "bacon/builder.h"
#include "bacon/location.h"
#include "bacon/value.h"
#include "bacon/writer.h"
#include "narrator/fixed.h"
#include "common/attrs.h"
#include "exec_value.h"

#include "config.h"
#include "configset.h"
#include "channel.h"
#include "request_handler.h"
#include "pattern.h"
#include "configset_parser.h"
#include "graph.h"
#include "marshal.h"
#include "zcat.h"
#include "system.h"
#include "shadow.h"

static llist freelist = LLIST_INITIALIZER(freelist);

static void __attribute__((nonnull)) config_dispose(config * restrict n)
{
  llist_delete(&n->owner);
  llist_append(&freelist, &n->owner);
}

void config_setup()
{

}

void config_cleanup()
{
  llist *T;
  config *n;

  llist_foreach_safe(&freelist, n, owner, T) {
    free(n);
  }
}

void config_alloc(config ** restrict rv)
{
  config * cfg;

  if((cfg = llist_shift(&freelist, typeof(*cfg), owner)) == 0) {
    xmalloc(&cfg, sizeof(*cfg));
  } else {
    memset(cfg, 0, sizeof(*cfg));
  }

  llist_init(&cfg->owner);
  rbtree_init(&cfg->filesystem.fstrees.set);
  rbtree_init(&cfg->filesystem.exclude.set);
  rbtree_init(&cfg->filesystem.special.set);
  llist_init(&cfg->plugins.list);
  rbtree_init(&cfg->namespaces.set);
  rbtree_init(&cfg->modules.varsets.set);
  rbtree_init(&cfg->modules.rulesets.set);

  *rv = cfg;
}

void config_free(config * restrict cfg, configset_parser * restrict parser)
{
  pattern *pat;
  config_fstree_mapping *fsm;
  config_plugin_entry *pe;
  config_namespace_entry *nse;
  config_specials_entry *cse;
  llist *T;
  llist list;

  if(!cfg) {
    return;
  }

  bacon_value_free(parser->bacon_parser, cfg->build.concurrency.v);
  bacon_value_free(parser->bacon_parser, cfg->build.stdout_buffer_size.v);
  bacon_value_free(parser->bacon_parser, cfg->build.stderr_buffer_size.v);
  bacon_value_free(parser->bacon_parser, cfg->build.auxout_buffer_size.v);
  bacon_value_free(parser->bacon_parser, cfg->build.retention.max_entries.v);

  /* notify */
  bacon_value_free(parser->bacon_parser, cfg->notify.interval.v);

  /* timeout */
  bacon_value_free(parser->bacon_parser, cfg->timeout.interval.v);
  bacon_value_free(parser->bacon_parser, cfg->timeout.refresh_on_request.v);
  bacon_value_free(parser->bacon_parser, cfg->timeout.refresh_on_autobuild.v);

  /* filesystem.exclude */
  llist_init(&list);
  rbtree_foreach(&cfg->filesystem.exclude.set, pat, rbn) {
    llist_push(&list, &pat->lln);
  }
  llist_foreach_safe(&list, pat, lln, T) {
    pattern_free(pat);
  }

  /* filesystem.special */
  llist_init(&list);
  rbtree_foreach(&cfg->filesystem.special.set, cse, rbn) {
    llist_push(&list, &cse->lln);
  }
  llist_foreach_safe(&list, cse, lln, T) {
    bacon_value_free(parser->bacon_parser, cse->name);
    free(cse);
  }

  /* filesystem.fstrees */
  llist_init(&list);
  rbtree_foreach(&cfg->filesystem.fstrees.set, fsm, rbn) {
    llist_push(&list, &fsm->lln);
  }

  llist_foreach_safe(&list, fsm, lln, T) {
    pattern_free(&fsm->key.v->pattern);
    bacon_value_free(parser->bacon_parser, fsm->val.invalidation.v);
    bacon_value_free(parser->bacon_parser, fsm->val.mkdir.v);
    bacon_value_free(parser->bacon_parser, fsm->val.unlink.v);
    bacon_value_free(parser->bacon_parser, fsm->val.readlink.v);
    bacon_value_free(parser->bacon_parser, fsm->val.mount.v);
    bacon_value_free(parser->bacon_parser, fsm->val.namespaces.list);
    free(fsm);
  }

  /* plugins */
  bacon_value_free(parser->bacon_parser, cfg->plugins.stdin_buffer_size.v);
  bacon_value_free(parser->bacon_parser, cfg->plugins.stdout_buffer_size.v);
  bacon_value_free(parser->bacon_parser, cfg->plugins.stderr_buffer_size.v);
  bacon_value_free(parser->bacon_parser, cfg->plugins.auxout_buffer_size.v);

  /* plugins.list */
  llist_init(&list);
  llist_foreach_safe(&cfg->plugins.list, pe, lln, T) {
    bacon_value_free(parser->bacon_parser, pe->name);
    bacon_value_free(parser->bacon_parser, pe->file.lookup);
    bacon_value_free(parser->bacon_parser, pe->args);
    bacon_value_free(parser->bacon_parser, pe->envs);
    free(pe);
  }

  /* namespaces.list */
  llist_init(&list);
  rbtree_foreach(&cfg->namespaces.set, nse, rbn) {
    llist_push(&list, &nse->lln);
  }
  llist_foreach_safe(&list, nse, lln, T) {
    bacon_value_free(parser->bacon_parser, nse->name);
    free(nse);
  }

  /* modules.varsets */
  llist_init(&list);
  rbtree_foreach(&cfg->modules.varsets.set, cse, rbn) {
    llist_push(&list, &cse->lln);
  }
  llist_foreach_safe(&list, cse, lln, T) {
    bacon_value_free(parser->bacon_parser, cse->name);
    free(cse);
  }

  /* modules.varsets */
  llist_init(&list);
  rbtree_foreach(&cfg->modules.rulesets.set, cse, rbn) {
    llist_push(&list, &cse->lln);
  }
  llist_foreach_safe(&list, cse, lln, T) {
    bacon_value_free(parser->bacon_parser, cse->name);
    free(cse);
  }

  config_dispose(cfg);
}

void config_ifree(config ** restrict cfg, configset_parser * restrict parser)
{
  config_free(*cfg, parser);
  *cfg = 0;
}

void config_describe(const config * restrict cfg, bacon_writer * const restrict builder_writer)
{
  const config_fstree_mapping *fsm;
  const config_plugin_entry *pe;
  const config_namespace_entry *nse;
  const config_specials_entry *cse;
  const pattern *pat;
  char buf[512];
  narrator_fixed fixed;
  bacon_value *v;

  if(cfg->build.concurrency.v
     || cfg->build.stdout_buffer_size.v
     || cfg->build.stderr_buffer_size.v
     || cfg->build.auxout_buffer_size.v
     || cfg->build.retention.max_entries.v)
  {
    MAPPING STRING("build")
    { SET
      if(cfg->build.concurrency.v) {
        MAPPING
        STRING("concurrency")
        INT(cfg->build.concurrency.u16)
        COMMENTF("@ %s [%d,%d %d,%d]"
          , cfg->build.concurrency.v->loc->fname
          , cfg->build.concurrency.v->loc->f_lin + 1
          , cfg->build.concurrency.v->loc->f_col + 1
          , cfg->build.concurrency.v->loc->l_lin + 1
          , cfg->build.concurrency.v->loc->l_col + 1
        )
      }
      if(cfg->build.stdout_buffer_size.v) {
        MAPPING STRING("stdout-buffer-size")
        UINT(cfg->build.stdout_buffer_size.u32)
        COMMENTF("@ %s [%d,%d %d,%d]"
          , cfg->build.stdout_buffer_size.v->loc->fname
          , cfg->build.stdout_buffer_size.v->loc->f_lin + 1
          , cfg->build.stdout_buffer_size.v->loc->f_col + 1
          , cfg->build.stdout_buffer_size.v->loc->l_lin + 1
          , cfg->build.stdout_buffer_size.v->loc->l_col + 1
        )
      }
      if(cfg->build.stderr_buffer_size.v) {
        MAPPING STRING("stderr-buffer-size")
        UINT(cfg->build.stderr_buffer_size.u32)
        COMMENTF("@ %s [%d,%d %d,%d]"
          , cfg->build.stderr_buffer_size.v->loc->fname
          , cfg->build.stderr_buffer_size.v->loc->f_lin + 1
          , cfg->build.stderr_buffer_size.v->loc->f_col + 1
          , cfg->build.stderr_buffer_size.v->loc->l_lin + 1
          , cfg->build.stderr_buffer_size.v->loc->l_col + 1
        )
      }
      if(cfg->build.auxout_buffer_size.v) {
        MAPPING STRING("auxout-buffer-size")
        UINT(cfg->build.auxout_buffer_size.u32)
        COMMENTF("@ %s [%d,%d %d,%d]"
          , cfg->build.auxout_buffer_size.v->loc->fname
          , cfg->build.auxout_buffer_size.v->loc->f_lin + 1
          , cfg->build.auxout_buffer_size.v->loc->f_col + 1
          , cfg->build.auxout_buffer_size.v->loc->l_lin + 1
          , cfg->build.auxout_buffer_size.v->loc->l_col + 1
        )
      }
      if(cfg->build.retention.max_entries.v) {
        MAPPING STRING("retention")
        { SET
          MAPPING STRING("max-entries")
          UINT(cfg->build.retention.max_entries.u32)
          COMMENTF("@ %s [%d,%d %d,%d]"
            , cfg->build.retention.max_entries.v->loc->fname
            , cfg->build.retention.max_entries.v->loc->f_lin + 1
            , cfg->build.retention.max_entries.v->loc->f_col + 1
            , cfg->build.retention.max_entries.v->loc->l_lin + 1
            , cfg->build.retention.max_entries.v->loc->l_col + 1
          )
        }
      }
    }
  }
  if(!rbtree_empty(&cfg->filesystem.fstrees.set)
    || !rbtree_empty(&cfg->filesystem.exclude.set)
    || !rbtree_empty(&cfg->filesystem.special.set))
  {
    MAPPING
    STRING("filesystem")
    { SET
      if(!rbtree_empty(&cfg->filesystem.fstrees.set)) {
        MAPPING
        STRING("fstrees")
        { SET
          rbtree_foreach(&cfg->filesystem.fstrees.set, fsm, rbn) {
            if(fsm->val.namespaces.list || fsm->val.readlink.v || fsm->val.mkdir.v || fsm->val.unlink.v || fsm->val.invalidation.v || fsm->val.mount.v) {
              bacon_writer_push_mapping(builder_writer);
            }

            bacon_writer_push_bytes(builder_writer);
            pattern_say(&fsm->key.v->pattern, builder_writer->N);
            bacon_writer_pop_bytes(builder_writer);

            if(fsm->val.namespaces.list || fsm->val.readlink.v || fsm->val.mkdir.v || fsm->val.unlink.v || fsm->val.invalidation.v || fsm->val.mount.v) {
              bacon_writer_push_set(builder_writer);
            } else {
              COMMENTF("@ %s [%d,%d %d,%d]"
                , fsm->key.v->pattern.fname
                , fsm->key.v->pattern.loc.f_lin + 1
                , fsm->key.v->pattern.loc.f_col + 1
                , fsm->key.v->pattern.loc.l_lin + 1
                , fsm->key.v->pattern.loc.l_col + 1
              )
            }

            if(fsm->val.invalidation.v) {
              MAPPING STRING("invalidation")
              STRING(attrs16_name_byvalue(invalidation_attrs, fsm->val.invalidation.e))
              COMMENTF("@ %s [%d,%d %d,%d]"
                , fsm->val.invalidation.v->loc->fname
                , fsm->val.invalidation.v->loc->f_lin + 1
                , fsm->val.invalidation.v->loc->f_col + 1
                , fsm->val.invalidation.v->loc->l_lin + 1
                , fsm->val.invalidation.v->loc->l_col + 1
              )
            }
            if(fsm->val.mkdir.v) {
              MAPPING STRING("mkdir")
              BOOL(fsm->val.mkdir.b)
              COMMENTF("@ %s [%d,%d %d,%d]"
                , fsm->val.mkdir.v->loc->fname
                , fsm->val.mkdir.v->loc->f_lin + 1
                , fsm->val.mkdir.v->loc->f_col + 1
                , fsm->val.mkdir.v->loc->l_lin + 1
                , fsm->val.mkdir.v->loc->l_col + 1
              )
            }
            if(fsm->val.unlink.v) {
              MAPPING STRING("unlink")
              BOOL(fsm->val.unlink.b)
              COMMENTF("@ %s [%d,%d %d,%d]"
                , fsm->val.unlink.v->loc->fname
                , fsm->val.unlink.v->loc->f_lin + 1
                , fsm->val.unlink.v->loc->f_col + 1
                , fsm->val.unlink.v->loc->l_lin + 1
                , fsm->val.unlink.v->loc->l_col + 1
              )
            }
            if(fsm->val.readlink.v) {
              MAPPING STRING("readlink")
              STRING(attrs16_name_byvalue(readlink_attrs, fsm->val.readlink.e))
              COMMENTF("@ %s [%d,%d %d,%d]"
                , fsm->val.readlink.v->loc->fname
                , fsm->val.readlink.v->loc->f_lin + 1
                , fsm->val.readlink.v->loc->f_col + 1
                , fsm->val.readlink.v->loc->l_lin + 1
                , fsm->val.readlink.v->loc->l_col + 1
              )
            }
            if(fsm->val.mount.v) {
              MAPPING STRING("mount")
              BOOL(fsm->val.mount.b)
              COMMENTF("@ %s [%d,%d %d,%d]"
                , fsm->val.mount.v->loc->fname
                , fsm->val.mount.v->loc->f_lin + 1
                , fsm->val.mount.v->loc->f_col + 1
                , fsm->val.mount.v->loc->l_lin + 1
                , fsm->val.mount.v->loc->l_col + 1
              )
            }
            if(fsm->val.namespaces.list) {
              { MAPPING
                STRING("namespaces")
                { SET
                  llist_foreach(&fsm->val.namespaces.list->list, v, lln) {
                    bacon_writer_value(builder_writer, v);
                    COMMENTF("@ %s [%d,%d %d,%d]"
                      , v->loc->fname
                      , v->loc->f_lin + 1
                      , v->loc->f_col + 1
                      , v->loc->l_lin + 1
                      , v->loc->l_col + 1
                    )
                  }
                }
              }
            }

            if(fsm->val.namespaces.list || fsm->val.readlink.v || fsm->val.mkdir.v || fsm->val.unlink.v || fsm->val.invalidation.v || fsm->val.mount.v) {
              bacon_writer_pop_set(builder_writer);
              bacon_writer_pop_mapping(builder_writer);
            }
          }
        }
      }
      if(!rbtree_empty(&cfg->filesystem.exclude.set)) {
        MAPPING STRING("exclude")
        { SET
          rbtree_foreach(&cfg->filesystem.exclude.set, pat, rbn) {
            narrator_fixed_init(&fixed, buf, sizeof(buf));
            pattern_say(pat, &fixed.base);
            BYTES(fixed.s, fixed.l)
            COMMENTF("@ %s [%d,%d %d,%d]"
              , pat->fname
              , pat->loc.f_lin + 1
              , pat->loc.f_col + 1
              , pat->loc.l_lin + 1
              , pat->loc.l_col + 1
            )
          }
        }
      }
      if(!rbtree_empty(&cfg->filesystem.special.set)) {
        MAPPING
        STRING("special")
        { SET
          rbtree_foreach(&cfg->filesystem.special.set, cse, rbn) {
            bacon_writer_value(builder_writer, cse->name);
            COMMENTF("@ %s [%d,%d %d,%d]"
              , cse->name->loc->fname
              , cse->name->loc->f_lin + 1
              , cse->name->loc->f_col + 1
              , cse->name->loc->l_lin + 1
              , cse->name->loc->l_col + 1
            )
          }
        }
      }
    }
  }
  if(!llist_empty(&cfg->plugins.list)
     || cfg->plugins.stdin_buffer_size.v
     || cfg->plugins.stdout_buffer_size.v
     || cfg->plugins.stderr_buffer_size.v
     || cfg->plugins.auxout_buffer_size.v)
  { MAPPING STRING("plugins")
    { SET
      if(!llist_empty(&cfg->plugins.list)) {
        LIST
        llist_foreach(&cfg->plugins.list, pe, lln) {
          SET
          if(pe->name) {
            { MAPPING STRING("name")
              bacon_writer_value(builder_writer, pe->name);
              COMMENTF("@ %s [%d,%d %d,%d]"
                , pe->name->loc->fname
                , pe->name->loc->f_lin + 1
                , pe->name->loc->f_col + 1
                , pe->name->loc->l_lin + 1
                , pe->name->loc->l_col + 1
              )
            }
          }
          if(pe->file.lookup) {
            MAPPING
            STRING("file")
            { SET
              bacon_writer_value(builder_writer, pe->file.lookup);
            }
            COMMENTF("@ %s [%d,%d %d,%d]"
              , pe->file.lookup->loc->fname
              , pe->file.lookup->loc->f_lin + 1
              , pe->file.lookup->loc->f_col + 1
              , pe->file.lookup->loc->l_lin + 1
              , pe->file.lookup->loc->l_col + 1
            )
          }
          if(pe->args) {
            MAPPING
            STRING("args")
            { LIST
              llist_foreach(&pe->args->list, v, lln) {
                bacon_writer_value(builder_writer, v);
                COMMENTF("@ %s [%d,%d %d,%d]"
                  , v->loc->fname
                  , v->loc->f_lin + 1
                  , v->loc->f_col + 1
                  , v->loc->l_lin + 1
                  , v->loc->l_col + 1
                )
              }
            }
          }
          if(pe->envs) {
            MAPPING
            STRING("envs")
            { SET
              rbtree_foreach(&pe->envs->set, v, rbn) {
                bacon_writer_value(builder_writer, v);
                COMMENTF("@ %s [%d,%d %d,%d]"
                  , v->loc->fname
                  , v->loc->f_lin + 1
                  , v->loc->f_col + 1
                  , v->loc->l_lin + 1
                  , v->loc->l_col + 1
                )
              }
            }
          }
        }
      }
      if(cfg->plugins.stdin_buffer_size.v) {
        MAPPING STRING("stdin-buffer-size")
        UINT(cfg->plugins.stdin_buffer_size.u32)
        COMMENTF("@ %s [%d,%d %d,%d]"
          , cfg->plugins.stdin_buffer_size.v->loc->fname
          , cfg->plugins.stdin_buffer_size.v->loc->f_lin + 1
          , cfg->plugins.stdin_buffer_size.v->loc->f_col + 1
          , cfg->plugins.stdin_buffer_size.v->loc->l_lin + 1
          , cfg->plugins.stdin_buffer_size.v->loc->l_col + 1
        )
      }
      if(cfg->plugins.stdout_buffer_size.v) {
        MAPPING STRING("stdout-buffer-size")
        UINT(cfg->plugins.stdout_buffer_size.u32)
        COMMENTF("@ %s [%d,%d %d,%d]"
          , cfg->plugins.stdout_buffer_size.v->loc->fname
          , cfg->plugins.stdout_buffer_size.v->loc->f_lin + 1
          , cfg->plugins.stdout_buffer_size.v->loc->f_col + 1
          , cfg->plugins.stdout_buffer_size.v->loc->l_lin + 1
          , cfg->plugins.stdout_buffer_size.v->loc->l_col + 1
        )
      }
      if(cfg->plugins.stderr_buffer_size.v) {
        MAPPING STRING("stderr-buffer-size")
        UINT(cfg->plugins.stderr_buffer_size.u32)
        COMMENTF("@ %s [%d,%d %d,%d]"
          , cfg->plugins.stderr_buffer_size.v->loc->fname
          , cfg->plugins.stderr_buffer_size.v->loc->f_lin + 1
          , cfg->plugins.stderr_buffer_size.v->loc->f_col + 1
          , cfg->plugins.stderr_buffer_size.v->loc->l_lin + 1
          , cfg->plugins.stderr_buffer_size.v->loc->l_col + 1
        )
      }
      if(cfg->plugins.auxout_buffer_size.v) {
        MAPPING STRING("auxout-buffer-size")
        UINT(cfg->plugins.auxout_buffer_size.u32)
        COMMENTF("@ %s [%d,%d %d,%d]"
          , cfg->plugins.auxout_buffer_size.v->loc->fname
          , cfg->plugins.auxout_buffer_size.v->loc->f_lin + 1
          , cfg->plugins.auxout_buffer_size.v->loc->f_col + 1
          , cfg->plugins.auxout_buffer_size.v->loc->l_lin + 1
          , cfg->plugins.auxout_buffer_size.v->loc->l_col + 1
        )
      }
    }
  }
  if(!rbtree_empty(&cfg->namespaces.set))
  { MAPPING STRING("namespaces")
    { SET
      rbtree_foreach(&cfg->namespaces.set, nse, rbn) {
        bacon_writer_value(builder_writer, nse->name);
        COMMENTF("@ %s [%d,%d %d,%d]"
          , nse->name->loc->fname
          , nse->name->loc->f_lin + 1
          , nse->name->loc->f_col + 1
          , nse->name->loc->l_lin + 1
          , nse->name->loc->l_col + 1
        )
      }
    }
  }
  if(!rbtree_empty(&cfg->modules.varsets.set) || !rbtree_empty(&cfg->modules.rulesets.set))
  {
    { MAPPING STRING("modules")
      { SET
        if(!rbtree_empty(&cfg->modules.varsets.set))
        { MAPPING STRING("varsets")
          { SET
            rbtree_foreach(&cfg->modules.varsets.set, cse, rbn) {
              bacon_writer_value(builder_writer, cse->name);
              COMMENTF("@ %s [%d,%d %d,%d]"
                , cse->name->loc->fname
                , cse->name->loc->f_lin + 1
                , cse->name->loc->f_col + 1
                , cse->name->loc->l_lin + 1
                , cse->name->loc->l_col + 1
              )
            }
          }
        }
        if(!rbtree_empty(&cfg->modules.rulesets.set))
        { MAPPING STRING("rulesets")
          { SET
            rbtree_foreach(&cfg->modules.rulesets.set, cse, rbn) {
              bacon_writer_value(builder_writer, cse->name);
              COMMENTF("@ %s [%d,%d %d,%d]"
                , cse->name->loc->fname
                , cse->name->loc->f_lin + 1
                , cse->name->loc->f_col + 1
                , cse->name->loc->l_lin + 1
                , cse->name->loc->l_col + 1
              )
            }
          }
        }
      }
    }
  }
  if(cfg->notify.interval.v)
  {
    MAPPING STRING("notify")
    { SET
      if(cfg->notify.interval.v) {
        MAPPING STRING("interval-msec")
        UINT(cfg->notify.interval.u32)
      }
    }
  }
  if(cfg->timeout.interval.v || cfg->timeout.refresh_on_request.v || cfg->timeout.refresh_on_autobuild.v)
  {
    MAPPING STRING("timeout")
    { SET
      if(cfg->timeout.interval.v) {
        MAPPING STRING("interval-sec")
        UINT(cfg->timeout.interval.i64)
      }
      if(cfg->timeout.refresh_on_request.v) {
        MAPPING STRING("refresh-on-request")
        BOOL(cfg->timeout.refresh_on_request.b)
      }
      if(cfg->timeout.refresh_on_autobuild.v) {
        MAPPING STRING("refresh-on-autobuild")
        BOOL(cfg->timeout.refresh_on_autobuild.b)
      }
    }
  }
}

void config_show(const config * restrict cfg)
{
  bacon_writer writer, *builder_writer;

  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, g_narrator_stdout);

  config_describe(cfg, builder_writer);

  bacon_writer_close(&writer);
  bacon_writer_destroy(&writer);
  printf("\n");
}

void config_say(const config * restrict cfg, narrator * restrict N)
{
  bacon_writer writer;

  bacon_writer_init(&writer);
  bacon_writer_open(&writer, N);

  config_describe(cfg, &writer);

  bacon_writer_close(&writer);
  bacon_writer_destroy(&writer);
}

static void config_erange(bacon_writer * restrict builder_writer, void *ctx)
{
  const config_param *param = ctx;
  const bacon_value *val = 0;;
  const bacon_location *loc = 0;

  if(param->val) {
    val = param->val;
    loc = val->loc;
  }

  { MAPPING STRING("value out of range")
    { SET
      { MAPPING STRING("value")
        if(val) {
          bacon_writer_value(builder_writer, val);
        } else {
          LOADF("-none- (0)")
        }
      }
      if(loc) {
        MAPPING STRING("location")
        LOADF("%s [%d,%d - %d,%d]", loc->fname, loc->f_lin + 1, loc->f_col + 1, loc->l_lin + 1, loc->l_col + 1);
      } else if(param->config_path) {
        MAPPING STRING("location") STRING(param->config_path)
      }
      { MAPPING STRING("min") UINT(param->min) }
      { MAPPING STRING("max") UINT(param->max) }
    }
  }
}

void config_validate(const config_param * restrict param)
{
  int128_t i;

  i = 0;
  if(param->val) {
    i = param->val->i;
  }
  if(i >= param->min && i <= param->max) {
    return;
  }

  system_error(ERANGE, config_erange, param);
}
