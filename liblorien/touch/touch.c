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
#include <string.h>

#include "xapi.h"
#include "xlinux/xfcntl.h"
#include "xlinux/xstat.h"
#include "xlinux/xunistd.h"

#include "internal.h"
#include "touch.internal.h"

API xapi touchw(mode_t mode, const char * const path, size_t pathl)
{
  enter;

  int fd = -1;

  // touch stamp file
  fatal(xopen_mode, path, O_CREAT | O_WRONLY, mode, &fd);
  fatal(xfutimens, fd, 0);

finally:
  xapi_infof("path", "%*s", pathl, path);
  fatal(ixclose, &fd);
coda;
}

API xapi touchs(mode_t mode, const char * const path)
{
  xproxy(touchw, mode, path, strlen(path));
}

API xapi touchf(mode_t mode, const char * const restrict fmt, ...)
{
  enter;

  va_list va;
  va_start(va, fmt);
  fatal(touchvf, mode, fmt, va);

finally:
  va_end(va);
coda;
}

API xapi touchvf(mode_t mode, const char * const restrict fmt, va_list va)
{
  enter;

  char space[256];

  size_t sz = vsnprintf(space, sizeof(space), fmt, va);
  if(sz < sizeof(space))
  {
    fatal(touchw, mode, space, sz);
  }

  finally : coda;
}
