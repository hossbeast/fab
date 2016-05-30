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

#include "xapi.h"
#include "xlinux.h"

#include "internal.h"
#include "window.internal.h"
#include "lwx.internal.h"
#include "lstack.internal.h"
#include "tune.internal.h"

#include "macros.h"

#define restrict __restrict

#define SAME  1
#define LEFT  2
#define RIGHT 3
#define INTER 4

//
// static
//

static xapi stage(lwx * const restrict lx, int y, int state, int off, int len)
{
  enter;

  // get from storage
  if(lx->win.s[y].staged == 0)
  {
    lx->win.s[y].staged = &lx->win.s[y].storage[0];
    lx->win.s[y].staged_storage_index = 0;

    if(lx->win.s[y].staged == lx->win.s[y].active)
    {
      lx->win.s[y].staged = &lx->win.s[y].storage[1];
      lx->win.s[y].staged_storage_index = 1;
    }
    else if(lx->win.s[y].active == 0 && lx->win.s[y].staged->mark)
    {
      lx->win.s[y].staged = &lx->win.s[y].storage[1];
      lx->win.s[y].staged_storage_index = 1;
    }

    lx->win.s[y].staged->mark = 0;
    lx->win.s[y].staged->l = 0;
  }
  
  // renew lease
  if(lx->win.s[y].staged->lease != lx->win.staged_era)// || lx->win.s[y].staged->nil)
  {
    lx->win.s[y].staged->lease = lx->win.staged_era;
    lx->win.s[y].staged->state = 0;
    lx->win.s[y].staged->l = 0;
    lx->win.s[y].staged->zl = 0;
  }

  lx->win.s[y].staged->state = state;

  if(state == LWX_WINDOWS_NONE)
  {
    lx->win.s[y].staged->zl = 0;
  }
  else
  {
    char * r;
    int rl;
    fatal(lstack_readrow, lx, 0, y, &r, &rl, 0, 1, 0, 0, 0);

    if(state == LWX_WINDOWS_ALL)
    {
      lx->win.s[y].staged->zl = rl;
    }
    else
    {
      int type = LEFT;

      int i;
      for(i = 0; i < lx->win.s[y].staged->l; i++)
      {
        if(off == lx->win.s[y].staged->s[i].o && len == lx->win.s[y].staged->s[i].l)
        {
          type = SAME; // same segment
          break;
        }
        else if((off + len) < lx->win.s[y].staged->s[i].o)
        {
          type = LEFT; // disjoint on the left
          break;
        }
        else if(off > (lx->win.s[y].staged->s[i].o + lx->win.s[y].staged->s[i].l))
        {
          type = RIGHT; // disjoint on the right
        }
        else
        {
          type = INTER; // overlapping

          int noff = MIN(lx->win.s[y].staged->s[i].o, off);
          int nlen = MAX(lx->win.s[y].staged->s[i].o + lx->win.s[y].staged->s[i].l, off + len) - noff;

          lx->win.s[y].staged->zl += (nlen - lx->win.s[y].staged->s[i].l);

          lx->win.s[y].staged->s[i].o = noff;
          lx->win.s[y].staged->s[i].l = nlen;

          if(lx->win.s[y].staged->zl == rl)
            lx->win.s[y].staged->state = LWX_WINDOWS_ALL;

          break;
        }
      }

      if(type == LEFT || type == RIGHT)
      {
        if((lx->win.s[y].staged->zl + len) == rl)
        {
          lx->win.s[y].staged->state = LWX_WINDOWS_ALL;
          lx->win.s[y].staged->zl = rl;
        }
        else
        {
          // reallocate if necessary
          if(lx->win.s[y].staged->a == lx->win.s[y].staged->l)
          {
            int ns = lx->win.s[y].staged->a ?: listwise_allocation_seed;
            ns = ns * 2 + ns / 2;

            fatal(xrealloc, &lx->win.s[y].staged->s, sizeof(*lx->win.s[0].staged->s), ns, lx->win.s[y].staged->a);
            lx->win.s[y].staged->a = ns;
          }

          if(type == LEFT)
          {
            // make room at s[i]
            memmove(
                &lx->win.s[y].staged->s[i + 1]
              , &lx->win.s[y].staged->s[i]
              , (lx->win.s[y].staged->l - i) * sizeof(lx->win.s[0].staged->s[0])
            );
          }

          lx->win.s[y].staged->s[i].o = off;
          lx->win.s[y].staged->s[i].l = len;
          lx->win.s[y].staged->l++;
          lx->win.s[y].staged->zl += len;
        }
      }
    }
  }

  finally : coda;
}

//
// public
//

int lstack_windows_staged_state(lwx * const restrict lx, int y, struct lwx_windows ** win)
{
  if(win)
    *win = lx->win.s[y].staged;

  if(lx->win.s[y].staged == 0 || lx->win.s[y].staged->lease != lx->win.staged_era)
    return LWX_WINDOWS_ALL;   // universe

  return lx->win.s[y].staged->state;
}

//
// api
//

API xapi lstack_windows_stage_all(lwx * const restrict lx, int y)
{
  xproxy(stage, lx, y, LWX_WINDOWS_ALL, 0, 0);
}

API xapi lstack_windows_stage_nil(lwx * const restrict lx, int y)
{
  xproxy(stage, lx, y, LWX_WINDOWS_NONE, 0, 0);
}

API xapi lstack_windows_stage(lwx * const restrict lx, int y, int off, int len)
{
  enter;

  if(off < 0 || len <= 0)
  {
    fatal(stage, lx, y, LWX_WINDOWS_NONE, 0, 0);
  }
  else
  {
    char * r;
    int rl;
    fatal(lstack_readrow, lx, 0, y, &r, &rl, 0, 1, 0, 0, 0);

    if((off + len) > rl)
    {
      fatal(stage, lx, y, LWX_WINDOWS_NONE, 0, 0);
    }
    else if(off == 0 && len == rl)
    {
      fatal(stage, lx, y, LWX_WINDOWS_ALL, 0, 0);
    }
    else
    {
      fatal(stage, lx, y, LWX_WINDOWS_SOME, off, len);
    }
  }

  finally : coda;
}

/*
** These api's ensure that the next active/staged window will be at the other index
*/

API xapi lstack_window_deactivate(lwx * const restrict lx, int y)
{
  enter;

  if(lx->win.s[y].active)
    lx->win.s[y].active->mark = 1;

  lx->win.s[y].active = 0;

  finally : coda;
}

API xapi lstack_windows_activate(lwx * const restrict lx)
{
  enter;

  int y;
  LSTACK_ITERATE_FWD(lx, 0, y, 1, 0, go)
  if(go)
  {
    if(lx->win.s[y].staged && lx->win.s[y].staged->lease == lx->win.staged_era)
    {
      lx->win.s[y].active = lx->win.s[y].staged;
      lx->win.s[y].active_storage_index = lx->win.s[y].staged_storage_index;
    }
    else
    {
      // no windows ; nil windows
      lx->win.s[y].active = &lx->win.s[y].storage[0];
      lx->win.s[y].active->state = LWX_WINDOWS_NONE;
      lx->win.s[y].active_storage_index = 0;
    }

    // renew lease
    lx->win.s[y].active->lease = lx->win.active_era;

    // reset staged
    lx->win.s[y].staged = 0;

    // reset temp
    lx->s[0].t[y].y = LWTMP_UNSET;
  }
  LSTACK_ITEREND

  finally : coda;
}

API int lstack_windows_state(lwx * const restrict lx, int y, struct lwx_windows ** win)
{
  if(win)
    *win = lx->win.s[y].active;

  if(lx->win.s[y].active == 0 || lx->win.s[y].active->lease != lx->win.active_era)
    return LWX_WINDOWS_ALL;   // universe

  return lx->win.s[y].active->state;
}
