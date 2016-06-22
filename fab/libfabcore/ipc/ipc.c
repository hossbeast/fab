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

#include <stdarg.h>

#include "xapi.h"
#include "xlinux/xfcntl.h"
#include "xlinux/xunistd.h"
#include "xlinux/SYS.errtab.h"

#include "internal.h"
#include "ipc.internal.h"

//
// api
//

API xapi ipc_lock_obtain(pid_t * pgid, char * const restrict fmt, ...)
{
  enter;

  char space[512];
  int fd = -1;

  va_list va;
  va_start(va, fmt);
  vsnprintf(space, sizeof(space), fmt, va);

  // create the pidfile
  if((fd = open(space, O_CREAT | O_WRONLY | O_EXCL, FABIPC_DATA)) == -1)
  {
    if(errno != EEXIST)
      tfail(perrtab_SYS, errno);

    else if((fd = open(space, O_RDONLY)) == -1)
      tfail(perrtab_SYS, errno);

    fatal(axread, fd, pgid, sizeof(*pgid));
  }
  else
  {
    fatal(axwrite, fd, (pid_t[]) { getpgid(0) }, sizeof(pid_t));
    *pgid = 0;
  }

finally:
  fatal(ixclose, &fd);
coda;
}

API xapi ipc_lock_update(char * const restrict fmt, ...)
{
  enter;

  char space[512];
  int fd = -1;

  va_list va;
  va_start(va, fmt);
  vsnprintf(space, sizeof(space), fmt, va);

  // create the pidfile
  fatal(xopen, space, O_WRONLY, &fd);
  fatal(axwrite, fd, (pid_t[]) { getpgid(0) }, sizeof(pid_t));

finally:
  fatal(ixclose, &fd);
coda;
}

API xapi ipc_lock_release(char * const restrict fmt, ...)
{
  enter;

  va_list va;
  va_start(va, fmt);
  fatal(xunlinkvf, fmt, va);

  finally : coda;
}
