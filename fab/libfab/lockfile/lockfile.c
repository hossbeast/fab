/* Copyright (c) 2012-2020 Todd Freed <todd.freed@gmail.com>

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

#include "xlinux/xfcntl.h"
#include "xlinux/xunistd.h"
#include "xlinux/xsignal.h"

#include "lockfile.h"

#include "common/fmt.h"

/* ugo+rw world read/write */
#define LOCKFILE_MODE 0666

//
// static
//

static void lockfile_obtain(pid_t * pid, int * restrict fdp, const char * restrict path)
{
  ssize_t bytes;
  int fd;

  // initialize the return value
  *pid = -1;

  // create the pidfile
  fd = uxopen_modes(O_CREAT | O_WRONLY | O_EXCL, LOCKFILE_MODE, path);
  if(fd == -1)
  {
    xopens(O_RDONLY, path);
    bytes = xread(fd, pid, sizeof(*pid));
    if(bytes != sizeof(*pid)) {
      *pid = -1;
    }
  }
  else
  {
    axwrite(fd, (pid_t[]) { getpid() }, sizeof(pid_t));
    *pid = 0;
  }

  *fdp = fd;
}

//
// api
//

void API fabipc_lockfile_obtain(pid_t * pid, int * restrict fd, char * const restrict fmt, ...)
{
  char path[512];
  int x;
  va_list va;
  int r;

  va_start(va, fmt);
  fmt_apply(path, sizeof(path), fmt, va);
  va_end(va);

  for(x = 1; 1; x++)
  {
    xclose(*fd);
    lockfile_obtain(pid, fd, path);

    if(*pid == 0)
      break;    // lock obtained

    if(*pid == -1)
    {
      // unable to determine lock holder from the pid file ; this can happen as a
      // result of a race reading/writing the pid file, or if the pid file is corrupted
      if((x % 3) == 0) {
        xunlinks(path);
      }

      continue;
    }

    r = 0;
    r = uxkill(*pid, 0);
    if(r == 0) {
      break;    // lock holder still running
    }

    // lock holder is not running ; forcibly release the lock
    xunlinks(path);
  }
}

void API fabipc_lockfile_update(char * const restrict fmt, ...)
{
  int fd = -1;
  va_list va;
  va_start(va, fmt);
  va_end(va);

  // write our pid to the lockfile
  fd = xopenvf(O_WRONLY, fmt, va);
  axwrite(fd, (pid_t[]) { getpid() }, sizeof(pid_t));

  ixclose(&fd);
}

void API fabipc_lockfile_release(char * const restrict fmt, ...)
{
  va_list va;
  va_start(va, fmt);
  xunlinkvf(fmt, va);
  va_end(va);
}
