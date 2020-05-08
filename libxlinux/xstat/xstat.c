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
#include <errno.h>

#include "xapi.h"
#include "types.h"

#include "internal.h"
#include "xstat/xstat.h"
#include "errtab/KERNEL.errtab.h"

#include "fmt.h"

API xapi xfstatats(int dirfd, int flags, struct stat * restrict buf, const char * restrict path)
{
  enter;

  if(fstatat(dirfd, path, buf, flags) != 0)
    tfail(perrtab_KERNEL, errno);

finally:
  xapi_infof("path", "%s", path);
coda;
}

API xapi xfstatatf(int dirfd, int flags, struct stat * restrict buf, const char * restrict path_fmt, ...)
{
  enter;

  va_list va;
  va_start(va, path_fmt);

  fatal(xfstatatvf, dirfd, flags, buf, path_fmt, va);

finally:
  va_end(va);
coda;
}

API xapi xfstatatvf(int dirfd, int flags, struct stat * restrict buf, const char * restrict path_fmt, va_list va)
{
  enter;

  char path[512];

  fatal(fmt_apply, path, sizeof(path), path_fmt, va);
  fatal(xfstatats, dirfd, flags, buf, path);

  finally : coda;
}

API xapi uxfstatats(int * restrict r, int dirfd, int flags, struct stat * restrict buf, const char * restrict path)
{
  enter;

  int lr;
  if(!r)
    r = &lr;

  if(((*r) = fstatat(dirfd, path, buf, flags)) != 0)
  {
    if(errno != ENOENT && errno != ENOTDIR)
      tfail(perrtab_KERNEL, errno);

    memset(buf, 0, sizeof(*buf));
  }

finally:
  xapi_infof("path", "%s", path);
coda;
}

API xapi uxfstatatf(int * restrict r, int dirfd, int flags, struct stat * restrict buf, const char * restrict path_fmt, ...)
{
  enter;

  va_list va;
  va_start(va, path_fmt);

  fatal(uxfstatatvf, r, dirfd, flags, buf, path_fmt, va);

finally:
  va_end(va);
coda;
}

API xapi uxfstatatvf(int * restrict r, int dirfd, int flags, struct stat * restrict buf, const char * restrict path_fmt, va_list va)
{
  enter;

  char path[512];

  fatal(fmt_apply, path, sizeof(path), path_fmt, va);
  fatal(uxfstatats, r, dirfd, flags, buf, path);

  finally : coda;
}

API xapi xfutimens(int fd, const struct timespec times[2])
{
  enter;

  tfatalize(perrtab_KERNEL, errno, futimens, fd, times);

  finally : coda;
}

API xapi xutimensats(int dirfd, const struct timespec times[2], int flags, const char * const restrict path)
{
  enter;

  tfatalize(perrtab_KERNEL, errno, utimensat, dirfd, path, times, flags);

  finally : coda;
}

API xapi uxutimensats(int dirfd, const struct timespec times[2], int flags, int * restrict r, const char * const restrict path)
{
  enter;

  if((r && ((*r) = utimensat(dirfd, path, times, flags)) != 0) || (!r && utimensat(dirfd, path, times, flags) != 0))
  {
    if(errno != ENOENT)
      tfail(perrtab_KERNEL, errno);
  }

finally:
  xapi_infof("path", "%s", path);
coda;
}

API xapi xmkdirs(mode_t mode, const char * path)
{
  enter;

  tfatalize(perrtab_KERNEL, errno, mkdir, path, mode);

finally:
  xapi_infos("path", path);
coda;
}

API xapi xmkdirf(mode_t mode, const char * path_fmt, ...)
{
  enter;

  va_list va;
  va_start(va, path_fmt);

  fatal(xmkdirvf, mode, path_fmt, va);

  finally : coda;
}

API xapi xmkdirvf(mode_t mode, const char * path_fmt, va_list va)
{
  enter;

  char path[512];

  fatal(fmt_apply, path, sizeof(path), path_fmt, va);
  fatal(xmkdirs, mode, path);

  finally : coda;
}

API xapi uxmkdirs(mode_t mode, const char * path)
{
  enter;

  if(mkdir(path, mode) != 0 && errno != EEXIST)
    tfail(perrtab_KERNEL, errno);

finally:
  xapi_infos("path", path);
coda;
}

API xapi uxmkdirf(mode_t mode, const char * path_fmt, ...)
{
  enter;

  va_list va;
  va_start(va, path_fmt);

  fatal(uxmkdirvf, mode, path_fmt, va);

  finally : coda;
}

API xapi uxmkdirvf(mode_t mode, const char * path_fmt, va_list va)
{
  enter;

  char path[512];

  fatal(fmt_apply, path, sizeof(path), path_fmt, va);
  fatal(uxmkdirs, mode, path);

  finally : coda;
}

API xapi xfchmod(int fd, mode_t mode)
{
  enter;

  tfatalize(perrtab_KERNEL, errno, fchmod, fd, mode);

finally:
  xapi_infof("fd", "%d", fd);
coda;
}
