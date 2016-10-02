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

#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>

#include "listwise/operator.h"

#include "parseint.h"

/*

cons operator - prepend items to the list

ARGUMENTS

 1+ - use each

OPERATION

*/

static xapi op_validate(operation*);
static xapi op_exec(operation*, lwx*, int**, int*);

operator op_desc[] = {
  {
      .s            = "cons"
    , .optype       = LWOP_SELECTION_RESET | LWOP_WINDOWS_RESET | LWOP_ARGS_CANHAVE | LWOP_EMPTYSTACK_YES | LWOP_OPERATION_PUSHBEFORE
    , .op_validate  = op_validate
    , .op_exec      = op_exec
    , .desc         = "create new list from the specified items"
    , .mnemonic     = "construct"
  }
  , {}
};

static xapi op_validate(operation* o)
{
  enter;

  if(o->argsl < 1)
  {
    failf(LISTWISE_ARGSNUM, "expected %s", "actual %d", "1 or more", o->argsl);
  }

  finally : coda;
}

static xapi op_exec(operation* o, lwx* ls, int** ovec, int* ovec_len)
{
  enter;

  fatal(lstack_unshift, ls);

  int x;
  for(x = 0; x < o->argsl; x++)
    fatal(lstack_addw, ls, o->args[x]->s, o->args[x]->l);

  finally : coda;
}
