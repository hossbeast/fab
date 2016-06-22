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

static xapi __attribute__((nonnull)) push(request * const restrict req, command * const restrict cmd)
{
  enter;

  fatal(assure, &req->commands, sizeof(*req->commands), req->commandsl + 1, &req->commandsa);
  req->commands[req->commandsl++] = cmd;

  finally : coda;
}

//
// api
//
//

API xapi request_create(request ** const restrict request, pid_t client_pid)
{
  enter;

  fatal(xmalloc, request, sizeof(**request));
  (*request)->client_pid = client_pid;

  finally : coda;
}

API void request_free(request * const restrict request)
{
  if(request)
  {
    size_t x;
    for(x = 0; x < request->commandsl; x++)
      command_free(request->commands[x]);

    free(request->commands);
  }

  free(request);
}

API void request_ifree(request ** const restrict request)
{
  request_free(*request);
  *request = 0;
}

API xapi request_command(request * const restrict req, uint32_t attrs)
{
  enter;

  command * cmd = 0;
  fatal(command_create, &cmd, attrs);
  fatal(push, req, cmd);
  cmd = 0;

finally:
  command_free(cmd);
coda;
}

API xapi request_commands(request * const restrict req, uint32_t attrs, const char * const restrict text)
{
  enter;

  command * cmd = 0;
  fatal(command_creates, &cmd, attrs, text);
  fatal(push, req, cmd);
  cmd = 0;

finally:
  command_free(cmd);
coda;
}

API xapi request_commandu8(request * const restrict req, uint32_t attrs, uint8_t value)
{
  enter;

  command * cmd = 0;
  fatal(command_createu8, &cmd, attrs, value);
  fatal(push, req, cmd);
  cmd = 0;

finally:
  command_free(cmd);
coda;
}

API xapi request_command_target(request * const restrict req, uint32_t attrs, uint8_t opt, const char * const restrict text)
{
  enter;

  command * cmd = 0;
  fatal(command_create_target, &cmd, attrs, opt, text);
  fatal(push, req, cmd);
  cmd = 0;

finally:
  command_free(cmd);
coda;
}

API void request_freeze(request * const restrict req, memblk * const restrict mb)
{
  size_t x;
  for(x = 0; x < req->commandsl; x++)
  {
    command_freeze(req->commands[x], mb);
    memblk_freeze(mb, &req->commands[x]);
  }

  memblk_freeze(mb, &req->commands);
}

API void request_thaw(request * const restrict req, char * const restrict mb)
{
  memblk_thaw(mb, &req->commands);

  size_t x;
  for(x = 0; x < req->commandsl; x++)
  {
    memblk_thaw(mb, &req->commands[x]);
    command_thaw(req->commands[x], mb);
  }
}

API xapi request_say(const request * const restrict req, struct narrator * const restrict N)
{
  enter;

  sayf("client_pid %ld, commands [ ", req->client_pid);

  size_t x;
  for(x = 0; x < req->commandsl; x++)
    fatal(command_say, req->commands[x], N);

  sayf(" ]");

  finally : coda;
}
