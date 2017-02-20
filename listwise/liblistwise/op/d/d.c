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
#include <alloca.h>

#include "listwise/operator.h"
#include "listwise/lwx.h"

/*

d operator - sunDer selected (Delete)

NO ARGUMENTS

OPERATION

  1. push an empty list on the stack
  2. invert the current selection
  3. move all selected items to the top list

*/

static xapi op_exec(operation*, lwx*, int**, int*);

operator op_desc[] = {
  {
      .s            = "d"
    , .optype       = LWOP_STACKOP | LWOP_SELECTION_RESET | LWOP_WINDOWS_RESET
    , .op_exec      = op_exec
    , .mnemonic     = "delete"
    , .desc         = "extract rows into a new list"
  }
  , {}
};

xapi op_exec(operation* o, lwx* lx, int** ovec, int* ovec_len)
{
  enter;

  // create a new list at index 0
  fatal(lstack_unshift, lx);

  if(lx->sel.active && lx->sel.active->lease == lx->sel.active_era)
  {
    size_t num;
    if(lx->sel.active->state == LWX_SELECTION_NONE)
      num = 0;
    else
      num = lx->sel.active->l;

    // ensure allocation in the new list @ [0]
    fatal(lstack_ensure, lx, 0, lx->s[1].l - num - 1, -1);

    int i = 0;
    int x;
    for(x = lx->s[1].l - 1; x >= 0; x--)
    {
      if(lx->sel.active->state == LWX_SELECTION_NONE || (lx->sel.active->sl <= (x/8)) || (lx->sel.active->s[x/8] & (0x01 << (x%8))) == 0)
      {
        fatal(lstack_move, lx, 0, lx->s[1].l - num - 1 - i, 1, x);
      }
    }
  }

  finally : coda;
}
