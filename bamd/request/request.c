/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "types.h"

#include "common/attrs.h"
#include "narrator.h"
#include "bacon/builder.h"
#include "xlinux/xstdlib.h"
#include "valyria/llist.h"
#include "bam/events.h"

#include "request.h"
#include "selector.internal.h"

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

attrs32 * traverse_type_attrs = (attrs32[]) {{
#undef TRAVERSE_ATTR_DEF
#define TRAVERSE_ATTR_DEF(x, s, r, y) + 1
    num : 0 TRAVERSE_TYPE_TABLE
  , members : (member32[]) {
#undef TRAVERSE_ATTR_DEF
#define TRAVERSE_ATTR_DEF(x, s, r, y) { name : s, value : y, range : UINT32_C(r) },
TRAVERSE_TYPE_TABLE
  }
}};

attrs32 * traverse_order_attrs = (attrs32[]) {{
#undef TRAVERSE_ATTR_DEF
#define TRAVERSE_ATTR_DEF(x, s, r, y) + 1
    num : 0 TRAVERSE_ORDER_TABLE
  , members : (member32[]) {
#undef TRAVERSE_ATTR_DEF
#define TRAVERSE_ATTR_DEF(x, s, r, y) { name : s, value : y, range : UINT32_C(r) },
TRAVERSE_ORDER_TABLE
  }
}};

static llist command_freelist = LLIST_INITIALIZER(command_freelist);

static void __attribute__((constructor)) init()
{
  attrs32_init(command_type_attrs);
  attrs32_init(traverse_type_attrs);
  attrs32_init(traverse_order_attrs);
}

//
// static
//

static void command_destroy(command * cmd)
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

static void command_writer_write(command * const restrict cmd, bacon_writer * const restrict builder_writer)
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
    case COMMAND_INVALIDATE_PRIMARY:
    case COMMAND_PLUGIN_READY:
    case COMMAND_AUTORUN:
    case COMMAND_RUN:
      STRING(attrs32_name_byvalue(command_type_attrs, cmd->type));
      break;
    case COMMAND_SELECT:
      { MAPPING
        STRING("select");
        { LIST
          selector_writer_write(cmd->selector, builder_writer);
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
              selector_writer_write(cmd->goals.target_direct, builder_writer);
            }
          }
          if(cmd->goals.target_transitive) {
            MAPPING
            STRING("target-transitive");
            { LIST
              selector_writer_write(cmd->goals.target_transitive, builder_writer);
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
    case COMMAND_CONTEXT:
      { MAPPING
        STRING("context");
        { SET
          { MAPPING
            STRING("module")
            BYTES(cmd->context.module_id, cmd->context.module_id_len)
          }
        }
      }
      break;
      RTA;
  }
}

static void request_writer_write(request * const restrict req, bacon_writer * const restrict builder_writer)
{
  command *cmd;

  llist_foreach(&req->commands, cmd, lln) {
    command_writer_write(cmd, builder_writer);
  }
}

//
// public
//

void request_cleanup()
{
  command *cmd;
  llist *T;

  llist_foreach_safe(&command_freelist, cmd, lln, T) {
    free(cmd);
  }
}

void request_init(request * restrict req)
{
  memset(req, 0, sizeof(*req));
  llist_init(&req->commands);
}

void request_destroy(request * restrict req)
{
  command *cmd;

  while((cmd = llist_shift(&req->commands, typeof(*cmd), lln))) {
    request_command_free(cmd);
  }
}

void request_say(request * restrict req, narrator * restrict N)
{
  bacon_writer * writer = 0;
  bacon_writer_create(&writer);
  bacon_writer_open(writer, N);

  request_writer_write(req, writer);

  bacon_writer_close(writer);
  bacon_writer_free(writer);
}

void command_say(command * restrict cmd, narrator * restrict N)
{
  bacon_writer * writer = 0;
  bacon_writer_create(&writer);
  bacon_writer_open(writer, N);

  command_writer_write(cmd, writer);

  bacon_writer_close(writer);
  bacon_writer_free(writer);
}

void command_show(command * restrict cmd)
{
  bacon_writer * writer = 0;
  bacon_writer_create(&writer);
  bacon_writer_open(writer, g_narrator_stdout);

  command_writer_write(cmd, writer);

  bacon_writer_close(writer);
  bacon_writer_free(writer);
  printf("\n");
}

command *request_command_alloc(request * restrict req)
{
  command *cmd;

  if((cmd = llist_shift(&command_freelist, typeof(*cmd), lln)) == 0) {
    xmalloc(&cmd, sizeof(*cmd));
  } else {
    memset(cmd, 0, sizeof(*cmd));
  }

  return cmd;
}

void request_command_free(command *cmd)
{
  if(!cmd) {
    return;
  }

  command_destroy(cmd);
  llist_push(&command_freelist, &cmd->lln);
}
