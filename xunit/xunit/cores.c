/* Copyright (c) 2012-2018 Todd Freed <todd.freed@gmail.com>

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

#include "narrator.h"
#include "narrator/fixed.h"
#include "xlinux/xstdlib.h"
#include "logger/arguments.h"

#include "cores.h"

static inline xapi runf(narrator * restrict N, int * restrict status, const char * const restrict fmt, ...)
{
  enter;

  va_list va;
  va_start(va, fmt);

  fatal(narrator_xreset, N);
  fatal(narrator_xsayvf, N, fmt, va);
  fatal(xsystem, narrator_fixed_buffer(N), status);

  finally : coda;
}

/// cwd_core
//
// REMARKS
//  sysctl -w kernel/core_pattern=core.%p
//
static xapi cwd_core(narrator * restrict N, pid_t pid, bool * restrict ready)
{
  enter;

  int status = 0;

  if(status == 0)
    fatal(runf, N, &status, "cp ./core.%d /tmp/core.%d 2>/dev/null", pid, pid);

  if(status == 0)
    *ready = true;

  finally : coda;
}

/// systemd_lz4_core
//
// REMARKS
//  decompresses and retrieves cores from the default location used by systemd
//
static xapi systemd_lz4_core(narrator * restrict N, pid_t pid, bool * ready)
{
  enter;

  int status = 0;

  if(status == 0)
    fatal(runf, N, &status, "cp /var/lib/systemd/coredump/*.%d.*.lz4 /tmp/core.%d.lz4 2>/dev/null", pid, pid);

  if(status == 0)
    fatal(runf, N, &status, "lz4 -fd /tmp/core.%d.lz4 /tmp/core.%d", pid, pid);

  if(status == 0)
    *ready = true;

  finally : coda;
}

xapi print_core_backtrace(pid_t pid)
{
  enter;

  char buf[512];
  char fixed[NARRATOR_STATIC_SIZE];
  bool ready = false;

  narrator * N = narrator_fixed_init(fixed, buf, sizeof(buf));

  if(!ready)
    fatal(cwd_core, N, pid, &ready);

  if(!ready)
    fatal(systemd_lz4_core, N, pid, &ready);

  if(ready)
    fatal(runf, N, 0, "gdb -batch -ex info\\ shared -ex info\\ locals -ex frame -ex bt %s /tmp/core.%d", g_binary, pid);

  finally : coda;
}
