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

#include "types.h"
#include "macros.h"

#include "xdirent.h"
#include "xfcntl.h"
#include "xunistd.h"


DIR * API xopendir(const char * name)
{
  DIR *dd;

  dd = opendir(name);
  RUNTIME_ASSERT(dd);

  return dd;
}

DIR * API uxopendir(const char * name)
{
  DIR *dd;

  if((dd = opendir(name)) == 0 && errno != ENOENT)
  {
    RUNTIME_ABORT();
  }

  return dd;
}

DIR * API xfdopendir(int fd)
{
  DIR *dd;

  if((dd = fdopendir(fd)) == 0)
  {
    RUNTIME_ABORT();
  }

  return dd;
}

DIR * API xopendirat(int dirfd, const char * const restrict path)
{
  DIR *dd;
  int fd;

  fd = xopenats(O_RDONLY | O_DIRECTORY, dirfd, path);
  dd = xfdopendir(fd);

  return dd;
}

struct dirent * API xreaddir(DIR * dd)
{
  struct dirent *result;

  errno = 0;
  if((result = readdir(dd)) == 0 && errno != 0)
  {
    RUNTIME_ABORT();
  }

  return result;
}

void API xclosedir(DIR * dd)
{
  RUNTIME_ASSERT(closedir(dd) == 0);
}

void API ixclosedir(DIR ** dd)
{
  xclosedir(*dd);
  *dd = 0;
}

long API xtelldir(DIR * dd)
{
  long loc;

  loc = telldir(dd);
  RUNTIME_ASSERT(loc != -1);

  return loc;
}
