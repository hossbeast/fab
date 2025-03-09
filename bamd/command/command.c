/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "common/attrs.h"
#include "narrator.h"
#include "bacon/builder.h"
#include "bacon/writer.h"
#include "xlinux/xstdlib.h"
#include "bam/events.h"

#include "command.h"
#include "selector.h"
#include "locks.h"

attrs32 * command_type_attrs = (attrs32[]) {{
#undef DEF
#define DEF(x, s, r) + 1
    num : 0 COMMAND_TYPE_TABLE
  , members : (member32[]) {
#undef DEF
#define DEF(x, s, y) { name : s, value : x, range : COMMAND_TYPE_OPT },
COMMAND_TYPE_TABLE
  }
}};

static llist freelist;
static struct futexlock freelist_lock;

static void __attribute__((constructor)) init()
{
  attrs32_init(command_type_attrs);
  llist_init(&freelist);
}

//
// static
//

static void __attribute__((nonnull)) command_destroy(command * cmd)
{
  if(cmd->type == COMMAND_SELECT)
  {
    selector_free(cmd->selector);
  }
  else if(cmd->type == COMMAND_GOALS)
  {
    selector_free(cmd->goals.target_direct);
    selector_free(cmd->goals.target_transitive);
  }
}

void command_describe(command * const restrict cmd, bacon_writer * const restrict builder_writer)
{
  int x;

  switch(cmd->type)
  {
    case COMMAND_GLOBAL_STATS_READ:
    case COMMAND_GLOBAL_STATS_RESET:
    case COMMAND_STATS_READ:
    case COMMAND_STATS_RESET:
    case COMMAND_RESET_SELECTION:
    case COMMAND_TREE_LIST:
    case COMMAND_DESCRIBE:
    case COMMAND_IDENTIFY:
    case COMMAND_TELL:
    case COMMAND_LIST:
    case COMMAND_INVALIDATE:
    case COMMAND_MODULES_INVALIDATE_PRIMARY:
    case COMMAND_PLUGIN_READY:
    case COMMAND_AUTORUN:
    case COMMAND_RUN:
      STRING(attrs32_name_byvalue(command_type_attrs, cmd->type));
      break;
    case COMMAND_SELECT:
      { MAPPING
        STRING("select");
        { LIST
          selector_describe(cmd->selector, builder_writer);
        }
      }
      break;
    case COMMAND_GOALS:
      { MAPPING
        STRING("goals");
        { SET
          if(cmd->goals.build) {
            STRING("build");
          }
          if(cmd->goals.target_direct) {
            MAPPING
            STRING("target-direct")
            { LIST
              selector_describe(cmd->goals.target_direct, builder_writer);
            }
          }
          if(cmd->goals.target_transitive) {
            MAPPING
            STRING("target-transitive");
            { LIST
              selector_describe(cmd->goals.target_transitive, builder_writer);
            }
          }
        }
      }
      break;
    case COMMAND_EVENTSUB:
      { MAPPING
        STRING("eventsub")
        { LIST
          for(x = 0; x < bamipc_event_type_attrs.num; x++) {
            if((cmd->mask & bamipc_event_type_attrs.members[x].range) == bamipc_event_type_attrs.members[x].value) {
              BYTES(bamipc_event_type_attrs.members[x].name, bamipc_event_type_attrs.members[x].namel)
            }
          }
        }
      }
    case COMMAND_RECONCILE:
      STRING("reconcile");
      break;
    case COMMAND_CONTEXT_MODULE:
      STRING("context-module");
      break;
      RTA;
  }
}

//
// public
//

void command_say(command * restrict cmd, narrator * restrict N)
{
  bacon_writer * writer = 0;
  bacon_writer_create(&writer);
  bacon_writer_open(writer, N);

  command_describe(cmd, writer);

  bacon_writer_close(writer);
  bacon_writer_free(writer);
}

void command_show(command * restrict cmd)
{
  command_say(cmd, g_narrator_stdout);
  printf("\n");
}

command *command_alloc()
{
  command *cmd = 0;

  if(!llist_empty(&freelist)) {
    futexlock_acquire(&freelist_lock);
    cmd = llist_shift(&freelist, typeof(*cmd), lln);
    futexlock_release(&freelist_lock);
  }

  if(cmd) {
    command_destroy(cmd);
    memset(cmd, 0, sizeof(*cmd));
  } else {
    xmalloc(&cmd, sizeof(*cmd));
  }

  llist_init(&cmd->lln);

  return cmd;
}

void command_free(command *cmd)
{
  if(!cmd) {
    return;
  }

  futexlock_acquire(&freelist_lock);
  llist_push(&freelist, &cmd->lln);
  futexlock_release(&freelist_lock);
}

void command_free_list(llist * restrict list)
{
  futexlock_acquire(&freelist_lock);
  llist_splice_head(&freelist, list);
  futexlock_release(&freelist_lock);
}

void command_cleanup()
{
  command *cmd;

  while((cmd = llist_shift(&freelist, typeof(*cmd), lln))) {
    command_destroy(cmd);
    free(cmd);
  }
}
