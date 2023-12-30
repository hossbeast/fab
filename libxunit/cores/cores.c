/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "narrator.h"
#include "narrator/fixed.h"
#include "xlinux/xstdlib.h"

#include "cores.h"
#include "macros.h"

#include <stdio.h>

static inline void runf(narrator * restrict N, int * restrict status, const char * const restrict fmt, ...)
{
  narrator_fixed *fixed;
  va_list va;
  int r;

  fixed = containerof(N, typeof(*fixed), base);

  narrator_reset(N);
  va_start(va, fmt);
  narrator_sayvf(N, fmt, va);
  va_end(va);
  r = system(fixed->s);
  if(status) {
    *status = r;
  }
}

/// cwd_core
//
// REMARKS
//  sysctl -w kernel/core_pattern=core.%p
//
static void cwd_core(narrator * restrict N, pid_t pid, bool * restrict ready)
{
  int status = 0;

  if(status == 0) {
    runf(N, &status, "cp ./core.%d /tmp/core.%d 2>/dev/null", pid, pid);
  }

  if(status == 0) {
    *ready = true;
  }
}

/// systemd_lz4_core
//
// REMARKS
//  decompresses and retrieves cores from the default location used by systemd
//
static void systemd_lz4_core(narrator * restrict N, pid_t pid, bool * ready)
{
  int status = 0;

  if(status == 0) {
    runf(N, &status, "cp /var/lib/systemd/coredump/*.%d.*.lz4 /tmp/core.%d.lz4 2>/dev/null", pid, pid);
  }

  if(status == 0) {
    runf(N, &status, "lz4 -fd /tmp/core.%d.lz4 /tmp/core.%d", pid, pid);
  }

  if(status == 0) {
    *ready = true;
  }
}

void print_core_backtrace(pid_t pid)
{
  char buf[PATH_MAX];
  bool ready = false;
  narrator_fixed fixed;

  narrator * N = narrator_fixed_init(&fixed, buf, sizeof(buf));

  if(!ready) {
    cwd_core(N, pid, &ready);
  }

  if(!ready) {
    systemd_lz4_core(N, pid, &ready);
  }

  if(ready) {
    runf(N, 0, "gdb -batch -ex info\\ shared -ex info\\ locals -ex frame -ex bt /proc/%d/exe /tmp/core.%d", getpid(), pid);
  }
}
