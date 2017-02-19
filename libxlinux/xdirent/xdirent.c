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

API xapi xopendir(const char * name, DIR ** dd)
{
  enter;

  if(((*dd) = opendir(name)) == 0)
    tfail(perrtab_KERNEL, errno);

finally:
  xapi_infof("path", "%s", name);
coda;
}

API xapi uxopendir(const char * name, DIR ** dd)
{
  enter;

  if(((*dd) = opendir(name)) == 0 && errno != ENOENT)
    tfail(perrtab_KERNEL, errno);

finally:
  xapi_infof("path", "%s", name);
coda;
}

API xapi xfdopendir(int fd, DIR ** dd)
{
  enter;

  if(((*dd) = fdopendir(fd)) == 0)
    tfail(perrtab_KERNEL, errno);

  finally : coda;
}

API xapi xopendirat(DIR ** dd, int dirfd, const char * const restrict path)
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

API xapi xreaddir_r(DIR * dirp, struct dirent * entry, struct dirent ** result)
{
  enter;

  int r;
  if((r = readdir_r(dirp, entry, result)))
    fail(r);

  finally : coda;
}

API xapi xclosedir(DIR * dd)
{
  enter;

  if(dd && closedir(dd) != 0)
    tfail(perrtab_KERNEL, errno);

  finally : coda;
}

API xapi ixclosedir(DIR ** dd)
{
  enter;

  fatal(xclosedir, *dd);
  *dd = 0;

  finally : coda;
}

API xapi xtelldir(long * loc, DIR * dirp)
{
  enter;

  if((*loc = telldir(dirp)) == -1)
    tfail(perrtab_KERNEL, errno);

  finally : coda;
}
