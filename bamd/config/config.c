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
#include "bam/config_tell.desc.h"
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

#include "config.h"
#include "configset.h"
#include "channel.h"
#include "pattern.h"
#include "configset_parser.h"
#include "graph.h"
#include "marshal.h"
#include "zcat.h"

static llist freelist = LLIST_INITIALIZER(freelist);
struct moria_vertex *g_config = (moria_vertex[]) {{ }};

static void __attribute__((nonnull)) config_dispose(config * restrict n)
{
  llist_delete(&n->owner);
  llist_append(&freelist, &n->owner);
}

void config_setup()
{
  moria_vertex_initw(g_config, &g_graph, VERTEX_CONFIG | VERTEX_OK, MMS("(config)"));
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
  rbtree_init(&cfg->formula.path.dirs.set);
  llist_init(&cfg->plugins.list);
  rbtree_init(&cfg->plugins.path.dirs.set);

  *rv = cfg;
}

void config_free(config * restrict cfg, configset_parser * restrict parser)
{
  pattern *pat;
  config_fstree_mapping *fsm;
  config_special_entry *wse;
  config_formula_path_dirs_entry *fpde;
  config_plugin_entry *pe;
  config_plugin_path_dirs_entry *ppde;
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
  rbtree_foreach(&cfg->filesystem.special.set, wse, rbn) {
    llist_push(&list, &wse->lln);
  }
  llist_foreach_safe(&list, wse, lln, T) {
    bacon_value_free(parser->bacon_parser, wse->v);
    free(wse);
  }

  /* filesystem.fstrees */
  llist_init(&list);
  rbtree_foreach(&cfg->filesystem.fstrees.set, fsm, rbn) {
    llist_push(&list, &fsm->lln);
  }

  llist_foreach_safe(&list, fsm, lln, T) {
    pattern_free(fsm->key.v);
    bacon_value_free(parser->bacon_parser, fsm->val.invalidation.v);
    bacon_value_free(parser->bacon_parser, fsm->val.mkdir.v);
    bacon_value_free(parser->bacon_parser, fsm->val.readlink.v);
    bacon_value_free(parser->bacon_parser, fsm->val.index.v);
    bacon_value_free(parser->bacon_parser, fsm->val.mount.v);
    free(fsm);
  }

  /* plugins.path.dirs */
  llist_init(&list);
  rbtree_foreach(&cfg->plugins.path.dirs.set, ppde, rbn) {
    llist_push(&list, &ppde->lln);
  }
  llist_foreach_safe(&list, ppde, lln, T) {
    bacon_value_free(parser->bacon_parser, ppde->v);
    free(ppde);
  }

  /* plugins.list */
  llist_init(&list);
  llist_foreach_safe(&cfg->plugins.list, pe, lln, T) {
    bacon_value_free(parser->bacon_parser, pe->path.v);
    bacon_value_free(parser->bacon_parser, pe->args.list);
    free(pe);
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
  const config_formula_path_dirs_entry *ent;
  const config_special_entry *wse;
  const config_plugin_entry *pe;
  const config_plugin_path_dirs_entry *ppde;
  const pattern *pat;
  const bacon_value *v;
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
  if(!rbtree_empty(&cfg->filesystem.fstrees.set) || !rbtree_empty(&cfg->filesystem.exclude.set) || !rbtree_empty(&cfg->filesystem.special.set))
  {
    MAPPING
    STRING("filesystem")
    { SET
      if(!rbtree_empty(&cfg->filesystem.fstrees.set)) {
        MAPPING
        STRING("fstrees")
        { SET
          rbtree_foreach(&cfg->filesystem.fstrees.set, fsm, rbn) {
            if(fsm->val.index.v || fsm->val.readlink.v || fsm->val.mkdir.v || fsm->val.invalidation.v || fsm->val.mount.v) {
              bacon_writer_push_mapping(builder_writer);
            }

            bacon_writer_push_bytes(builder_writer);
            pattern_say(fsm->key.v, builder_writer->N);
            bacon_writer_pop_bytes(builder_writer);

            if(fsm->val.index.v || fsm->val.readlink.v || fsm->val.mkdir.v || fsm->val.invalidation.v || fsm->val.mount.v) {
              bacon_writer_push_set(builder_writer);
            } else {
              COMMENTF("@ %s [%d,%d %d,%d]"
                , fsm->key.v->fname
                , fsm->key.v->loc.f_lin + 1
                , fsm->key.v->loc.f_col + 1
                , fsm->key.v->loc.l_lin + 1
                , fsm->key.v->loc.l_col + 1
              )
            }

            if(fsm->val.invalidation.v) {
              MAPPING
              STRING("invalidation")
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
              MAPPING
              STRING("mkdir")
              BOOL(fsm->val.mkdir.b)
              COMMENTF("@ %s [%d,%d %d,%d]"
                , fsm->val.mkdir.v->loc->fname
                , fsm->val.mkdir.v->loc->f_lin + 1
                , fsm->val.mkdir.v->loc->f_col + 1
                , fsm->val.mkdir.v->loc->l_lin + 1
                , fsm->val.mkdir.v->loc->l_col + 1
              )
            }
            if(fsm->val.readlink.v) {
              MAPPING
              STRING("readlink")
              BOOL(fsm->val.readlink.b)
              COMMENTF("@ %s [%d,%d %d,%d]"
                , fsm->val.readlink.v->loc->fname
                , fsm->val.readlink.v->loc->f_lin + 1
                , fsm->val.readlink.v->loc->f_col + 1
                , fsm->val.readlink.v->loc->l_lin + 1
                , fsm->val.readlink.v->loc->l_col + 1
              )
            }
            if(fsm->val.index.v) {
              MAPPING
              STRING("index")
              BOOL(fsm->val.index.b)
              COMMENTF("@ %s [%d,%d %d,%d]"
                , fsm->val.index.v->loc->fname
                , fsm->val.index.v->loc->f_lin + 1
                , fsm->val.index.v->loc->f_col + 1
                , fsm->val.index.v->loc->l_lin + 1
                , fsm->val.index.v->loc->l_col + 1
              )
            }
            if(fsm->val.mount.v) {
              MAPPING
              STRING("mount")
              BOOL(fsm->val.mount.b)
              COMMENTF("@ %s [%d,%d %d,%d]"
                , fsm->val.mount.v->loc->fname
                , fsm->val.mount.v->loc->f_lin + 1
                , fsm->val.mount.v->loc->f_col + 1
                , fsm->val.mount.v->loc->l_lin + 1
                , fsm->val.mount.v->loc->l_col + 1
              )
            }

            if(fsm->val.index.v || fsm->val.readlink.v || fsm->val.mkdir.v || fsm->val.invalidation.v || fsm->val.mount.v) {
              bacon_writer_pop_set(builder_writer);
              bacon_writer_pop_mapping(builder_writer);
            }
          }
        }
      }
      if(!rbtree_empty(&cfg->filesystem.exclude.set)) {
        MAPPING
        STRING("exclude")
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
          rbtree_foreach(&cfg->filesystem.special.set, wse, rbn) {
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
  if(!llist_empty(&cfg->plugins.list) || !rbtree_empty(&cfg->plugins.path.dirs.set))
  {
    MAPPING
    STRING("plugins")
    { SET
      if(!rbtree_empty(&cfg->plugins.path.dirs.set)) {
        MAPPING
        STRING("path")
        { SET
          { MAPPING
            STRING("dirs")
            { SET
              rbtree_foreach(&cfg->plugins.path.dirs.set, ppde, rbn) {
                BYTES(ppde->s, ppde->len);
                COMMENTF("@ %s [%d,%d %d,%d]"
                  , ppde->v->loc->fname
                  , ppde->v->loc->f_lin + 1
                  , ppde->v->loc->f_col + 1
                  , ppde->v->loc->l_lin + 1
                  , ppde->v->loc->l_col + 1
                )
              }
            }
          }
        }
      }
      if(!llist_empty(&cfg->plugins.list)) {
        LIST
        llist_foreach(&cfg->plugins.list, pe, lln) {
          SET
          { MAPPING
            STRING("path")
            BYTES(pe->path.s, pe->path.len)
            COMMENTF("@ %s [%d,%d %d,%d]"
              , pe->path.v->loc->fname
              , pe->path.v->loc->f_lin + 1
              , pe->path.v->loc->f_col + 1
              , pe->path.v->loc->l_lin + 1
              , pe->path.v->loc->l_col + 1
            )
          }
          if(pe->args.list) {
            MAPPING
            STRING("args")
            { LIST
              llist_foreach(&pe->args.list->list, v, lln) {
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

void config_vertex_tell(struct channel * restrict channel)
{
  bamipc_message *msg;
  narrator_growing growing;
  narrator * N;
  int x;
  configset *cfgn;
  size_t z;
  size_t list_size;
  size_t list_size_off;
  size_t list_len_off;
  char *dst;
  size_t sz;

  msg = channel_produce(channel);
  msg->type = BAMIPC_MSG_RESULT;
  msg->attrs = descriptor_bam_config_tell_result.id;
  dst = msg->text;
  sz = sizeof(msg->text);
  z = 0;
  z += marshal_u32(dst + z, sz - z, descriptor_bam_config_tell_result.id);

  /* list each contributing config file */
  z += align(z, 4);
  list_size_off = z;
  z += marshal_u32(dst + z, sz - z, 0);            /* size of the list blob */
  z += align(z, 2);
  list_len_off = z;
  z += marshal_u16(dst + z, sz - z, 0);            /* items in the list blob */

  x = 0;
  llist_foreach(&configset_list, cfgn, vertex.owner) {
    z += marshal_u32(dst + z, sz - z, descriptor_bam_config_tell_item.id);

    /* id */
    z += marshal_u16(dst + z, sz - z, cfgn->id_len);
    z += zcatw(dst + z, sz - z, cfgn->id, cfgn->id_len);

    /* abspath */
    z += marshal_u16(dst + z, sz - z, cfgn->self_node_abspath_len);
    z += zcatw(dst + z, sz - z, cfgn->self_node_abspath, cfgn->self_node_abspath_len);
    x++;
  }

  list_size = z - list_size_off;
  marshal_u32(dst + list_size_off, sz - list_size_off, list_size);
  marshal_u16(dst + list_len_off, sz - list_len_off, x);

  msg->size = z;
  channel_post(channel, msg);

  N = narrator_growing_init(&growing);
  if(config_active) {
    config_say(config_active, N);
  }

  x = 0;
  while(x < growing.l)
  {
    msg = channel_produce(channel);
    msg->type = BAMIPC_MSG_RESULT;

    msg->size = MIN(growing.l - x, sizeof(msg->text));
    memcpy(msg->text, growing.s + x, msg->size);

    channel_post(channel, msg);
    x += msg->size;
  }

  narrator_growing_destroy(&growing);
}

void config_vertex_stats_build(bool reset, bacon_writer * const restrict builder_writer)
{
  RTA;
}

void config_vertex_describe(bacon_writer * const restrict builder_writer)
{
  { MAPPING
    STRING("//config")
    LOADF("%p", config_active)
  }
}
