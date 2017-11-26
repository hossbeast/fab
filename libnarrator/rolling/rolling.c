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

#include "internal.h"
#include "rolling.internal.h"

#include "parseint.h"
#include "strutil.h"

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

  struct dirent ent;
  struct dirent * entp = 0;
  fatal(xreaddir_r, dd, &ent, &entp);
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
            if(parseuint(entp->d_name + name_base_len + 1, "%"SCNu16, 0, UINT16_MAX, 5, 5, &counter, 0) == 0)
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

    fatal(xreaddir_r, dd, &ent, &entp);
  }

  if(found)
    fatal(rollover, n, 1);

  n->scanned = 1;

finally:
  if(dd)
    fatal(xclosedir, dd);
coda;
}

//
// public
//

xapi rolling_xsayvf(narrator_rolling * const restrict n, const char * const restrict fmt, va_list va)
{
  enter;

  // one-time directory scan
  if(n->scanned == 0)
    fatal(scan, n);

  if(n->fd == -1 || n->written > n->threshold)
    fatal(rollover, n, 0);

  fatal(xvdprintf, n->fd, fmt, va);

  finally : coda;
}

xapi rolling_xsayw(narrator_rolling * const restrict n, const char * const restrict b, size_t l)
{
  enter;

  // one-time directory scan
  if(n->scanned == 0)
    fatal(scan, n);

  if(n->fd == -1 || n->written > n->threshold)
    fatal(rollover, n, 0);

  fatal(axwrite, n->fd, b, l);
  n->written += l;

  finally : coda;
}

xapi rolling_xseek(narrator_rolling * const restrict n, off_t offset, int whence, off_t * restrict res)
{
  enter;

  fail(SYS_NOTSUPP);

  finally : coda;
}

xapi rolling_xdestroy(narrator_rolling * const restrict n)
{
  enter;

  wfree(n->directory);
  wfree(n->name_base);
  if(n->fd != -1)
    fatal(xclose, n->fd);

  finally : coda;
}

xapi rolling_xread(narrator_rolling * restrict n, void * dst, size_t count)
{
  enter;

  fail(SYS_NOTSUPP);

  finally : coda;
}

//
// api
//

API xapi narrator_rolling_create(narrator ** const restrict n, const char * const restrict path_base, mode_t mode, uint32_t threshold, uint16_t max_files)
{
  enter;

  fatal(xmalloc, n, sizeof(**n));
  (*n)->type = NARRATOR_ROLLING;
  (*n)->rolling.threshold = threshold;
  (*n)->rolling.max_files = max_files;
  (*n)->rolling.mode = mode;
  (*n)->rolling.fd = -1;

  size_t path_base_len = strlen(path_base);
  const char * e = path_base + path_base_len;
  while(e != path_base && *e != '/')
    e--;

  // no slash
  if(e == path_base)
    fail(SYS_INVALID);

  // trailing slash
  if((e - path_base) == path_base_len)
    fail(SYS_INVALID);

  fatal(ixstrndup, &(*n)->rolling.directory, path_base, e - path_base);
  fatal(ixstrdup, &(*n)->rolling.name_base, e + 1);

  finally : coda;
}
