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
#include "xlinux/xstring.h"
#include "narrator.h"

#include "internal.h"
#include "request.internal.h"
#include "command.internal.h"

#include "assure.h"
#include "memblk.def.h"

#define restrict __restrict

//
// public
//

API void command_free(fab_command * const restrict command)
{
  if(command)
  {
    size_t x;
    for(x = 0; x < sizeof(command->strings) / sizeof(command->strings[0]); x++)
      wfree(command->strings[x]);
  }

  wfree(command);
}

API void command_ifree(fab_command ** const restrict command)
{
  command_free(*command);
  *command= 0;
}

API void command_freeze(fab_command * const restrict command, memblk * const restrict mb)
{
  size_t x;
  for(x = 0; x < sizeof(command->strings) / sizeof(command->strings[0]); x++)
    memblk_freeze(mb, &command->strings[x]);
}

API void command_thaw(fab_command * const restrict command, char * const restrict mb)
{
  size_t x;
  for(x = 0; x < sizeof(command->strings) / sizeof(command->strings[0]); x++)
    memblk_thaw(mb, &command->strings[x]);
}

xapi command_say(const fab_command * const restrict command, struct narrator * const restrict N)
{
  enter;

  sayf("{ attrs ", command->attrs);

  int first = 1;
  if(command->attrs & FABCORE_COMMAND_OPT)
  {
    if(!first)
      sayc(',');
    first = 0;

    says(FABCORE_COMMAND_STRING(command->attrs));
  }

  if(command->attrs & FABCORE_LIST_OP_OPT)
  {
    if(!first)
      sayc(',');
    first = 0;

    says(FABCORE_LIST_OP_STRING(command->attrs));
  }

#define IN1(x, S0) \
  ((x) == S0)

#define IN2(x, S0, S1) \
  ((x) == S0 || (x) == S1)

#define IN4(x, S0, S1, S2, S3) \
  ((x) == S0 || (x) == S1 || (x) == S2 || (x) == S3)

  if(IN4(command->attrs & FABCORE_COMMAND_OPT, FABCORE_TARGET_FABRICATE, FABCORE_TARGET_FABRICATE_EXACT, FABCORE_TARGET_FABRICATE_NOFILE, FABCORE_TARGET_INVALIDATE))
  {
    if(IN1(command->attrs & FABCORE_LIST_OP_OPT, FABCORE_CLEAR)) { }

    else if(IN2(command->attrs & FABCORE_LIST_OP_OPT, FABCORE_SET, FABCORE_PUSH))
    {
      sayf(", text %s", command->text);

      if(command->u8)
        sayf(", opt %s ", FABCORE_TARGET_STRING(command->u8));
    }
  }

  else if(IN1(command->attrs & FABCORE_COMMAND_OPT, FABCORE_CONFIGURATION_MERGE))
    sayf(", text %s", command->text);

  else if(IN1(command->attrs & FABCORE_COMMAND_OPT, FABCORE_MODE_BUILD))
    sayf(", mode %s", FABCORE_BUILD_STRING(command->u8));

  else if(IN1(command->attrs & FABCORE_COMMAND_OPT, FABCORE_MODE_LICENSE))
    sayf(", mode %s", FABCORE_LICENSE_STRING(command->u8));

  else if(IN1(command->attrs & FABCORE_COMMAND_OPT, FABCORE_MODE_TRACE))
    sayf(", mode %s", FABCORE_TRACE_STRING(command->u8));

  else if(IN1(command->attrs & FABCORE_COMMAND_OPT, FABCORE_BUILDSCRIPT_PATH))
    sayf(", path %s", command->path);

  says(" }");

  finally : coda;
}

xapi command_create(fab_command ** const restrict ret, uint32_t attrs)
{
  enter;

  fatal(xmalloc, ret, sizeof(**ret));
  (*ret)->attrs = attrs;

  finally : coda;
}

xapi command_creates(fab_command ** const restrict ret, uint32_t attrs, const char * const restrict text)
{
  enter;

  fab_command * command = 0;
  fatal(xmalloc, &command, sizeof(*command));
  command->attrs = attrs;
  fatal(ixstrdup, &command->text, text);

  *ret = command;
  command = 0;

finally:
  command_free(command);
coda;
}

xapi command_createvf(fab_command ** const restrict ret, uint32_t attrs, const char * const restrict fmt, va_list va)
{
  enter;

  fab_command * command = 0;

  fatal(xmalloc, &command, sizeof(*command));
  command->attrs = attrs;

  va_list va2;
  va_copy(va2, va);
  size_t sz = vsnprintf(0, 0, fmt, va2);

  fatal(xmalloc, &command->text, sz + 1);
  vsprintf(command->text, fmt, va);

  *ret = command;
  command = 0;

finally:
  command_free(command);
coda;
}

xapi command_createu8(fab_command ** const restrict ret, uint32_t attrs, uint8_t value)
{
  enter;

  fatal(xmalloc, ret, sizeof(**ret));
  (*ret)->attrs = attrs;
  (*ret)->u8 = value;

  finally : coda;
}

xapi command_create_target(fab_command ** const restrict ret, uint32_t attrs, uint8_t opt, const char * const restrict text)
{
  enter;

  fab_command * command = 0;
  fatal(xmalloc, &command, sizeof(*command));
  command->attrs = attrs;
  command->u8 = opt;
  fatal(ixstrdup, &command->text, text);

  *ret = command;
  command = 0;

finally:
  command_free(command);
coda;
}
