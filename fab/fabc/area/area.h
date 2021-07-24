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

#ifndef _AREA_H
#define _AREA_H

#include <curses.h>

typedef struct region {
  WINDOW *w;
  int x;
  int y;
  int yext;
  int xext;
} region;

/* section of the gui */
typedef struct area {
  region border;        /* 1px box */
  region body;          /* the area, minus the border and the footer */
  region footer;        /* 1px row at the bottom */
} area;

void area_init(area * restrict a, int yext, int xext, int y, int x)
  __attribute__((nonnull));

void region_print_rightjust(region *reg, const char * restrict fmt, ...)
  __attribute__((nonnull(1, 2)))
  __attribute__((format(printf, 2, 3)));

#endif
