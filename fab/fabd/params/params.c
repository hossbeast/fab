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

#include <time.h>
#include <stdio.h>
#include <inttypes.h>

#include "xlinux/xstdlib.h"
#include "xlinux/xstring.h"
#include "xlinux/xfcntl.h"
#include "xlinux/xunistd.h"
#include "xlinux/xpwd.h"

#include "params.h"

#include "macros.h"

__thread int32_t tid;
struct g_params g_params;

//
// public
//

void params_setup(uint64_t hash)
{
  char space[512];
  struct passwd *pwd;
  uid_t ruid;
  uid_t euid;
  uid_t suid;
  int __attribute__((unused)) r;

  g_params.pid = getpid();
  g_params.proj_dirfd = -1;

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

  snprintf(space, sizeof(space), "%s/%016"PRIx64, XQUOTE(FABIPCDIR), hash);
  ixstrdup(&g_params.ipcdir, space);

  g_params.proj_dir = xrealpathf(0, "%s/%016"PRIx64"/projdir", XQUOTE(FABIPCDIR), hash);
  g_params.proj_dirfd = xopens(O_PATH | O_DIRECTORY, g_params.proj_dir);

  // get real user identity
  xgetresuid(&ruid, &euid, &suid);
  pwd = xgetpwuid(ruid);
  ixstrdup(&g_params.homedir, pwd->pw_dir);
}

void params_cleanup()
{
  iwfree(&g_params.ipcdir);
  iwfree(&g_params.proj_dir);
  ixclose(&g_params.proj_dirfd);
  iwfree(&g_params.homedir);
}
