/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "types.h"

#include "common/attrs.h"
#include "narrator.h"
#include "bacon/builder.h"
#include "xlinux/xstdlib.h"
#include "valyria/llist.h"

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

static void request_writer_write(request * const restrict req, bacon_writer * const restrict builder_writer)
{
  command *cmd;

  llist_foreach(&req->commands, cmd, lln) {
    switch(cmd->type)
    {
      case COMMAND_GLOBAL_STATS_READ:
      case COMMAND_GLOBAL_STATS_RESET:
      case COMMAND_STATS_READ:
      case COMMAND_STATS_RESET:
      case COMMAND_CONFIG_DESCRIBE:
      case COMMAND_VARS_DESCRIBE:
      case COMMAND_ENVS_DESCRIBE:
      case COMMAND_RMA_DESCRIBE:
      case COMMAND_RULES_DESCRIBE:
      case COMMAND_FSTREE_DESCRIBE:
      case COMMAND_BUILDPLAN_DESCRIBE:
      case COMMAND_FILESYSTEM_DESCRIBE:
      case COMMAND_RESET_SELECTION:
      case COMMAND_LIST:
      case COMMAND_DESCRIBE:
      case COMMAND_INVALIDATE:
      case COMMAND_GLOBAL_INVALIDATE:
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
            if(cmd->goals.script) {
              STRING("script");
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
      case COMMAND_METADATA:
        STRING("metadata");
        break;
      case COMMAND_RECONCILE:
        STRING("reconcile");
        break;
      case COMMAND_CONTEXT:
        { MAPPING
          STRING("context");
          { SET
            { MAPPING
              STRING("module")
              HEX(cmd->context.module_id)
            }
          }
        }
        break;
        RTA;
    }
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
  llist *T;

  llist_foreach_safe(&req->commands, cmd, lln, T) {
    command_destroy(cmd);
    llist_delete(&cmd->lln);
    llist_append(&command_freelist, &cmd->lln);
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

void request_command_alloc(request * restrict req, command ** restrict cmdp)
{
  command *cmd;

  if((cmd = llist_shift(&command_freelist, typeof(*cmd), lln)) == 0) {
    xmalloc(&cmd, sizeof(*cmd));
  } else {
    memset(cmd, 0, sizeof(*cmd));
  }

  llist_append(&req->commands, &cmd->lln);

  *cmdp = cmd;
}
