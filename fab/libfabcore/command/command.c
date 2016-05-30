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
#include "xlinux.h"
#include "narrator.h"

#include "internal.h"
#include "request.internal.h"
#include "command.internal.h"

#include "assure.h"
#include "memblk.def.h"

#define restrict __restrict

//
// api
//
//

API void command_free(command * const restrict cmd)
{
  if(cmd)
  {
    size_t x;
    for(x = 0; x < sizeof(cmd->strings) / sizeof(cmd->strings[0]); x++)
      free(cmd->strings[x]);
  }

  free(cmd);
}

API void command_ifree(command ** const restrict cmd)
{
  command_free(*cmd);
  *cmd= 0;
}

API xapi command_create(command ** const restrict cmd, uint32_t attrs)
{
  enter;

  fatal(xmalloc, cmd, sizeof(**cmd));
  (*cmd)->attrs = attrs;

  finally : coda;
}

API xapi command_creates(command ** const restrict ret, uint32_t attrs, const char * const restrict text)
{
  enter;

  command * cmd = 0;
  fatal(xmalloc, &cmd, sizeof(*cmd));
  cmd->attrs = attrs;
  fatal(ixstrdup, &cmd->text, text);

  *ret = cmd;
  cmd = 0;

finally:
  command_free(cmd);
coda;
}

API xapi command_createu8(command ** const restrict cmd, uint32_t attrs, uint8_t value)
{
  enter;

  fatal(xmalloc, cmd, sizeof(**cmd));
  (*cmd)->attrs = attrs;
  (*cmd)->u8 = value;

  finally : coda;
}

API xapi command_create_target(command ** const restrict ret, uint32_t attrs, uint8_t opt, const char * const restrict text)
{
  enter;

  command * cmd = 0;
  fatal(xmalloc, &cmd, sizeof(*cmd));
  cmd->attrs = attrs;
  cmd->u8 = opt;
  fatal(ixstrdup, &cmd->text, text);

  *ret = cmd;
  cmd = 0;

finally:
  command_free(cmd);
coda;
}

API void command_freeze(command * const restrict cmd, memblk * const restrict mb)
{
  size_t x;
  for(x = 0; x < sizeof(cmd->strings) / sizeof(cmd->strings[0]); x++)
    memblk_freeze(mb, &cmd->strings[x]);
}

API void command_thaw(command * const restrict cmd, char * const restrict mb)
{
  size_t x;
  for(x = 0; x < sizeof(cmd->strings) / sizeof(cmd->strings[0]); x++)
    memblk_thaw(mb, &cmd->strings[x]);
}

xapi command_say(const command * const restrict cmd, struct narrator * const restrict N)
{
  enter;

  sayf("{ attrs ", cmd->attrs);

  int first = 1;
  if(cmd->attrs & FABCORE_COMMAND_OPT)
  {
    if(!first)
      sayc(',');
    first = 0;

    says(FABCORE_COMMAND_STRING(cmd->attrs));
  }

  if(cmd->attrs & FABCORE_LIST_OP_OPT)
  {
    if(!first)
      sayc(',');
    first = 0;

    says(FABCORE_LIST_OP_STRING(cmd->attrs));
  }

#define IN1(x, S0) \
  ((x) == S0)

#define IN2(x, S0, S1) \
  ((x) == S0 || (x) == S1)

#define IN4(x, S0, S1, S2, S3) \
  ((x) == S0 || (x) == S1 || (x) == S2 || (x) == S3)

  if(IN4(cmd->attrs & FABCORE_COMMAND_OPT, FABCORE_TARGET_FABRICATE, FABCORE_TARGET_FABRICATE_EXACT, FABCORE_TARGET_FABRICATE_NOFILE, FABCORE_TARGET_INVALIDATE))
  {
    if(IN1(cmd->attrs & FABCORE_LIST_OP_OPT, FABCORE_CLEAR)) { }

    else if(IN2(cmd->attrs & FABCORE_LIST_OP_OPT, FABCORE_SET, FABCORE_PUSH))
    {
      sayf(", text %s", cmd->text);

      if(cmd->u8)
        sayf(", opt %s ", FABCORE_TARGET_STRING(cmd->u8));
    }
  }

  else if(IN1(cmd->attrs & FABCORE_COMMAND_OPT, FABCORE_MODE_BUILD))
    sayf(", mode %s", FABCORE_BUILD_STRING(cmd->u8));

  else if(IN1(cmd->attrs & FABCORE_COMMAND_OPT, FABCORE_MODE_LICENSE))
    sayf(", mode %s", FABCORE_LICENSE_STRING(cmd->u8));

  else if(IN1(cmd->attrs & FABCORE_COMMAND_OPT, FABCORE_MODE_TRACE))
    sayf(", mode %s", FABCORE_TRACE_STRING(cmd->u8));

  else if(IN1(cmd->attrs & FABCORE_COMMAND_OPT, FABCORE_BUILDSCRIPT_PATH))
    sayf(", path %s", cmd->path);

  says(" }");

  finally : coda;
}
