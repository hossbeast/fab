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

#define restrict __restrict

//
// static
//

static xapi __attribute__((nonnull)) push(fab_request * const restrict request, fab_command * const restrict cmd)
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

API xapi fab_request_create(fab_request ** const restrict request, pid_t client_pid)
{
  enter;

  fatal(xmalloc, request, sizeof(**request));
  (*request)->client_pid = client_pid;

  finally : coda;
}

API void fab_request_free(fab_request * const restrict request)
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

API void fab_request_ifree(fab_request ** const restrict request)
{
  fab_request_free(*request);
  *request = 0;
}

API void fab_request_freeze(fab_request * const restrict request, memblk * const restrict mb)
{
  size_t x;
  for(x = 0; x < request->commandsl; x++)
  {
    command_freeze(request->commands[x], mb);
    memblk_freeze(mb, &request->commands[x]);
  }

  memblk_freeze(mb, &request->commands);
}

API void fab_request_thaw(fab_request * const restrict request, char * const restrict mb)
{
  memblk_thaw(mb, &request->commands);

  size_t x;
  for(x = 0; x < request->commandsl; x++)
  {
    memblk_thaw(mb, &request->commands[x]);
    command_thaw(request->commands[x], mb);
  }
}

API xapi fab_request_say(const fab_request * const restrict request, struct narrator * const restrict N)
{
  enter;

  sayf("client_pid %ld, commands [ ", request->client_pid);

  size_t x;
  for(x = 0; x < request->commandsl; x++)
    fatal(command_say, request->commands[x], N);

  sayf(" ]");

  finally : coda;
}



API xapi fab_request_command(fab_request * const restrict request, uint32_t attrs)
{
  enter;

  fab_command * cmd = 0;
  fatal(command_create, &cmd, attrs);
  fatal(push, request, cmd);
  cmd = 0;

finally:
  command_free(cmd);
coda;
}

API xapi fab_request_commands(fab_request * const restrict request, uint32_t attrs, const char * const restrict text)
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

API xapi fab_request_commandf(fab_request * const restrict request, uint32_t attrs, const char * const restrict fmt, ...)
{
  enter;

  va_list va;
  va_start(va, fmt);
  fatal(fab_request_commandvf, request, attrs, fmt, va);

finally:
  va_end(va);
coda;
}

API xapi fab_request_commandvf(fab_request * const restrict request, uint32_t attrs, const char * const restrict fmt, va_list va)
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

API xapi fab_request_commandu8(fab_request * const restrict request, uint32_t attrs, uint8_t value)
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

API xapi fab_request_command_target(fab_request * const restrict request, uint32_t attrs, uint8_t opt, const char * const restrict text)
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
