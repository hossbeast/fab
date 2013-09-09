/* Copyright (c) 2012-2013 Todd Freed <todd.freed@gmail.com>

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

#define COLORHEX(x) (o_colors[(x & COLOR_VALUE) >> 60])
#define COLOR(x)    (char[7]){ 0x1b, 0x5b, 0x31, 0x3b, 0x33, COLORHEX(x), 0x6d }, 7
#define NOCOLOR     (char[6]){ 0x1b, 0x5b, 0x30, 0x3b, 0x30             , 0x6d }, 6

#define COLOR_VALUE   0xF000000000000000ULL
#define RED           0x1000000000000000ULL // this message red in the terminal
#define GREEN         0x2000000000000000ULL // this message green in the terminal
#define YELLOW        0x3000000000000000ULL // this message yellow in the terminal
#define CYAN          0x4000000000000000ULL // this message cyan in the terminal
#define BLUE          0x5000000000000000ULL // this message blue in the terminal

static unsigned char o_colors[] = {
  /* NO COLOR */   0x00
  /* RED    */   , 0x31
  /* GREEN  */   , 0x32
  /* YELLOW */   , 0x33
  /* CYAN   */   , 0x36
  /* BLUE   */   , 0x34
};
