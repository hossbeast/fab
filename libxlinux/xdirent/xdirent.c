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

#include "internal.h"
#include "xdirent.h"
#include "KERNEL.errtab.h"
#include "xfcntl.h"
#include "xunistd.h"

xapi API xopendir(const char * name, DIR ** dd)
{
  enter;

  if(((*dd) = opendir(name)) == 0)
    tfail(perrtab_KERNEL, errno);

finally:
  xapi_infof("path", "%s", name);
coda;
}

xapi API uxopendir(const char * name, DIR ** dd)
{
  enter;

  if(((*dd) = opendir(name)) == 0 && errno != ENOENT)
    tfail(perrtab_KERNEL, errno);

finally:
  xapi_infof("path", "%s", name);
coda;
}

xapi API xfdopendir(int fd, DIR ** dd)
{
  enter;

  if(((*dd) = fdopendir(fd)) == 0)
    tfail(perrtab_KERNEL, errno);

  finally : coda;
}

xapi API xopendirat(DIR ** dd, int dirfd, const char * const restrict path)
{
  enter;

  int fd = -1;

  fatal(xopenats, &fd, O_RDONLY | O_DIRECTORY, dirfd, path);
  fatal(xfdopendir, fd, dd);
  fd = -1;

finally:
  fatal(ixclose, &fd);
coda;
}

xapi API xreaddir(DIR * dirp, struct dirent ** result)
{
  enter;

  errno = 0;
  if((*result = readdir(dirp)) == 0 && errno)
    fail(errno);

  finally : coda;
}

xapi API xclosedir(DIR * dd)
{
  enter;

  if(dd && closedir(dd) != 0)
    tfail(perrtab_KERNEL, errno);

  finally : coda;
}

xapi API ixclosedir(DIR ** dd)
{
  enter;

  fatal(xclosedir, *dd);
  *dd = 0;

  finally : coda;
}

xapi API xtelldir(long * loc, DIR * dirp)
{
  enter;

  if((*loc = telldir(dirp)) == -1)
    tfail(perrtab_KERNEL, errno);

  finally : coda;
}
