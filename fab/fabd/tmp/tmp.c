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

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#include "xapi.h"

#include "lorien/mkdirp.h"
#include "lorien/rmdirp.h"
#include "xlinux/xftw.h"
#include "xlinux/xstat.h"

#include "tmp.h"
#include "logging.h"

#include "parseint.h"
#include "macros.h"

/*
** time after which fabd will delete unknown entries in the tmp directory
** directories for related processes are (also) deleted on another schedule
*/
#define EXPIRATION_POLICY (60 * 60 * 24)    // 24 hours

struct context
{
  pid_t * pids;
  size_t pidsl;
};

static xapi fn(const char* fpath, const struct stat * sb, int typeflag, struct FTW * ftwbuf, void * arg)
{
  enter;

  struct context * ctx = arg;

  // unexpected file, skip
  if(typeflag != FTW_D)
    goto XAPI_FINALIZE;

  // unexpected directory name, skip
  pid_t pid = 0;
  if(parseint(fpath + ftwbuf->base, SCNd32, 0, INT32_MAX, 1, 10, &pid, 0))
    goto XAPI_FINALIZE;

  // still executing, skip
  if(kill(pid, 0))
    goto XAPI_FINALIZE;

  int x;
  for(x = 0; x < ctx->pidsl; x++)
  {
    if(pid == ctx->pids[x])
      break;
  }

  int r = 1;
  if(x == ctx->pidsl)  // not in pids
  {
    // check stamp file
    struct stat stb;
    fatal(uxstatf, &r, &stb, "%s/%s", fpath, "stamp");

    // directory does not contain a stamp file, or the stamp file is older than the expiration policy
    if(r == 0 && ((time(0) - stb.st_atim.tv_sec) > EXPIRATION_POLICY))
      r = 1;
  }

  // directory is for del, does not contain a stamp file, or the stamp file is older than the expiration policy
  if(r)
    fatal(rmdirp, fpath, 1);

finally :
  xapi_infof("path", "%s", fpath);
coda;
};

//
// public
//

xapi tmp_cleanup(pid_t * pids, size_t pidsl)
{
  enter;

  struct context ctx = { 0 };
  ctx.pids = pids;
  ctx.pidsl = pidsl;

  fatal(mkdirps, S_IRWXU | S_IRWXG | S_IRWXO, XQUOTE(FABTMPDIR) "/pid");
  fatal(xnftw_nth, XQUOTE(FABTMPDIR) "/pid", fn, 32, 0, 1, &ctx);

  finally : coda;
}
