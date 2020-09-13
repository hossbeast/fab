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

#include <string.h>
#include <stdio.h>

#include "xapi.h"
#include "xapi/SYS.errtab.h"

#include "xlinux/xdirent.h"
#include "xlinux/xstdio.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xstring.h"
#include "xlinux/xunistd.h"
#include "xlinux/xfcntl.h"

#include "narrator.h"
#include "rolling.h"
#include "vtable.h"

#include "common/parseint.h"
#include "macros.h"

//
// static
//

static xapi __attribute__((nonnull)) rollover(narrator_rolling * const restrict n, int append)
{
  enter;

  char space[256];
  snprintf(space, sizeof(space), "%s/%s.%05hu", n->directory, n->name_base, n->counter);

  // unlink an existing file, if any
  if(!append)
    fatal(uxunlinks, space);

  fatal(ixclose, &n->fd);
  fatal(xopen_modes, &n->fd, O_CREAT | O_WRONLY, n->mode, space);

  if(append)
  {
    off_t res;
    fatal(xlseek, n->fd, 0, SEEK_END, &res);
    n->written = res;
  }
  else
  {
    n->written = 0;
  }

  n->counter++;
  if(n->counter >= n->max_files)
    n->counter = 0;

  finally : coda;
}

static xapi __attribute__((nonnull)) scan(narrator_rolling * const restrict n)
{
  enter;

  int found = 0;
  DIR * dd = 0;
  if((dd = opendir(n->directory)) == 0)
    goto XAPI_FINALIZE;

  size_t name_base_len = strlen(n->name_base);

  struct dirent * entp = 0;
  fatal(xreaddir, dd, &entp);
  while(entp)
  {
    if(strcmp(entp->d_name, ".") && strcmp(entp->d_name, ".."))
    {
      if(strlen(entp->d_name) > name_base_len)
      {
        if(memcmp(entp->d_name, n->name_base, name_base_len) == 0)
        {
          if(entp->d_name[name_base_len] == '.')
          {
            uint16_t counter;
            if(parseuint(entp->d_name + name_base_len + 1, SCNu16, 0, UINT16_MAX, 5, 5, &counter, 0) == 0)
            {
              if(counter < n->max_files && counter >= n->counter)
              {
                n->counter = counter;
                found = 1;
              }
            }
          }
        }
      }
    }

    fatal(xreaddir, dd, &entp);
  }

  if(found)
    fatal(rollover, n, 1);

  n->scanned = 1;

finally:
  if(dd)
    fatal(xclosedir, dd);
coda;
}

static xapi rolling_sayvf(narrator * const restrict N, const char * const restrict fmt, va_list va)
{
  enter;

  narrator_rolling *n = containerof(N, typeof(*n), base);

  // one-time directory scan
  if(n->scanned == 0)
    fatal(scan, n);

  if(n->fd == -1 || n->written > n->threshold)
    fatal(rollover, n, 0);

  fatal(xvdprintf, n->fd, fmt, va);

  finally : coda;
}

static xapi rolling_sayw(narrator * const restrict N, const void * const restrict b, size_t l)
{
  enter;

  narrator_rolling *n = containerof(N, typeof(*n), base);

  // one-time directory scan
  if(n->scanned == 0)
    fatal(scan, n);

  if(n->fd == -1 || n->written > n->threshold)
    fatal(rollover, n, 0);

  fatal(axwrite, n->fd, b, l);
  n->written += l;

  finally : coda;
}

static xapi rolling_flush(narrator * restrict N)
{
  enter;

  finally : coda;
}

#define rolling_seek 0
#define rolling_read 0
struct narrator_vtable API narrator_rolling_vtable = NARRATOR_VTABLE(rolling);

//
// api
//

xapi API narrator_rolling_create(narrator_rolling ** const restrict n, const char * const restrict path_base, mode_t mode, uint32_t threshold, uint16_t max_files)
{
  enter;

  narrator_rolling *rolling;
  size_t path_base_len;
  const char * e;

  fatal(xmalloc, &rolling, sizeof(*rolling));

  rolling->base.vtab = &narrator_rolling_vtable;
  rolling->threshold = threshold;
  rolling->max_files = max_files;
  rolling->mode = mode;
  rolling->fd = -1;

  path_base_len = strlen(path_base);
  e = path_base + path_base_len;
  while(e != path_base && *e != '/')
    e--;

  // no slash
  if(e == path_base)
    fail(SYS_INVALID);

  // trailing slash
  if((e - path_base) == path_base_len)
    fail(SYS_INVALID);

  fatal(ixstrndup, &rolling->directory, path_base, e - path_base);
  fatal(ixstrdup, &rolling->name_base, e + 1);

  *n = rolling;

  finally : coda;
}

xapi API narrator_rolling_free(narrator_rolling *n)
{
  enter;

  if(n) {
    fatal(narrator_rolling_destroy, n);
  }
  wfree(n);

  finally : coda;
}

xapi API narrator_rolling_destroy(narrator_rolling * restrict n)
{
  enter;

  wfree(n->directory);
  wfree(n->name_base);
  if(n->fd != -1)
    fatal(xclose, n->fd);

  finally : coda;
}
