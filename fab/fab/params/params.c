/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <unistd.h>

#include "types.h"

#include "xlinux/xstdlib.h"
#include "xlinux/xstring.h"

#include "params.h"

struct g_params g_params;

//
// public
//

void params_setup()
{
  g_params.pid = getpid();

#if DEVEL
  char space[512];
  ssize_t r;

  // exedir is the canonical path to directory containing the executing binary
  r = xreadlinks("/proc/self/exe", space, sizeof(space));
  r--;
  while(space[r] != '/') {
    r--;
  }

  ixstrndup(&g_params.exedir, space, r);
#endif
}

void params_teardown()
{
#if DEVEL
  ifree(&g_params.exedir);
#endif
}
