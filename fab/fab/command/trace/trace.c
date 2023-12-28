/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "types.h"

#include "bam/ipc.h"
#include "bam/client.h"
#include "xlinux/xunistd.h"

#include "trace.h"
#include "params.h"
#include "zcat.h"

struct trace_args trace_args;

//
// static
//

static void usage(command * restrict cmd)
{
  printf(
"\n"
"trace options\n"
  );
}

static void attached(command * restrict cmd, bam_client * restrict client)
{
  char *args[5];
  char pid[16];

  snprintf(pid, sizeof(pid), "%d", client->bamd_pid);

  args[0] = "/usr/bin/sudo";
  args[1] = (void*)trace_args.script_path.s;
  args[2] = "-p";
  args[3] = pid;
  args[4] = 0;

  xexecv(args[0], args);
}

//
// public
//

static void disconnected(command * restrict cmd) { }
struct command trace_command = {
    name : "trace"
  , attached : attached
  , usage : usage
  , disconnected : disconnected
};
