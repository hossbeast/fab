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

#include "color.h"
#include "macros.h"

// whether to include the color escapes in the windowed segment
#define INCLUDE_ESCAPES 1

/*

hl operator - highlight window within each row with terminal color escapes

ARGUMENTS
   1  - offset to start of window
        if negative, interpreted as offset from the end of the string
        default : 0
  *2  - length of window
        default : 0 - entire string

OPERATION

  1. if nothing selected, select all
  2. foreach selected string
    3. replace that string with a slice of itself specified by the arguments

*/

static xapi op_exec(operation*, lwx*, int**, int*);

operator op_desc[] = {
  {
      .s            = "hl"
    , .optype       = LWOP_OPERATION_INPLACE | LWOP_WINDOWS_ACTIVATE | LWOP_SELECTION_STAGE
    , .op_exec      = op_exec
    , .mnemonic     = "highlight"
    , .desc         = "highlight windows"
  }
  , {}
};

static xapi op_exec(operation* o, lwx* lx, int** ovec, int* ovec_len)
{
  enter;

  int x;
  LSTACK_ITERATE(lx, x, go)
  if(go)
  {
    lwx_windows * win;
    int state;
    if((state = lstack_windows_state(lx, x, &win)) != LWX_WINDOWS_NONE)
    {
      // request that readrow return temp space
      char * zs;
      int    zsl;
      fatal(lstack_readrow, lx, 0, x, &zs, &zsl, 0, 1, 0, 1, 0);

      if(state == LWX_WINDOWS_ALL)
      {
        fatal(lstack_clear, lx, 0, x);

#if INCLUDE_ESCAPES
        fatal(lstack_catw, lx, 0, x, COLOR(RED));
        fatal(lstack_catw, lx, 0, x, zs, zsl);
        fatal(lstack_catw, lx, 0, x, COLOR(NONE));

        fatal(lstack_windows_stage, lx, x, 0, zsl + CSIZE(NONE) + CSIZE(RED));
#else
        fatal(lstack_catw, lx, 0, x, COLOR(RED));
        z += CSIZE(RED);
        fatal(lstack_catw, lx, 0, x, zs + win->s[i].o, win->s[i].l);
        fatal(lstack_catw, lx, 0, x, COLOR(NONE));

        fatal(lstack_windows_stage, lx, x, CSIZE(NONE), zsl);
#endif
      }
      else
      {
        // clear this string on the stack
        fatal(lstack_clear, lx, 0, x);

        // cumulative offset within the new string
        int z = 0;

        // text in the subject before the first windowed segment
        fatal(lstack_catw, lx, 0, x, zs, win->s[0].o);
        z += win->s[0].o;

        int i;
        for(i = 0; i < win->l; i++)
        {
          // text following the last segment, and preceeding this segment
          if(i)
          {
            fatal(lstack_catw, lx, 0, x, zs + win->s[i - 1].o + win->s[i - 1].l, win->s[i].o - (win->s[i - 1].o + win->s[i - 1].l));
            z += win->s[i].o - (win->s[i - 1].o + win->s[i - 1].l);
          }

#if INCLUDE_ESCAPES
          fatal(lstack_catw, lx, 0, x, COLOR(RED));
          fatal(lstack_catw, lx, 0, x, zs + win->s[i].o, win->s[i].l);
          fatal(lstack_catw, lx, 0, x, COLOR(NONE));

          fatal(lstack_windows_stage, lx, x, z, win->s[i].l + CSIZE(NONE) + CSIZE(RED));

          z += win->s[i].l + CSIZE(NONE) + CSIZE(RED);
#else
          fatal(lstack_catw, lx, 0, x, COLOR(RED));
          z += CSIZE(RED);
          fatal(lstack_catw, lx, 0, x, zs + win->s[i].o, win->s[i].l);
          fatal(lstack_catw, lx, 0, x, COLOR(NONE));

          fatal(lstack_windows_stage, lx, x, z, win->s[i].l);

          z += win->s[i].l + CSIZE(NONE);
#endif
        }

        // text in the subject following the last windowed segment
        if(i)
        {
          i--;
          fatal(lstack_catw, lx, 0, x, zs + win->s[i].o + win->s[i].l, zsl - (win->s[i].o + win->s[i].l));
        }

        fatal(lstack_selection_stage, lx, x);
      }
    }
  }
  LSTACK_ITEREND

  finally : coda;
}
