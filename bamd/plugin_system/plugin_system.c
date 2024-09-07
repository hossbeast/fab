/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>
#include <signal.h>

#include "narrator.h"
#include "narrator/fixed.h"
#include "valyria/rbtree.h"
#include "valyria/llist.h"
#include "bacon/builder.h"
#include "bacon/location.h"
#include "bacon/value.h"
#include "bam/sigutil.h"

#include "channel.h"
#include "config.h"
#include "fsent.h"
#include "system.h"
#include "plugin_system.h"
#include "params.h"
#include "plugin.h"
#include "plugin_thread.h"
#include "vertex.h"

#include "events.h"

llist plugin_invalid_list = LLIST_INITIALIZER(plugin_invalid_list);
static rbtree *plugins_path_dirs_set;
static llist *plugins_list;

static fsent *plugin_resolve(config_plugin_entry *pe)
{
  config_plugin_path_dirs_entry *ppde;
  fsent *base;
  fsent *fse;
  narrator_fixed fixed;
  bacon_writer writer, *builder_writer;
  events_context ectx;
  event_bacon *msg;

  rbtree_foreach(plugins_path_dirs_set, ppde, rbn) {
    if((base = fsent_path_lookup(g_root, 0, ppde->s, ppde->len, true))) {
      if((fse = fsent_path_lookup(base, 0, pe->path.s, pe->path.len, true))) {
        return fse;
      }
    }
  }
  if((fse = fsent_path_lookup(g_root, 0, pe->path.s, pe->path.len, true))) {
    return fse;
  }

  msg = system_error_event(&ectx);
  narrator_fixed_init(&fixed, msg->text, sizeof(msg->text));
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, &fixed.base);

  { MAPPING
    STRING("unresolved plugin")
    { SET
      { MAPPING
        STRING("path")
        BYTES(pe->path.s, pe->path.len)
      }
      { MAPPING
        STRING("location")
        LOADF("%s [%d,%d - %d,%d]", pe->path.v->loc->fname, pe->path.v->loc->f_lin + 1, pe->path.v->loc->f_col + 1, pe->path.v->loc->l_lin + 1, pe->path.v->loc->l_col + 1);
      }
    }
  }

  bacon_writer_close(&writer);
  msg->code = EINVAL;
  msg->size = fixed.l;
  events_publish(&ectx);

  return 0;
}

static void plugins_resolve(graph_invalidation_context * restrict invalidation)
{
  config_plugin_entry *pe;
  fsent *fse;

  llist_foreach(plugins_list, pe, lln) {
    if((!(fse = plugin_resolve(pe)))) {
      break;
    }

    fsent_plugin_bootstrap(fse, invalidation);
    if(pe->args.list) {
      fse->self_plugin->args = &pe->args.list->list;
    }
  }
}

void plugin_system_reconfigure(config * restrict cfg, bool dryrun, graph_invalidation_context * restrict invalidation)
{
  plugins_path_dirs_set = &cfg->plugins.path.dirs.set;
  plugins_list = &cfg->plugins.list;
}

void plugin_system_reconcile(graph_invalidation_context * restrict invalidation)
{
  sigset_t sigs;
  siginfo_t siginfo;
  struct plugin *plugin;
  bool any;
  struct plugin_thread *thread;

  /* raises an error if any plugin doesnt resolve */
  plugins_resolve(invalidation);
  if(system_error) {
    return;
  }

  // signals handled on this thread
  sigemptyset(&sigs);
  sigaddset(&sigs, SIGUSR1);

  /* kill any invalidated plugins */
  any = false;
  rcu_list_foreach(&plugin_thread_list, thread, stk) {
    if(!vertex_invalid_get(&thread->plugin->vertex)) {
      continue;
    }

    any = true;
    plugin_thread_kill(thread);
  }

  /* wait for plugins to terminate */
  while(any)
  {
    sigutil_wait(&sigs, &siginfo);

    any = false;
    rcu_list_foreach(&plugin_thread_list, thread, stk) {
      if(!vertex_invalid_get(&thread->plugin->vertex)) {
        continue;
      }
      any = true;
      plugin_thread_kill(thread);
    }
  }

  /* launch plugins */
  any = false;
  llist_foreach(&plugin_list, plugin, vertex.owner) {
    if(!vertex_invalid_get(&plugin->vertex)) {
      continue;
    }

    any = true;
    plugin_thread_launch(plugin);
  }

  /* ask plugins for readiness */
  rcu_list_foreach(&plugin_thread_list, thread, stk) {
    any = true;
    plugin_thread_unready(thread);
  }

  /* wait for plugins to quiesce */
  while(any)
  {
    sigutil_wait(&sigs, &siginfo);

    any = false;
    rcu_list_foreach(&plugin_thread_list, thread, stk) {
      if(!thread->ready_quiescent || !thread->ready_reported) {
        any = true;
        break;
      }
    }
  }
}
