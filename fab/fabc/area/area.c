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

#include "xapi.h"

#include "area.h"
#include "xcurses.h"

static xapi region_init(region * restrict reg, int yext, int xext, int y, int x)
{
  enter;

  fatal(xnewwin, &reg->w, yext, xext, y, x);
  reg->yext = yext;
  reg->xext = xext;
  reg->y = y;
  reg->x = x;

  finally : coda;
}

xapi area_init(area * restrict a, int yext, int xext, int y, int x)
{
  enter;

  fatal(region_init, &a->body, yext - 2, xext - 2, y + 1, x + 1);
  fatal(region_init, &a->footer, 1, xext - 2, y + yext - 2, x + 1);
  fatal(region_init, &a->border, yext, xext, y, x);

  finally : coda;
}

void region_print_rightjust(region *reg, const char * restrict fmt, ...)
{
  va_list va;
  va_list va2;
  int len;

  va_start(va, fmt);
  va_copy(va2, va);

  len = vsnprintf(0, 0, fmt, va2);
  va_end(va2);

  wmove(reg->w, reg->w->_cury, reg->xext - len);

  vw_printw(reg->w, fmt, va);
  va_end(va);
}
