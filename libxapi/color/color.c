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

#include "color.h"

// five-byte color sequences
char (* g_colors_5)[5] = (char[][5]) {
#define COLOR(x, y) { 0x1b, 0x5b, x, y, 0x6d }
    COLOR(0x33, 0x31)  // 0 RED
  , COLOR(0x33, 0x32)  // 1 GREEN
  , COLOR(0x33, 0x33)  // 2 YELLOW
  , COLOR(0x33, 0x34)  // 3 BLUE
  , COLOR(0x33, 0x35)  // 4 MAGENTA
  , COLOR(0x33, 0x36)  // 5 CYAN
  , COLOR(0x33, 0x37)  // 6 GRAY
  , COLOR(0x39, 0x31)  // 7 LIGHT_RED
  , COLOR(0x39, 0x32)  // 8 LIGHT_GREEN
  , COLOR(0x39, 0x33)  // 9 LIGHT_YELLOW
  , COLOR(0x39, 0x34)  // a LIGHT_BLUE
  , COLOR(0x39, 0x35)  // b LIGHT_MAGENTA
  , COLOR(0x39, 0x36)  // c LIGHT_CYAN
  , COLOR(0x39, 0x37)  // d LIGHT_GRAY
#undef COLOR
};

// six-byte color sequences
char (* g_colors_6)[6] = (char[][6]){
#define COLOR(x) { 0x1b, 0x5b, 0x30, 0x3b, x, 0x6d }
    COLOR(0x30)  // NOCOLOR
#undef COLOR
};

// seven-byte byte color sequences
char (* g_colors_7)[7] = (char[][7]){
#define COLOR(a, b, c) { 0x1b, 0x5b, a, 0x3b, b, c, 0x6d }
    COLOR(0x31, 0x33, 0x31)  //   0 BOLD_RED
  , COLOR(0x31, 0x33, 0x32)  //   1 BOLD_GREEN
  , COLOR(0x31, 0x33, 0x33)  //   2 BOLD_YELLOW
  , COLOR(0x31, 0x33, 0x34)  //   3 BOLD_BLUE
  , COLOR(0x31, 0x33, 0x35)  //   4 BOLD_MAGENTA
  , COLOR(0x31, 0x33, 0x36)  //   5 BOLD_CYAN
  , COLOR(0x31, 0x33, 0x37)  //   6 BOLD_GRAY
  , COLOR(0x32, 0x33, 0x31)  //   7 DIM_RED
  , COLOR(0x32, 0x33, 0x32)  //   8 DIM_GREEN
  , COLOR(0x32, 0x33, 0x33)  //   9 DIM_YELLOW
  , COLOR(0x32, 0x33, 0x34)  //   a DIM_BLUE
  , COLOR(0x32, 0x33, 0x35)  //   b DIM_MAGENTA
  , COLOR(0x32, 0x33, 0x36)  //   c DIM_CYAN
  , COLOR(0x32, 0x33, 0x37)  //   d DIM_GRAY
  , COLOR(0x32, 0x39, 0x31)  //   e DIM_LIGHT_RED
  , COLOR(0x32, 0x39, 0x32)  //   f DIM_LIGHT_GREEN
  , COLOR(0x32, 0x39, 0x33)  //  11 DIM_LIGHT_YELLOW
  , COLOR(0x32, 0x39, 0x34)  //  12 DIM_LIGHT_BLUE
  , COLOR(0x32, 0x39, 0x35)  //  12 DIM_LIGHT_MAGENTA
  , COLOR(0x32, 0x39, 0x36)  //  13 DIM_LIGHT_CYAN
  , COLOR(0x32, 0x39, 0x37)  //  14 DIM_LIGHT_GRAY
#undef COLOR
};
