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
#include "xlinux/KERNEL.errtab.h"
#include "xlinux/xfcntl.h"
#include "xlinux/xunistd.h"
#include "xlinux/xsignal.h"

#include "ipc.internal.h"

#include "common/fmt.h"

//
// static
//
static xapi lock_obtain(pid_t * pgid, int * restrict fdp, const char * restrict path)
{
  enter;

  ssize_t bytes;
  int fd = -1;

  // initialize the return value
  *pgid = -1;

  // create the pidfile
  fatal(uxopen_modes, &fd, O_CREAT | O_WRONLY | O_EXCL, FABIPC_MODE_DATA, path);
  if(fd == -1)
  {
    fatal(xopens, &fd, O_RDONLY, path);
    fatal(xread, fd, pgid, sizeof(*pgid), &bytes);
    if(bytes != sizeof(*pgid))
      *pgid = -1;
  }
  else
  {
    fatal(axwrite, fd, (pid_t[]) { getpgid(0) }, sizeof(pid_t));
    *pgid = 0;
    if(fdp)
      *fdp = fd;
    fd = -1; // do not close
  }

finally:
  fatal(ixclose, &fd);
coda;
}

//
// api
//

xapi API ipc_lock_obtain(pid_t * pgid, int * restrict fdp, char * const restrict fmt, ...)
{
  enter;

  char path[512];

  va_list va;
  va_start(va, fmt);
  fatal(fmt_apply, path, sizeof(path), fmt, va);

  int x;
  for(x = 1; 1; x++)
  {
    fatal(lock_obtain, pgid, fdp, path);

    if(*pgid == 0)
      break;    // lock obtained

    if(*pgid == -1)
    {
      // unable to determine lock holder from the pid file ; this can happen as a
      // result of a race reading/writing the pid file, or if the pid file is corrupted

      if((x % 3) == 0)
        fatal(xunlinks, path);

      continue;
    }

    int r = 0;
    fatal(uxkill, (*pgid) * -1, 0, &r);
    if(r == 0) {
      break;    // lock holder still running
    }

    // lock holder is not running ; forcibly release the lock
    fatal(xunlinks, path);
  }

finally:
  va_end(va);
coda;
}

xapi API ipc_lock_update(char * const restrict fmt, ...)
{
  enter;

  int fd = -1;
  va_list va;
  va_start(va, fmt);

  // write our pgid to the lockfile
  fatal(xopenvf, &fd, O_WRONLY, fmt, va);
  fatal(axwrite, fd, (pid_t[]) { getpgid(0) }, sizeof(pid_t));

finally:
  fatal(ixclose, &fd);
coda;
}

xapi API ipc_lock_release(char * const restrict fmt, ...)
{
  enter;

  va_list va;
  va_start(va, fmt);
  fatal(xunlinkvf, fmt, va);

  finally : coda;
}
