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

#include "color.h"

char (*g_colors)[7] = (char[][7]){
#define COLORDEF(x) { 0x1b, 0x5b, 0x31, 0x3b, 0x33, x, 0x6d }
	  COLORDEF(0x31)	// RED
	, COLORDEF(0x32)	// GREEN
	, COLORDEF(0x33)	// YELLOW
	, COLORDEF(0x34)	// BLUE
	, COLORDEF(0x35)	// THIRTFIVE
	, COLORDEF(0x36)	// CYAN
#undef COLORDEF
};

char (*g_color_6)[6] = (char[][6]){
	  { 0x1b, 0x5b, 0x30, 0x3b, 0x30, 0x6d }
};
