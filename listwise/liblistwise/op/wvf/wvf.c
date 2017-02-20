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
#include "listwise/lwx.h"

#include "macros.h"

/*

wvf operator - window following

NO ARGUMENTS

*/

static xapi op_exec(operation*, lwx*, int**, int*);

operator op_desc[] = {
  {
      .s            = "wvf"
    , .optype       = LWOP_WINDOWS_ACTIVATE | LWOP_SELECTION_STAGE
    , .op_exec      = op_exec
    , .desc         = "window section following last windowed section"
    , .mnemonic     = "window-following"
  }
  , {}
};

xapi op_exec(operation* o, lwx* lx, int** ovec, int* ovec_len)
{
  enter;

  int x;
  LSTACK_ITERATE(lx, x, go)
  if(go)
  {
    lwx_windows * win;
    if(lstack_windows_state(lx, x, &win) == LWX_WINDOWS_SOME)
    {
      // read the row
      int ssl = 0;
      fatal(lstack_readrow, lx, 0, x, 0, &ssl, 0, 1, 0, 0, 0);

      // following the last windowed segment
      fatal(lstack_windows_stage, lx, x, win->s[win->l - 1].o + win->s[win->l - 1].l, ssl - (win->s[win->l - 1].o + win->s[win->l - 1].l));
      fatal(lstack_selection_stage, lx, x);
    }
  }
  LSTACK_ITEREND

  finally : coda;
}
