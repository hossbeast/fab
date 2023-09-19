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

static void __attribute__((nonnull)) rollover(narrator_rolling * const restrict n, int append)
{
  char space[256];
  off_t res;

  snprintf(space, sizeof(space), "%s/%s.%05hu", n->directory, n->name_base, n->counter);

  // unlink an existing file, if any
  if(!append)
    uxunlinks(space);

  ixclose(&n->fd);
  n->fd = xopen_modes(O_CREAT | O_WRONLY, n->mode, space);

  if(append)
  {
    res = xlseek(n->fd, 0, SEEK_END);
    n->written = res;
  }
  else
  {
    n->written = 0;
  }

  n->counter++;
  if(n->counter >= n->max_files)
    n->counter = 0;
}

static void __attribute__((nonnull)) scan(narrator_rolling * const restrict n)
{
  int found = 0;
  DIR * dd = 0;
  size_t name_base_len;
  uint16_t counter;
  struct dirent * entp = 0;

  if((dd = opendir(n->directory)) == 0)
    return;

  name_base_len = strlen(n->name_base);

  entp = xreaddir(dd);
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

    entp = xreaddir(dd);
  }

  if(found)
    rollover(n, 1);

  n->scanned = 1;

  if(dd) {
    xclosedir(dd);
  }
}

static void rolling_sayvf(narrator * const restrict N, const char * const restrict fmt, va_list va)
{
  narrator_rolling *n = containerof(N, typeof(*n), base);

  // one-time directory scan
  if(n->scanned == 0)
    scan(n);

  if(n->fd == -1 || n->written > n->threshold)
    rollover(n, 0);

  xvdprintf(n->fd, fmt, va);
}

static void rolling_sayw(narrator * const restrict N, const void * const restrict b, size_t l)
{
  narrator_rolling *n = containerof(N, typeof(*n), base);

  // one-time directory scan
  if(n->scanned == 0)
    scan(n);

  if(n->fd == -1 || n->written > n->threshold)
    rollover(n, 0);

  axwrite(n->fd, b, l);
  n->written += l;
}

static void rolling_flush(narrator * restrict N)
{

}

#define rolling_seek 0
#define rolling_read 0
struct narrator_vtable API narrator_rolling_vtable = NARRATOR_VTABLE(rolling);

//
// api
//

void API narrator_rolling_create(narrator_rolling ** const restrict n, const char * const restrict path_base, mode_t mode, uint32_t threshold, uint16_t max_files)
{
  narrator_rolling *rolling;
  size_t path_base_len;
  const char * e;

  xmalloc(&rolling, sizeof(*rolling));

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
    RUNTIME_ABORT();

  // trailing slash
  if((e - path_base) == path_base_len)
    RUNTIME_ABORT();

  ixstrndup(&rolling->directory, path_base, e - path_base);
  ixstrdup(&rolling->name_base, e + 1);

  *n = rolling;
}

void API narrator_rolling_free(narrator_rolling *n)
{
  if(n) {
    narrator_rolling_destroy(n);
  }
  wfree(n);
}

void API narrator_rolling_destroy(narrator_rolling * restrict n)
{
  wfree(n->directory);
  wfree(n->name_base);
  xclose(n->fd);
}
