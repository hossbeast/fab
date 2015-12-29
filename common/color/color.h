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

#include <sys/types.h>

extern char (*g_colors_7)[7];
extern char (*g_colors_6)[6];

#define CSEVEN(x) g_colors_7[x]
#define CSIX(x)   g_colors_6[x]

#define RED					CSEVEN(0)
#define GREEN				CSEVEN(1)
#define YELLOW			CSEVEN(2)
#define BLUE				CSEVEN(3)
#define CYAN				CSEVEN(5)
#define NONE				CSIX(0)

#define COLOR(x) x, sizeof(x)
#define CSIZE(x) sizeof(x)
