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

#include "cores.h"
#include "macros.h"

static inline void runf(narrator * restrict N, int * restrict status, const char * const restrict fmt, ...)
{
  narrator_fixed *fixed;

  va_list va;
  va_start(va, fmt);
  fixed = containerof(N, typeof(*fixed), base);

  narrator_xreset(N);
  narrator_xsayvf(N, fmt, va);
  *status = xsystem(fixed->s);
}

/// cwd_core
//
// REMARKS
//  sysctl -w kernel/core_pattern=core.%p
//
static void cwd_core(narrator * restrict N, pid_t pid, bool * restrict ready)
{
  int status = 0;

  if(status == 0)
    runf(N, &status, "cp ./core.%d /tmp/core.%d 2>/dev/null", pid, pid);

  if(status == 0)
    *ready = true;
}

/// systemd_lz4_core
//
// REMARKS
//  decompresses and retrieves cores from the default location used by systemd
//
static void systemd_lz4_core(narrator * restrict N, pid_t pid, bool * ready)
{
  int status = 0;

  if(status == 0)
    runf(N, &status, "cp /var/lib/systemd/coredump/*.%d.*.lz4 /tmp/core.%d.lz4 2>/dev/null", pid, pid);

  if(status == 0)
    runf(N, &status, "lz4 -fd /tmp/core.%d.lz4 /tmp/core.%d", pid, pid);

  if(status == 0)
    *ready = true;
}

void print_core_backtrace(pid_t pid)
{
  char buf[512];
  bool ready = false;
  narrator_fixed fixed;

  narrator * N = narrator_fixed_init(&fixed, buf, sizeof(buf));

  if(!ready)
    cwd_core(N, pid, &ready);

  if(!ready)
    systemd_lz4_core(N, pid, &ready);

  if(ready)
    runf(N, 0, "gdb -batch -ex info\\ shared -ex info\\ locals -ex frame -ex bt /proc/%d/exe /tmp/core.%d", getpid(), pid);
}

