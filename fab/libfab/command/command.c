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

const char ** fab_command_names = (const char *[]){
#define FAB_COMMAND(a, b, c) [c] = #b,
FAB_COMMAND_TABLE
#undef FAB_COMMAND
};

#define FAB_COMMAND(a, b, c) + 1
size_t fab_command_num = 0 + FAB_COMMAND_TABLE;

//
// public
//

API void command_free(fab_command * const restrict cmd)
{
  if(cmd)
  {
    int x;
    for(x = 0; x < sizeof(cmd->strings) / sizeof(cmd->strings[0]); x++)
      wfree(cmd->strings[x]);
  }

  wfree(cmd);
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

xapi command_say(const fab_command * const restrict cmd, struct narrator * const restrict N)
{
  enter;


  sayf("{ %s", COMMAND_NAME(cmd->type));

  int first = 1;
  if(cmd->attr & FAB_ATTR_TARGET_OPT)
  {
    if(first)
      sayc(' ');
    else
      sayc(',');
    first = 0;

    says(FAB_ATTR_TARGET_NAME(cmd->attr));
  }

  if(cmd->strings[0])
    sayf(" %s", cmd->strings[0]);

  says(" }");

  finally : coda;
}

xapi command_create(fab_command ** const restrict ret, uint8_t type, uint32_t attr)
{
  enter;

  fatal(xmalloc, ret, sizeof(**ret));
  (*ret)->type = type;
  (*ret)->attr = attr;

  finally : coda;
}

xapi command_creates(fab_command ** const restrict ret, uint8_t type, uint32_t attr, const char * const restrict text)
{
  enter;

  fab_command * command = 0;
  fatal(xmalloc, &command, sizeof(*command));
  command->type = type;
  command->attr = attr;
  fatal(ixstrdup, &command->text, text);

  *ret = command;
  command = 0;

finally:
  command_free(command);
coda;
}

xapi command_createvf(fab_command ** const restrict ret, uint8_t type, uint32_t attr, const char * const restrict fmt, va_list va)
{
  enter;

  fab_command * command = 0;

  fatal(xmalloc, &command, sizeof(*command));
  command->type = type;
  command->attr = attr;

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
