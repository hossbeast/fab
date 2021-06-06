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
#include "types.h"

#include "common/attrs.h"
#include "narrator.h"
#include "value/writer.h"
#include "xlinux/xstdlib.h"
#include "valyria/llist.h"

#include "request.internal.h"
#include "selector.internal.h"

attrs32 * command_type_attrs = (attrs32[]) {{
#undef DEF
#define DEF(x, s, r) + 1
    num : 0 COMMAND_TYPE_TABLE
  , members : (member32[]) {
#undef DEF
#define DEF(x, s, r) { name : s, value : x, range : UINT32_C(r) },
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

static llist command_freelist = LLIST_INITIALIZER(command_freelist);

static void __attribute__((constructor)) init()
{
  attrs32_init(command_type_attrs);
  attrs32_init(traverse_type_attrs);
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

static xapi request_writer_write(request * const restrict req, value_writer * const restrict writer)
{
  enter;

  command *cmd;

  llist_foreach(&req->commands, cmd, lln) {
    switch(cmd->type)
    {
      case COMMAND_RUN:
        fatal(value_writer_string, writer, "run");
        break;
      case COMMAND_DESCRIBE:
        fatal(value_writer_string, writer, "describe");
        break;
      case COMMAND_INVALIDATE:
        fatal(value_writer_string, writer, "invalidate");
        break;
      case COMMAND_GLOBAL_INVALIDATE:
        fatal(value_writer_string, writer, "global-invalidate");
        break;
      case COMMAND_CONFIG_READ:
        fatal(value_writer_string, writer, "config-read");
        break;
      case COMMAND_LIST:
        fatal(value_writer_string, writer, "list");
        break;
      case COMMAND_RESET_SELECTION:
        fatal(value_writer_string, writer, "reset-selection");
        break;
      case COMMAND_GLOBAL_STATS_READ:
        fatal(value_writer_string, writer, "global-stats-read");
        break;
      case COMMAND_GLOBAL_STATS_RESET:
        fatal(value_writer_string, writer, "global-stats-reset");
        break;
      case COMMAND_STATS_READ:
        fatal(value_writer_string, writer, "stats-read");
        break;
      case COMMAND_STATS_RESET:
        fatal(value_writer_string, writer, "stats-reset");
        break;
      case COMMAND_SELECT:
        fatal(value_writer_push_mapping, writer);
        fatal(value_writer_string, writer, "select");
          fatal(value_writer_push_list, writer);
          fatal(selector_writer_write, cmd->selector, writer);
          fatal(value_writer_pop_list, writer);
        fatal(value_writer_pop_mapping, writer);
        break;
      case COMMAND_GOALS:
        fatal(value_writer_push_mapping, writer);
        fatal(value_writer_string, writer, "goals");
          fatal(value_writer_push_set, writer);
          if(cmd->goals.build)
            fatal(value_writer_string, writer, "build");
          if(cmd->goals.script)
            fatal(value_writer_string, writer, "script");
          if(cmd->goals.target_direct)
          {
            fatal(value_writer_push_mapping, writer);
              fatal(value_writer_string, writer, "target-direct");
              fatal(value_writer_push_list, writer);
                fatal(selector_writer_write, cmd->goals.target_direct, writer);
              fatal(value_writer_pop_list, writer);
            fatal(value_writer_pop_mapping, writer);
          }
          if(cmd->goals.target_transitive)
          {
            fatal(value_writer_push_mapping, writer);
              fatal(value_writer_string, writer, "target-transitive");
              fatal(value_writer_push_list, writer);
                fatal(selector_writer_write, cmd->goals.target_transitive, writer);
              fatal(value_writer_pop_list, writer);
            fatal(value_writer_pop_mapping, writer);
          }
          fatal(value_writer_pop_set, writer);
        fatal(value_writer_pop_mapping, writer);
        break;
      case COMMAND_METADATA:
        fatal(value_writer_string, writer, "metadata");
        break;
      case COMMAND_BOOTSTRAP:
        fatal(value_writer_string, writer, "bootstrap");
        break;
      case COMMAND_RECONCILE:
        fatal(value_writer_string, writer, "reconcile");
        break;
      default:
printf("type %d\n", cmd->type);
        RUNTIME_ABORT();
    }
  }

  finally : coda;
}

//
// public
//

xapi request_cleanup()
{
  enter;

  command *cmd;
  llist *T;

  llist_foreach_safe(&command_freelist, cmd, lln, T) {
    wfree(cmd);
  }

  finally : coda;
}

void request_init(request * restrict req)
{
  llist_init_node(&req->commands);
}

void request_destroy(request * restrict req)
{
  command *cmd;
  llist *T;

  llist_foreach_safe(&req->commands, cmd, lln, T) {
    command_destroy(cmd);
    llist_delete(cmd, lln);
    llist_append(&command_freelist, cmd, lln);
  }
}

xapi request_say(request * restrict req, narrator * restrict N)
{
  enter;

  value_writer * writer = 0;
  fatal(value_writer_create, &writer);
  fatal(value_writer_open, writer, N);

  fatal(request_writer_write, req, writer);

  fatal(value_writer_close, writer);

finally:
  fatal(value_writer_xfree, writer);
coda;
}

xapi request_command_alloc(request * restrict req, command ** restrict cmdp)
{
  enter;

  command *cmd;

  if((cmd = llist_shift(&command_freelist, typeof(*cmd), lln)) == 0) {
    fatal(xmalloc, &cmd, sizeof(*cmd));
  } else {
    memset(cmd, 0, sizeof(*cmd));
  }

  llist_append(&req->commands, cmd, lln);

  *cmdp = cmd;

  finally : coda;
}
