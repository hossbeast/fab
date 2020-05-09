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

#ifndef _XCURSES_H
#define _XCURSES_H

#include <curses.h>

#include "xapi.h"
#include "types.h"

#include "xapi/SYS.errtab.h"

static inline xapi xinitscr(WINDOW ** winp)
{
  enter;

  WINDOW *win = 0;

  if((win = initscr()) == 0) {
    fail(SYS_ABORT);
  }

finally:
  if(winp) {
    *winp = win;
  }
coda;
}

static inline xapi xnewwin(WINDOW ** winp, int height, int width, int y, int x)
{
  enter;

  WINDOW *win = 0;

  if((win = newwin(height, width, y, x)) == 0) {
    fail(SYS_ABORT);
  }

finally:
  if(winp) {
    *winp = win;
  }
coda;
}

#endif