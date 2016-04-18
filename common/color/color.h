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

#include <sys/types.h>

// byte sequence tables
extern char (*g_colors_7)[7];
extern char (*g_colors_6)[6];
extern char (*g_colors_5)[5];

#define CSEVEN(x) g_colors_7[x]
#define CSIX(x)   g_colors_6[x]
#define CFIVE(x)  g_colors_5[x]

// definitions
#define RED             CFIVE(0)
#define GREEN           CFIVE(1)
#define YELLOW          CFIVE(2)
#define BLUE            CFIVE(3)
#define MAGENTA         CFIVE(4)
#define CYAN            CFIVE(5)
#define WHITE           CFIVE(6)

// bold definitions
#define BOLD_RED        CSEVEN(0)
#define BOLD_GREEN      CSEVEN(1)
#define BOLD_YELLOW     CSEVEN(2)
#define BOLD_BLUE       CSEVEN(3)
#define BOLD_MAGENTA    CSEVEN(4)
#define BOLD_CYAN       CSEVEN(5)
#define BOLD_WHITE      CSEVEN(6)

#define NONE            CSIX(0)

//
// example
//  write(1, COLOR(RED));
//  write(1, COLOR(NONE));
//

#define COLOR(x) x, sizeof(x)
#define CSIZE(x) sizeof(x)

#endif
