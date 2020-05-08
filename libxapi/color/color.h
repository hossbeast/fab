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

#ifndef _COLOR_H
#define _COLOR_H

/*

EXAMPLE
 write(1, RED);
 write(1, NOCOLOR);

*/

#include <sys/types.h>

// byte sequence tables
extern char (*g_colors_7)[7];
extern char (*g_colors_6)[6];
extern char (*g_colors_5)[5];

#define _COLOR5(x) g_colors_5[x], sizeof(g_colors_5[x])
#define _COLOR6(x) g_colors_6[x], sizeof(g_colors_6[x])
#define _COLOR7(x) g_colors_7[x], sizeof(g_colors_7[x])

// foreground colors
#define DIM_LIGHT_BLUE    _COLOR7(0x11)
#define DIM_BLUE          _COLOR7(0x0a)
#define LIGHT_BLUE        _COLOR5(0xa)
#define BLUE              _COLOR5(0x3)
#define BOLD_BLUE         _COLOR7(0x03)
#define DIM_LIGHT_CYAN    _COLOR7(0x13)
#define DIM_CYAN          _COLOR7(0x0c)
#define LIGHT_CYAN        _COLOR5(0xc)
#define CYAN              _COLOR5(0x5)
#define BOLD_CYAN         _COLOR7(0x05)
#define DIM_LIGHT_GRAY    _COLOR7(0x14)
#define DIM_GRAY          _COLOR7(0x0d)
#define LIGHT_GRAY        _COLOR5(0xd)
#define GRAY              _COLOR5(0x6)
#define BOLD_GRAY         _COLOR7(0x06)
#define DIM_LIGHT_GREEN   _COLOR7(0x0f)
#define DIM_GREEN         _COLOR7(0x08)
#define LIGHT_GREEN       _COLOR5(0x8)
#define GREEN             _COLOR5(0x1)
#define BOLD_GREEN        _COLOR7(0x01)
#define DIM_LIGHT_MAGENTA _COLOR7(0x12)
#define DIM_MAGENTA       _COLOR7(0x0b)
#define LIGHT_MAGENTA     _COLOR5(0xb)
#define MAGENTA           _COLOR5(0x4)
#define BOLD_MAGENTA      _COLOR7(0x04)
#define DIM_LIGHT_RED     _COLOR7(0x0e)
#define DIM_RED           _COLOR7(0x07)
#define LIGHT_RED         _COLOR5(0x7)
#define RED               _COLOR5(0x0)
#define BOLD_RED          _COLOR7(0x00)
#define DIM_LIGHT_YELLOW  _COLOR7(0x11)
#define DIM_YELLOW        _COLOR7(0x09)
#define LIGHT_YELLOW      _COLOR5(0x9)
#define YELLOW            _COLOR5(0x2)
#define BOLD_YELLOW       _COLOR7(0x02)

// other
#define WHITE             LIGHT_GRAY
#define NOCOLOR           _COLOR6(0)

#endif
