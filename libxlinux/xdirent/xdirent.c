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
#include "xdirent/xdirent.h"
#include "errtab/SYS.errtab.h"

API xapi xopendir(const char * name, DIR ** dd)
{
  enter;

  if(((*dd) = opendir(name)) == 0)
    fail(errno);

finally:
  xapi_infof("path", "%s", name);
coda;
}

API xapi uxopendir(const char * name, DIR ** dd)
{
  enter;

  if(((*dd) = opendir(name)) == 0 && errno != ENOENT)
    fail(errno);

finally:
  xapi_infof("path", "%s", name);
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
