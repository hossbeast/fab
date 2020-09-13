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
#include "common/attrs.h"

attrs32 * APIDATA fabipc_msg_type_attrs = (attrs32[]) {{
#undef DEF
#define DEF(x, s, r, y) + 1
    num : 0
      FABIPC_MSG_TYPE_TABLE
  , members : (member32[]) {
#undef DEF
#define DEF(x, s, r, y) { name : s, value : UINT32_C(y), range : UINT32_C(r) },
FABIPC_MSG_TYPE_TABLE
#undef DEF
  }
}};

attrs32 * APIDATA fabipc_event_type_attrs = (attrs32[]) {{
#undef DEF
#define DEF(x, s, r, y) + 1
    num : 0
      FABIPC_EVENT_TYPE_TABLE
  , members : (member32[]) {
#undef DEF
#define DEF(x, s, r, y) { name : s, value : UINT32_C(y), range : UINT32_C(r) },
FABIPC_EVENT_TYPE_TABLE
#undef DEF
  }
}};

static void __attribute__((constructor)) init()
{
  attrs32_init(fabipc_msg_type_attrs);
  attrs32_init(fabipc_event_type_attrs);
}

//
// static
//

static xapi lockfile_obtain(pid_t * pid, int * restrict fd, const char * restrict path)
{
  enter;

  ssize_t bytes;

  // initialize the return value
  *pid = -1;

  // create the pidfile
  fatal(uxopen_modes, fd, O_CREAT | O_WRONLY | O_EXCL, FABIPC_MODE_DATA, path);
  if(*fd == -1)
  {
    fatal(xopens, fd, O_RDONLY, path);
    fatal(xread, *fd, pid, sizeof(*pid), &bytes);
    if(bytes != sizeof(*pid))
      *pid = -1;
  }
  else
  {
    fatal(axwrite, *fd, (pid_t[]) { getpid() }, sizeof(pid_t));
    *pid = 0;
  }

  finally : coda;
}

//
// api
//

xapi API fabipc_lockfile_obtain(pid_t * pid, int * restrict fd, char * const restrict fmt, ...)
{
  enter;

  char path[512];
  int x;
  va_list va;
  int r;

  va_start(va, fmt);
  fatal(fmt_apply, path, sizeof(path), fmt, va);

  for(x = 1; 1; x++)
  {
    fatal(xclose, *fd);
    fatal(lockfile_obtain, pid, fd, path);

    if(*pid == 0)
      break;    // lock obtained

    if(*pid == -1)
    {
      // unable to determine lock holder from the pid file ; this can happen as a
      // result of a race reading/writing the pid file, or if the pid file is corrupted

      if((x % 3) == 0) {
        fatal(xunlinks, path);
      }

      continue;
    }

    r = 0;
    fatal(uxkill, &r, *pid, 0);
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

xapi API fabipc_lockfile_update(char * const restrict fmt, ...)
{
  enter;

  int fd = -1;
  va_list va;
  va_start(va, fmt);

  // write our pid to the lockfile
  fatal(xopenvf, &fd, O_WRONLY, fmt, va);
  fatal(axwrite, fd, (pid_t[]) { getpid() }, sizeof(pid_t));

finally:
  fatal(ixclose, &fd);
coda;
}

xapi API fabipc_lockfile_release(char * const restrict fmt, ...)
{
  enter;

  va_list va;
  va_start(va, fmt);
  fatal(xunlinkvf, fmt, va);

  finally : coda;
}
