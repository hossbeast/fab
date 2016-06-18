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

APIDATA narrator * g_narrator_stdout;
APIDATA narrator * g_narrator_stderr;

//
// public
//

xapi file_setup()
{
  enter;

  fatal(narrator_file_create, &g_narrator_stdout, 1);
  fatal(narrator_file_create, &g_narrator_stderr, 2);

  finally : coda;
}

void file_teardown()
{
  narrator_free(g_narrator_stdout);
  narrator_free(g_narrator_stderr);
}

xapi file_vsayf(narrator_file * const restrict n, const char * const restrict fmt, va_list va)
{
  xproxy(xvdprintf, n->fd, fmt, va);
}

xapi file_sayw(narrator_file * const restrict n, const char * const restrict b, size_t l)
{
  xproxy(axwrite, n->fd, b, l);
}

xapi file_seek(narrator_file * const restrict n, off_t offset, int whence, off_t * restrict res)
{
  xproxy(xlseek, n->fd, offset, whence, res);
}

xapi file_reset(narrator_file * const restrict n)
{
  xproxy(xlseek, n->fd, 0, SEEK_SET, 0);
}

void file_destroy(narrator_file * n)
{
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

API int narrator_file_fd(narrator * const restrict n)
{
  return n->file.fd;
}
