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
#include <string.h>

#include "listwise/operator.h"

#include "strutil.h"

/*

l operator - (Locate) substring matches - select matching rows, window matches

ARGUMENTS

  0  - string
  1* - options string

OPERATION

  1. foreach item in selection, or, if no selection, in top list
  2. select that item if it contains the substring, subject to options

*/

static xapi op_validate(operation* o);
static xapi op_exec(operation*, lwx*, int**, int*);

operator op_desc[] = {
  {
      .s            = "l"
    , .optype       = LWOP_SELECTION_ACTIVATE | LWOP_WINDOWS_STAGE | LWOP_MODIFIERS_CANHAVE | LWOP_ARGS_CANHAVE
    , .op_validate  = op_validate
    , .op_exec      = op_exec
    , .mnemonic     = "locate"
    , .desc         = "select by substring match"
  }
  , {}
};

static xapi op_validate(operation* o)
{
  enter;

  if(o->argsl == 1 || o->argsl == 2)
  {
    if(o->args[0]->l == 0)
      fails(LISTWISE_ARGSDOM, "length", "0");
  }
  else
  {
    failf(LISTWISE_ARGSNUM, "expected %s", "actual %d", "1 or 2", o->argsl);
  }

  finally : coda;
}

xapi op_exec(operation* o, lwx* ls, int** ovec, int* ovec_len)
{
  enter;

  int isncase  = o->argsl == 2 && o->args[1]->l && strchr(o->args[0]->s, 'i');
  int isglobal = o->argsl == 2 && o->args[1]->l && strchr(o->args[1]->s, 'g');

  int x;
  LSTACK_ITERATE(ls, x, go)
  if(go)
  {
    char * ss;
    int ssl;
    fatal(lstack_getbytes, ls, 0, x, &ss, &ssl);

    char * s = estrstr(ss, ssl, o->args[0]->s, o->args[0]->l, isncase);

    if(s)
    {
      do
      {
        fatal(lstack_windows_stage, ls, x, s - ss, o->args[0]->l);

        if(isglobal)
        {
          s += o->args[0]->l;
          if((ssl - (s - ss)) > 0)
            s = estrstr(s, ssl - (s - ss), o->args[0]->s, o->args[0]->l, isncase);
          else
            s = 0;
        }
        else
        {
          s = 0;
        }
      } while(s);

      // record this index was hit
      fatal(lstack_selection_stage, ls, x);
    }
  }
  LSTACK_ITEREND

  finally : coda;
}
