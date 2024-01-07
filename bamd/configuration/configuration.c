/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

/*

config keys that should *only* be legal in the global/user config files
 filesystems
 special

*/

#include <unistd.h>
#include <errno.h>

#include "xlinux/xstdlib.h"
#include "narrator.h"
#include "bacon/builder.h"
#include "bacon/location.h"
#include "bacon/value.h"
#include "narrator/fixed.h"
#include "common/attrs.h"

#include "configuration.h"
#include "pattern.h"
#include "config_parser.h"

static llist freelist = LLIST_INITIALIZER(freelist);

static void __attribute__((nonnull)) configuration_dispose(configuration * restrict n)
{
  llist_delete(&n->owner);
  llist_append(&freelist, &n->owner);
}

void configuration_setup()
{
}

void configuration_cleanup()
{
  llist *T;
  configuration *n;

  llist_foreach_safe(&freelist, n, owner, T) {
    free(n);
  }
}

void configuration_say(const configuration * restrict cfg, narrator * restrict N)
{
  bacon_writer writer;

  bacon_writer_init(&writer);
  bacon_writer_open(&writer, N);

  configuration_describe(cfg, &writer);

  bacon_writer_close(&writer);
  bacon_writer_destroy(&writer);
}

void configuration_alloc(configuration ** restrict rv)
{
  configuration * cfg;

  if((cfg = llist_shift(&freelist, typeof(*cfg), owner)) == 0) {
    xmalloc(&cfg, sizeof(*cfg));
  } else {
    memset(cfg, 0, sizeof(*cfg));
  }

  llist_init(&cfg->owner);
  rbtree_init(&cfg->walker.include.set);
  rbtree_init(&cfg->walker.exclude.set);
  rbtree_init(&cfg->walker.special.set);
  rbtree_init(&cfg->formula.path.dirs.set);
  rbtree_init(&cfg->filesystems.set);

  *rv = cfg;
}

void configuration_free(configuration * restrict cfg, config_parser * restrict parser)
{
  pattern *pat;
  configuration_filesystems_mapping *fm;
  configuration_walker_special_entry *wse;
  configuration_formula_path_dirs_entry *fpde;
  llist *T;
  llist list;

  if(!cfg) {
    return;
  }

  bacon_value_free(parser->bacon_parser, cfg->build.concurrency.v);
  bacon_value_free(parser->bacon_parser, cfg->formula.path.copy_from_env.v);

  /* formula.path.dirs */
  llist_init(&list);
  rbtree_foreach(&cfg->formula.path.dirs.set, fpde, rbn) {
    llist_push(&list, &fpde->lln);
  }
  llist_foreach_safe(&list, fpde, lln, T) {
    bacon_value_free(parser->bacon_parser, fpde->v);
    free(fpde);
  }

  /* walker.include */
  llist_init(&list);
  rbtree_foreach(&cfg->walker.include.set, pat, rbn) {
    llist_push(&list, &pat->lln);
  }
  llist_foreach_safe(&list, pat, lln, T) {
    pattern_free(pat);
  }

  /* walker.exclude */
  llist_init(&list);
  rbtree_foreach(&cfg->walker.exclude.set, pat, rbn) {
    llist_push(&list, &pat->lln);
  }
  llist_foreach_safe(&list, pat, lln, T) {
    pattern_free(pat);
  }

  /* walker.special */
  llist_init(&list);
  rbtree_foreach(&cfg->walker.special.set, wse, rbn) {
    llist_push(&list, &wse->lln);
  }
  llist_foreach_safe(&list, wse, lln, T) {
    bacon_value_free(parser->bacon_parser, wse->v);
    free(wse);
  }

  /* filesystems */
  llist_init(&list);
  rbtree_foreach(&cfg->filesystems.set, fm, rbn) {
    llist_push(&list, &fm->lln);
  }

  llist_foreach_safe(&list, fm, lln, T) {
    bacon_value_free(parser->bacon_parser, fm->val.invalidate.v);
    bacon_value_free(parser->bacon_parser, fm->val.mkdirs.v);
    bacon_value_free(parser->bacon_parser, fm->val.readlinks.v);
    bacon_value_free(parser->bacon_parser, fm->val.indexed.v);
    free(fm);
  }

  configuration_dispose(cfg);
}

void configuration_ifree(configuration ** restrict cfg, config_parser * restrict parser)
{
  configuration_free(*cfg, parser);
  *cfg = 0;
}

void configuration_describe(const configuration * restrict cfg, bacon_writer * const restrict builder_writer)
{
  const struct configuration_filesystems_entry *fe;
  configuration_filesystems_mapping *fm;
  const configuration_formula_path_dirs_entry *ent;
  const configuration_walker_special_entry *wse;
  const pattern *pat;
  char buf[512];
  narrator_fixed fixed;

  if(cfg->build.concurrency.v)
  {
    MAPPING
    STRING("build")
    { SET
      { MAPPING
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
    }
  }
  if(!rbtree_empty(&cfg->walker.include.set) || !rbtree_empty(&cfg->walker.exclude.set) || !rbtree_empty(&cfg->walker.special.set))
  {
    MAPPING
    STRING("walker")
    { SET
      if(!rbtree_empty(&cfg->walker.include.set)) {
        MAPPING
        STRING("include")
        { SET
          rbtree_foreach(&cfg->walker.include.set, pat, rbn) {
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
      if(!rbtree_empty(&cfg->walker.exclude.set)) {
        MAPPING
        STRING("exclude")
        { SET
          rbtree_foreach(&cfg->walker.exclude.set, pat, rbn) {
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
      if(!rbtree_empty(&cfg->walker.special.set)) {
        MAPPING
        STRING("special")
        { SET
          rbtree_foreach(&cfg->walker.special.set, wse, rbn) {
            BYTES(wse->s, wse->len)
            COMMENTF("@ %s [%d,%d %d,%d]"
              , wse->v->loc->fname
              , wse->v->loc->f_lin + 1
              , wse->v->loc->f_col + 1
              , wse->v->loc->l_lin + 1
              , wse->v->loc->l_col + 1
            )
          }
        }
      }
    }
  }
  if(!rbtree_empty(&cfg->filesystems.set))
  {
    MAPPING
    STRING("filesystems")
    { SET
      rbtree_foreach(&cfg->filesystems.set, fm, rbn) {
        fe = &fm->val;
        { MAPPING
          BYTES(fm->key.path, fm->key.path_len)
          { SET
            if(fe->invalidate.v) {
              MAPPING
              STRING("invalidate")
              STRING(attrs16_name_byvalue(invalidate_attrs, fe->invalidate.e))
              COMMENTF("@ %s [%d,%d %d,%d]"
                , fe->invalidate.v->loc->fname
                , fe->invalidate.v->loc->f_lin + 1
                , fe->invalidate.v->loc->f_col + 1
                , fe->invalidate.v->loc->l_lin + 1
                , fe->invalidate.v->loc->l_col + 1
              )
            }
            if(fe->mkdirs.v) {
              MAPPING
              STRING("mkdirs")
              BOOL(fe->mkdirs.b)
              COMMENTF("@ %s [%d,%d %d,%d]"
                , fe->mkdirs.v->loc->fname
                , fe->mkdirs.v->loc->f_lin + 1
                , fe->mkdirs.v->loc->f_col + 1
                , fe->mkdirs.v->loc->l_lin + 1
                , fe->mkdirs.v->loc->l_col + 1
              )
            }
            if(fe->readlinks.v) {
              MAPPING
              STRING("readlinks")
              BOOL(fe->readlinks.b)
              COMMENTF("@ %s [%d,%d %d,%d]"
                , fe->readlinks.v->loc->fname
                , fe->readlinks.v->loc->f_lin + 1
                , fe->readlinks.v->loc->f_col + 1
                , fe->readlinks.v->loc->l_lin + 1
                , fe->readlinks.v->loc->l_col + 1
              )
            }
            if(fe->indexed.v) {
              MAPPING
              STRING("indexed")
              BOOL(fe->indexed.b)
              COMMENTF("@ %s [%d,%d %d,%d]"
                , fe->indexed.v->loc->fname
                , fe->indexed.v->loc->f_lin + 1
                , fe->indexed.v->loc->f_col + 1
                , fe->indexed.v->loc->l_lin + 1
                , fe->indexed.v->loc->l_col + 1
              )
            }
          }
        }
      }
    }
  }
  if(cfg->formula.path.copy_from_env.v || !rbtree_empty(&cfg->formula.path.dirs.set))
  {
    MAPPING
    STRING("formula")
    { SET
      { MAPPING
        STRING("path")
        { SET
          if(cfg->formula.path.copy_from_env.v) {
            MAPPING
            STRING("copy-from-env")
            BOOL(cfg->formula.path.copy_from_env.b)
            COMMENTF("@ %s [%d,%d %d,%d]"
              , cfg->formula.path.copy_from_env.v->loc->fname
              , cfg->formula.path.copy_from_env.v->loc->f_lin + 1
              , cfg->formula.path.copy_from_env.v->loc->f_col + 1
              , cfg->formula.path.copy_from_env.v->loc->l_lin + 1
              , cfg->formula.path.copy_from_env.v->loc->l_col + 1
            )
          }

          if(!rbtree_empty(&cfg->formula.path.dirs.set))
          {
            MAPPING
            STRING("dirs")
            { SET
              rbtree_foreach(&cfg->formula.path.dirs.set, ent, rbn) {
                BYTES(ent->s, ent->len);
                COMMENTF("@ %s [%d,%d %d,%d]"
                  , ent->v->loc->fname
                  , ent->v->loc->f_lin + 1
                  , ent->v->loc->f_col + 1
                  , ent->v->loc->l_lin + 1
                  , ent->v->loc->l_col + 1
                )
              }
            }
          }
        }
      }
    }
  }
  if(cfg->timeout.interval.v)
  {
    MAPPING
    STRING("timeout")
    { SET
      { MAPPING
        STRING("interval")
        UINT(cfg->timeout.interval.v->i)
      }
    }
  }
}

void configuration_show(const configuration * restrict cfg)
{
  bacon_writer writer, *builder_writer;

  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, g_narrator_stdout);

  configuration_describe(cfg, builder_writer);

  bacon_writer_close(&writer);
  bacon_writer_destroy(&writer);
  printf("\n");
}
