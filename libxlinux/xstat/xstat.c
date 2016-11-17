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

#include "internal.h"
#include "xstat/xstat.h"
#include "errtab/KERNEL.errtab.h"

#include "fmt.internal.h"

#define restrict __restrict

API xapi xstats(struct stat * restrict buf, const char * restrict path)
{
  enter;

  if(stat(path, buf) != 0)
    tfail(perrtab_KERNEL, errno);
  
finally:
  xapi_infof("path", "%s", path);
coda;
}

API xapi xstatf(struct stat * restrict buf, const char * restrict path_fmt, ...)
{
  enter;

  va_list va;
  va_start(va, path_fmt);

  fatal(xstatvf, buf, path_fmt, va);

  finally : coda;
}

API xapi xstatvf(struct stat * restrict buf, const char * restrict path_fmt, va_list va)
{
  enter;

  char path[512];

  fatal(fmt_apply, path, sizeof(path), path_fmt, va);
  fatal(xstats, buf, path);

  finally : coda;
}

API xapi uxstats(int * restrict r, struct stat * restrict buf, const char * restrict path)
{
  enter;

  if((r && ((*r) = stat(path, buf)) != 0) || (!r && stat(path, buf) != 0))
  {
    if(errno != ENOENT && errno != ENOTDIR)
      tfail(perrtab_KERNEL, errno);

    memset(buf, 0, sizeof(*buf));
  }
  
finally:
  xapi_infof("path", "%s", path);
coda;
}

API xapi uxstatf(int * restrict r, struct stat * restrict buf, const char * restrict path_fmt, ...)
{
  enter;

  va_list va;
  va_start(va, path_fmt);

  fatal(uxstatvf, r, buf, path_fmt, va);

  finally : coda;
}

API xapi uxstatvf(int * restrict r, struct stat * restrict buf, const char * restrict path_fmt, va_list va)
{
  enter;

  char path[512];

  fatal(fmt_apply, path, sizeof(path), path_fmt, va);
  fatal(uxstats, r, buf, path);

  finally : coda;
}

API xapi xlstats(int * restrict r, struct stat * restrict buf, const char * restrict path)
{
  enter;

  if((r && ((*r) = lstat(path, buf)) != 0) || (!r && lstat(path, buf) != 0))
    tfail(perrtab_KERNEL, errno);
  
finally:
  xapi_infof("path", "%s", path);
coda;
}

API xapi xlstatf(int * restrict r, struct stat * restrict buf, const char * restrict path_fmt, ...)
{
  enter;

  va_list va;
  va_start(va, path_fmt);

  fatal(xlstatvf, r, buf, path_fmt, va);

  finally : coda;
}

API xapi xlstatvf(int * restrict r, struct stat * restrict buf, const char * restrict path_fmt, va_list va)
{
  enter;

  char path[512];

  fatal(fmt_apply, path, sizeof(path), path_fmt, va);
  fatal(xlstats, r, buf, path);

  finally : coda;
}

API xapi uxlstats(int * restrict r, struct stat * restrict buf, const char * restrict path)
{
  enter;

  if((r && ((*r) = lstat(path, buf)) != 0) || (!r && lstat(path, buf) != 0))
  {
    if(errno != ENOENT && errno != ENOTDIR)
      tfail(perrtab_KERNEL, errno);

    memset(buf, 0, sizeof(*buf));
  }
  
finally:
  xapi_infof("path", "%s", path);
coda;
}

API xapi uxlstatf(int * restrict r, struct stat * restrict buf, const char * restrict path_fmt, ...)
{
  enter;

  va_list va;
  va_start(va, path_fmt);

  fatal(uxlstatvf, r, buf, path_fmt, va);

  finally : coda;
}

API xapi uxlstatvf(int * restrict r, struct stat * restrict buf, const char * restrict path_fmt, va_list va)
{
  enter;

  char path[512];

  fatal(fmt_apply, path, sizeof(path), path_fmt, va);
  fatal(uxlstats, r, buf, path);

  finally : coda;
}

API xapi xfstat(int fd, struct stat * buf)
{
  enter;

  if(fstat(fd, buf) != 0)
    tfail(perrtab_KERNEL, errno);
  
  finally : coda;
}

API xapi uxfstat(int fd, struct stat * buf)
{
  enter;

  if(fstat(fd, buf) != 0)
  {
    if(errno != ENOENT && errno != ENOTDIR)
      tfail(perrtab_KERNEL, errno);

    memset(buf, 0, sizeof(*buf));
  }
  
finally:
  xapi_infof("fd", "%d", fd);
coda;
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
