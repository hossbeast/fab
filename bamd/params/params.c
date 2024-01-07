/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <time.h>
#include <stdio.h>
#include <inttypes.h>

#include "xlinux/xstdlib.h"
#include "xlinux/xstring.h"
#include "xlinux/xfcntl.h"
#include "xlinux/xunistd.h"

#include "args.h"
#include "params.h"

#include "macros.h"
#include "zcat.h"

struct g_params g_params;

//
// public
//

void params_setup()
{
  int __attribute__((unused)) r;
  const char *homeenv;
  const char *ipc;

  g_params.pid = getpid();

  // get available processors
  if((g_params.procs = sysconf(_SC_NPROCESSORS_ONLN)) == -1)
  {
    // unable to determine available CPU count
    g_params.procs = 0;
  }
  else if(g_params.procs < 1)
  {
    // shenanigans
    g_params.procs = 0;
  }

  // start time
  r = clock_gettime(CLOCK_MONOTONIC, &g_params.starttime);
  RUNTIME_ASSERT(r == 0);

  // ipc directory
  if((ipc = getenv("BAM_IPC_DIR"))) {
    g_params.ipcdir_len = zcatf(g_params.ipcdir, sizeof(g_params.ipcdir), "%s/%016"PRIx64, ipc, g_args.hash);
  } else if((ipc = getenv("XDG_RUNTIME_DIR"))) {
    g_params.ipcdir_len = zcatf(g_params.ipcdir, sizeof(g_params.ipcdir), "%s/bam/%016"PRIx64, ipc, g_args.hash);
  } else {
    dprintf(2, "neither $BAM_IPC_DIR nor $XDG_RUNTIME_DIR is set, exiting\n");
    exit(1);
  }

  g_params.homedir_len = 0;
  if((homeenv = getenv("HOME"))) {
    g_params.homedir_len = zcats(g_params.homedir, sizeof(g_params.homedir), homeenv);
  }
}
