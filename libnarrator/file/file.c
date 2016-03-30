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

#include <stdio.h>

#include "xapi.h"
#include "xlinux.h"

#include "internal.h"
#include "file/file.internal.h"

#define restrict __restrict

//
// public
//

xapi file_vsayf(narrator_file * const restrict n, const char * const restrict fmt, va_list va)
{
 	xproxy(xvdprintf, n->fd, fmt, va);
}

xapi file_sayw(narrator_file * const restrict n, char * const restrict b, size_t l)
{
	xproxy(axwrite, n->fd, b, l);
}

xapi file_mark(narrator_file * const restrict n, size_t * const restrict mark)
{
  enter;

  // (*mark) = lseek(n->fd, 0, SEEK_SET) 

  finally : coda;
}

void file_free(narrator_file * n)
{
  free(n);
}

const char * file_first(narrator_file * const restrict n)
{
  return 0;
}

//
// api
//

API xapi narrator_file_create(narrator ** const restrict n, int fd)
{
  enter;

  fatal(xmalloc, n, sizeof(**n));
  (*n)->type = NARRATOR_FILE;
  (*n)->file.fd = fd;

  finally : coda;
}
