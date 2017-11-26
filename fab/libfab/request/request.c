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

#include <stdarg.h>

#include "xapi.h"
#include "xlinux/xstdlib.h"
#include "narrator.h"

#include "internal.h"
#include "request.internal.h"
#include "command.internal.h"

#include "memblk.def.h"
#include "assure.h"

const char ** fab_attr_names = (const char *[]) {
#define FAB_ATTR(a, b, c, x) [c] = #b,
FAB_ATTR_TABLE(ATTR_TARGET_OPT)
#undef FAB_ATTR
};

#define FAB_ATTR(a, b, c, x) + 1
size_t fab_attr_num = 0 + FAB_ATTR_TABLE(ATTR_TARGET_OPT);

#define restrict __restrict

//
// static
//

static xapi __attribute__((nonnull)) push(fab_request * restrict request, fab_command * restrict cmd)
{
  enter;

  fatal(assure, &request->commands, sizeof(*request->commands), request->commandsl + 1, &request->commandsa);
  request->commands[request->commandsl++] = cmd;

  finally : coda;
}

//
// api
//
//

API xapi fab_request_create(fab_request ** restrict request)
{
  enter;

  fatal(xmalloc, request, sizeof(**request));

  finally : coda;
}

API void fab_request_free(fab_request * restrict request)
{
  if(request)
  {
    size_t x;
    for(x = 0; x < request->commandsl; x++)
      command_free(request->commands[x]);

    wfree(request->commands);
  }

  wfree(request);
}

API void fab_request_ifree(fab_request ** restrict request)
{
  fab_request_free(*request);
  *request = 0;
}

API void fab_request_freeze(fab_request * restrict request, memblk * restrict mb)
{
  size_t x;
  for(x = 0; x < request->commandsl; x++)
  {
    command_freeze(request->commands[x], mb);
    memblk_freeze(mb, &request->commands[x]);
  }

  memblk_freeze(mb, &request->commands);
}

API void fab_request_thaw(fab_request * restrict request, void * restrict mb)
{
  memblk_thaw(mb, &request->commands);

  size_t x;
  for(x = 0; x < request->commandsl; x++)
  {
    memblk_thaw(mb, &request->commands[x]);
    command_thaw(request->commands[x], mb);
  }
}

API xapi fab_request_say(const fab_request * restrict request, struct narrator * restrict N)
{
  enter;

  xsays("commands [ ");

  size_t x;
  for(x = 0; x < request->commandsl; x++)
  {
    if(x)
      xsays(" ");

    fatal(command_say, request->commands[x], N);
  }

  xsays(" ]");

  finally : coda;
}

API xapi fab_request_select(fab_request * restrict req, uint32_t attr, const char * restrict target)
{
  enter;

  fab_command * cmd = 0;

  fatal(command_creates, &cmd, FAB_COMMAND_SELECT, attr, target);
  fatal(push, req, cmd);
  cmd = 0;

finally:
  command_free(cmd);
coda;
}

API xapi fab_request_build(fab_request * restrict req)
{
  enter;

  fab_command * cmd = 0;
  fatal(command_create, &cmd, FAB_COMMAND_BUILD, 0);
  fatal(push, req, cmd);
  cmd = 0;

finally:
  command_free(cmd);
coda;
}

API xapi fab_request_plan(fab_request * restrict req)
{
  enter;

  fab_command * cmd = 0;
  fatal(command_create, &cmd, FAB_COMMAND_PLAN, 0);
  fatal(push, req, cmd);
  cmd = 0;

finally:
  command_free(cmd);
coda;
}

API xapi fab_request_script(fab_request * restrict req)
{
  enter;

  fab_command * cmd = 0;
  fatal(command_create, &cmd, FAB_COMMAND_SCRIPT, 0);
  fatal(push, req, cmd);
  cmd = 0;

finally:
  command_free(cmd);
coda;
}

API xapi fab_request_invalidate(fab_request * restrict req, const char * restrict text)
{
  enter;

  fab_command * cmd = 0;
  fatal(command_creates, &cmd, FAB_COMMAND_SCRIPT, 0, text);
  fatal(push, req, cmd);
  cmd = 0;

finally:
  command_free(cmd);
coda;
}

API xapi fab_request_invalidate_all(fab_request * restrict req)
{
  enter;

  fab_command * cmd = 0;
  fatal(command_create, &cmd, FAB_COMMAND_INVALIDATE_ALL, 0);
  fatal(push, req, cmd);
  cmd = 0;

finally:
  command_free(cmd);
coda;
}

API xapi fab_request_config_stages(fab_request * restrict req, const char * restrict s)
{
  enter;

  fab_command * cmd = 0;
  fatal(command_creates, &cmd, FAB_COMMAND_CONFIG_STAGE, 0, s);
  fatal(push, req, cmd);
  cmd = 0;

finally:
  command_free(cmd);
coda;
}

API xapi fab_request_config_stagef(fab_request * restrict req, const char * restrict fmt, ...)
{
  enter;

  va_list va;
  va_start(va, fmt);

  fab_command * cmd = 0;
  fatal(command_createvf, &cmd, FAB_COMMAND_CONFIG_STAGE, 0, fmt, va);
  fatal(push, req, cmd);
  cmd = 0;

finally:
  va_end(va);
  command_free(cmd);
coda;
}

API xapi fab_request_config_apply(fab_request * restrict req)
{
  enter;

  fab_command * cmd = 0;
  fatal(command_create, &cmd, FAB_COMMAND_CONFIG_APPLY, 0);
  fatal(push, req, cmd);
  cmd = 0;

finally:
  command_free(cmd);
coda;
}

#if 0
API xapi fab_request_commands(fab_request * restrict request, uint32_t attrs, const char * restrict text)
{
  enter;

  fab_command * cmd = 0;
  fatal(command_creates, &cmd, attrs, text);
  fatal(push, request, cmd);
  cmd = 0;

finally:
  command_free(cmd);
coda;
}

API xapi fab_request_commandf(fab_request * restrict request, uint32_t attrs, const char * restrict fmt, ...)
{
  enter;

  va_list va;
  va_start(va, fmt);
  fatal(fab_request_commandvf, request, attrs, fmt, va);

finally:
  va_end(va);
coda;
}

API xapi fab_request_commandvf(fab_request * restrict request, uint32_t attrs, const char * restrict fmt, va_list va)
{
  enter;

  fab_command * cmd = 0;
  fatal(command_createvf, &cmd, attrs, fmt, va);
  fatal(push, request, cmd);
  cmd = 0;

finally:
  command_free(cmd);
coda;
}

API xapi fab_request_commandu8(fab_request * restrict request, uint32_t attrs, uint8_t value)
{
  enter;

  fab_command * cmd = 0;
  fatal(command_createu8, &cmd, attrs, value);
  fatal(push, request, cmd);
  cmd = 0;

finally:
  command_free(cmd);
coda;
}

API xapi fab_request_command_target(fab_request * restrict request, uint32_t attrs, uint8_t opt, const char * restrict text)
{
  enter;

  fab_command * cmd = 0;
  fatal(command_create_target, &cmd, attrs, opt, text);
  fatal(push, request, cmd);
  cmd = 0;

finally:
  command_free(cmd);
coda;
}
#endif
