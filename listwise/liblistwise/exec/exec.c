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

#include <string.h>

#include "xapi.h"
#include "xlinux/xstdlib.h"
#include "narrator.h"
#include "narrator/fixed.h"

#include "internal.h"
#include "exec.internal.h"
#include "lwx.internal.h"
#include "transform.internal.h"
#include "lstack.internal.h"
#include "operator.internal.h"
#include "logging.internal.h"
#include "selection.internal.h"
#include "window.internal.h"

#if SANITY
#include "sanity.internal.h"
#endif

#include "macros.h"

#define restrict __restrict

///
/// API
///

API xapi listwise_exec_transform(
    transform * const restrict g
  , char ** const restrict init
  , size_t * const restrict initls
  , const size_t initl
  , lwx ** restrict lx
)
{
  enter;

  int token = 0;
  narrator * N = 0;

  // ovec workspace
  int* ovec = 0;
  int ovec_len = 0;

#if SANITY
  // sanity
  sanityblock * sb = 0;
#endif

  fatal(narrator_fixed_create, &N, 2048);

  // list stack allocation
  if(!*lx)
    fatal(lwx_alloc, lx);

  // write init elements to the top of list stack
  int x;
  for(x = 0; x < initl; x++)
  {
    fatal(lstack_addw, *lx, init[x], initls ? initls[x] : strlen(init[x]));
  }

  // write initial transform args at top of list stack
  for(x = 0; x < g->argsl; x++)
  {
    fatal(lstack_addw, *lx, g->args[x]->s, g->args[x]->l);
  }

  // the initial state of selection/windows is all
  (*lx)->sel.active_era++;
  (*lx)->sel.staged_era++;
  (*lx)->win.active_era++;
  (*lx)->win.staged_era++;

  if(log_would(L_LISTWISE | L_EXEC))
  {
    fatal(log_start, L_LISTWISE | L_EXEC, &token);
    logs(0, " >>      ");
    fatal(transform_canon_say, g, log_narrator(&token));
    fatal(log_finish, &token);
    

    // log the lstack before beginning
    fatal(log_start, L_LISTWISE | L_EXEC, &token);
    fatal(lwx_say, *lx, log_narrator(&token));
    fatal(log_finish, &token);
  }

#if SANITY
  if(listwise_sanity)
  {
    fatal(sanityblock_create, &sb);
    fatal(sanity, *lx, sb);
  }
#endif

  // execute operations
  int y;
  for(x = 0; x < g->opsl; x = y)
  {
    int sel_activate = 0;
    int win_activate = 0;

    // locate an alternating sequence of OR operators
    for(y = x; y < g->opsl; y++)
    {
      if((y - x) && (y - x) % 2)
      {
        if(!(g->ops[y]->op->optype & LWOPT_AGGREGATE))
          break;
      }

      if(g->ops[y]->op->optype & LWOPT_SELECTION_ACTIVATE)
        sel_activate = 1;

      if(g->ops[y]->op->optype & LWOPT_WINDOWS_ACTIVATE)
        win_activate = 1;
    }

    // possibly terminating with a y operator
    if(y < g->opsl && (g->ops[y]->op->optype & LWOPT_ACTIVATION_OVERRIDE))
    {
      // override activation settings
      sel_activate = !!(g->ops[y]->op->optype & LWOPT_SELECTION_ACTIVATE);
      win_activate = !!(g->ops[y]->op->optype & LWOPT_WINDOWS_ACTIVATE);
      y++;
    }

    int i;
    for(i = x; i < y; i++)
    {
      if(i != x)
      {
        // log the lstack
        if(log_would(L_LISTWISE | L_EXEC))
        {
          fatal(log_start, L_LISTWISE | L_EXEC, &token);
          fatal(lwx_say, *lx, log_narrator(&token));
          fatal(log_finish, &token);
        }
      }

      // log the operation
      if(log_would(L_LISTWISE | L_EXEC))
      {
        fatal(narrator_reset, N);
        fatal(operation_canon_say, g->ops[i], 0, N);

        fatal(log_start, L_LISTWISE | L_EXEC, &token);
        logf(L_LISTWISE | L_EXEC, " >> [%2d] %.*s%*s"
          , i
          , (int)narrator_fixed_size(N)
          , narrator_fixed_buffer(N)
          , MAX(35 - narrator_fixed_size(N), 0)
          , ""
        );
        fatal(listwise_lwop_say, g->ops[i]->op->optype, 1, log_narrator(&token));
        fatal(log_finish, &token);
      }

      // execute the operation
      if((*lx)->l || (g->ops[i]->op->optype & LWOPT_EMPTYSTACK_YES))
        if(g->ops[i]->op->op_exec)
          fatal(g->ops[i]->op->op_exec, g->ops[i], *lx, &ovec, &ovec_len);

      // age active windows
      if(g->ops[x]->op->optype & LWOPT_WINDOWS_RESET)
        (*lx)->win.active_era++;

      // age active selections
      if(g->ops[x]->op->optype & LWOPT_SELECTION_RESET)
        (*lx)->sel.active_era++;

#if SANITY
      if(listwise_sanity)
        fatal(sanity, *lx, sb);
#endif
    }

    // staged selection
    if(sel_activate)
      fatal(lstack_selection_activate, *lx);
    (*lx)->sel.staged_era++;

    // staged windows
    if(win_activate)
      fatal(lstack_windows_activate, *lx);
    (*lx)->win.staged_era++;

    // log the lstack
    if(log_would(L_LISTWISE | L_EXEC))
    {
      fatal(log_start, L_LISTWISE | L_EXEC, &token);
      fatal(lwx_say, *lx, log_narrator(&token));
      fatal(log_finish, &token);
    }
  }

finally:
  fatal(log_finish, &token);

  fatal(narrator_xfree, N);
  wfree(ovec);
#if SANITY
  sanityblock_free(sb);
#endif
coda;
}

API xapi listwise_exec(
    const char * const restrict ts
  , char ** const restrict init
  , size_t * const restrict initls
  , const size_t initl
  , lwx ** restrict lx
)
{
  enter;

  // transform
  transform * g = 0;

  fatal(transform_parse, 0, ts, 0, &g);
  fatal(listwise_exec_transform, g, init, initls, initl, lx);

finally:
  transform_free(g);
coda;
}
