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

#ifndef _FAB_SAY_H
#define _FAB_SAY_H

/*

these macros provide a convenient way of writing to stdout

ptmp must be in scope

*/

#define SAYW(s, l) do {							\
	fatal(axwrite, 1, s, l);					\
} while(0)

#define SAYS(s) SAYW(s, strlen(s))

#define SAYF(fmt, ...) do {											\
	fatal(psprintf, &ptmp, fmt, ##__VA_ARGS__);		\
	fatal(axwrite, 1, ptmp->s, ptmp->l);					\
} while(0)

#endif
