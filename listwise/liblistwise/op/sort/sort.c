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

#include "listwise/operator.h"
#include "listwise/lwx.h"

#include "xlinux/xstdlib.h"

#include "parseint.h"
#include "strutil.h"

/*

sn operator  - sort numbers (in ascending order)
ss operator  - sort strings (in ascending order)

NO ARGUMENTS

 use current selection, ELSE
 use entire top list

OPERATION

 1. sort the items amongst themselves

*/

static xapi op_exec_ss(operation*, lwx*, int**, int*);
static xapi op_exec_sn(operation*, lwx*, int**, int*);

operator op_desc[] = {
  {
      .s            = "ss"
    , .optype       = LWOP_MODIFIERS_CANHAVE
    , .op_exec      = op_exec_ss
    , .desc         = "sort stringwise"
  }
  , {
      .s            = "sn"
    , .optype       = 0
    , .op_exec      = op_exec_sn
    , .desc         = "sort numeric"
  }
  , {}
};

#define STRING_WCASE  1
#define STRING_NCASE  2
#define NUMERIC       3

static xapi op_exec(operation* o, lwx* lx, int** ovec, int* ovec_len, int mode)
{
  enter;

  int * mema = 0;
  int * memb = 0;

  size_t num = lx->s[0].l;
  if(lx->sel.active && lx->sel.active->lease == lx->sel.active_era)
    num = lx->sel.active->l;

  // indexes to be sorted
  fatal(xmalloc, &mema, num * sizeof(*mema));

  // copy of indexes
  fatal(xmalloc, &memb, num * sizeof(*memb));

  // populate the indexes
  int i = 0;
  int x;
  LSTACK_ITERATE(lx, x, go)
  if(go)
  {
    mema[i] = x;
    memb[i] = x;
    i++;
  }
  LSTACK_ITEREND;

  xapi compar(const void * A, const void * B, void * T, int * r)
  {
    enter;

    char *  As = 0;
    int     Asl = 0;
    char *  Bs = 0;
    int     Bsl = 0;

    (*r) = 0;

    if(mode == NUMERIC)
    {
      fatal(lstack_getstring, lx, 0, *(int*)A, &As, &Asl);
      fatal(lstack_getstring, lx, 0, *(int*)B, &Bs, &Bsl);

      intmax_t Aval;
      intmax_t Bval;

      if(parseint(As, SCNdMAX, INTMAX_MIN, INTMAX_MAX, 0, 0xFF, &Aval, 0) == 0)
      {
        if(parseint(Bs, SCNdMAX, INTMAX_MIN, INTMAX_MAX, 0, 0xFF, &Bval, 0) == 0)
        {
          (*r) = Aval - Bval;
        }
      }
    }
    else
    {
      fatal(lstack_getbytes, lx, 0, *(int*)A, &As, &Asl);
      fatal(lstack_getbytes, lx, 0, *(int*)B, &Bs, &Bsl);

      (*r) = estrcmp(As, Asl, Bs, Bsl, mode == STRING_NCASE);
    }

    finally : coda;
  }

  // sort mema
  fatal(xqsort_r, mema, i, sizeof(*mema), compar, 0);

  for(x = 0; x < i; x++)
  {
    int a = mema[x];  // new location
    int b = memb[x];  // old location

    mema[x] = 0xFFFF;

    if(a != b)
    {
      int y;
      for(y = 0; y < i; y++)
      {
        if(mema[y] == b)
          break;
      }

      fatal(lstack_swaptop, lx, a, b);

      if(y < i)
        mema[y] = a;
    }
  }

finally:
  wfree(mema);
  wfree(memb);
coda;
}

xapi op_exec_ss(operation * o, lwx * lx, int ** ovec, int * ovec_len)
{
  enter;

  if(lx->sel.active && lx->sel.active->lease == lx->sel.active_era && lx->sel.active->state == LWX_SELECTION_NONE)
  {
    // nothing selected
  }
  else
  {
    int ncase = 0;

    if(o->argsl >= 1)
      ncase = strchr(o->args[0]->s, 'i') != 0;

    fatal(op_exec, o, lx, ovec, ovec_len, ncase ? STRING_NCASE : STRING_WCASE);
  }

  finally : coda;
}

xapi op_exec_sn(operation * o, lwx * lx, int ** ovec, int * ovec_len)
{
  enter;

  if(lx->sel.active && lx->sel.active->lease == lx->sel.active_era && lx->sel.active->state == LWX_SELECTION_NONE)
  {
    // nothing selected
  }
  else
  {
    fatal(op_exec, o, lx, ovec, ovec_len, NUMERIC);
  }

  finally : coda;
}
