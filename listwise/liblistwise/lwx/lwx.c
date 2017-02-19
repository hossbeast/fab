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

#include <inttypes.h>

#include "xapi.h"
#include "xlinux/xstdlib.h"
#include "narrator.h"

#include "internal.h"
#include "lwx.internal.h"
#include "lstack.internal.h"
#include "selection.internal.h"
#include "window.internal.h"

#define restrict __restrict

lwx * APIDATA listwise_identity = 0;

//
// public
//

xapi lwx_setup()
{
  enter;

  fatal(xmalloc, &listwise_identity, sizeof(*listwise_identity));
  fatal(xmalloc, &listwise_identity->s, sizeof(listwise_identity->s[0]) * 1);
  listwise_identity->l = 1;
  listwise_identity->a = 1;

  finally : coda;
}

void lwx_teardown()
{
  if(listwise_identity)
    wfree(listwise_identity->s);
  iwfree(&listwise_identity);
}

///
/// API
///

API xapi lwx_alloc(lwx ** const lx)
{
  enter;

  fatal(xmalloc, lx, sizeof(**lx));

  finally : coda;
}

API uint64_t lwx_getflags(lwx * const lx)
{
  return lx->flags;
}

API uint64_t lwx_setflags(lwx * const lx, const uint64_t g)
{
  return ((lx->flags = g));
}

API void * lwx_getptr(lwx * const lx)
{
  return lx->ptr;
}

API void * lwx_setptr(lwx * const lx, void * const g)
{
  return ((lx->ptr = g));
}

API void lwx_free(lwx * lx)
{
  if(lx && lx != listwise_identity)
  {
    int x;
    int y;
    for(x = 0; x < lx->a; x++)
    {
      for(y = 0; y < lx->s[x].a; y++)
      {
        wfree(lx->s[x].s[y].s);
        wfree(lx->s[x].t[y].s);

        if(x == 0)
        {
          wfree(lx->win.s[y].storage[0].s);
          wfree(lx->win.s[y].storage[1].s);
        }
      }

      wfree(lx->s[x].s);
      wfree(lx->s[x].t);
    }

    wfree(lx->s);
    wfree(lx->win.s);
    wfree(lx->sel.storage[0].s);
    wfree(lx->sel.storage[1].s);
    wfree(lx);
  }
}

API xapi lwx_reset(lwx * lx)
{
  enter;

  int x;
  for(x = 0; x < lx->l; x++)
  {
    int y;
    for(y = 0; y < lx->s[x].l; y++)
      fatal(lstack_clear, lx, x, y);

    lx->s[x].l = 0;
  }

  lx->l = 0;
  lx->sel.active = 0;
  lx->sel.staged = 0;

  finally : coda;
}

API xapi lwx_say(lwx * const restrict lx, narrator * const restrict N)
{
  enter;

  sayf("sel active { lease(%3"PRIu64") %s era(%3"PRIu64") %s nil=%d } staged { lease(%3"PRIu64") %s era(%3"PRIu64") }\n"
    , lx->sel.active ? lx->sel.active->lease : 0
    , (lx->sel.active ? lx->sel.active->lease : 0) == lx->sel.active_era ? "= " : "!="
    , lx->sel.active_era
    , (lx->sel.active ? lx->sel.active->state == LWX_SELECTION_NONE : 0) ? "but" : "and"
    , lx->sel.active ? lx->sel.active->state == LWX_SELECTION_NONE : 0
    , lx->sel.staged ? lx->sel.staged->lease : 0
    , (lx->sel.staged ? lx->sel.staged->lease : 0) == lx->sel.staged_era ? "= " : "!="
    , lx->sel.staged_era
  );
  sayf("win active { %*sera(%3"PRIu64")%*s } staged { %*sera(%3"PRIu64") }\n"
    , 14, ""
    , lx->win.active_era
    , 10, ""
    , 14, ""
    , lx->win.staged_era
  );

  int x;
  int y;
  for(x = lx->l - 1; x >= 0; x--)
  {
    if(x != lx->l - 1)
      sayf("\n");

    if(lx->s[x].l == 0)
    {
      sayf("[%4d     ] -- empty", x);
    }

    for(y = 0; y < lx->s[x].l; y++)
    {
      int __attribute__((unused)) select = 0;
      int __attribute__((unused)) staged = 0;
      if(x == 0)
      {
        select = 1;
        if(lx->sel.active && lx->sel.active->lease == lx->sel.active_era)
        {
          select = 0;
          if(lx->sel.active->state != LWX_SELECTION_NONE && lx->sel.active->sl > (y / 8))
          {
            select = lx->sel.active->s[y / 8] & (0x01 << (y % 8));
          }
        }

        if(lx->sel.staged && lx->sel.staged->lease == lx->sel.staged_era)
        {
          if(lx->sel.staged->sl > (y / 8))
          {
            staged = lx->sel.staged->s[y / 8] & (0x01 << (y % 8));
          }
        }
      }
      
      if(x != lx->l - 1 || y)
        sayf("\n");

      // for each entry : indexes, whether selected, staged
      sayf("[%4d,%4d] %s%s "
        , x
        , y
        , select ? ">" : " "
        , staged ? "+" : " "
      );

      // display the string value of the row
      char * s = 0;
      int sl = 0;
      fatal(lstack_readrow, lx, x, y, &s, &sl, 0, 1, 0, 0, 0);

      sayf("'%.*s'", sl, s);

      // also display object properties if applicable
      if(lx->s[x].s[y].type)
      {
        sayf("[%hhu]%p/%p", lx->s[x].s[y].type, *(void**)lx->s[x].s[y].s, lx->s[x].s[y].s);
      }

      // indicate active windows other than ALL, which is the default state
      if(x == 0)
      {
        lwx_windows * win;
        int state = lstack_windows_state(lx, y, &win);

        if(state != LWX_WINDOWS_ALL)
        {
          sayf("\n");
          off_t oz;
          fatal(narrator_seek, N, 0, NARRATOR_SEEK_CUR, &oz);
          sayf("%16s", " ");

          if(state != LWX_WINDOWS_NONE)
          {
            int escaping = 0;
            int w = -1;
            int i;

            for(i = 0; i < sl; i++)
            {
              if(w == -1 && lx->win.s[y].active->s[0].o == i)
              {
                w = 0;
              }

              if(escaping)
              {
                if(s[i] == 0x6d)
                  escaping = 0;
              }
              else if(s[i] == 0x1b)
              {
                escaping = 1;
              }
              else
              {
                int marked = 0;
                if(w >= 0 && w < lx->win.s[y].active->l && i >= lx->win.s[y].active->s[w].o)
                {
                  if((i - lx->win.s[y].active->s[w].o) < lx->win.s[y].active->s[w].l)
                  {
                    sayf("^");
                    marked = 1;
                  }
                }

                if(!marked)
                {
                  // between internal windows
                  sayf(" ");
                  if(w >= 0 && w < lx->win.s[y].active->l && i >= lx->win.s[y].active->s[w].o)
                    w++;
                }
              }
            }
          }

          off_t z;
          fatal(narrator_seek, N, 0, NARRATOR_SEEK_CUR, &z);
          if((z - oz) < 45)
            sayf("%*s", (int)(45 - (z - oz)), "");

          sayf(" active { lease(%3"PRIu64") %s era(%3"PRIu64")"
            , lx->win.s[y].active->lease
            , lx->win.s[y].active->lease == lx->win.active_era ? "= " : "!="
            , lx->win.active_era
          );

          if(lx->win.s[y].active->lease == lx->win.active_era)
          {
            sayf(" %s state=%s"
              , lx->win.s[y].active->state == LWX_WINDOWS_SOME ? "and" : "but"
              , LWX_WINDOWS_STR(lx->win.s[y].active->state)
            );
          }
          sayf(" }");
        }
      }

      // indicate staged windows
      if(x == 0)// && lx->win.s[y].staged && lx->win.s[y].staged->lease == lx->win.staged_era && !lx->win.s[y].staged->state == LWX_WINDOWS_NONE)
      {
        if(lx->win.s[y].staged)
        {
          sayf("\n");
          off_t oz;
          fatal(narrator_seek, N, 0, NARRATOR_SEEK_CUR, &oz);
          sayf("%16s", " ");

          lwx_windows * win;
          int state = lstack_windows_staged_state(lx, y, &win);

          if(state != LWX_WINDOWS_NONE)
          {
            int escaping = 0;
            int w = -1;
            int i;
            for(i = 0; i < sl; i++)
            {
              if(state == LWX_WINDOWS_SOME && w == -1 && lx->win.s[y].staged->s[0].o == i)
              {
                w = 0;
              }

              if(escaping)
              {
                if(s[i] == 0x6d)
                  escaping = 0;
              }
              else if(s[i] == 0x1b)
              {
                escaping = 1;
              }
              else if(state == LWX_WINDOWS_SOME)
              {
                int marked = 0;
                if(w >= 0 && w < lx->win.s[y].staged->l && i >= lx->win.s[y].staged->s[w].o)
                {
                  if((i - lx->win.s[y].staged->s[w].o) < lx->win.s[y].staged->s[w].l)
                  {
                    sayf("+");
                    marked = 1;
                  }
                }

                if(!marked)
                {
                  // between internal windows
                  sayf(" ");
                  if(w >= 0 && w < lx->win.s[y].staged->l && i >= lx->win.s[y].staged->s[w].o)
                    w++;
                }
              }
            }
          }

          off_t z;
          fatal(narrator_seek, N, 0, NARRATOR_SEEK_CUR, &z);
          if((z - oz) < 45)
            sayf("%*s", (int)(45 - (z - oz)), "");

          sayf(" staged { lease(%3"PRIu64") %s era(%3"PRIu64")"
            , lx->win.s[y].staged->lease
            , lx->win.s[y].staged->lease == lx->win.staged_era ? "= " : "!="
            , lx->win.staged_era
          );

          if(lx->win.s[y].staged->lease == lx->win.staged_era)
          {
            sayf(" %s state=%s"
              , lx->win.s[y].staged->state == LWX_WINDOWS_SOME ? "and" : "but"
              , LWX_WINDOWS_STR(lx->win.s[y].staged->state)
            );
          }
          sayf(" }");
        }
      }
    }
  }

  finally : coda;
}
