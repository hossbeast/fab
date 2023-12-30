/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <unistd.h>

#include "types.h"

#include "xlinux/xstdlib.h"
#include "xlinux/xstring.h"
#include "xlinux/xunistd.h"

#include "params.h"

struct g_params g_params;

//
// public
//

void params_setup()
{
  g_params.pid = getpid();

  xgetcwd(g_params.cwd, sizeof(g_params.cwd));
  g_params.cwd_len = strlen(g_params.cwd);

#if CONFIG_DEVEL
  char space[PATH_MAX];
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
#if CONFIG_DEVEL
  ifree(&g_params.exedir);
#endif
}
