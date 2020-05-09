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

#include "narrator.h"
#include "xlinux/xstdlib.h"
#include "valyria/list.h"
#include "valyria/array.h"
#include "value.h"
#include "value/parser.h"
#include "value/VALUE.errtab.h"
#include "value/writer.h"

#include "request.internal.h"
#include "selector.internal.h"
#include "config.internal.h"

#include "attrs.h"
#include "assure.h"

attrs32 * command_type_attrs = (attrs32[]) {{
#define DEF(x, s, r, y) + 1
    num : 0 COMMAND_TYPE_TABLE
  , members : (member32[]) {
#undef DEF
#define DEF(x, s, r, y) { name : s, value : UINT32_C(y), range : UINT32_C(r) },
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

static void __attribute__((constructor)) init()
{
  attrs32_init(command_type_attrs);
  attrs32_init(traverse_type_attrs);
}

//
// static
//

static xapi command_xdestroy(command * cmd)
{
  enter;

  if(cmd->type == COMMAND_SELECT)
  {
    selector_free(cmd->selector);
  }
  else if(cmd->type == COMMAND_STAGE_CONFIG)
  {
    fatal(config_xfree, cmd->config);
  }
  else if(cmd->type == COMMAND_GOALS)
  {
    selector_free(cmd->goals.target_direct);
    selector_free(cmd->goals.target_transitive);
  }

  finally : coda;
}

static xapi request_writer_write(request * const restrict req, value_writer * const restrict writer)
{
  enter;

  int x;
  for(x = 0; x < req->commands->size; x++)
  {
    command * cmd = array_get(req->commands, x);

    if(cmd->type == COMMAND_RUN)
    {
      fatal(value_writer_string, writer, "run");
    }
    else if(cmd->type == COMMAND_AUTORUN)
    {
      fatal(value_writer_string, writer, "autorun");
    }
    else if(cmd->type == COMMAND_DESCRIBE)
    {
      fatal(value_writer_string, writer, "describe");
    }
    else if(cmd->type == COMMAND_INVALIDATE)
    {
      fatal(value_writer_string, writer, "invalidate");
    }
    else if(cmd->type == COMMAND_LIST)
    {
      if(cmd->property)
      {
        fatal(value_writer_push_mapping, writer);
        fatal(value_writer_string, writer, "list");
        const char * property = attrs32_name_byvalue(node_property_attrs, NODE_PROPERTY_OPT & cmd->property);
        RUNTIME_ASSERT(property != NULL);
        fatal(value_writer_string, writer, property);
        fatal(value_writer_pop_mapping, writer);
      }
      else
      {
        fatal(value_writer_string, writer, "list");
      }
    }
    else if(cmd->type == COMMAND_RESET_SELECTION)
    {
      fatal(value_writer_string, writer, "reset-selection");
    }
    else if(cmd->type == COMMAND_RECONFIGURE)
    {
      fatal(value_writer_string, writer, "reconfigure");
    }
    else if(cmd->type == COMMAND_SELECT)
    {
      fatal(value_writer_push_mapping, writer);
      fatal(value_writer_string, writer, "select");
        fatal(value_writer_push_list, writer);
        fatal(selector_writer_write, cmd->selector, writer);
        fatal(value_writer_pop_list, writer);
      fatal(value_writer_pop_mapping, writer);
    }
    else if(cmd->type == COMMAND_STAGE_CONFIG)
    {
      fatal(value_writer_push_mapping, writer);
      fatal(value_writer_string, writer, "stage-config");
        fatal(value_writer_push_set, writer);
        fatal(config_writer_write, cmd->config, writer);
        fatal(value_writer_pop_set, writer);
      fatal(value_writer_pop_mapping, writer);
    }
    else if(cmd->type == COMMAND_GOALS)
    {
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
    }
    else
    {
      RUNTIME_ABORT();
    }
  }

  finally : coda;
}

//
// public
//

xapi request_setup()
{
  enter;

  finally : coda;
}

xapi request_cleanup()
{
  enter;

  finally : coda;
}

xapi request_create(request ** restrict rv)
{
  enter;

  request * req = 0;

  fatal(xmalloc, &req, sizeof(*req));
  fatal(array_createx, &req->commands, sizeof(command), 0, 0, 0, 0, 0, command_xdestroy);

  *rv = req;
  req = 0;

finally:
  fatal(request_xfree, req);
coda;
}

xapi request_xfree(request * restrict req)
{
  enter;

  if(req)
    fatal(array_xfree, req->commands);

  wfree(req);

  finally : coda;
}

xapi request_ixfree(request ** restrict req)
{
  enter;

  fatal(request_xfree, *req);
  *req = 0;

  finally : coda;
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
