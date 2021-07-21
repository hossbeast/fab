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
#include <stdarg.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#include "xlinux/xunistd.h"
#include "xlinux/xftw.h"
#include "xlinux/xstat.h"

#include "mkdirp.h"

void API mkdirpw(mode_t mode, const char * const path, size_t pathl)
{
  char space[256];
  const char * t;
  const char * e;

  t = path;
  e = path + pathl;
  while(t != e)
  {
    t++;
    while(*t != '/' && t != e)
      t++;

    memcpy(space, path, t - path);
    space[t - path] = 0;

    uxmkdirs(mode, space);
  }
}

void API mkdirps(mode_t mode, const char * const path)
{
  mkdirpw(mode, path, strlen(path));
}

void API mkdirpf(mode_t mode, const char * const restrict fmt, ...)
{
  va_list va;
  va_start(va, fmt);

  mkdirpvf(mode, fmt, va);

  va_end(va);
}

void API mkdirpvf(mode_t mode, const char * const restrict fmt, va_list va)
{
  char space[256];
  size_t sz;

  sz = vsnprintf(space, sizeof(space), fmt, va);
  if(sz < sizeof(space))
  {
    mkdirpw(mode, space, sz);
  }
}
