/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

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

void bamipc_lockfile_acquire(pid_t * pid, int * restrict fdp, const char * restrict path)
{
  ssize_t bytes;
  int fd;

  // initialize the return value
  *pid = -1;

  // create the pidfile
  fd = uxopen_modes(O_CREAT | O_WRONLY | O_EXCL, LOCKFILE_MODE, path);
  if(fd == -1)
  {
    fd = xopens(O_RDONLY, path);
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

void API bamipc_lockfile_obtain(pid_t * pid, int * restrict fd, char * const restrict fmt, ...)
{
  char path[PATH_MAX];
  int x;
  va_list va;
  int r;

  va_start(va, fmt);
  fmt_apply(path, sizeof(path), fmt, va);
  va_end(va);

  for(x = 1; 1; x++)
  {
    xclose(*fd);
    bamipc_lockfile_acquire(pid, fd, path);

    if(*pid == 0) {
      break;    // lock obtained
    }

    if(*pid == -1) {
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

void API bamipc_lockfile_update(char * const restrict fmt, ...)
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

void API bamipc_lockfile_release(char * const restrict fmt, ...)
{
  va_list va;
  va_start(va, fmt);
  xunlinkvf(fmt, va);
  va_end(va);
}
