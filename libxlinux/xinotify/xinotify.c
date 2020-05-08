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

#include <errno.h>

#include "internal.h"
#include "xinotify.h"
#include "errtab/KERNEL.errtab.h"

API xapi xinotify_init(int * id, int flags)
{
  enter;

  int rv;
  if((rv = inotify_init1(flags)) < 0)
    tfail(perrtab_KERNEL, rv);

  *id = rv;

  finally : coda;
}

/// inotify_add_watch
//
// SUMMARY
//  proxy for inotify_add_watch
//
API xapi xinotify_add_watch(int * wd, int id, const char *path, uint32_t mask)
{
  enter;

  int rv;
  if((rv = inotify_add_watch(id, path, mask)) < 0)
    tfail(perrtab_KERNEL, errno);

  *wd = rv;

finally:
  xapi_infos("path", path);
  xapi_infof("id", "%d", id);
coda;
}
